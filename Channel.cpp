#include "include.h"
#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>
#include <atomic>
#include <map>
using namespace std;

/**
module Channel
	create(name: string): Channel, isCreated: boolean
	open(name: string): Channel or nil
	collect()

class Channel
	put(value: nil, number, boolean, string)
	get(): nil, number, boolean, string

	try_put(value: nil, number, boolean, string, timeout_ms: int): isSuccess: boolean
	try_get(timeout_ms: int): isSuccess: boolean, value: nil, number, boolean, string
	wait([timeout_ms: int]): hasValue: boolean
		Block until has value if no timeout specified. Return false on reaching timeout.
		Negative timeout is regarded as 0.
		Zero timeout means instant check-and-return.
*/

class Channel
{
public:
	int capacity;
	queue<tuple<int, string>> bus;
	mutex m;
	condition_variable cond;

	Channel(int cap) : capacity(cap)
	{

	}
};

class LuaChannel
{
public:
	shared_ptr<Channel> sp;
};

int channel_dtor(lua_State* L)
{
	auto c = lua_checkblock<LuaChannel>(L, 1, "LuaChannel");
	c->~LuaChannel();
	return 0;
}

static bool push_to_lua(lua_State* L, int type, const string& value)
{
	switch (type)
	{
	case LUA_TNIL:
		lua_pushnil(L);
		return true;
	case LUA_TNUMBER:
		lua_stringtonumber(L, value.c_str());
		return true;
	case LUA_TBOOLEAN:
		lua_pushboolean(L, value.empty());
		return true;
	case LUA_TSTRING:
		lua_pushlstring(L, value.data(), value.size());
		return true;
	default:
		return false;
	}
}

static bool get_from_lua(lua_State* L, int index, int& type, string& value)
{
	type = lua_type(L, index);
	switch (type)
	{
	case LUA_TNIL:
		value = string();
		return true;
	case LUA_TNUMBER:
		value = lua_tostring(L, index);
		return true;
	case LUA_TBOOLEAN:
	{
		if (lua_toboolean(L, index))
		{
			value = "true";
		}
		else
		{
			value = string();
		}
		return true;
	}
	case LUA_TSTRING:
	{
		size_t sz;
		const char* p = lua_tolstring(L, index, &sz);
		value = string(p, sz);
		return true;
	}
	default:
		return false;
	}
}

// Get a value from channel
int channel_get(lua_State* L)
{
	auto c = lua_checkblock<LuaChannel>(L, 1, "LuaChannel");

	int type;
	string value;

	{
		unique_lock<mutex> ulk(c->sp->m);
		c->sp->cond.wait(ulk, [&]() { return !c->sp->bus.empty(); });
		// Un-serialize from string to Lua object

		tie(type, value) = c->sp->bus.front();
		c->sp->bus.pop();
		c->sp->cond.notify_all();
	}

	if (!push_to_lua(L, type, value))
	{
		return luaL_error(L, "channel_get: unsupported type %s", lua_typename(L, type));
	}
	return 1;
}

int channel_try_get(lua_State* L)
{
	auto c = lua_checkblock<LuaChannel>(L, 1, "LuaChannel");
	int ms = luaL_checkinteger(L, 2);

	bool success;
	int type;
	string value;

	{
		unique_lock<mutex> ulk(c->sp->m);
		if (ms > 0)
		{
			success = c->sp->cond.wait_for(ulk, chrono::milliseconds(ms), [&]() { return !c->sp->bus.empty(); });
		}
		else
		{
			success = !c->sp->bus.empty();
		}

		if (success)
		{
			tie(type, value) = c->sp->bus.front();
			c->sp->bus.pop();
			c->sp->cond.notify_all();
		}
	}

	if (success)
	{
		lua_pushboolean(L, true);
		if (!push_to_lua(L, type, value))
		{
			return luaL_error(L, "channel_try_get: unsupported type %s", lua_typename(L, type));
		}
		return 2;
	}
	else
	{
		lua_pushboolean(L, false);
		return 1;
	}
}

// Push a value to channel. Types are limited to nil, boolean, number, string.
int channel_put(lua_State* L)
{
	auto c = lua_checkblock<LuaChannel>(L, 1, "LuaChannel");

	int type;
	string value;
	if (!get_from_lua(L, 2, type, value))
	{
		return luaL_error(L, "channel_put: unsupported type %s", lua_typename(L, type));
	}

	{
		unique_lock<mutex> ulk(c->sp->m);
		c->sp->cond.wait(ulk, [&]() { return (c->sp->capacity > 0 && c->sp->bus.size() < c->sp->capacity) || (c->sp->capacity == 0 && c->sp->bus.empty()); });
		c->sp->bus.emplace(type, value);
		c->sp->cond.notify_all();
	}

	return 0;
}

int channel_try_put(lua_State* L)
{
	auto c = lua_checkblock<LuaChannel>(L, 1, "LuaChannel");
	int ms = luaL_checkinteger(L, 2);

	bool success;
	int type;
	string value;
	if (!get_from_lua(L, 3, type, value))
	{
		return luaL_error(L, "channel_try_put: unsupported type %s", lua_typename(L, type));
	}

	{
		unique_lock<mutex> ulk(c->sp->m);
		if (ms > 0)
		{
			success = c->sp->cond.wait_for(ulk, chrono::milliseconds(ms), [&]() { return (c->sp->capacity > 0 && c->sp->bus.size() < c->sp->capacity) || (c->sp->capacity == 0 && c->sp->bus.empty()); });
		}
		else
		{
			success = (c->sp->capacity > 0 && c->sp->bus.size() < c->sp->capacity) || (c->sp->capacity == 0 && c->sp->bus.empty());
		}

		if (success)
		{
			c->sp->bus.emplace(type, value);
			c->sp->cond.notify_all();
		}
	}

	lua_pushboolean(L, success);
	return 1;
}

int channel_wait(lua_State* L)
{
	auto c = lua_checkblock<LuaChannel>(L, 1, "LuaChannel");
	int ms = -1;
	if (!lua_isnone(L, 2))
	{
		ms = luaL_checkinteger(L, 2);
		if (ms < 0)
		{
			ms = 0;
		}
	}

	bool hasValue;

	{
		unique_lock<mutex> ulk(c->sp->m);
		if (ms > 0)
		{
			hasValue = c->sp->cond.wait_for(ulk, chrono::milliseconds(ms), [&]() { return !c->sp->bus.empty(); });
		}
		else if (ms == 0)
		{
			hasValue = !c->sp->bus.empty();
		}
		else
		{
			c->sp->cond.wait(ulk, [&]() { return !c->sp->bus.empty(); });
			hasValue = true;
		}
	}

	lua_pushboolean(L, hasValue);
	return 1;
}

// This will push a LuaChannel userdata onto the lvm stack.
void put_channel(lua_State* L, const shared_ptr<Channel>& spChan)
{
	auto c = new (lua_newblock<LuaChannel>(L)) LuaChannel;
	c->sp = spChan;
	printf("LuaChannel %p pushed, underlying channel: %p\n", c, c->sp.get());

	if (luaL_newmetatable(L, "LuaChannel"))
	{
		lua_setfield_function(L, "__gc", channel_dtor);
		lua_newtable(L);
		lua_setfield_function(L, "get", channel_get);
		lua_setfield_function(L, "put", channel_put);
		lua_setfield_function(L, "try_get", channel_try_get);
		lua_setfield_function(L, "try_put", channel_try_put);
		lua_setfield_function(L, "wait", channel_wait);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}

// Global Variables for managing LuaChannel openning and creating
mutex globalChannelNSLock;
map<string, weak_ptr<Channel>> globalChannelNS;

int channel_open(lua_State* L)
{
	size_t namelen;
	const char* name = luaL_checklstring(L, 1, &namelen);
	string channelName(name, namelen);

	{
		unique_lock<mutex> ulk(globalChannelNSLock);
		auto iter = globalChannelNS.find(channelName);
		if (iter == globalChannelNS.end())
		{
			// not found.
			lua_pushnil(L);
			return 1;
		}
		else
		{
			auto spChan = iter->second.lock();
			if (spChan)
			{
				printf("channel %p(%s) opened.\n", spChan.get(), iter->first.c_str());
				put_channel(L, spChan);
				return 1;
			}
			else
			{
				printf("channel %s expired.\n", iter->first.c_str());
				globalChannelNS.erase(iter);
				// found but expired.
				lua_pushnil(L);
				return 1;
			}
		}
	}
}

int channel_create(lua_State* L)
{
	size_t namelen;
	const char* name = luaL_checklstring(L, 1, &namelen);
	string channelName(name, namelen);
	int cap = 0;
	if (lua_isinteger(L, 2))
	{
		cap = lua_tointeger(L, 2);
	}

	{
		unique_lock<mutex> ulk(globalChannelNSLock);
		auto iter = globalChannelNS.find(channelName);
		if (iter != globalChannelNS.end())
		{
			auto spChan = iter->second.lock();
			if (spChan)
			{
				// found.
				printf("channel %p(%s) skip create and opened.\n", spChan.get(), iter->first.c_str());

				put_channel(L, spChan);
				lua_pushboolean(L, 0);
				return 2;
			}
			else
			{
				// found but expired.
				printf("channel %s expired.\n", iter->first.c_str());
				globalChannelNS.erase(iter);
			}
		}

		// not found, create one
		shared_ptr<Channel> spChan(new Channel(cap));
		put_channel(L, spChan);
		lua_pushboolean(L, 1);
		globalChannelNS.emplace(channelName, spChan);

		printf("channel %p(%s) created.\n", spChan.get(), channelName.c_str());

		return 2;
	}
}

int channel_collect(lua_State* L)
{
	unique_lock<mutex> ulk(globalChannelNSLock);
	for (auto iter = globalChannelNS.begin(); iter != globalChannelNS.end(); )
	{
		auto sp = iter->second.lock();
		if (!sp)
		{
			// expired.
			iter = globalChannelNS.erase(iter);
		}
		else
		{
			++iter;
		}
	}
	return 0;
}

void InitChannel(lua_State* L)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaded");
	lua_newtable(L);
	lua_setfield_function(L, "create", channel_create);
	lua_setfield_function(L, "open", channel_open);
	lua_setfield_function(L, "collect", channel_collect);
	lua_setfield(L, -2, "Channel");
	lua_pop(L, 2);
}
