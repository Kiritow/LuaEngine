#include "include.h"
#include <cstring>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>
#include <atomic>
#include <map>
#include <variant>
using namespace std;

/**
module Channel
	create(name: string): Channel, isCreated: boolean
	open(name: string): Channel or nil
		Opening non-exist channels return nil.
	collect()

class Channel
	ChannelDataTypes: nil, integer, number, boolean, string, SurfaceObject

	put(value: ChannelDataTypes)
	get(): ChannelDataTypes

	try_put(value: ChannelDataTypes, timeout_ms: int): isSuccess: boolean
	try_get(timeout_ms: int): isSuccess: boolean, value: ChannelDataTypes

	wait([timeout_ms: int]): hasValue: boolean
		Block until has value if no timeout specified. Return false on reaching timeout.
		Negative timeout is regarded as 0.
		Zero timeout means instant check-and-return.
*/

template<typename ResourceType>
static ShareableResource<ResourceType>* lua_tofullres(lua_State* L, int index)
{
	return (ShareableResource<ResourceType>*)lua_touserdata(L, index);
}

enum class ChannelDataType
{
	SUBTYPE_NONE = 0,
	SUBTYPE_NIL,
	SUBTYPE_INTEGER,
	SUBTYPE_NUMBER,
	SUBTYPE_STRING,
	SUBTYPE_BOOLEAN,
	SUBTYPE_ENGINE_SURFACE
};

struct ChannelPackage
{
public:
	ChannelDataType subtype;  // LuaEngine specified types.
	string subname; // LUA_TUSERDATA
	variant<bool, lua_Integer, lua_Number, string, shared_ptr<SDL_Surface>> value;

	ChannelPackage() : subtype(ChannelDataType::SUBTYPE_NONE)
	{

	}
};

class Channel
{
public:
	int capacity;
	queue<ChannelPackage> bus;
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

static bool push_to_lua(lua_State* L, const ChannelPackage& pack)
{
	switch (pack.subtype)
	{
	case ChannelDataType::SUBTYPE_NIL:
		lua_pushnil(L);
		return true;
	case ChannelDataType::SUBTYPE_INTEGER:
		lua_pushinteger(L, get<lua_Integer>(pack.value));
		return true;
	case ChannelDataType::SUBTYPE_NUMBER:
		lua_pushnumber(L, get<lua_Number>(pack.value));
		return true;
	case ChannelDataType::SUBTYPE_BOOLEAN:
		lua_pushboolean(L, get<bool>(pack.value));
		return true;
	case ChannelDataType::SUBTYPE_STRING:
		lua_pushlstring(L, get<string>(pack.value).data(), get<string>(pack.value).size());
		return true;
	case ChannelDataType::SUBTYPE_ENGINE_SURFACE:
		put_surface(L, get<shared_ptr<SDL_Surface>>(pack.value));
		return true;
	default:
		return false;
	}
}

static bool get_from_lua(lua_State* L, int index, ChannelPackage& pack)
{
	int type = lua_type(L, index);
	switch (type)
	{
	case LUA_TNIL:
		pack.subtype = ChannelDataType::SUBTYPE_NIL;
		return true;
	case LUA_TNUMBER:
	{
		int isnum;
		lua_Integer val = lua_tointegerx(L, index, &isnum);
		if (isnum)
		{
			pack.subtype = ChannelDataType::SUBTYPE_INTEGER;
			pack.value = val;
		}
		else
		{
			pack.subtype = ChannelDataType::SUBTYPE_NUMBER;
			pack.value = lua_tonumber(L, index);
		}
		return true;
	}
	case LUA_TBOOLEAN:
		pack.subtype = ChannelDataType::SUBTYPE_BOOLEAN;
		pack.value = (bool)lua_toboolean(L, index);
		return true;
	case LUA_TSTRING:
	{
		size_t sz;
		const char* p = lua_tolstring(L, index, &sz);
		pack.subtype = ChannelDataType::SUBTYPE_STRING;
		pack.value =  string(p, sz);
		return true;
	}
	case LUA_TUSERDATA:
	{
		bool ok = false;
		if (lua_getmetatable(L, index))
		{
			int mtype = lua_getfield(L, -1, "__name");
			if (mtype == LUA_TSTRING)
			{
				const char* tname = lua_tostring(L, -1);
				if (strcmp(tname, "LuaEngineSurface") == 0)
				{
					ok = true;
					auto pres = lua_tofullres<SDL_Surface>(L, index);
					pres->enable_share();
					pack.subtype = ChannelDataType::SUBTYPE_ENGINE_SURFACE;
					pack.value = pres->sp;
				}
			}
			lua_pop(L, 2);  // getfield, metatable.
		}
		return ok;
	}
	default:
		return false;
	}
}

// Get a value from channel
int channel_get(lua_State* L)
{
	auto c = lua_checkblock<LuaChannel>(L, 1, "LuaChannel");

	ChannelPackage pack;

	{
		unique_lock<mutex> ulk(c->sp->m);
		c->sp->cond.wait(ulk, [&]() { return !c->sp->bus.empty(); });
		// Un-serialize from string to Lua object

		pack = c->sp->bus.front();
		c->sp->bus.pop();
		c->sp->cond.notify_all();
	}

	if (!push_to_lua(L, pack))
	{
		SDL_Log("channel_get: invalid package get from channel. Returning none instead.\n");
		return 0;
	}

	return 1;
}

int channel_try_get(lua_State* L)
{
	auto c = lua_checkblock<LuaChannel>(L, 1, "LuaChannel");
	int ms = luaL_checkinteger(L, 2);

	bool success;
	ChannelPackage pack;

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
			pack = c->sp->bus.front();
			c->sp->bus.pop();
			c->sp->cond.notify_all();
		}
	}

	if (success)
	{
		lua_pushboolean(L, true);
		if (!push_to_lua(L, pack))
		{
			SDL_Log("channel_try_get: invalid package get from channel. Returning none instead.\n");
			return 1;
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

	ChannelPackage pack;
	if (!get_from_lua(L, 2, pack))
	{
		return luaL_error(L, "channel_put: unsupported type %s", lua_typename(L, lua_type(L, 2)));
	}

	{
		unique_lock<mutex> ulk(c->sp->m);
		c->sp->cond.wait(ulk, [&]() { return (c->sp->capacity > 0 && c->sp->bus.size() < c->sp->capacity) || (c->sp->capacity == 0 && c->sp->bus.empty()); });
		c->sp->bus.push(pack);
		c->sp->cond.notify_all();
	}

	return 0;
}

int channel_try_put(lua_State* L)
{
	auto c = lua_checkblock<LuaChannel>(L, 1, "LuaChannel");
	int ms = luaL_checkinteger(L, 2);

	bool success;
	ChannelPackage pack;
	if (!get_from_lua(L, 3, pack))
	{
		return luaL_error(L, "channel_try_put: unsupported type %s", lua_typename(L, lua_type(L, 3)));
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
			c->sp->bus.push(pack);
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
