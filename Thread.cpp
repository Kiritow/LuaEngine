#include "include.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
using namespace std;

class SubLuaVM
{
public:
	lua_State* L;
	unique_ptr<thread> td;
	mutex mLock;
	condition_variable cond;
	string inBuffer;
	string outBuffer;
	int status;  // -1 Not started 0 Running 1 Exited 2 Exited with exception

	SubLuaVM()
	{
		status = -1;
		L = luaL_newstate();
		luaL_openlibs(L);
		InitTCPSocket(L);
	}

	~SubLuaVM()
	{
		if (td && td->joinable()) td->join();
		if(L) lua_close(L);
	}
};

static void SubLuaVMEntry(SubLuaVM* vm)
{
	{
		unique_lock<mutex> ulk(vm->mLock);
		if (luaL_loadstring(vm->L, vm->inBuffer.c_str()))
		{
			vm->outBuffer = lua_tostring(vm->L, -1);
			vm->status = 2;
			vm->cond.notify_all();
			return;
		}
		vm->inBuffer.clear();
	}

	if(lua_pcall(vm->L, 0, LUA_MULTRET, 0))
	{
		unique_lock<mutex> ulk(vm->mLock);
		vm->outBuffer = lua_tostring(vm->L, -1);
		vm->status = 2;
		vm->cond.notify_all();
		return;
	}
	else
	{
		vm->status = 1;
		vm->cond.notify_all();
	}
}

int thread_dtor(lua_State* L)
{
	auto vm = lua_checkblock<SubLuaVM>(L, 1, "LuaEngineThread");
	vm->~SubLuaVM();
	return 0;
}

int thread_put(lua_State* L)
{
	auto vm = lua_checkblock<SubLuaVM>(L, 1, "LuaEngineThread");
	size_t datasz;
	const char* rawdata = luaL_checklstring(L, 2, &datasz);
	string data(rawdata, datasz);

	int timeout_ms = -1;
	if (!lua_isnone(L, 3))
	{
		timeout_ms = luaL_checkinteger(L, 3);
	}
	if (timeout_ms >= 0)
	{
		unique_lock<mutex> ulk(vm->mLock);
		while (!vm->inBuffer.empty())
		{
			auto ret = vm->cond.wait_for(ulk, chrono::milliseconds(timeout_ms));
			if (ret == cv_status::timeout)
			{
				lua_pushboolean(L, false);
				return 1;
			}
			else continue;
		}
		vm->inBuffer = data;
		lua_pushboolean(L, true);
		return 1;
	}
	else
	{
		unique_lock<mutex> ulk(vm->mLock);
		while (!vm->inBuffer.empty())
		{
			vm->cond.wait(ulk);
		}
		vm->inBuffer = data;
		return 0;
	}
}

int thread_get(lua_State* L)
{
	auto vm = lua_checkblock<SubLuaVM>(L, 1, "LuaEngineThread");
	int timeout_ms = -1;
	if (!lua_isnone(L, 2))
	{
		timeout_ms = luaL_checkinteger(L, 2);
	}
	if (timeout_ms >= 0)
	{
		unique_lock<mutex> ulk(vm->mLock);
		while (vm->outBuffer.empty())
		{
			auto ret = vm->cond.wait_for(ulk, chrono::milliseconds(timeout_ms));
			if (ret == cv_status::timeout)
			{
				lua_pushboolean(L, false);
				return 1;
			}
			else continue;
		}
		string temp = vm->outBuffer;
		vm->outBuffer.clear();
		lua_pushboolean(L, true);
		lua_pushlstring(L, temp.data(), temp.size());
		return 2;
	}
	else
	{
		unique_lock<mutex> ulk(vm->mLock);
		while (vm->outBuffer.empty())
		{
			vm->cond.wait(ulk);
		}
		string temp = vm->outBuffer;
		vm->outBuffer.clear();
		lua_pushlstring(L, temp.data(), temp.size());
		return 1;
	}
}

int thread_inner_get(lua_State* L)
{
	auto vm = lua_checkpointer<SubLuaVM>(L, 1, "LuaEngineThreadWorker");
	int timeout_ms = -1;
	if (!lua_isnone(L, 2))
	{
		timeout_ms = luaL_checkinteger(L, 2);
	}
	if (timeout_ms >= 0)
	{
		unique_lock<mutex> ulk(vm->mLock);
		while (vm->inBuffer.empty())
		{
			auto ret = vm->cond.wait_for(ulk, chrono::milliseconds(timeout_ms));
			if (ret == cv_status::timeout)
			{
				lua_pushboolean(L, false);
				return 1;
			}
			else continue;
		}
		string temp = vm->inBuffer;
		vm->inBuffer.clear();
		lua_pushboolean(L, true);
		lua_pushlstring(L, temp.data(), temp.size());
		return 2;
	}
	else
	{
		unique_lock<mutex> ulk(vm->mLock);
		while (vm->inBuffer.empty())
		{
			vm->cond.wait(ulk);
		}
		string temp = vm->inBuffer;
		vm->inBuffer.clear();
		lua_pushlstring(L, temp.data(), temp.size());
		return 1;
	}
}

int thread_inner_put(lua_State* L)
{
	auto vm = lua_checkblock<SubLuaVM>(L, 1, "LuaEngineThreadWorker");
	size_t datasz;
	const char* rawdata = luaL_checklstring(L, 2, &datasz);
	string data(rawdata, datasz);

	int timeout_ms = -1;
	if (!lua_isnone(L, 3))
	{
		timeout_ms = luaL_checkinteger(L, 3);
	}
	if (timeout_ms >= 0)
	{
		unique_lock<mutex> ulk(vm->mLock);
		while (!vm->outBuffer.empty())
		{
			auto ret = vm->cond.wait_for(ulk, chrono::milliseconds(timeout_ms));
			if (ret == cv_status::timeout)
			{
				lua_pushboolean(L, false);
				return 1;
			}
			else continue;
		}
		vm->outBuffer = data;
		lua_pushboolean(L, true);
		return 1;
	}
	else
	{
		unique_lock<mutex> ulk(vm->mLock);
		while (!vm->outBuffer.empty())
		{
			vm->cond.wait(ulk);
		}
		vm->outBuffer = data;
		return 0;
	}
}

int thread_new(lua_State* L)
{
	size_t codesz;
	const char* rawcode = luaL_checklstring(L, 1, &codesz);
	string code(rawcode, codesz);

	// 在主端这一侧添加全部内容, 包扩gc.
	auto vm = new (lua_newblock<SubLuaVM>(L)) SubLuaVM;
	if (luaL_newmetatable(L, "LuaEngineThread"))
	{
		lua_setfield_function(L, "__gc", thread_dtor);
		lua_newtable(L);
		lua_setfield_function(L, "get", thread_get);
		lua_setfield_function(L, "put", thread_put);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);

	// 在从端这一侧添加this_thread, 只有get和set.
	lua_getglobal(vm->L, "package");
	lua_getfield(vm->L, -1, "loaded");
	lua_newpointer(vm->L, vm);
	if (luaL_newmetatable(vm->L, "LuaEngineThreadWorker"))
	{
		lua_newtable(vm->L);
		lua_setfield_function(vm->L, "get", thread_inner_get);
		lua_setfield_function(vm->L, "put", thread_inner_put);
		lua_setfield(vm->L, -2, "__index");
	}
	lua_setmetatable(vm->L, -2);
	lua_setfield(vm->L, -2, "this_thread");
	lua_pop(vm->L, 2);

	vm->inBuffer = code;
	vm->td.reset(new thread(SubLuaVMEntry, vm));
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
