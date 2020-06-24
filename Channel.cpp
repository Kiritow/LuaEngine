#include "include.h"
#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>
#include <atomic>
#include <map>
using namespace std;

class Channel
{
public:
	int capacity;
	bool closed;
	queue<tuple<int, string>> bus;
	mutex m;
	condition_variable cond;

	Channel(int cap) : capacity(cap), closed(false)
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

// Get a value from channel
int channel_get(lua_State* L)
{
	auto c = lua_checkblock<LuaChannel>(L, 1, "LuaChannel");
	int type;
	string value;

	printf("Call get on LuaChannel %p\n", c);

	{
		unique_lock<mutex> ulk(c->sp->m);
		if (c->sp->closed)
		{
			lua_pushnil(L);
			lua_pushboolean(L, 1);
			return 2;
		}
		c->sp->cond.wait(ulk, [&]() { return !c->sp->bus.empty(); });
		// Un-serialize from string to Lua object

		tie(type, value) = c->sp->bus.front();
		c->sp->bus.pop();
		c->sp->cond.notify_all();
	}

	switch (type)
	{
	case LUA_TNIL:
		lua_pushnil(L);
		break;
	case LUA_TNUMBER:
		lua_stringtonumber(L, value.c_str());
		break;
	case LUA_TBOOLEAN:
		lua_pushboolean(L, value.empty());
		break;
	case LUA_TSTRING:
		lua_pushlstring(L, value.c_str(), value.size());
		break;
	default:
		return luaL_error(L, "channel_get: unsupported type %s", lua_typename(L, type));
	}

	return 1;
}

// Push a value to channel. Types are limited to nil, boolean, number, string.
int channel_put(lua_State* L)
{
	auto c = lua_checkblock<LuaChannel>(L, 1, "LuaChannel");
	int type = lua_type(L, 2);
	string value;
	switch (type)
	{
	case LUA_TNIL:
		break;
	case LUA_TNUMBER:
		value = lua_tostring(L, 2);
		break;
	case LUA_TBOOLEAN:
		if (lua_toboolean(L, 2))
		{
			value = "true";
		}
		break;
	case LUA_TSTRING:
		value = lua_tostring(L, 2);
		break;
	default:
		return luaL_error(L, "channel_put: unsupported type %s", lua_typename(L, type));
	}

	printf("Call put on LuaChannel %p\n", c);

	{
		unique_lock<mutex> ulk(c->sp->m);
		if (c->sp->closed)
		{
			return luaL_error(L, "channel_put: cannot put to closed channel.");
		}
		c->sp->cond.wait(ulk, [&]() { return (c->sp->capacity > 0 && c->sp->bus.size() < c->sp->capacity) || (c->sp->capacity == 0 && c->sp->bus.empty()); });
		c->sp->bus.emplace(type, value);
		c->sp->cond.notify_all();
	}

	return 0;
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
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}

// Global Variables for managing LuaChannel openning and creating
mutex gChannelNSLock;
map<string, weak_ptr<Channel>> gChannelNS;

int channel_open(lua_State* L)
{
	size_t namelen;
	const char* name = luaL_checklstring(L, 1, &namelen);
	string channelName(name, namelen);

	{
		unique_lock<mutex> ulk(gChannelNSLock);
		auto iter = gChannelNS.find(channelName);
		if (iter == gChannelNS.end())
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
				gChannelNS.erase(iter);
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
		unique_lock<mutex> ulk(gChannelNSLock);
		auto iter = gChannelNS.find(channelName);
		if (iter != gChannelNS.end())
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
				gChannelNS.erase(iter);
			}
		}

		// not found, create one
		shared_ptr<Channel> spChan(new Channel(cap));
		put_channel(L, spChan);
		lua_pushboolean(L, 1);
		gChannelNS.emplace(channelName, spChan);

		printf("channel %p(%s) created.\n", spChan.get(), channelName.c_str());

		return 2;
	}
}

int channel_collect(lua_State* L)
{
	unique_lock<mutex> ulk(gChannelNSLock);
	for (auto iter = gChannelNS.begin(); iter != gChannelNS.end(); )
	{
		auto sp = iter->second.lock();
		if (!sp)
		{
			// expired.
			iter = gChannelNS.erase(iter);
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
