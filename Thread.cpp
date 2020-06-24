#include "include.h"
#include <thread>
#include <queue>
#include <atomic>
using namespace std;

class LuaThread
{
private:
	void _WorkerEntry()
	{
		status = 1;
		if (lua_pcall(L, nArgs, LUA_MULTRET, 0))
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
	int nArgs;
	// 0 Not started 1 Running 2 Finished 3 Errored
	atomic<int> status;
	thread td;

	LuaThread(lua_State* subLVM, int subArgs) : L(subLVM), nArgs(subArgs), status(0), td(&LuaThread::_WorkerEntry, this)
	{

	}

	~LuaThread()
	{
		if (td.joinable())
		{
			td.join();
		}
		lua_close(L);
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
	if (t->td.joinable())
	{
		t->td.join();
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
	if (t->td.joinable())
	{
		t->td.join();
	}

	if (t->status == 2)
	{
		lua_pushboolean(L, 1);
	}
	else
	{
		lua_pushboolean(L, 0);
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
			lua_pushstring(L, lua_tostring(t->L, i));
			break;
		default:
			return luaL_error(L, "thread_get: return value #%d has unsupported type: %s", i, lua_typename(t->L, lua_type(t->L, i)));
		}
	}

	return stackTop + 1;
}

int thread_status(lua_State* L)
{
	auto t = lua_checkblock<LuaThread>(L, 1, "LuaThread");
	lua_pushinteger(L, t->status);
	return 1;
}

int thread_new(lua_State* L)
{
	size_t codelen;
	const char* code = luaL_checklstring(L, 1, &codelen);
	
	lua_State* subL = CreateLuaEngine();
	
	// compile
	if (luaL_loadbuffer(subL, code, codelen, "ThreadMain"))
	{
		// Compile error, cannot load. Return error message to caller.
		lua_pushnil(L);
		lua_pushstring(L, lua_tostring(subL, 1));
		lua_close(subL);
		return 2;
	}

	// push args
	int stackTop = lua_gettop(L);
	for (int idx = 2; idx <= stackTop; idx++)
	{
		switch (lua_type(L, idx))
		{
		case LUA_TNIL:
			lua_pushnil(subL);
			break;
		case LUA_TNUMBER:
			lua_pushnumber(subL, lua_tonumber(L, idx));
			break;
		case LUA_TBOOLEAN:
			lua_pushboolean(subL, lua_toboolean(L, idx));
			break;
		case LUA_TSTRING:
		{
			size_t datalen;
			const char* data = lua_tolstring(L, idx, &datalen);
			lua_pushlstring(subL, data, datalen);
			break;
		}
		default:
			lua_pushnil(L);
			lua_pushfstring(L, "thread_create: parameter #%d has unsupported type: %s", idx, lua_typename(L, lua_type(L, idx)));
			lua_close(subL);
			return 2;
		}
	}

	auto c = new (lua_newblock<LuaThread>(L)) LuaThread(subL, stackTop - 1);
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
