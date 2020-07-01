#include "include.h"
#include <cstdlib>
#include <ctime>

void InitLuaEngine(lua_State* L)
{
	InitEvent(L);
	InitWindow(L);
	InitRenderer(L);
	InitFont(L);
	InitMusic(L);
	InitCommon(L);
	InitTCPSocket(L);
	InitUDPSocket(L);
	InitNetwork(L);
	InitSocketSelector(L);
	InitThread(L);
	InitChannel(L);
	InitSurface(L);
}

lua_State* CreateLuaEngine()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	InitLuaEngine(L);
	return L;
}

void InitEngine()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	TTF_Init();
	Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) 
	{
		SDL_Log("Mix_OpenAudio: %s\n", Mix_GetError());
		exit(2);
	}
	Mix_AllocateChannels(16);
	// Stop text input event
	SDL_StopTextInput();

	PlatInit();
	srand(time(NULL));
}

void CloseEngine()
{
	Mix_CloseAudio();
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}
