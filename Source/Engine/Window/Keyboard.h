#pragma once

#include "Core/Flags.h"

namespace bl {

enum class KeyModifierFlagBits {
    Control   = 0x00000001,
    Shift     = 0x00000002,
    Alternate = 0x00000004,
    Meta      = 0x00000008,
};

using KeyModifierFlags = uint32_t;

/// @brief Keyboard Usb Codes
enum class Scancode {
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    Num1,
    Num2,
    Num3,
    Num4,
    Num5,
    Num6,
    Num7,
    Num8,
    Num9,
    Num0,
    Return,
    Escape,
    Backspace,
    Tab,
    Space,
    Minus,
    Equals,
    LeftBracket,
    RightBracket,
    Backslash,
    NonUsHash, 
    Semicolon,
    Apostrophe,
    Grave,
    Comma,
    Period,
    Slash,
    CapsLock,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    PrintScreen,
    ScrollLock,
    Pause,
    Insert,
    Home,
    PageUp,
    Delete,
    End,
    PageDown,
    Right,
    Left,
    Down,
    Up,
    NumLockClear,
    KeypadDivide,
    KeypadMultiply,
    KeypadMinus,
    KeypadPlus,
    KeypadEnter,
    Keypad1,
    Keypad2,
    Keypad3,
    Keypad4,
    Keypad5,
    Keypad6,
    Keypad7,
    Keypad8,
    Keypad9,
    Keypad0,
    KeypadPeriod,
    NonUsBackSlash,
    Application, 
    Power,
    KeypadEquals,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    F21,
    F22,
    F23,
    F24,
    Execute,
    Help,
    Menu,
    Select,
    Stop, ///!< AC Stop
    Again, ///!< AC Redo/Repeat
    Undo, ///!< AC Undo
    Cut, ///!< AC Cut
    Copy, ///!< AC Copy
    Paste, ///!< AC Paste
    Find, ///!< AC Find
    Mute,
    VolumeUp,
    VolumeDown,
    KeypadComma,
    KeypadEqualsAs400, ///!< Used on AS/400 Keyboards
    International1,
    International2,
    International3, ///!< Yen
    International4,
    International5,
    International6,
    International7,
    International8,
    International9,
    Language1, ///!< Hangul/English toggle
    Language2, ///!< Hanja conversion
    Language3, ///!< Katakana
    Language4, ///!< Hiragana
    Language5, ///!< Zenkaku/Hankaku
    Language6, ///!< reserved
    Language7, ///!< reserved
    Language8, ///!< reserved
    Language9, ///!< reserved
    AltErase, ///!< Erase-Eaze
    SysReq,
    Cancel, ///!< AC Cancel
    Clear,
    Prior,
    Return2,
    Separator,
    Out,
    Oper,
    ClearAgain,
    CrSel,
    ExSel,
    Keypad00,
    Keypad000,
    ThousandsSeparator,
    DecimalSeparator,
    CurrencyUnit,
    CurrencySubunit,
    KeypadLeftParenthesis,
    KeypadRightParenthesis,
    KeypadLeftBrace,
    KeypadRightBrace,
    KeypadTab,
    KeypadBackspace,
    KeypadA,
    KeypadB,
    KeypadC,
    KeypadD,
    KeypadE,
    KeypadF,
    KeypadXor,
    KeypadPower,
    KeypadPercent,
    KeypadLess,
    KeypadGreater,
    KeypadAmpersand,
    KeypadDoubleAmpersand,
    KeypadVerticalBar,
    KeypadDoubleVerticalBar,
    KeypadColon,
    KeypadHash,
    KeypadSpace,
    KeypadAt,
    KeypadExclamation,
    KeypadMemStore,
    KeypadMemRecall,
    KeypadMemClear,
    KeypadMemAdd,
    KeypadMemSubtract,
    KeypadMemMultiply,
    KeypadMemDivide,
    KeypadPlusMinus,
    KeypadClear,
    KeypadClearEntry,
    KeypadBinary,
    KeypadOctal,
    KeypadDecimal,
    KeypadHexadecimal,
    LeftCtrl,
    LeftShift,
    LeftAlt, ///!< alt, option
    LeftMeta, ///!< windows, command (apple), meta
    RightCtrl,
    RightShift,
    RightAlt, ///!< alt gr, option
    RightMeta, ///!< windows, command (apple), meta
    Mode,
    AudioNext,
    AudioPrev,
    AudioStop,
    AudioPlay,
    AudioMute,
    MediaSelect,
    Www, ///!< AL Internet Browser
    Mail,
    Calculator, ///!< AL Calculator
    Computer,
    AppControlSearch, ///!< AC Search
    AppControlHome, ///!< AC Home
    AppControlBack, ///!< AC Back
    AppControlForward, ///!< AC Forward
    AppControlStop, ///!< AC Stop
    AppControlRefresh, ///!< AC Refresh
    AppControlBookmarks, ///!< AC Bookmarks
    BrightnessDown,
    BrightnessUp,
    DisplaySwitch, ///!< display mirroring/dual display switch, video mode switch
    KeyboardIlluminationToggle,
    KeyboardIlluminationDown,
    KeyboardIlluminationUp,
    Eject,
    Sleep, ///!< SC System Sleep
    App1,
    App2,
    AudioRewind,
    AudioFastForward,
};

class Keyboard {
public:
    virtual ~Keyboard() = default;
    virtual bool IsKeyDown(Scancode key) = 0;
    virtual void ScancodeToKeycode();
    virtual KeyModifierFlagBits GetKeyModifiers() = 0;
};

} // namespace bl