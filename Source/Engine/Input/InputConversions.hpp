#pragma once

#include "InputEvents.hpp"

#include <SDL_video.h>
#include <SDL_keycode.h>

static inline blWindowEvent blWindowEventFromSDL(uint8_t windowEvent)
{
    switch (windowEvent)
    {
    case SDL_WINDOWEVENT_MOVED:     return blWindowEvent::eMove;
    case SDL_WINDOWEVENT_RESIZED:   return blWindowEvent::eResize;
    case SDL_WINDOWEVENT_MINIMIZED: return blWindowEvent::eMinimize;
    case SDL_WINDOWEVENT_MAXIMIZED: return blWindowEvent::eMaximize;
    case SDL_WINDOWEVENT_CLOSE:     return blWindowEvent::eClose;
    default:                        return blWindowEvent::eUnknown;
    }
}


static inline blEventTypeFlagBits blGetEventType(blGenericEvent event)
{
    if (event <= (blGenericEvent)blKey::eEndEnum) return blEventTypeFlagBits::eKeyboardBit;
    else if (event <= (blGenericEvent)blWindowEvent::eEndEnum) return blEventTypeFlagBits::eWindowBit;
    else if (event <= (blGenericEvent)blMouseButtonEvent::eEndEnum) return blEventTypeFlagBits::eMouseButtonBit;
    else if (event <= (blGenericEvent)blMouseEvent::eEndEnum) return blEventTypeFlagBits::eMouseBit;
    else if (event <= (blGenericEvent)blGamepadButtonEvent::eEndEnum) return blEventTypeFlagBits::eGamepadButton;
    else if (event <= (blGenericEvent)blGamepadThumbstickEvent::eEndEnum) return blEventTypeFlagBits::eGamepadThumbstickBit;
    else if (event <= (blGenericEvent)blGamepadTriggerEvent::eEndEnum) return blEventTypeFlagBits::eGamepadTriggerBit;
    else return blEventTypeFlagBits::eAllWithSDLBit;
}

static inline blMouseButtonEvent blMouseButtonEventFromSDL(uint8_t key)
{
    switch (key)
    {
    case SDL_BUTTON_LEFT:   return blMouseButtonEvent::eLeft;
    case SDL_BUTTON_RIGHT:  return blMouseButtonEvent::eRight;
    case SDL_BUTTON_MIDDLE: return blMouseButtonEvent::eMiddle;
    case SDL_BUTTON_X1:     return blMouseButtonEvent::eSideFront;
    case SDL_BUTTON_X2:     return blMouseButtonEvent::eSideBack;
    default:                return blMouseButtonEvent::eUnknown;
    }
}

static inline blKey blKeyFromSDLKeycode(SDL_Keycode keycode)
{
    switch (keycode)
    {
    case SDLK_RETURN: return blKey::eReturn;
    case SDLK_ESCAPE: return blKey::eEscape;
    case SDLK_BACKSPACE: return blKey::eBackspace;
    case SDLK_TAB: return blKey::eTab;
    case SDLK_SPACE: return blKey::eSpace;
    case SDLK_EXCLAIM: return blKey::eExclaim;
    case SDLK_QUOTEDBL: return blKey::eQuoteDbl;
    case SDLK_HASH: return blKey::eHash;
    case SDLK_PERCENT: return blKey::ePercent;
    case SDLK_DOLLAR: return blKey::eDollar;
    case SDLK_AMPERSAND: return blKey::eAmpersand;
    case SDLK_QUOTE: return blKey::eQuote;
    case SDLK_LEFTPAREN: return blKey::eLeftParen;
    case SDLK_RIGHTPAREN: return blKey::eRightParen;
    case SDLK_ASTERISK: return blKey::eAsterisk;
    case SDLK_PLUS: return blKey::ePlus;
    case SDLK_COMMA: return blKey::eComma;
    case SDLK_MINUS: return blKey::eMinus;
    case SDLK_PERIOD: return blKey::ePeriod;
    case SDLK_SLASH: return blKey::eSlash;
    case SDLK_0: return blKey::e0;
    case SDLK_1: return blKey::e1;
    case SDLK_2: return blKey::e2;
    case SDLK_3: return blKey::e3;
    case SDLK_4: return blKey::e4;
    case SDLK_5: return blKey::e5;
    case SDLK_6: return blKey::e6;
    case SDLK_7: return blKey::e7;
    case SDLK_8: return blKey::e8;
    case SDLK_9: return blKey::e9;
    case SDLK_COLON: return blKey::eColon;
    case SDLK_SEMICOLON: return blKey::eSemicolon;
    case SDLK_LESS: return blKey::eLess;
    case SDLK_EQUALS: return blKey::eEquals;
    case SDLK_GREATER: return blKey::eGreater;
    case SDLK_QUESTION: return blKey::eQuestion;
    case SDLK_AT: return blKey::eAt;
    case SDLK_LEFTBRACKET: return blKey::eLeftBracket;
    case SDLK_BACKSLASH: return blKey::eBackslash;
    case SDLK_RIGHTBRACKET: return blKey::eRightBracket;
    case SDLK_CARET: return blKey::eCaret;
    case SDLK_UNDERSCORE: return blKey::eUnderscore;
    case SDLK_BACKQUOTE: return blKey::eBackquote;
    case SDLK_a: return blKey::eA;
    case SDLK_b: return blKey::eB;
    case SDLK_c: return blKey::eC;
    case SDLK_d: return blKey::eD;
    case SDLK_e: return blKey::eE;
    case SDLK_f: return blKey::eF;
    case SDLK_g: return blKey::eG;
    case SDLK_h: return blKey::eH;
    case SDLK_i: return blKey::eI;
    case SDLK_j: return blKey::eJ;
    case SDLK_k: return blKey::eK;
    case SDLK_l: return blKey::eL;
    case SDLK_m: return blKey::eM;
    case SDLK_n: return blKey::eN;
    case SDLK_o: return blKey::eO;
    case SDLK_p: return blKey::eP;
    case SDLK_q: return blKey::eQ;
    case SDLK_r: return blKey::eR;
    case SDLK_s: return blKey::eS;
    case SDLK_t: return blKey::eT;
    case SDLK_u: return blKey::eU;
    case SDLK_v: return blKey::eV;
    case SDLK_w: return blKey::eW;
    case SDLK_x: return blKey::eX;
    case SDLK_y: return blKey::eY;
    case SDLK_z: return blKey::eZ;
    case SDLK_CAPSLOCK: return blKey::eCapsLock;
    case SDLK_F1: return blKey::eF1;
    case SDLK_F2: return blKey::eF2;
    case SDLK_F3: return blKey::eF3;
    case SDLK_F4: return blKey::eF4;
    case SDLK_F5: return blKey::eF5;
    case SDLK_F6: return blKey::eF6;
    case SDLK_F7: return blKey::eF7;
    case SDLK_F8: return blKey::eF8;
    case SDLK_F9: return blKey::eF9;
    case SDLK_F10: return blKey::eF10;
    case SDLK_F11: return blKey::eF11;
    case SDLK_F12: return blKey::eF12;
    case SDLK_PRINTSCREEN: return blKey::ePrintScreen;
    case SDLK_SCROLLLOCK: return blKey::eScrollLock;
    case SDLK_PAUSE: return blKey::ePause;
    case SDLK_INSERT: return blKey::eInsert;
    case SDLK_HOME: return blKey::eHome;
    case SDLK_PAGEUP: return blKey::ePageUp;
    case SDLK_DELETE: return blKey::eDelete;
    case SDLK_END: return blKey::eEnd;
    case SDLK_PAGEDOWN: return blKey::ePageDown;
    case SDLK_RIGHT: return blKey::eRight;
    case SDLK_LEFT: return blKey::eLeft;
    case SDLK_DOWN: return blKey::eDown;
    case SDLK_UP: return blKey::eUp;
    case SDLK_NUMLOCKCLEAR: return blKey::eNumLockClear;
    case SDLK_KP_DIVIDE: return blKey::eKpDivide;
    case SDLK_KP_MULTIPLY: return blKey::eKpMultiply;
    case SDLK_KP_MINUS: return blKey::eKpMinus;
    case SDLK_KP_PLUS: return blKey::eKpPlus;
    case SDLK_KP_ENTER: return blKey::eKpEnter;
    case SDLK_KP_1: return blKey::eKp_1;
    case SDLK_KP_2: return blKey::eKp_2;
    case SDLK_KP_3: return blKey::eKp_3;
    case SDLK_KP_4: return blKey::eKp_4;
    case SDLK_KP_5: return blKey::eKp_5;
    case SDLK_KP_6: return blKey::eKp_6;
    case SDLK_KP_7: return blKey::eKp_7;
    case SDLK_KP_8: return blKey::eKp_8;
    case SDLK_KP_9: return blKey::eKp_9;
    case SDLK_KP_0: return blKey::eKp_0;
    case SDLK_KP_PERIOD: return blKey::eKpPeriod;
    case SDLK_APPLICATION: return blKey::eApplication;
    case SDLK_POWER: return blKey::ePower;
    case SDLK_KP_EQUALS: return blKey::eKpEquals;
    case SDLK_F13: return blKey::eF13;
    case SDLK_F14: return blKey::eF14;
    case SDLK_F15: return blKey::eF15;
    case SDLK_F16: return blKey::eF16;
    case SDLK_F17: return blKey::eF17;
    case SDLK_F18: return blKey::eF18;
    case SDLK_F19: return blKey::eF19;
    case SDLK_F20: return blKey::eF20;
    case SDLK_F21: return blKey::eF21;
    case SDLK_F22: return blKey::eF22;
    case SDLK_F23: return blKey::eF23;
    case SDLK_F24: return blKey::eF24;
    case SDLK_EXECUTE: return blKey::eExecute;
    case SDLK_HELP: return blKey::eHelp;
    case SDLK_MENU: return blKey::eMenu;
    case SDLK_SELECT: return blKey::eSelect;
    case SDLK_STOP: return blKey::eStop;
    case SDLK_AGAIN: return blKey::eAgain;
    case SDLK_UNDO: return blKey::eUndo;
    case SDLK_CUT: return blKey::eCut;
    case SDLK_COPY: return blKey::eCopy;
    case SDLK_PASTE: return blKey::ePaste;
    case SDLK_FIND: return blKey::eFind;
    case SDLK_MUTE: return blKey::eMute;
    case SDLK_VOLUMEUP: return blKey::eVolumeUp;
    case SDLK_VOLUMEDOWN: return blKey::eVolumeDown;
    case SDLK_KP_COMMA: return blKey::eKpComma;
    case SDLK_KP_EQUALSAS400: return blKey::eKpEqualSas400;
    case SDLK_ALTERASE: return blKey::eAltErase;
    case SDLK_SYSREQ: return blKey::eSysReq;
    case SDLK_CANCEL: return blKey::eCancel;
    case SDLK_CLEAR: return blKey::eClear;
    case SDLK_PRIOR: return blKey::ePrior;
    case SDLK_RETURN2: return blKey::eReturn2;
    case SDLK_SEPARATOR: return blKey::eSeparator;
    case SDLK_OUT: return blKey::eOut;
    case SDLK_OPER: return blKey::eOper;
    case SDLK_CLEARAGAIN: return blKey::eClearAgain;
    case SDLK_CRSEL: return blKey::eCrsel;
    case SDLK_EXSEL: return blKey::eExsel;
    case SDLK_KP_00: return blKey::eKp_00;
    case SDLK_KP_000: return blKey::eKp_000;
    case SDLK_THOUSANDSSEPARATOR: return blKey::eThousandsSeparator;
    case SDLK_DECIMALSEPARATOR: return blKey::eDecimalSeparator;
    case SDLK_CURRENCYUNIT: return blKey::eCurrencyUnit;
    case SDLK_CURRENCYSUBUNIT: return blKey::eCurrencySubUnit;
    case SDLK_KP_LEFTPAREN: return blKey::eKpLeftParen;
    case SDLK_KP_RIGHTPAREN: return blKey::eKpRightParen;
    case SDLK_KP_LEFTBRACE: return blKey::eKpLeftBrace;
    case SDLK_KP_RIGHTBRACE: return blKey::eKpRightBrace;
    case SDLK_KP_TAB: return blKey::eKpTab;
    case SDLK_KP_BACKSPACE: return blKey::eKpBackspace;
    case SDLK_KP_A: return blKey::eKpA;
    case SDLK_KP_B: return blKey::eKpB;
    case SDLK_KP_C: return blKey::eKpC;
    case SDLK_KP_D: return blKey::eKpD;
    case SDLK_KP_E: return blKey::eKpE;
    case SDLK_KP_F: return blKey::eKpF;
    case SDLK_KP_XOR: return blKey::eKpXor;
    case SDLK_KP_POWER: return blKey::eKpPower;
    case SDLK_KP_PERCENT: return blKey::eKpPercent;
    case SDLK_KP_LESS: return blKey::eKpLess;
    case SDLK_KP_GREATER: return blKey::eKpGreater;
    case SDLK_KP_AMPERSAND: return blKey::eKpAmpersand;
    case SDLK_KP_DBLAMPERSAND: return blKey::eKpDblAmpersand;
    case SDLK_KP_VERTICALBAR: return blKey::eKpVerticalBar;
    case SDLK_KP_DBLVERTICALBAR: return blKey::eKpDblVerticalBar;
    case SDLK_KP_COLON: return blKey::eKpColon;
    case SDLK_KP_HASH: return blKey::eKpHash;
    case SDLK_KP_SPACE: return blKey::eKpSpace;
    case SDLK_KP_AT: return blKey::eKpAt;
    case SDLK_KP_EXCLAM: return blKey::eKpExclam;
    case SDLK_KP_MEMSTORE: return blKey::eKpMemStore;
    case SDLK_KP_MEMRECALL: return blKey::eKpMemRecall;
    case SDLK_KP_MEMCLEAR: return blKey::eKpMemClear;
    case SDLK_KP_MEMADD: return blKey::eKpMemAdd;
    case SDLK_KP_MEMSUBTRACT: return blKey::eKpMemSubtract;
    case SDLK_KP_MEMMULTIPLY: return blKey::eKpMemMultiply;
    case SDLK_KP_MEMDIVIDE: return blKey::eKpMemDivide;
    case SDLK_KP_PLUSMINUS: return blKey::eKpPlusMinus;
    case SDLK_KP_CLEAR: return blKey::eKpClear;
    case SDLK_KP_CLEARENTRY: return blKey::eKpClearEntry;
    case SDLK_KP_BINARY: return blKey::eKpBinary;
    case SDLK_KP_OCTAL: return blKey::eKpOctal;
    case SDLK_KP_DECIMAL: return blKey::eKpDecimal;
    case SDLK_KP_HEXADECIMAL: return blKey::eKpHexadecimal;
    case SDLK_LCTRL: return blKey::eLCtrl;
    case SDLK_LSHIFT: return blKey::eLShift;
    case SDLK_LALT: return blKey::eLAlt;
    case SDLK_LGUI: return blKey::eLGui;
    case SDLK_RCTRL: return blKey::eRCtrl;
    case SDLK_RSHIFT: return blKey::eRShift;
    case SDLK_RALT: return blKey::eRAlt;
    case SDLK_RGUI: return blKey::eRGui;
    case SDLK_MODE: return blKey::eMode;
    case SDLK_AUDIONEXT: return blKey::eAudioNext;
    case SDLK_AUDIOPREV: return blKey::eAudioPrev;
    case SDLK_AUDIOSTOP: return blKey::eAudioStop;
    case SDLK_AUDIOPLAY: return blKey::eAudioPlay;
    case SDLK_AUDIOMUTE: return blKey::eAudioMute;
    case SDLK_MEDIASELECT: return blKey::eMediaSelect;
    case SDLK_WWW: return blKey::eWww;
    case SDLK_MAIL: return blKey::eMail;
    case SDLK_CALCULATOR: return blKey::eCalculator;
    case SDLK_COMPUTER: return blKey::eComputer;
    case SDLK_AC_SEARCH: return blKey::eAcSearch;
    case SDLK_AC_HOME: return blKey::eAcHome;
    case SDLK_AC_BACK: return blKey::eAcBack;
    case SDLK_AC_FORWARD: return blKey::eAcForward;
    case SDLK_AC_STOP: return blKey::eAcStop;
    case SDLK_AC_REFRESH: return blKey::eAcRefresh;
    case SDLK_AC_BOOKMARKS: return blKey::eAcBookmarks;
    case SDLK_BRIGHTNESSDOWN: return blKey::eBrightnessDown;
    case SDLK_BRIGHTNESSUP: return blKey::eBrightnessUp;
    case SDLK_DISPLAYSWITCH: return blKey::eDisplaySwitch;
    case SDLK_KBDILLUMTOGGLE: return blKey::eKbdIllumToggle;
    case SDLK_KBDILLUMDOWN: return blKey::eKbdIllumDown;
    case SDLK_KBDILLUMUP: return blKey::eKbdIllumUp;
    case SDLK_EJECT: return blKey::eEject;
    case SDLK_SLEEP: return blKey::eSleep;
    case SDLK_APP1: return blKey::eApp1;
    case SDLK_APP2: return blKey::eApp2;
    case SDLK_AUDIOREWIND: return blKey::eAudioRewind;
    case SDLK_AUDIOFASTFORWARD: return blKey::eAudioFastForward;
    case SDLK_SOFTLEFT: return blKey::eSoftLeft;
    case SDLK_SOFTRIGHT: return blKey::eSoftRight;
    case SDLK_CALL: return blKey::eCall;
    case SDLK_ENDCALL: return blKey::eEndCall;
    default: return blKey::eUnknown;
    }
}

static inline SDL_Keycode blKeyToSDLKeycode(blKey key)
{
    switch (key)
    {
    case blKey::eReturn: return SDLK_RETURN;
    case blKey::eEscape: return SDLK_ESCAPE;
    case blKey::eBackspace: return SDLK_BACKSPACE;
    case blKey::eTab: return SDLK_TAB;
    case blKey::eSpace: return SDLK_SPACE;
    case blKey::eExclaim: return SDLK_EXCLAIM;
    case blKey::eQuoteDbl: return SDLK_QUOTEDBL;
    case blKey::eHash: return SDLK_HASH;
    case blKey::ePercent: return SDLK_PERCENT;
    case blKey::eDollar: return SDLK_DOLLAR;
    case blKey::eAmpersand: return SDLK_AMPERSAND;
    case blKey::eQuote: return SDLK_QUOTE;
    case blKey::eLeftParen: return SDLK_LEFTPAREN;
    case blKey::eRightParen: return SDLK_RIGHTPAREN;
    case blKey::eAsterisk: return SDLK_ASTERISK;
    case blKey::ePlus: return SDLK_PLUS;
    case blKey::eComma: return SDLK_COMMA;
    case blKey::eMinus: return SDLK_MINUS;
    case blKey::ePeriod: return SDLK_PERIOD;
    case blKey::eSlash: return SDLK_SLASH;
    case blKey::e0: return SDLK_0;
    case blKey::e1: return SDLK_1;
    case blKey::e2: return SDLK_2;
    case blKey::e3: return SDLK_3;
    case blKey::e4: return SDLK_4;
    case blKey::e5: return SDLK_5;
    case blKey::e6: return SDLK_6;
    case blKey::e7: return SDLK_7;
    case blKey::e8: return SDLK_8;
    case blKey::e9: return SDLK_9;
    case blKey::eColon: return SDLK_COLON;
    case blKey::eSemicolon: return SDLK_SEMICOLON;
    case blKey::eLess: return SDLK_LESS;
    case blKey::eEquals: return SDLK_EQUALS;
    case blKey::eGreater: return SDLK_GREATER;
    case blKey::eQuestion: return SDLK_QUESTION;
    case blKey::eAt: return SDLK_AT;
    case blKey::eLeftBracket: return SDLK_LEFTBRACKET;
    case blKey::eBackslash: return SDLK_BACKSLASH;
    case blKey::eRightBracket: return SDLK_RIGHTBRACKET;
    case blKey::eCaret: return SDLK_CARET;
    case blKey::eUnderscore: return SDLK_UNDERSCORE;
    case blKey::eBackquote: return SDLK_BACKQUOTE;
    case blKey::eA: return SDLK_a;
    case blKey::eB: return SDLK_b;
    case blKey::eC: return SDLK_c;
    case blKey::eD: return SDLK_d;
    case blKey::eE: return SDLK_e;
    case blKey::eF: return SDLK_f;
    case blKey::eG: return SDLK_g;
    case blKey::eH: return SDLK_h;
    case blKey::eI: return SDLK_i;
    case blKey::eJ: return SDLK_j;
    case blKey::eK: return SDLK_k;
    case blKey::eL: return SDLK_l;
    case blKey::eM: return SDLK_m;
    case blKey::eN: return SDLK_n;
    case blKey::eO: return SDLK_o;
    case blKey::eP: return SDLK_p;
    case blKey::eQ: return SDLK_q;
    case blKey::eR: return SDLK_r;
    case blKey::eS: return SDLK_s;
    case blKey::eT: return SDLK_t;
    case blKey::eU: return SDLK_u;
    case blKey::eV: return SDLK_v;
    case blKey::eW: return SDLK_w;
    case blKey::eX: return SDLK_x;
    case blKey::eY: return SDLK_y;
    case blKey::eZ: return SDLK_z;
    case blKey::eCapsLock: return SDLK_CAPSLOCK;
    case blKey::eF1: return SDLK_F1;
    case blKey::eF2: return SDLK_F2;
    case blKey::eF3: return SDLK_F3;
    case blKey::eF4: return SDLK_F4;
    case blKey::eF5: return SDLK_F5;
    case blKey::eF6: return SDLK_F6;
    case blKey::eF7: return SDLK_F7;
    case blKey::eF8: return SDLK_F8;
    case blKey::eF9: return SDLK_F9;
    case blKey::eF10: return SDLK_F10;
    case blKey::eF11: return SDLK_F11;
    case blKey::eF12: return SDLK_F12;
    case blKey::ePrintScreen: return SDLK_PRINTSCREEN;
    case blKey::eScrollLock: return SDLK_SCROLLLOCK;
    case blKey::ePause: return SDLK_PAUSE;
    case blKey::eInsert: return SDLK_INSERT;
    case blKey::eHome: return SDLK_HOME;
    case blKey::ePageUp: return SDLK_PAGEUP;
    case blKey::eDelete: return SDLK_DELETE;
    case blKey::eEnd: return SDLK_END;
    case blKey::ePageDown: return SDLK_PAGEDOWN;
    case blKey::eRight: return SDLK_RIGHT;
    case blKey::eLeft: return SDLK_LEFT;
    case blKey::eDown: return SDLK_DOWN;
    case blKey::eUp: return SDLK_UP;
    case blKey::eNumLockClear: return SDLK_NUMLOCKCLEAR;
    case blKey::eKpDivide: return SDLK_KP_DIVIDE;
    case blKey::eKpMultiply: return SDLK_KP_MULTIPLY;
    case blKey::eKpMinus: return SDLK_KP_MINUS;
    case blKey::eKpPlus: return SDLK_KP_PLUS;
    case blKey::eKpEnter: return SDLK_KP_ENTER;
    case blKey::eKp_1: return SDLK_KP_1;
    case blKey::eKp_2: return SDLK_KP_2;
    case blKey::eKp_3: return SDLK_KP_3;
    case blKey::eKp_4: return SDLK_KP_4;
    case blKey::eKp_5: return SDLK_KP_5;
    case blKey::eKp_6: return SDLK_KP_6;
    case blKey::eKp_7: return SDLK_KP_7;
    case blKey::eKp_8: return SDLK_KP_8;
    case blKey::eKp_9: return SDLK_KP_9;
    case blKey::eKp_0: return SDLK_KP_0;
    case blKey::eKpPeriod: return SDLK_KP_PERIOD;
    case blKey::eApplication: return SDLK_APPLICATION;
    case blKey::ePower: return SDLK_POWER;
    case blKey::eKpEquals: return SDLK_KP_EQUALS;
    case blKey::eF13: return SDLK_F13;
    case blKey::eF14: return SDLK_F14;
    case blKey::eF15: return SDLK_F15;
    case blKey::eF16: return SDLK_F16;
    case blKey::eF17: return SDLK_F17;
    case blKey::eF18: return SDLK_F18;
    case blKey::eF19: return SDLK_F19;
    case blKey::eF20: return SDLK_F20;
    case blKey::eF21: return SDLK_F21;
    case blKey::eF22: return SDLK_F22;
    case blKey::eF23: return SDLK_F23;
    case blKey::eF24: return SDLK_F24;
    case blKey::eExecute: return SDLK_EXECUTE;
    case blKey::eHelp: return SDLK_HELP;
    case blKey::eMenu: return SDLK_MENU;
    case blKey::eSelect: return SDLK_SELECT;
    case blKey::eStop: return SDLK_STOP;
    case blKey::eAgain: return SDLK_AGAIN;
    case blKey::eUndo: return SDLK_UNDO;
    case blKey::eCut: return SDLK_CUT;
    case blKey::eCopy: return SDLK_COPY;
    case blKey::ePaste: return SDLK_PASTE;
    case blKey::eFind: return SDLK_FIND;
    case blKey::eMute: return SDLK_MUTE;
    case blKey::eVolumeUp: return SDLK_VOLUMEUP;
    case blKey::eVolumeDown: return SDLK_VOLUMEDOWN;
    case blKey::eKpComma: return SDLK_KP_COMMA;
    case blKey::eKpEqualSas400: return SDLK_KP_EQUALSAS400;
    case blKey::eAltErase: return SDLK_ALTERASE;
    case blKey::eSysReq: return SDLK_SYSREQ;
    case blKey::eCancel: return SDLK_CANCEL;
    case blKey::eClear: return SDLK_CLEAR;
    case blKey::ePrior: return SDLK_PRIOR;
    case blKey::eReturn2: return SDLK_RETURN2;
    case blKey::eSeparator: return SDLK_SEPARATOR;
    case blKey::eOut: return SDLK_OUT;
    case blKey::eOper: return SDLK_OPER;
    case blKey::eClearAgain: return SDLK_CLEARAGAIN;
    case blKey::eCrsel: return SDLK_CRSEL;
    case blKey::eExsel: return SDLK_EXSEL;
    case blKey::eKp_00: return SDLK_KP_00;
    case blKey::eKp_000: return SDLK_KP_000;
    case blKey::eThousandsSeparator: return SDLK_THOUSANDSSEPARATOR;
    case blKey::eDecimalSeparator: return SDLK_DECIMALSEPARATOR;
    case blKey::eCurrencyUnit: return SDLK_CURRENCYUNIT;
    case blKey::eCurrencySubUnit: return SDLK_CURRENCYSUBUNIT;
    case blKey::eKpLeftParen: return SDLK_KP_LEFTPAREN;
    case blKey::eKpRightParen: return SDLK_KP_RIGHTPAREN;
    case blKey::eKpLeftBrace: return SDLK_KP_LEFTBRACE;
    case blKey::eKpRightBrace: return SDLK_KP_RIGHTBRACE;
    case blKey::eKpTab: return SDLK_KP_TAB;
    case blKey::eKpBackspace: return SDLK_KP_BACKSPACE;
    case blKey::eKpA: return SDLK_KP_A;
    case blKey::eKpB: return SDLK_KP_B;
    case blKey::eKpC: return SDLK_KP_C;
    case blKey::eKpD: return SDLK_KP_D;
    case blKey::eKpE: return SDLK_KP_E;
    case blKey::eKpF: return SDLK_KP_F;
    case blKey::eKpXor: return SDLK_KP_XOR;
    case blKey::eKpPower: return SDLK_KP_POWER;
    case blKey::eKpPercent: return SDLK_KP_PERCENT;
    case blKey::eKpLess: return SDLK_KP_LESS;
    case blKey::eKpGreater: return SDLK_KP_GREATER;
    case blKey::eKpAmpersand: return SDLK_KP_AMPERSAND;
    case blKey::eKpDblAmpersand: return SDLK_KP_DBLAMPERSAND;
    case blKey::eKpVerticalBar: return SDLK_KP_VERTICALBAR;
    case blKey::eKpDblVerticalBar: return SDLK_KP_DBLVERTICALBAR;
    case blKey::eKpColon: return SDLK_KP_COLON;
    case blKey::eKpHash: return SDLK_KP_HASH;
    case blKey::eKpSpace: return SDLK_KP_SPACE;
    case blKey::eKpAt: return SDLK_KP_AT;
    case blKey::eKpExclam: return SDLK_KP_EXCLAM;
    case blKey::eKpMemStore: return SDLK_KP_MEMSTORE;
    case blKey::eKpMemRecall: return SDLK_KP_MEMRECALL;
    case blKey::eKpMemClear: return SDLK_KP_MEMCLEAR;
    case blKey::eKpMemAdd: return SDLK_KP_MEMADD;
    case blKey::eKpMemSubtract: return SDLK_KP_MEMSUBTRACT;
    case blKey::eKpMemMultiply: return SDLK_KP_MEMMULTIPLY;
    case blKey::eKpMemDivide: return SDLK_KP_MEMDIVIDE;
    case blKey::eKpPlusMinus: return SDLK_KP_PLUSMINUS;
    case blKey::eKpClear: return SDLK_KP_CLEAR;
    case blKey::eKpClearEntry: return SDLK_KP_CLEARENTRY;
    case blKey::eKpBinary: return SDLK_KP_BINARY;
    case blKey::eKpOctal: return SDLK_KP_OCTAL;
    case blKey::eKpDecimal: return SDLK_KP_DECIMAL;
    case blKey::eKpHexadecimal: return SDLK_KP_HEXADECIMAL;
    case blKey::eLCtrl: return SDLK_LCTRL;
    case blKey::eLShift: return SDLK_LSHIFT;
    case blKey::eLAlt: return SDLK_LALT;
    case blKey::eLGui: return SDLK_LGUI;
    case blKey::eRCtrl: return SDLK_RCTRL;
    case blKey::eRShift: return SDLK_RSHIFT;
    case blKey::eRAlt: return SDLK_RALT;
    case blKey::eRGui: return SDLK_RGUI;
    case blKey::eMode: return SDLK_MODE;
    case blKey::eAudioNext: return SDLK_AUDIONEXT;
    case blKey::eAudioPrev: return SDLK_AUDIOPREV;
    case blKey::eAudioStop: return SDLK_AUDIOSTOP;
    case blKey::eAudioPlay: return SDLK_AUDIOPLAY;
    case blKey::eAudioMute: return SDLK_AUDIOMUTE;
    case blKey::eMediaSelect: return SDLK_MEDIASELECT;
    case blKey::eWww: return SDLK_WWW;
    case blKey::eMail: return SDLK_MAIL;
    case blKey::eCalculator: return SDLK_CALCULATOR;
    case blKey::eComputer: return SDLK_COMPUTER;
    case blKey::eAcSearch: return SDLK_AC_SEARCH;
    case blKey::eAcHome: return SDLK_AC_HOME;
    case blKey::eAcBack: return SDLK_AC_BACK;
    case blKey::eAcForward: return SDLK_AC_FORWARD;
    case blKey::eAcStop: return SDLK_AC_STOP;
    case blKey::eAcRefresh: return SDLK_AC_REFRESH;
    case blKey::eAcBookmarks: return SDLK_AC_BOOKMARKS;
    case blKey::eBrightnessDown: return SDLK_BRIGHTNESSDOWN;
    case blKey::eBrightnessUp: return SDLK_BRIGHTNESSUP;
    case blKey::eDisplaySwitch: return SDLK_DISPLAYSWITCH;
    case blKey::eKbdIllumToggle: return SDLK_KBDILLUMTOGGLE;
    case blKey::eKbdIllumDown: return SDLK_KBDILLUMDOWN;
    case blKey::eKbdIllumUp: return SDLK_KBDILLUMUP;
    case blKey::eEject: return SDLK_EJECT;
    case blKey::eSleep: return SDLK_SLEEP;
    case blKey::eApp1: return SDLK_APP1;
    case blKey::eApp2: return SDLK_APP2;
    case blKey::eAudioRewind: return SDLK_AUDIOREWIND;
    case blKey::eAudioFastForward: return SDLK_AUDIOFASTFORWARD;
    case blKey::eSoftLeft: return SDLK_SOFTLEFT;
    case blKey::eSoftRight: return SDLK_SOFTRIGHT;
    case blKey::eCall: return SDLK_CALL;
    case blKey::eEndCall: return SDLK_ENDCALL;
    default: return SDLK_UNKNOWN;
    }
}