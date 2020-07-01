#include "include.h"
#include <thread>
#include <queue>
#include <atomic>
using namespace std;

/**
class Thread
	constructor(code: string, name: string, args: table/array)
		code can be binary or text.
		Only the following types of value can be included in args:
			number, boolean, string
	wait(): boolean
		Block until thread finished or errored.
		If return after blocks, return true. Otherwise, return false.
	get(): boolean, ...
		Block until thread finished or errored.
		If thread finished successfully, return true and values returned from thread.
		Otherwise, return false and error message.
	status(): string
		pending, running, finished, errored

NOTICE
	You should always call wait() method on Thread objects.
	By default, Thread objects are not required to be joined/waited.
	If a running Thread object is garbage-collected, in order not to block gc, a distached thread is spawned to wait it and clean up lua state.
	If the main thread exited, other running threads will be killed, causing undefined behavior.

REMARK
	Channels are encouraged to be used as a method of exchanging values between Threads. It supports more data types and is bidirectional.
*/

class LuaThread
{
private:
	void _WorkerEntry()
	{
		status = 1;
		if (lua_pcall(L, nArgs, LUA_MULTRET, 1))
		{
			status = 3;
		}
		else
		{
			status = 2;
		}
	}
public:
	lua_State* L;
	atomic<int> nArgs;
	// 0 Not started 1 Running 2 Finished 3 Errored
	atomic<int> status;
	shared_ptr<thread> sptd;

	LuaThread(lua_State* subLVM, int subArgs) : L(subLVM), nArgs(subArgs), status(0), sptd(make_shared<thread>(&LuaThread::_WorkerEntry, this)) {}

	~LuaThread()
	{
		if (sptd->joinable())
		{
			if (status < 2)
			{
				SDL_Log("LuaThread %d being collected while running, spawning a new backgroud thread and wait for it.\n", sptd->get_id());
				thread ntd([](lua_State* L, shared_ptr<thread> spoldtd)
					{
						thread::id oldtid = spoldtd->get_id();
						spoldtd->join();
						SDL_Log("LuaThread %d finished in background, cleaning up.\n", oldtid);
						lua_close(L);
					}, L, sptd);
				L = nullptr;
				ntd.detach();
			}
			else
			{
				sptd->join();
				lua_close(L);
			}
		}
		else
		{
			lua_close(L);
		}
	}
};

int thread_dtor(lua_State* L)
{
	auto t = lua_checkblock<LuaThread>(L, 1, "LuaThread");
	t->~LuaThread();
	return 0;
}

int thread_wait(lua_State* L)
{
	auto t = lua_checkblock<LuaThread>(L, 1, "LuaThread");
	if (t->sptd->joinable())
	{
		t->sptd->join();
		lua_pushboolean(L, 1);
	}
	else
	{
		lua_pushboolean(L, 0);
	}
	return 1;
}

int thread_get(lua_State* L)
{
	auto t = lua_checkblock<LuaThread>(L, 1, "LuaThread");
	if (t->sptd->joinable())
	{
		t->sptd->join();
	}

	if (t->status == 2)
	{
		lua_pushboolean(L, true);
	}
	else
	{
		lua_pushboolean(L, false);
	}

	int stackTop = lua_gettop(t->L);
	for (int i = 1; i <= stackTop; i++)
	{
		switch (lua_type(t->L, i))
		{
		case LUA_TNIL:
			lua_pushnil(L);
			break;
		case LUA_TNUMBER:
			lua_pushnumber(L, lua_tonumber(t->L, i));
			break;
		case LUA_TBOOLEAN:
			lua_pushboolean(L, lua_toboolean(t->L, i));
			break;
		case LUA_TSTRING:
		{
			size_t datalen;
			const char* data = lua_tolstring(t->L, -1, &datalen);
			lua_pushlstring(L, data, datalen);
			break;
		}
		default:
			return luaL_error(L, "thread_get: return value #%d has unsupported type: %s", i, lua_typename(t->L, lua_type(t->L, i)));
		}
	}

	return stackTop + 1;
}

int thread_status(lua_State* L)
{
	auto t = lua_checkblock<LuaThread>(L, 1, "LuaThread");
	switch (t->status)
	{
	case 0:
		lua_pushstring(L, "pending");
		break;
	case 1:
		lua_pushstring(L, "running");
		break;
	case 2:
		lua_pushstring(L, "finished");
		break;
	case 3:
		lua_pushstring(L, "errored");
		break;
	default:
		lua_pushstring(L, "unknown");
	}
	return 1;
}

static int thread_traceback(lua_State* L)
{
	luaL_traceback(L, L, NULL, 1);
	SDL_Log("ThreadError: %s\n", lua_tostring(L, -1));
	return 1;
}

int thread_new(lua_State* L)
{
	size_t codelen;
	const char* code = luaL_checklstring(L, 1, &codelen);
	const char* cname = luaL_checkstring(L, 2);
	luaL_checktype(L, 3, LUA_TTABLE);

	lua_State* subL = CreateLuaEngine();
	lua_pushcfunction(L, thread_traceback);
	
	// compile
	if (luaL_loadbuffer(subL, code, codelen, cname))
	{
		// Compile error, cannot load. Return error message to caller.
		lua_pushnil(L);
		lua_pushstring(L, lua_tostring(subL, 1));
		lua_close(subL);
		return 2;
	}

	// push args
	bool reachEnd = false;
	int idxArgs = 1;
	for (; !reachEnd; idxArgs++)
	{
		int type = lua_geti(L, 3, idxArgs);
		switch (type)
		{
		case LUA_TNIL:
			SDL_Log("Total %d args for new thread\n", idxArgs - 1);
			reachEnd = true;
			break;
		case LUA_TNUMBER:
			lua_pushnumber(subL, lua_tonumber(L, -1));
			break;
		case LUA_TBOOLEAN:
			lua_pushboolean(subL, lua_toboolean(L, -1));
			break;
		case LUA_TSTRING:
		{
			size_t datalen;
			const char* data = lua_tolstring(L, -1, &datalen);
			lua_pushlstring(subL, data, datalen);
			break;
		}
		default:
			lua_pushnil(L);
			lua_pushfstring(L, "thread_create: parameter #%d has unsupported type: %s", idxArgs, lua_typename(L, lua_type(L, -2)));
			lua_close(subL);
			return 2;
		}
		lua_pop(L, 1);
	}

	auto c = new (lua_newblock<LuaThread>(L)) LuaThread(subL, idxArgs - 2);  // Here idxArgs starts from 1, accumulate to 2 at least.
	if (luaL_newmetatable(L, "LuaThread"))
	{
		lua_setfield_function(L, "__gc", thread_dtor);
		lua_newtable(L);
		lua_setfield_function(L, "wait", thread_wait);
		lua_setfield_function(L, "get", thread_get);
		lua_setfield_function(L, "status", thread_status);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
	return 1;
}

void InitThread(lua_State* L)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaded");
	lua_pushcfunction(L, thread_new);
	lua_setfield(L, -2, "Thread");
	lua_pop(L, 2);
}
