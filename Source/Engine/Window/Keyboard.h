#pragma once

#include "Precompiled.h"
#include "Core/Flags.h"

namespace bl {

/// @brief Key Modifiers
///
/// Keyboard modifiers are simple ways to change the functionality of keys 
/// by using a modification button. Control, Shift are examples of such keys.
/// The program can change behavior of functionality depending on modifiers.
///
enum class KeyboardModifierFlagBits : uint32_t {
    Ctrl  = 0x00000001,
    Shift = 0x00000002,
    Alt   = 0x00000004,
    Meta  = 0x00000008,
};

using KeyboardModifierFlags = uint32_t;

static inline KeyboardModifierFlags operator|(KeyboardModifierFlags modifiers, KeyboardModifierFlagBits bit) {
    return (uint32_t)modifiers | (uint32_t)bit;
}

static inline KeyboardModifierFlags operator|=(KeyboardModifierFlags modifiers, KeyboardModifierFlagBits bit) {
    return (uint32_t)modifiers |= (uint32_t)bit;
}

/// @brief Keyboard USB Scancodes
///
/// Scancodes pertaining to standard USB positioning of keys. Because they are
/// standardised it makes it easy in games to use them for binding positions.
/// When displaying to the user they should be translated into their literal 
/// names from the system to give an accurate representation of their 
/// keyboard layout. Users can and will have different keyboard layouts other 
/// than QWERTY because of locale or desire.
///
/// To translate a scancode to a key name use Keyboard::ScancodeToLocalKeyName.
/// Here comes a long list of scancodes...
///
enum class Scancode {
    A = 0,
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

    Count,
};

class Keyboard {
public:

    /// @brief Destructor
    virtual ~Keyboard() = default;

    /// @brief Checks if a scancode was pressed between poll time.
    ///
    /// At poll time the keyboard internal class state may change and upade
    /// it's internal state array. Use this function to check if the state
    /// of the keyboard's state has changed.
    ///
    /// @param key The scancode to check.
    ///
    /// @return True if the scancode/key was pressed.
    ///
    virtual bool IsKeyDown(Scancode code) = 0;

    /// @brief Returns the name of the key on the users keyboard.
    /// 
    /// Scancodes are based on position their position cannot be changed
    /// but the usage of the key can change depending on the users keyboard 
    /// layout or locale.
    ///
    /// @param key The key to check.
    ///
    /// @return The name of the key from scancode.
    ///
    virtual std::string ScancodeToLocalKeyName(Scancode key) = 0;
    
    /// @brief An easy way to check what modifiers are currently down.
    ///
    /// @return The modifier bit flags.
    ///
    virtual KeyboardModifierFlags GetKeyModifiers() {
        KeyboardModifierFlags flags;
        if (IsKeyDown(Scancode::LeftCtrl) || IsKeyDown(Scancode::RightCtrl)) 
            flags |= KeyboardModifierFlagBits::Ctrl;
        if (IsKeyDown(Scancode::LeftShift) || IsKeyDown(Scancode::RightShift))
            flags |= KeyboardModifierFlagBits::Shift;
        if (IsKeyDown(Scancode::LeftAlt) || IsKeyDown(Scancode::RightAlt))
            flags |= KeyboardModifierFlagBits::Alt;
        if (IsKeyDown(Scancode::LeftMeta) || IsKeyDown(Scancode::RightMeta))
            flags |= KeyboardModifierFlagBits::Meta;
        return flags;
    }
};

} // namespace bl