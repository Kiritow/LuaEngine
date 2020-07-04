#pragma once
#include "LuaEngine.h"
#include <atomic>

#define SDLError(L, prompt) luaL_error(L, #prompt ": %s", SDL_GetError())
#define TTFError(L, prompt) luaL_error(L, #prompt ": %s", TTF_GetError())
#define IMGError(L, prompt) luaL_error(L, #prompt ": %s", IMG_GetError())
#define MixError(L, prompt) luaL_error(L, #prompt ": %s", Mix_GetError())

#define lua_setfield_function(L, name, func) lua_pushcfunction(L, func);lua_setfield(L, -2, name)

template<typename PointerType>
void lua_newpointer(lua_State* L, PointerType* p)
{
	auto ptr = (PointerType**)lua_newuserdata(L, sizeof(PointerType*));
	*ptr = p;
}

template<typename PointerType>
PointerType* lua_checkpointer(lua_State* L, int idx, const char* name)
{
	return *(PointerType**)luaL_checkudata(L, idx, name);
}

template<typename ResourceType>
class ShareableResource
{
public:
	std::atomic<bool> isShared;
	ResourceType* ptr;
	void (*fnDeleter)(ResourceType*);
	std::shared_ptr<ResourceType> sp;

	ShareableResource(ResourceType* in, void (*delfn)(ResourceType*)) : isShared(false), ptr(in), fnDeleter(delfn)
	{

	}

	ShareableResource(const std::shared_ptr<ResourceType>& in) : isShared(true), ptr(nullptr), fnDeleter(nullptr), sp(in)
	{

	}

	~ShareableResource()
	{
		if (ptr)
		{
			fnDeleter(ptr);
			ptr = nullptr;
		}
	}

	void enable_share()
	{
		bool b = false;
		if (isShared.compare_exchange_strong(b, true))
		{
			sp.reset(ptr, fnDeleter);
			ptr = nullptr;
		}
	}

	ResourceType* get()
	{
		if (!isShared) return ptr;
		else return sp.get();
	}
};

template<typename ResourceType>
void lua_newres(lua_State* L, const std::shared_ptr<ResourceType>& sp)
{
	new (lua_newuserdata(L, sizeof(ShareableResource<ResourceType>))) ShareableResource<ResourceType>(sp);
}

template<typename ResourceType>
void lua_newres(lua_State* L, ResourceType* p, void(*delfn)(ResourceType*))
{
	new (lua_newuserdata(L, sizeof(ShareableResource<ResourceType>))) ShareableResource<ResourceType>(p, delfn);
}

template<typename ResourceType>
ResourceType* lua_checkres(lua_State* L, int idx, const char* name)
{
	auto pres = (ShareableResource<ResourceType>*)luaL_checkudata(L, idx, name);
	return pres->get();
}

template<typename ResourceType>
ShareableResource<ResourceType>* lua_checkfullres(lua_State* L, int idx, const char* name)
{
	return (ShareableResource<ResourceType>*)luaL_checkudata(L, idx, name);
}

template<typename T>
T* lua_newblock(lua_State* L)
{
	return (T*)lua_newuserdata(L, sizeof(T));
}

template<typename T>
T* lua_checkblock(lua_State* L, int idx, const char* name)
{
	return (T*)luaL_checkudata(L, idx, name);
}

template<typename T>
T* lua_testblock(lua_State* L, int idx, const char* name)
{
	return (T*)luaL_testudata(L, idx, name);
}

// Shared Functions
// Surface
void put_surface(lua_State* L, SDL_Surface* surf);
void put_surface(lua_State* L, const std::shared_ptr<SDL_Surface>& surf);

void put_texture(lua_State* L, SDL_Texture* text);  // Texture
const char* VirtualKeyToString(SDL_Keycode vkey);  // Event

// Init Functions
void InitWindow(lua_State* L);
void InitRenderer(lua_State* L);
void InitSurface(lua_State* L);
void InitFont(lua_State* L);
void InitMusic(lua_State* L);
void InitEvent(lua_State* L);
void InitCommon(lua_State* L);
void InitTCPSocket(lua_State* L);
void InitUDPSocket(lua_State* L);
void InitNetwork(lua_State* L);
void InitThread(lua_State* L);
void InitChannel(lua_State* L);
void InitSocketSelector(lua_State* L);

void PlatInit();

#ifdef _WIN32
void put_winerror(lua_State* L, int errcode, const char* hint);

#else
void put_linuxerror(lua_State* L, int errcode, const char* hint);

#endif  // ifdef _WIN32

int lua_errno(lua_State* L, const char* hint);
