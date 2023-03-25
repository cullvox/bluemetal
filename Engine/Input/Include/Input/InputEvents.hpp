#pragma once

#include "Math/Vector2.hpp"
#include "Window/Window.hpp"

#include <SDL2/SDL_keycode.h>


namespace bl {

enum class Key
{

// Window Events
    WindowResize,
    WindowMinimize,
    WindowMaximize,
    WindowClose,
    WindowMove,

// Keyboard Events
    KeyUnknown = SDLK_UNKNOWN,
    KeyReturn = SDLK_RETURN,
    KeyEscape = SDLK_ESCAPE,
    KeyBackspace = SDLK_BACKSPACE,
    KeyTab = SDLK_TAB,
    KeySpace = SDLK_SPACE,
    KeyExclaim = SDLK_EXCLAIM,
    KeyQuoteDbl = SDLK_QUOTEDBL,
    KeyHash = SDLK_HASH,
    KeyPercent = SDLK_PERCENT,
    KeyDollar = SDLK_DOLLAR,
    KeyAmpersand = SDLK_AMPERSAND,
    KeyQuote = SDLK_QUOTE,
    KeyLeftParen = SDLK_LEFTPAREN,
    KeyRightParen = SDLK_RIGHTPAREN,
    KeyAsterisk = SDLK_ASTERISK,
    KeyPlus = SDLK_PLUS,
    KeyComma = SDLK_COMMA,
    KeyMinus = SDLK_MINUS,
    KeyPeriod = SDLK_PERIOD,
    KeySlash = SDLK_SLASH,
    Key0 = SDLK_0,
    Key1 = SDLK_1,
    Key2 = SDLK_2,
    Key3 = SDLK_3,
    Key4 = SDLK_4,
    Key5 = SDLK_5,
    Key6 = SDLK_6,
    Key7 = SDLK_7,
    Key8 = SDLK_8,
    Key9 = SDLK_9,
    KeyColon = SDLK_COLON,
    KeySemicolon = SDLK_SEMICOLON,
    KeyLess = SDLK_LESS,
    KeyEquals = SDLK_EQUALS,
    KeyGreater = SDLK_GREATER,
    KeyQuestion = SDLK_QUESTION,
    KeyAt = SDLK_AT,
    KeyLeftBracket = SDLK_LEFTBRACKET,
    KeyBackslash = SDLK_BACKSLASH,
    KeyRightBracket = SDLK_RIGHTBRACKET,
    KeyCaret = SDLK_CARET,
    KeyUnderscore = SDLK_UNDERSCORE,
    KeyBackquote = SDLK_BACKQUOTE,
    KeyA = SDLK_a,
    KeyB = SDLK_b,
    KeyC = SDLK_c,
    KeyD = SDLK_d,
    KeyE = SDLK_e,
    KeyF = SDLK_f,
    KeyG = SDLK_g,
    KeyH = SDLK_h,
    KeyI = SDLK_i,
    KeyJ = SDLK_j,
    KeyK = SDLK_k,
    KeyL = SDLK_l,
    KeyM = SDLK_m,
    KeyN = SDLK_n,
    KeyO = SDLK_o,
    KeyP = SDLK_p,
    KeyQ = SDLK_q,
    KeyR = SDLK_r,
    KeyS = SDLK_s,
    KeyT = SDLK_t,
    KeyU = SDLK_u,
    KeyV = SDLK_v,
    KeyW = SDLK_w,
    KeyX = SDLK_x,
    KeyY = SDLK_y,
    KeyZ = SDLK_z,
    KeyCapsLock = SDLK_CAPSLOCK,
    KeyF1 = SDLK_F1,
    KeyF2 = SDLK_F2,
    KeyF3 = SDLK_F3,
    KeyF4 = SDLK_F4,
    KeyF5 = SDLK_F5,
    KeyF6 = SDLK_F6,
    KeyF7 = SDLK_F7,
    KeyF8 = SDLK_F8,
    KeyF9 = SDLK_F9,
    KeyF10 = SDLK_F10,
    KeyF11 = SDLK_F11,
    KeyF12 = SDLK_F12,
    KeyPrintScreen = SDLK_PRINTSCREEN,
    KeyScrollLock = SDLK_SCROLLLOCK,
    KeyPause = SDLK_PAUSE,
    KeyInsert = SDLK_INSERT,
    KeyHome = SDLK_HOME,
    KeyPageUp = SDLK_PAGEUP,
    KeyDelete = SDLK_DELETE,
    KeyEnd = SDLK_END,
    KeyPageDown = SDLK_PAGEDOWN,
    KeyRight = SDLK_RIGHT,
    KeyLeft = SDLK_LEFT,
    KeyDown = SDLK_DOWN,
    KeyUp = SDLK_UP,
    KeyNumlockClear = SDLK_NUMLOCKCLEAR,
    KeyKpDivide = SDLK_KP_DIVIDE,
    KeyKpMultiply = SDLK_KP_MULTIPLY,
    KeyKpMinus = SDLK_KP_MINUS,
    KeyKpPlus = SDLK_KP_PLUS,
    KeyKpEnter = SDLK_KP_ENTER,
    KeyKp_1 = SDLK_KP_1,
    KeyKp_2 = SDLK_KP_2,
    KeyKp_3 = SDLK_KP_3,
    KeyKp_4 = SDLK_KP_4,
    KeyKp_5 = SDLK_KP_5,
    KeyKp_6 = SDLK_KP_6,
    KeyKp_7 = SDLK_KP_7,
    KeyKp_8 = SDLK_KP_8,
    KeyKp_9 = SDLK_KP_9,
    KeyKp_0 = SDLK_KP_0,
    KeyKpPeriod = SDLK_KP_PERIOD,
    KeyApplication = SDLK_APPLICATION,
    KeyPower = SDLK_POWER,
    KeyKpEquals = SDLK_KP_EQUALS,
    KeyF13 = SDLK_F13,
    KeyF14 = SDLK_F14,
    KeyF15 = SDLK_F15,
    KeyF16 = SDLK_F16,
    KeyF17 = SDLK_F17,
    KeyF18 = SDLK_F18,
    KeyF19 = SDLK_F19,
    KeyF20 = SDLK_F20,
    KeyF21 = SDLK_F21,
    KeyF22 = SDLK_F22,
    KeyF23 = SDLK_F23,
    KeyF24 = SDLK_F24,
    KeyExecute = SDLK_EXECUTE,
    KeyHelp = SDLK_HELP,
    KeyMenu = SDLK_MENU,
    KeySelect = SDLK_SELECT,
    KeyStop = SDLK_STOP,
    KeyAgain = SDLK_AGAIN,
    KeyUndo = SDLK_UNDO,
    KeyCut = SDLK_CUT,
    KeyCopy = SDLK_COPY,
    KeyPaste = SDLK_PASTE,
    KeyFind = SDLK_FIND,
    KeyMute = SDLK_MUTE,
    KeyVolumeUp = SDLK_VOLUMEUP,
    KeyVolumeDown = SDLK_VOLUMEDOWN,
    KeyKpComma = SDLK_KP_COMMA,
    KeyKpEqualSas400 = SDLK_KP_EQUALSAS400,
    KeyAltErase = SDLK_ALTERASE,
    KeySysReq = SDLK_SYSREQ,
    KeyCancel = SDLK_CANCEL,
    KeyClear = SDLK_CLEAR,
    KeyPrior = SDLK_PRIOR,
    KeyReturn2 = SDLK_RETURN2,
    KeySeparator = SDLK_SEPARATOR,
    KeyOut = SDLK_OUT,
    KeyOper = SDLK_OPER,
    KeyClearAgain = SDLK_CLEARAGAIN,
    KeyCrsel = SDLK_CRSEL,
    KeyExsel = SDLK_EXSEL,
    KeyKp_00 = SDLK_KP_00,
    KeyKp_000 = SDLK_KP_000,
    KeyThousandsSeparator = SDLK_THOUSANDSSEPARATOR,
    KeyDecimalSeparator = SDLK_DECIMALSEPARATOR,
    KeyCurrencyUnit = SDLK_CURRENCYUNIT,
    KeyCurrencySubUnit = SDLK_CURRENCYSUBUNIT,
    KeyKpLeftParen = SDLK_KP_LEFTPAREN,
    KeyKpRightParen = SDLK_KP_RIGHTPAREN,
    KeyKpLeftBrace = SDLK_KP_LEFTBRACE,
    KeyKpRightBrace = SDLK_KP_RIGHTBRACE,
    KeyKpTab = SDLK_KP_TAB,
    KeyKpBackspace = SDLK_KP_BACKSPACE,
    KeyKpA = SDLK_KP_A,
    KeyKpB = SDLK_KP_B,
    KeyKpC = SDLK_KP_C,
    KeyKpD = SDLK_KP_D,
    KeyKpE = SDLK_KP_E,
    KeyKpF = SDLK_KP_F,
    KeyKpXor = SDLK_KP_XOR,
    KeyKpPower = SDLK_KP_POWER,
    KeyKpPercent = SDLK_KP_PERCENT,
    KeyKpLess = SDLK_KP_LESS,
    KeyKpGreater = SDLK_KP_GREATER,
    KeyKpAmpersand = SDLK_KP_AMPERSAND,
    KeyKpDblAmpersand = SDLK_KP_DBLAMPERSAND,
    KeyKpVerticalBar = SDLK_KP_VERTICALBAR,
    KeyKpDblVerticalBar = SDLK_KP_DBLVERTICALBAR,
    KeyKpColon = SDLK_KP_COLON,
    KeyKpHash = SDLK_KP_HASH,
    KeyKpSpace = SDLK_KP_SPACE,
    KeyKpAt = SDLK_KP_AT,
    KeyKpExclam = SDLK_KP_EXCLAM,
    KeyKpMemStore = SDLK_KP_MEMSTORE,
    KeyKpMemRecall = SDLK_KP_MEMRECALL,
    KeyKpMemClear = SDLK_KP_MEMCLEAR,
    KeyKpMemAdd = SDLK_KP_MEMADD,
    KeyKpMemSubtract = SDLK_KP_MEMSUBTRACT,
    KeyKpMemMultiply = SDLK_KP_MEMMULTIPLY,
    KeyKpMemDivide = SDLK_KP_MEMDIVIDE,
    KeyKpPlusMinus = SDLK_KP_PLUSMINUS,
    KeyKpClear = SDLK_KP_CLEAR,
    KeyKpClearEntry = SDLK_KP_CLEARENTRY,
    KeyKpBinary = SDLK_KP_BINARY,
    KeyKpOctal = SDLK_KP_OCTAL,
    KeyKpDecimal = SDLK_KP_DECIMAL,
    KeyKpHexadecimal = SDLK_KP_HEXADECIMAL,
    KeyLCtrl = SDLK_LCTRL,
    KeyLShift = SDLK_LSHIFT,
    KeyLAlt = SDLK_LALT,
    KeyLGui = SDLK_LGUI,
    KeyRCtrl = SDLK_RCTRL,
    KeyRShift = SDLK_RSHIFT,
    KeyRAlt = SDLK_RALT,
    KeyRGui = SDLK_RGUI,
    KeyMode = SDLK_MODE,
    KeyAudioNext = SDLK_AUDIONEXT,
    KeyAudioPrev = SDLK_AUDIOPREV,
    KeyAudioStop = SDLK_AUDIOSTOP,
    KeyAudioPlay = SDLK_AUDIOPLAY,
    KeyAudioMute = SDLK_AUDIOMUTE,
    KeyMediaSelect = SDLK_MEDIASELECT,
    KeyWww = SDLK_WWW,
    KeyMail = SDLK_MAIL,
    KeyCalculator = SDLK_CALCULATOR,
    KeyComputer = SDLK_COMPUTER,
    KeyAcSearch = SDLK_AC_SEARCH,
    KeyAcHome = SDLK_AC_HOME,
    KeyAcBack = SDLK_AC_BACK,
    KeyAcForward = SDLK_AC_FORWARD,
    KeyAcStop = SDLK_AC_STOP,
    KeyAcRefresh = SDLK_AC_REFRESH,
    KeyAcBookmarks = SDLK_AC_BOOKMARKS,
    KeyBrightnessDown = SDLK_BRIGHTNESSDOWN,
    KeyBrightnessUp = SDLK_BRIGHTNESSUP,
    KeyDisplaySwitch = SDLK_DISPLAYSWITCH,
    KeyKbdIllumToggle = SDLK_KBDILLUMTOGGLE,
    KeyKbdIllumDown = SDLK_KBDILLUMDOWN,
    KeyKbdIllumUp = SDLK_KBDILLUMUP,
    KeyEject = SDLK_EJECT,
    KeySleep = SDLK_SLEEP,
    KeyApp1 = SDLK_APP1,
    KeyApp2 = SDLK_APP2,
    KeyAudioRewind = SDLK_AUDIOREWIND,
    KeyAudioFastForward = SDLK_AUDIOFASTFORWARD,
    KeySoftLeft = SDLK_SOFTLEFT,
    KeySoftRight = SDLK_SOFTRIGHT,
    KeyCall = SDLK_CALL,
    KeyEndCall = SDLK_ENDCALL,

// Mouse Button Events
    MouseButtonLeft,
    MouseButtonRight,
    MouseButtonMiddle,
    MouseButtonSideFront,
    MouseButtonSideBack,
    MouseButtonYAxis,
    MouseButtonXAxis,

// Gamepad Button Events
    GamepadA,
    GamepadB,
    GamepadX,
    GamepadY,
    GamepadLeftBumper,
    GamepadRightBumper,
    GamepadBack,
    GamepadStart,
    GamepadGuide,
    GamepadLeftThumb,
    GamepadRightThumb,
    GamepadDPadUp,
    GamepadDPadRight,
    GamepadDPadDown,
    GamepadDPadLeft,
    GamepadLeftThumbstickXAxis,
    GamepadLeftThumbstickYAxis,
    GamepadRightThumbstickXAxis,
    GamepadRightThumbstickYAxis,
};

enum class InputEvent {
    Released = SDL_RELEASED,
    Pressed = SDL_PRESSED,
};



static inline Key GetSDLMouseButtonIndex(uint8_t key)
{
    switch (key)
    {
        case SDL_BUTTON_LEFT: return Key::MouseButtonLeft;
        case SDL_BUTTON_RIGHT: return Key::MouseButtonRight;
        case SDL_BUTTON_MIDDLE: return Key::MouseButtonMiddle;
        case SDL_BUTTON_X1: return Key::MouseButtonSideFront;
        case SDL_BUTTON_X2: return Key::MouseButtonSideBack;
        default:
            return Key::KeyUnknown;
    }
}

static inline bool IsKeyFromWindow(Key key)
{
    switch (key)
    {
    case Key::WindowResize:
    case Key::WindowMinimize:
    case Key::WindowMaximize:
    case Key::WindowClose:
    case Key::WindowMove:
        return true;
    default:
        return false;
    }
}

static inline Key GetWindowKeyFromSDL(uint8_t windowEvent)
{
    switch (windowEvent)
    {
    case SDL_WINDOWEVENT_MOVED: return Key::WindowMove;
    case SDL_WINDOWEVENT_RESIZED: return Key::WindowResize;
    case SDL_WINDOWEVENT_MINIMIZED: return Key::WindowMinimize;
    case SDL_WINDOWEVENT_MAXIMIZED: return Key::WindowMaximize;
    case SDL_WINDOWEVENT_CLOSE: return Key::WindowClose;
    default: return Key::KeyUnknown;
    }
}

static inline bool IsKeyFromKeyboard(Key key)
{
    switch (key)
    {
        case Key::KeyUnknown:
        case Key::KeyReturn:
        case Key::KeyEscape:
        case Key::KeyBackspace:
        case Key::KeyTab:
        case Key::KeySpace:
        case Key::KeyExclaim:
        case Key::KeyQuoteDbl:
        case Key::KeyHash:
        case Key::KeyPercent:
        case Key::KeyDollar:
        case Key::KeyAmpersand:
        case Key::KeyQuote:
        case Key::KeyLeftParen:
        case Key::KeyRightParen:
        case Key::KeyAsterisk:
        case Key::KeyPlus:
        case Key::KeyComma:
        case Key::KeyMinus:
        case Key::KeyPeriod:
        case Key::KeySlash:
        case Key::Key0:
        case Key::Key1:
        case Key::Key2:
        case Key::Key3:
        case Key::Key4:
        case Key::Key5:
        case Key::Key6:
        case Key::Key7:
        case Key::Key8:
        case Key::Key9:
        case Key::KeyColon:
        case Key::KeySemicolon:
        case Key::KeyLess:
        case Key::KeyEquals:
        case Key::KeyGreater:
        case Key::KeyQuestion:
        case Key::KeyAt:
        case Key::KeyLeftBracket:
        case Key::KeyBackslash:
        case Key::KeyRightBracket:
        case Key::KeyCaret:
        case Key::KeyUnderscore:
        case Key::KeyBackquote:
        case Key::KeyA:
        case Key::KeyB:
        case Key::KeyC:
        case Key::KeyD:
        case Key::KeyE:
        case Key::KeyF:
        case Key::KeyG:
        case Key::KeyH:
        case Key::KeyI:
        case Key::KeyJ:
        case Key::KeyK:
        case Key::KeyL:
        case Key::KeyM:
        case Key::KeyN:
        case Key::KeyO:
        case Key::KeyP:
        case Key::KeyQ:
        case Key::KeyR:
        case Key::KeyS:
        case Key::KeyT:
        case Key::KeyU:
        case Key::KeyV:
        case Key::KeyW:
        case Key::KeyX:
        case Key::KeyY:
        case Key::KeyZ:
        case Key::KeyCapsLock:
        case Key::KeyF1:
        case Key::KeyF2:
        case Key::KeyF3:
        case Key::KeyF4:
        case Key::KeyF5:
        case Key::KeyF6:
        case Key::KeyF7:
        case Key::KeyF8:
        case Key::KeyF9:
        case Key::KeyF10:
        case Key::KeyF11:
        case Key::KeyF12:
        case Key::KeyPrintScreen:
        case Key::KeyScrollLock:
        case Key::KeyPause:
        case Key::KeyInsert:
        case Key::KeyHome:
        case Key::KeyPageUp:
        case Key::KeyDelete:
        case Key::KeyEnd:
        case Key::KeyPageDown:
        case Key::KeyRight:
        case Key::KeyLeft:
        case Key::KeyDown:
        case Key::KeyUp:
        case Key::KeyNumlockClear:
        case Key::KeyKpDivide:
        case Key::KeyKpMultiply:
        case Key::KeyKpMinus:
        case Key::KeyKpPlus:
        case Key::KeyKpEnter:
        case Key::KeyKp_1:
        case Key::KeyKp_2:
        case Key::KeyKp_3:
        case Key::KeyKp_4:
        case Key::KeyKp_5:
        case Key::KeyKp_6:
        case Key::KeyKp_7:
        case Key::KeyKp_8:
        case Key::KeyKp_9:
        case Key::KeyKp_0:
        case Key::KeyKpPeriod:
        case Key::KeyApplication:
        case Key::KeyPower:
        case Key::KeyKpEquals:
        case Key::KeyF13:
        case Key::KeyF14:
        case Key::KeyF15:
        case Key::KeyF16:
        case Key::KeyF17:
        case Key::KeyF18:
        case Key::KeyF19:
        case Key::KeyF20:
        case Key::KeyF21:
        case Key::KeyF22:
        case Key::KeyF23:
        case Key::KeyF24:
        case Key::KeyExecute:
        case Key::KeyHelp:
        case Key::KeyMenu:
        case Key::KeySelect:
        case Key::KeyStop:
        case Key::KeyAgain:
        case Key::KeyUndo:
        case Key::KeyCut:
        case Key::KeyCopy:
        case Key::KeyPaste:
        case Key::KeyFind:
        case Key::KeyMute:
        case Key::KeyVolumeUp:
        case Key::KeyVolumeDown:
        case Key::KeyKpComma:
        case Key::KeyKpEqualSas400:
        case Key::KeyAltErase:
        case Key::KeySysReq:
        case Key::KeyCancel:
        case Key::KeyClear:
        case Key::KeyPrior:
        case Key::KeyReturn2:
        case Key::KeySeparator:
        case Key::KeyOut:
        case Key::KeyOper:
        case Key::KeyClearAgain:
        case Key::KeyCrsel:
        case Key::KeyExsel:
        case Key::KeyKp_00:
        case Key::KeyKp_000:
        case Key::KeyThousandsSeparator:
        case Key::KeyDecimalSeparator:
        case Key::KeyCurrencyUnit:
        case Key::KeyCurrencySubUnit:
        case Key::KeyKpLeftParen:
        case Key::KeyKpRightParen:
        case Key::KeyKpLeftBrace:
        case Key::KeyKpRightBrace:
        case Key::KeyKpTab:
        case Key::KeyKpBackspace:
        case Key::KeyKpA:
        case Key::KeyKpB:
        case Key::KeyKpC:
        case Key::KeyKpD:
        case Key::KeyKpE:
        case Key::KeyKpF:
        case Key::KeyKpXor:
        case Key::KeyKpPower:
        case Key::KeyKpPercent:
        case Key::KeyKpLess:
        case Key::KeyKpGreater:
        case Key::KeyKpAmpersand:
        case Key::KeyKpDblAmpersand:
        case Key::KeyKpVerticalBar:
        case Key::KeyKpDblVerticalBar:
        case Key::KeyKpColon:
        case Key::KeyKpHash:
        case Key::KeyKpSpace:
        case Key::KeyKpAt:
        case Key::KeyKpExclam:
        case Key::KeyKpMemStore:
        case Key::KeyKpMemRecall:
        case Key::KeyKpMemClear:
        case Key::KeyKpMemAdd:
        case Key::KeyKpMemSubtract:
        case Key::KeyKpMemMultiply:
        case Key::KeyKpMemDivide:
        case Key::KeyKpPlusMinus:
        case Key::KeyKpClear:
        case Key::KeyKpClearEntry:
        case Key::KeyKpBinary:
        case Key::KeyKpOctal:
        case Key::KeyKpDecimal:
        case Key::KeyKpHexadecimal:
        case Key::KeyLCtrl:
        case Key::KeyLShift:
        case Key::KeyLAlt:
        case Key::KeyLGui:
        case Key::KeyRCtrl:
        case Key::KeyRShift:
        case Key::KeyRAlt:
        case Key::KeyRGui:
        case Key::KeyMode:
        case Key::KeyAudioNext:
        case Key::KeyAudioPrev:
        case Key::KeyAudioStop:
        case Key::KeyAudioPlay:
        case Key::KeyAudioMute:
        case Key::KeyMediaSelect:
        case Key::KeyWww:
        case Key::KeyMail:
        case Key::KeyCalculator:
        case Key::KeyComputer:
        case Key::KeyAcSearch:
        case Key::KeyAcHome:
        case Key::KeyAcBack:
        case Key::KeyAcForward:
        case Key::KeyAcStop:
        case Key::KeyAcRefresh:
        case Key::KeyAcBookmarks:
        case Key::KeyBrightnessDown:
        case Key::KeyBrightnessUp:
        case Key::KeyDisplaySwitch:
        case Key::KeyKbdIllumToggle:
        case Key::KeyKbdIllumDown:
        case Key::KeyKbdIllumUp:
        case Key::KeyEject:
        case Key::KeySleep:
        case Key::KeyApp1:
        case Key::KeyApp2:
        case Key::KeyAudioRewind:
        case Key::KeyAudioFastForward:
        case Key::KeySoftLeft:
        case Key::KeySoftRight:
        case Key::KeyCall:
        case Key::KeyEndCall:
            return true;
        default:
            return false;
    }
} 

} // namespace bl