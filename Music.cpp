#include "include.h"

/*
module Music
	load(filename: string): MusicMedia
	loadChunk(filename: string): ChunkMedia
	play(music: MusicMedia, [loops: int])
	pause()
	resume()
*/

int media_close(lua_State* L)
{
	auto music = lua_checkpointer<Mix_Music>(L, 1, "LuaEngineMusicMedia");
	Mix_FreeMusic(music);
	return 0;
}

void put_media(lua_State* L, Mix_Music* music)
{
	lua_newpointer(L, music);
	if (luaL_newmetatable(L, "LuaEngineMusicMedia"))
	{
		lua_setfield_function(L, "__gc", media_close);
		lua_newtable(L);

		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}

int chunk_close(lua_State* L)
{
	auto chunk = lua_checkpointer<Mix_Music>(L, 1, "LuaEngineChunkMedia");
	Mix_FreeMusic(chunk);
	return 0;
}

void put_chunk(lua_State* L, Mix_Chunk* chunk)
{
	lua_newpointer(L, chunk);
	if (luaL_newmetatable(L, "LuaEngineChunkMedia"))
	{
		lua_setfield_function(L, "__gc", chunk_close);
		lua_newtable(L);

		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}

int music_load(lua_State* L)
{
	const char* filename = luaL_checkstring(L, 1);
	Mix_Music* music = Mix_LoadMUS(filename);
	if (!music)
	{
		return MixError(L, Mix_LoadMUS);
	}
	put_media(L, music);
	return 1;
}

int music_loadchunk(lua_State* L)
{
	const char* filename = luaL_checkstring(L, 1);
	Mix_Chunk* chunk = Mix_LoadWAV(filename);
	if (!chunk)
	{
		return MixError(L, Mix_LoadWAV);
	}
	put_chunk(L, chunk);
	return 1;
}

int music_play(lua_State* L)
{
	auto music = lua_checkpointer<Mix_Music>(L, 1, "LuaEngineMusicMedia");
	int loops = -1;
	if (!lua_isnone(L, 2))
	{
		loops = luaL_checkinteger(L, 2);
	}
	if (Mix_PlayMusic(music, loops) != 0)
	{
		return MixError(L, Mix_PlayMusic);
	}
	return 0;
}

int music_pause(lua_State* L)
{
	Mix_PauseMusic();
	return 0;
}

int music_resume(lua_State* L)
{
	Mix_ResumeMusic();
	return 0;
}

void InitMusic(lua_State* L)
{
	lua_newtable(L);
	lua_setfield_function(L, "load", music_load);
	lua_setfield_function(L, "loadChunk", music_loadchunk);
	lua_setfield_function(L, "play", music_play);
	lua_setfield_function(L, "pause", music_pause);
	lua_setfield_function(L, "resume", music_resume);
	lua_setglobal(L, "Music");
}
