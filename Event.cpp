#include "include.h"

/*
Event
	quit timestamp
	mousemove x y xrel yrel button_state which windowID timestamp
	mousedown/mouseup button x y clicks press_state which windowID timestamp
	keydown/keyup vkey repeat press_state windowID timestamp
	fingermove/fingerdown/fingerup x y dx dy pressure fingerId touchId timestamp

	windowshown windowID timestamp
	windowhidden windowID timestamp
	windowexposed windowID timestamp
	windowmoved windowID x y timestamp
	windowresized windowID w h timestmap
	windowsizechanged windowID w h timestamp
	windowminimized windowID timestamp
	windowmaximized windowID timestamp
	windowrestored windowID timestamp
	windowenter windowID timestamp
	windowleave windowID timestamp
	windowgainfocus windowID timestamp
	windowlostfocus windowID timestamp
	windowtakefocus windowID timestamp
	windowclose windowID timestamp
	windowhittest windowID timestamp
module event
	wait(): EventType, ...
	poll(): EventType or nil, ...
	push(name: string, ...)  # no implemented
*/

const char* VirtualKeyToString(SDL_Keycode vkey)
{
	switch (vkey)
	{
	case SDLK_0: return "0";
	case SDLK_1: return "1";
	case SDLK_2: return "2";
	case SDLK_3: return "3";
	case SDLK_4: return "4";
	case SDLK_5: return "5";
	case SDLK_6: return "6";
	case SDLK_7: return "7";
	case SDLK_8: return "8";
	case SDLK_9: return "9";
	case SDLK_a: return "a";
	case SDLK_AC_BACK: return "back";
	case SDLK_AC_BOOKMARKS: return "bookmarks";
	case SDLK_AC_FORWARD: return "forward";
	case SDLK_AC_HOME: return "home";
	case SDLK_AC_REFRESH: return "refresh";
	case SDLK_AC_SEARCH: return "search";
	case SDLK_AC_STOP: return "stop";
	case SDLK_AGAIN: return "again";
	case SDLK_ALTERASE: return "alterase";
	case SDLK_QUOTE: return "quote";
	case SDLK_APPLICATION: return "application";
	case SDLK_AUDIOMUTE: return "audiomute";
	case SDLK_AUDIONEXT: return "audionext";
	case SDLK_AUDIOPLAY: return "audioplay";
	case SDLK_AUDIOPREV: return "audioprev";
	case SDLK_AUDIOSTOP: return "audiostop";
	case SDLK_b: return "b";
	case SDLK_BACKSLASH: return "backslash";
	case SDLK_BACKSPACE: return "backspace";
	case SDLK_BRIGHTNESSDOWN: return "brightnessdown";
	case SDLK_BRIGHTNESSUP: return "brightnessup";
	case SDLK_c: return "c";
	case SDLK_CALCULATOR: return "calculator";
	case SDLK_CANCEL: return "cancel";
	case SDLK_CAPSLOCK: return "capslock";
	case SDLK_CLEAR: return "clear";
	case SDLK_CLEARAGAIN: return "clearagain";
	case SDLK_COMMA: return "comma";
	case SDLK_COMPUTER: return "computer";
	case SDLK_COPY: return "copy";
	case SDLK_CRSEL: return "crsel";
	case SDLK_CURRENCYSUBUNIT: return "currencysubunit";
	case SDLK_CURRENCYUNIT: return "currencyunit";
	case SDLK_CUT: return "cut";
	case SDLK_d: return "d";
	case SDLK_DECIMALSEPARATOR: return "decimalseparator";
	case SDLK_DELETE: return "delete";
	case SDLK_DISPLAYSWITCH: return "displayswitch";
	case SDLK_DOWN: return "down";
	case SDLK_e: return "e";
	case SDLK_EJECT: return "eject";
	case SDLK_END: return "end";
	case SDLK_EQUALS: return "equals";
	case SDLK_ESCAPE: return "escape";
	case SDLK_EXECUTE: return "execute";
	case SDLK_EXSEL: return "exsel";
	case SDLK_f: return "f";
	case SDLK_F1: return "f1";
	case SDLK_F10: return "f10";
	case SDLK_F11: return "f11";
	case SDLK_F12: return "f12";
	case SDLK_F13: return "f13";
	case SDLK_F14: return "f14";
	case SDLK_F15: return "f15";
	case SDLK_F16: return "f16";
	case SDLK_F17: return "f17";
	case SDLK_F18: return "f18";
	case SDLK_F19: return "f19";
	case SDLK_F2: return "f2";
	case SDLK_F20: return "f20";
	case SDLK_F21: return "f21";
	case SDLK_F22: return "f22";
	case SDLK_F23: return "f23";
	case SDLK_F24: return "f24";
	case SDLK_F3: return "f3";
	case SDLK_F4: return "f4";
	case SDLK_F5: return "f5";
	case SDLK_F6: return "f6";
	case SDLK_F7: return "f7";
	case SDLK_F8: return "f8";
	case SDLK_F9: return "f9";
	case SDLK_FIND: return "find";
	case SDLK_g: return "g";
	case SDLK_BACKQUOTE: return "backquote";
	case SDLK_h: return "h";
	case SDLK_HELP: return "help";
	case SDLK_HOME: return "home";
	case SDLK_i: return "i";
	case SDLK_INSERT: return "insert";
	case SDLK_j: return "j";
	case SDLK_k: return "k";
	case SDLK_KBDILLUMDOWN: return "kbdillumdown";
	case SDLK_KBDILLUMTOGGLE: return "kbdillumtoggle";
	case SDLK_KBDILLUMUP: return "kbdillumup";
	case SDLK_KP_0: return "0";
	case SDLK_KP_00: return "00";
	case SDLK_KP_000: return "000";
	case SDLK_KP_1: return "1";
	case SDLK_KP_2: return "2";
	case SDLK_KP_3: return "3";
	case SDLK_KP_4: return "4";
	case SDLK_KP_5: return "5";
	case SDLK_KP_6: return "6";
	case SDLK_KP_7: return "7";
	case SDLK_KP_8: return "8";
	case SDLK_KP_9: return "9";
	case SDLK_KP_A: return "a";
	case SDLK_KP_AMPERSAND: return "ampersand";
	case SDLK_KP_AT: return "at";
	case SDLK_KP_B: return "b";
	case SDLK_KP_BACKSPACE: return "backspace";
	case SDLK_KP_BINARY: return "binary";
	case SDLK_KP_C: return "c";
	case SDLK_KP_CLEAR: return "clear";
	case SDLK_KP_CLEARENTRY: return "clearentry";
	case SDLK_KP_COLON: return "colon";
	case SDLK_KP_COMMA: return "comma";
	case SDLK_KP_D: return "d";
	case SDLK_KP_DBLAMPERSAND: return "dblampersand";
	case SDLK_KP_DBLVERTICALBAR: return "dblverticalbar";
	case SDLK_KP_DECIMAL: return "decimal";
	case SDLK_KP_DIVIDE: return "divide";
	case SDLK_KP_E: return "e";
	case SDLK_KP_ENTER: return "enter";
	case SDLK_KP_EQUALS: return "equals";
	case SDLK_KP_EQUALSAS400: return "equalsas400";
	case SDLK_KP_EXCLAM: return "exclam";
	case SDLK_KP_F: return "f";
	case SDLK_KP_GREATER: return "greater";
	case SDLK_KP_HASH: return "hash";
	case SDLK_KP_HEXADECIMAL: return "hexadecimal";
	case SDLK_KP_LEFTBRACE: return "leftbrace";
	case SDLK_KP_LEFTPAREN: return "leftparen";
	case SDLK_KP_LESS: return "less";
	case SDLK_KP_MEMADD: return "memadd";
	case SDLK_KP_MEMCLEAR: return "memclear";
	case SDLK_KP_MEMDIVIDE: return "memdivide";
	case SDLK_KP_MEMMULTIPLY: return "memmultiply";
	case SDLK_KP_MEMRECALL: return "memrecall";
	case SDLK_KP_MEMSTORE: return "memstore";
	case SDLK_KP_MEMSUBTRACT: return "memsubtract";
	case SDLK_KP_MINUS: return "minus";
	case SDLK_KP_MULTIPLY: return "multiply";
	case SDLK_KP_OCTAL: return "octal";
	case SDLK_KP_PERCENT: return "percent";
	case SDLK_KP_PERIOD: return "period";
	case SDLK_KP_PLUS: return "plus";
	case SDLK_KP_PLUSMINUS: return "plusminus";
	case SDLK_KP_POWER: return "power";
	case SDLK_KP_RIGHTBRACE: return "rightbrace";
	case SDLK_KP_RIGHTPAREN: return "rightparen";
	case SDLK_KP_SPACE: return "space";
	case SDLK_KP_TAB: return "tab";
	case SDLK_KP_VERTICALBAR: return "verticalbar";
	case SDLK_KP_XOR: return "xor";
	case SDLK_l: return "l";
	case SDLK_LALT: return "lalt";
	case SDLK_LCTRL: return "lctrl";
	case SDLK_LEFT: return "left";
	case SDLK_LEFTBRACKET: return "leftbracket";
	case SDLK_LGUI: return "lgui";
	case SDLK_LSHIFT: return "lshift";
	case SDLK_m: return "m";
	case SDLK_MAIL: return "mail";
	case SDLK_MEDIASELECT: return "mediaselect";
	case SDLK_MENU: return "menu";
	case SDLK_MINUS: return "minus";
	case SDLK_MODE: return "mode";
	case SDLK_MUTE: return "mute";
	case SDLK_n: return "n";
	case SDLK_NUMLOCKCLEAR: return "numlockclear";
	case SDLK_o: return "o";
	case SDLK_OPER: return "oper";
	case SDLK_OUT: return "out";
	case SDLK_p: return "p";
	case SDLK_PAGEDOWN: return "pagedown";
	case SDLK_PAGEUP: return "pageup";
	case SDLK_PASTE: return "paste";
	case SDLK_PAUSE: return "pause";
	case SDLK_PERIOD: return "period";
	case SDLK_POWER: return "power";
	case SDLK_PRINTSCREEN: return "printscreen";
	case SDLK_PRIOR: return "prior";
	case SDLK_q: return "q";
	case SDLK_r: return "r";
	case SDLK_RALT: return "ralt";
	case SDLK_RCTRL: return "rctrl";
	case SDLK_RETURN: return "return";
	case SDLK_RETURN2: return "return2";
	case SDLK_RGUI: return "rgui";
	case SDLK_RIGHT: return "right";
	case SDLK_RIGHTBRACKET: return "rightbracket";
	case SDLK_RSHIFT: return "rshift";
	case SDLK_s: return "s";
	case SDLK_SCROLLLOCK: return "scrolllock";
	case SDLK_SELECT: return "select";
	case SDLK_SEMICOLON: return "semicolon";
	case SDLK_SEPARATOR: return "separator";
	case SDLK_SLASH: return "slash";
	case SDLK_SLEEP: return "sleep";
	case SDLK_SPACE: return "space";
	case SDLK_STOP: return "stop";
	case SDLK_SYSREQ: return "sysreq";
	case SDLK_t: return "t";
	case SDLK_TAB: return "tab";
	case SDLK_THOUSANDSSEPARATOR: return "thousandsseparator";
	case SDLK_u: return "u";
	case SDLK_UNDO: return "undo";
	case SDLK_UNKNOWN: return "unknown";
	case SDLK_UP: return "up";
	case SDLK_v: return "v";
	case SDLK_VOLUMEDOWN: return "volumedown";
	case SDLK_VOLUMEUP: return "volumeup";
	case SDLK_w: return "w";
	case SDLK_WWW: return "www";
	case SDLK_x: return "x";
	case SDLK_y: return "y";
	case SDLK_z: return "z";
	case SDLK_AMPERSAND: return "ampersand";
	case SDLK_ASTERISK: return "asterisk";
	case SDLK_AT: return "at";
	case SDLK_CARET: return "caret";
	case SDLK_COLON: return "colon";
	case SDLK_DOLLAR: return "dollar";
	case SDLK_EXCLAIM: return "exclaim";
	case SDLK_GREATER: return "greater";
	case SDLK_HASH: return "hash";
	case SDLK_LEFTPAREN: return "leftparen";
	case SDLK_LESS: return "less";
	case SDLK_PERCENT: return "percent";
	case SDLK_PLUS: return "plus";
	case SDLK_QUESTION: return "question";
	case SDLK_QUOTEDBL: return "quotedbl";
	case SDLK_RIGHTPAREN: return "rightparen";
	case SDLK_UNDERSCORE: return "underscore";
	default: return "unknown";
	}
}

int unpack_event(lua_State* L, const SDL_Event& e)
{
	switch (e.type)
	{
	case SDL_QUIT:
	{
		lua_pushstring(L, "quit");
		lua_pushinteger(L, e.quit.timestamp);
		return 2;
	}
	case SDL_MOUSEMOTION:
	{
		lua_pushstring(L, "mousemove");
		lua_pushinteger(L, e.motion.x);
		lua_pushinteger(L, e.motion.y);
		lua_pushinteger(L, e.motion.xrel);
		lua_pushinteger(L, e.motion.yrel);
		int c = 0;
		char buff[128] = { 0 };
		if (e.motion.state & SDL_BUTTON_LMASK)
		{
			if(c) strcat(buff, " left");
			else
			{
				c = 1;
				strcat(buff, "left");
			}
		}
		if (e.motion.state & SDL_BUTTON_MMASK)
		{
			if (c) strcat(buff, " middle");
			else
			{
				c = 1;
				strcat(buff, "middle");
			}
		}
		if (e.motion.state & SDL_BUTTON_RMASK)
		{
			if (c) strcat(buff, " right");
			else
			{
				c = 1;
				strcat(buff, "right");
			}
		}
		if (e.motion.state & SDL_BUTTON_X1MASK)
		{
			if (c) strcat(buff, " x1");
			else
			{
				c = 1;
				strcat(buff, "x1");
			}
		}
		if (e.motion.state & SDL_BUTTON_X2MASK)
		{
			if (c) strcat(buff, " x2");
			else
			{
				c = 1;
				strcat(buff, "x2");
			}
		}
		lua_pushstring(L, buff);
		lua_pushinteger(L, e.motion.which);
		lua_pushinteger(L, e.motion.windowID);
		lua_pushinteger(L, e.motion.timestamp);
		return 9;
	}
	case SDL_MOUSEBUTTONUP:
	case SDL_MOUSEBUTTONDOWN:
	{
		if (e.type == SDL_MOUSEBUTTONUP) lua_pushstring(L, "mouseup");
		else lua_pushstring(L, "mousedown");

		switch (e.button.button)
		{
		case SDL_BUTTON_LEFT:
			lua_pushstring(L, "left");
			break;
		case SDL_BUTTON_MIDDLE:
			lua_pushstring(L, "middle");
			break;
		case SDL_BUTTON_RIGHT:
			lua_pushstring(L, "right");
			break;
		case SDL_BUTTON_X1:
			lua_pushstring(L, "x1");
			break;
		case SDL_BUTTON_X2:
			lua_pushstring(L, "x2");
			break;
		default:
			lua_pushstring(L, "unknown");
		}
		lua_pushinteger(L, e.button.x);
		lua_pushinteger(L, e.button.y);
		lua_pushinteger(L, e.button.clicks);
		switch (e.button.state)
		{
		case SDL_PRESSED:
			lua_pushstring(L, "pressed");
			break;
		case SDL_RELEASED:
			lua_pushstring(L, "released");
			break;
		default:
			lua_pushstring(L, "default");
		}

		lua_pushinteger(L, e.button.which);
		lua_pushinteger(L, e.button.windowID);
		lua_pushinteger(L, e.button.timestamp);

		return 9;
	}
	case SDL_KEYUP:
	case SDL_KEYDOWN:
	{
		if (e.type == SDL_KEYUP) lua_pushstring(L, "keyup");
		else lua_pushstring(L, "keydown");

		lua_pushstring(L, VirtualKeyToString(e.key.keysym.sym));
		lua_pushnumber(L, e.key.repeat);
		switch (e.key.state)
		{
		case SDL_PRESSED:
			lua_pushstring(L, "pressed");
			break;
		case SDL_RELEASED:
			lua_pushstring(L, "released");
			break;
		default:
			lua_pushstring(L, "default");
		}
		lua_pushinteger(L, e.key.windowID);
		lua_pushinteger(L, e.key.timestamp);
		return 6;
	}
	case SDL_FINGERUP:
	case SDL_FINGERDOWN:
	case SDL_FINGERMOTION:
	{
		if (e.type == SDL_FINGERUP) lua_pushstring(L, "fingerup");
		else if (e.type == SDL_FINGERDOWN) lua_pushstring(L, "fingerdown");
		else lua_pushstring(L, "fingermove");

		lua_pushnumber(L, e.tfinger.x);
		lua_pushnumber(L, e.tfinger.y);
		lua_pushnumber(L, e.tfinger.dx);
		lua_pushnumber(L, e.tfinger.dy);
		lua_pushnumber(L, e.tfinger.pressure);
		lua_pushinteger(L, e.tfinger.fingerId);
		lua_pushinteger(L, e.tfinger.touchId);
		lua_pushinteger(L, e.tfinger.timestamp);
		return 9;
	}
	case SDL_WINDOWEVENT:
	{
		switch (e.window.event)
		{
		case SDL_WINDOWEVENT_SHOWN:
			lua_pushstring(L, "windowshown");
			lua_pushinteger(L, e.window.windowID);
			lua_pushinteger(L, e.window.timestamp);
			return 3;
		case SDL_WINDOWEVENT_HIDDEN:
			lua_pushstring(L, "windowhidden");
			lua_pushinteger(L, e.window.windowID);
			lua_pushinteger(L, e.window.timestamp);
			return 3;
		case SDL_WINDOWEVENT_EXPOSED:
			lua_pushstring(L, "windowexposed");
			lua_pushinteger(L, e.window.windowID);
			lua_pushinteger(L, e.window.timestamp);
			return 3;
		case SDL_WINDOWEVENT_MOVED:
			lua_pushstring(L, "windowmoved");
			lua_pushinteger(L, e.window.windowID);
			lua_pushinteger(L, e.window.timestamp);
			lua_pushinteger(L, e.window.data1);
			lua_pushinteger(L, e.window.data2);
			return 5;
		case SDL_WINDOWEVENT_RESIZED:
			lua_pushstring(L, "windowresized");
			lua_pushinteger(L, e.window.windowID);
			lua_pushinteger(L, e.window.timestamp);
			lua_pushinteger(L, e.window.data1);
			lua_pushinteger(L, e.window.data2);
			return 5;
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			lua_pushstring(L, "windowsizechanged");
			lua_pushinteger(L, e.window.windowID);
			lua_pushinteger(L, e.window.timestamp);
			lua_pushinteger(L, e.window.data1);
			lua_pushinteger(L, e.window.data2);
			return 5;
		case SDL_WINDOWEVENT_MINIMIZED:
			lua_pushstring(L, "windowminimized");
			lua_pushinteger(L, e.window.windowID);
			lua_pushinteger(L, e.window.timestamp);
			return 3;
		case SDL_WINDOWEVENT_MAXIMIZED:
			lua_pushstring(L, "windowmaximized");
			lua_pushinteger(L, e.window.windowID);
			lua_pushinteger(L, e.window.timestamp);
			return 3;
		case SDL_WINDOWEVENT_RESTORED:
			lua_pushstring(L, "windowrestored");
			lua_pushinteger(L, e.window.windowID);
			lua_pushinteger(L, e.window.timestamp);
			return 3;
		case SDL_WINDOWEVENT_ENTER:
			lua_pushstring(L, "windowenter");
			lua_pushinteger(L, e.window.windowID);
			lua_pushinteger(L, e.window.timestamp);
			return 3;
		case SDL_WINDOWEVENT_LEAVE:
			lua_pushstring(L, "windowleave");
			lua_pushinteger(L, e.window.windowID);
			lua_pushinteger(L, e.window.timestamp);
			return 3;
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			lua_pushstring(L, "windowgainfocus");
			lua_pushinteger(L, e.window.windowID);
			lua_pushinteger(L, e.window.timestamp);
			return 3;
		case SDL_WINDOWEVENT_FOCUS_LOST:
			lua_pushstring(L, "windowlostfocus");
			lua_pushinteger(L, e.window.windowID);
			lua_pushinteger(L, e.window.timestamp);
			return 3;
		case SDL_WINDOWEVENT_TAKE_FOCUS:
			lua_pushstring(L, "windowtakefocus");
			lua_pushinteger(L, e.window.windowID);
			lua_pushinteger(L, e.window.timestamp);
			return 3;
		case SDL_WINDOWEVENT_CLOSE:
			lua_pushstring(L, "windowclose");
			lua_pushinteger(L, e.window.windowID);
			lua_pushinteger(L, e.window.timestamp);
			return 3;
		case SDL_WINDOWEVENT_HIT_TEST:
			lua_pushstring(L, "windowhittest");
			lua_pushinteger(L, e.window.windowID);
			lua_pushinteger(L, e.window.timestamp);
			return 3;
		default:
			lua_pushstring(L, "unknown");
			lua_pushinteger(L, e.type);
			lua_pushinteger(L, e.window.event);
			return 3;
		}
	}
	default:
		lua_pushstring(L, "unknown");
		lua_pushinteger(L, e.type);
		return 2;
	}
}

int event_wait(lua_State* L)
{
	SDL_Event e;
	if (!SDL_WaitEvent(&e))
	{
		return SDLError(L, SDL_WaitEvent);
	}
	return unpack_event(L, e);
}

int event_poll(lua_State* L)
{
	SDL_Event e;
	if (SDL_PollEvent(&e))
	{
		return unpack_event(L, e);
	}
	else return 0;
}

void InitEvent(lua_State* L)
{
	lua_newtable(L);
	lua_setfield_function(L, "wait", event_wait);
	lua_setfield_function(L, "poll", event_poll);
	lua_setglobal(L, "Event");
}
