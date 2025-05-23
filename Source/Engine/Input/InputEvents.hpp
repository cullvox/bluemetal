#pragma once

#include "Precompiled.h"

namespace bl {

enum class Modifier : uint32_t {

};

enum class InputKey : uint32_t {
    eUnknown = 0x00000000,

    eKeyReturn = 0x00000081,
    eKeyEscape = 0x00000082,
    eKeyBackspace = 0x00000083,
    eKeyTab = 0x00000084,
    eKeySpace = 0x00000085,
    eKeyExclaim = 0x00000086,
    eKeyQuoteDbl = 0x00000087,
    eKeyHash = 0x00000088,
    eKeyPercent = 0x00000089,
    eKeyDollar = 0x0000008A,
    eKeyAmpersand = 0x0000008B,
    eKeyQuote = 0x0000008C,
    eKeyLeftParen = 0x0000008D,
    eKeyRightParen = 0x0000008E,
    eKeyAsterisk = 0x0000008F,
    eKeyPlus = 0x00000090,
    eKeyComma = 0x00000091,
    eKeyMinus = 0x00000092,
    eKeyPeriod = 0x00000093,
    eKeySlash = 0x00000094,
    eKey0 = 0x00000095,
    eKey1 = 0x00000096,
    eKey2 = 0x00000097,
    eKey3 = 0x00000098,
    eKey4 = 0x00000099,
    eKey5 = 0x0000009A,
    eKey6 = 0x0000009B,
    eKey7 = 0x0000009C,
    eKey8 = 0x0000009D,
    eKey9 = 0x0000009E,
    eKeyColon = 0x0000009F,
    eKeySemicolon = 0x000000A0,
    eKeyLess = 0x000000A1,
    eKeyEquals = 0x000000A2,
    eKeyGreater = 0x000000A3,
    eKeyQuestion = 0x000000A4,
    eKeyAt = 0x000000A5,
    eKeyLeftBracket = 0x000000A6,
    eKeyBackslash = 0x000000A7,
    eKeyRightBracket = 0x000000A8,
    eKeyCaret = 0x000000A9,
    eKeyUnderscore = 0x000000AA,
    eKeyBackquote = 0x000000AB,
    eKeyA = 0x000000AC,
    eKeyB = 0x000000AD,
    eKeyC = 0x000000AE,
    eKeyD = 0x000000AF,
    eKeyE = 0x000000B0,
    eKeyF = 0x000000B1,
    eKeyG = 0x000000B2,
    eKeyH = 0x000000B3,
    eKeyI = 0x000000B4,
    eKeyJ = 0x000000B5,
    eKeyK = 0x000000B6,
    eKeyL = 0x000000B7,
    eKeyM = 0x000000B8,
    eKeyN = 0x000000B9,
    eKeyO = 0x000000BA,
    eKeyP = 0x000000BB,
    eKeyQ = 0x000000BC,
    eKeyR = 0x000000BD,
    eKeyS = 0x000000BE,
    eKeyT = 0x000000BF,
    eKeyU = 0x000000C0,
    eKeyV = 0x000000C1,
    eKeyW = 0x000000C2,
    eKeyX = 0x000000C3,
    eKeyY = 0x000000C4,
    eKeyZ = 0x000000C5,
    eKeyCapsLock = 0x000000C6,
    eKeyF1 = 0x000000C7,
    eKeyF2 = 0x000000C8,
    eKeyF3 = 0x000000C9,
    eKeyF4 = 0x000000CA,
    eKeyF5 = 0x000000CB,
    eKeyF6 = 0x000000CC,
    eKeyF7 = 0x000000CD,
    eKeyF8 = 0x000000CE,
    eKeyF9 = 0x000000CF,
    eKeyF10 = 0x000000D0,
    eKeyF11 = 0x000000D1,
    eKeyF12 = 0x000000D2,
    eKeyPrintScreen = 0x000000D3,
    eKeyScrollLock = 0x000000D4,
    eKeyPause = 0x000000D5,
    eKeyInsert = 0x000000D6,
    eKeyHome = 0x000000D7,
    eKeyPageUp = 0x000000D8,
    eKeyDelete = 0x000000D9,
    eKeyEnd = 0x000000DA,
    eKeyPageDown = 0x000000DB,
    eKeyRight = 0x000000DC,
    eKeyLeft = 0x000000DD,
    eKeyDown = 0x000000DE,
    eKeyUp = 0x000000DF,
    eKeyNumLockClear = 0x000000E0,
    eKeyKeypadDivide = 0x000000E1,
    eKeyKeypadMultiply = 0x000000E2,
    eKeyKeypadMinus = 0x000000E3,
    eKeyKeypadPlus = 0x000000E4,
    eKeyKeypadEnter = 0x000000E5,
    eKeyKeypad1 = 0x000000E6,
    eKeyKeypad2 = 0x000000E7,
    eKeyKeypad3 = 0x000000E8,
    eKeyKeypad4 = 0x000000E9,
    eKeyKeypad5 = 0x000000EA,
    eKeyKeypad6 = 0x000000EB,
    eKeyKeypad7 = 0x000000EC,
    eKeyKeypad8 = 0x000000ED,
    eKeyKeypad9 = 0x000000EE,
    eKeyKeypad0 = 0x000000EF,
    eKeyKeypadPeriod = 0x000000F0,
    eKeyApplication = 0x000000F1,
    eKeyPower = 0x000000F2,
    eKeyKeypadEquals = 0x000000F3,
    eKeyF13 = 0x000000F4,
    eKeyF14 = 0x000000F5,
    eKeyF15 = 0x000000F6,
    eKeyF16 = 0x000000F7,
    eKeyF17 = 0x000000F8,
    eKeyF18 = 0x000000F9,
    eKeyF19 = 0x000000FA,
    eKeyF20 = 0x000000FB,
    eKeyF21 = 0x000000FC,
    eKeyF22 = 0x000000FD,
    eKeyF23 = 0x000000FE,
    eKeyF24 = 0x000000FF,
    eKeyExecute = 0x00000100,
    eKeyHelp = 0x00000101,
    eKeyMenu = 0x00000102,
    eKeySelect = 0x00000103,
    eKeyStop = 0x00000104,
    eKeyAgain = 0x00000105,
    eKeyUndo = 0x00000106,
    eKeyCut = 0x00000107,
    eKeyCopy = 0x00000108,
    eKeyPaste = 0x00000109,
    eKeyFind = 0x0000010A,
    eKeyMute = 0x0000010B,
    eKeyVolumeUp = 0x0000010C,
    eKeyVolumeDown = 0x0000010D,
    eKeyKpComma = 0x0000010E,
    eKeyKpEqualSas400 = 0x0000010F,
    eKeyAltErase = 0x00000110,
    eKeySysReq = 0x00000111,
    eKeyCancel = 0x00000112,
    eKeyClear = 0x00000113,
    eKeyPrior = 0x00000114,
    eKeyReturn2 = 0x00000115,
    eKeySeparator = 0x00000116,
    eKeyOut = 0x00000117,
    eKeyOper = 0x00000118,
    eKeyClearAgain = 0x00000119,
    eKeyCrsel = 0x0000011A,
    eKeyExsel = 0x0000011B,
    eKeyKeypad00 = 0x0000011C,
    eKeyKeypad000 = 0x0000011D,
    eKeyThousandsSeparator = 0x0000011E,
    eKeyDecimalSeparator = 0x0000011F,
    eKeyCurrencyUnit = 0x00000120,
    eKeyCurrencySubUnit = 0x00000121,
    eKeyKeypadLeftParen = 0x00000122,
    eKeyKeypadRightParen = 0x00000123,
    eKeyKeypadLeftBrace = 0x00000124,
    eKeyKeypadRightBrace = 0x00000125,
    eKeyKeypadTab = 0x00000126,
    eKeyKeypadBackspace = 0x00000127,
    eKeyKeypadA = 0x00000128,
    eKeyKeypadB = 0x00000129,
    eKeyKeypadC = 0x0000012A,
    eKeyKeypadD = 0x0000012B,
    eKeyKeypadE = 0x0000012C,
    eKeyKeypadF = 0x0000012D,
    eKeyKeypadXor = 0x0000012E,
    eKeyKeypadPower = 0x0000012F,
    eKeyKeypadPercent = 0x00000130,
    eKeyKeypadLess = 0x00000131,
    eKeyKeypadGreater = 0x00000132,
    eKeyKeypadAmpersand = 0x00000133,
    eKeyKeypadDblAmpersand = 0x00000134,
    eKeyKeypadVerticalBar = 0x00000135,
    eKeyKeypadDblVerticalBar = 0x00000136,
    eKeyKeypadColon = 0x00000137,
    eKeyKeypadHash = 0x00000138,
    eKeyKeypadSpace = 0x00000139,
    eKeyKeypadAt = 0x0000013A,
    eKeyKeypadExclam = 0x0000013B,
    eKeyKeypadMemStore = 0x0000013C,
    eKeyKeypadMemRecall = 0x0000013D,
    eKeyKeypadMemClear = 0x0000013E,
    eKeyKeypadMemAdd = 0x0000013F,
    eKeyKeypadMemSubtract = 0x00000140,
    eKeyKeypadMemMultiply = 0x00000141,
    eKeyKeypadMemDivide = 0x00000142,
    eKeyKeypadPlusMinus = 0x00000143,
    eKeyKeypadClear = 0x00000144,
    eKeyKeypadClearEntry = 0x00000145,
    eKeyKeypadBinary = 0x00000146,
    eKeyKeypadOctal = 0x00000147,
    eKeyKeypadDecimal = 0x00000148,
    eKeyKeypadHexadecimal = 0x00000149,
    eKeyLeftCtrl = 0x0000014A,
    eKeyLeftShift = 0x0000014B,
    eKeyLeftAlt = 0x0000014C,
    eKeyLeftGui = 0x0000014D,
    eKeyRightCtrl = 0x0000014E,
    eKeyRightShift = 0x0000014F,
    eKeyRightAlt = 0x00000150,
    eKeyRightGui = 0x00000151,
    eKeyMode = 0x00000152,
    eKeyAudioNext = 0x00000153,
    eKeyAudioPrev = 0x00000154,
    eKeyAudioStop = 0x00000155,
    eKeyAudioPlay = 0x00000156,
    eKeyAudioMute = 0x00000157,
    eKeyMediaSelect = 0x00000158,
    eKeyWWW = 0x00000159,
    eKeyMail = 0x0000015A,
    eKeyCalculator = 0x0000015B,
    eKeyComputer = 0x0000015C,
    eKeyAcSearch = 0x0000015D,
    eKeyAcHome = 0x0000015E,
    eKeyAcBack = 0x0000015F,
    eKeyAcForward = 0x00000160,
    eKeyAcStop = 0x00000161,
    eKeyAcRefresh = 0x00000162,
    eKeyAcBookmarks = 0x00000163,
    eKeyBrightnessDown = 0x00000164,
    eKeyBrightnessUp = 0x00000165,
    eKeyDisplaySwitch = 0x00000166,
    eKeyKbdIllumToggle = 0x00000167,
    eKeyKbdIllumDown = 0x00000168,
    eKeyKbdIllumUp = 0x00000169,
    eKeyEject = 0x0000016A,
    eKeySleep = 0x0000016B,
    eKeyApp1 = 0x0000016C,
    eKeyApp2 = 0x0000016D,
    eKeyAudioRewind = 0x0000016E,
    eKeyAudioFastForward = 0x0000016F,
    eKeySoftLeft = 0x00000170,
    eKeySoftRight = 0x00000171,
    eKeyCall = 0x00000172,
    eKeyEndCall = 0x00000173,

    // Mouse Button Event
    eMouseButtonLeft = 0X0002001,
    eMouseButtonRight = 0x0002002,
    eMouseButtonMiddle = 0x0002003,
    eMouseButtonSideFront = 0x0002004,
    eMouseButtonSideBack = 0x0002005,

    // Mouse Analog Events
    eMouseMove = 0x0003000,
    eMouseScroll = 0x0003001,

    // Gamepad Button Events
    eGamepadA = 0x0004001,
    eGamepadB = 0x0004002,
    eGamepadX = 0x0004003,
    eGamepadY = 0x0004004,
    eGamepadLeftBumper = 0x0004005,
    eGamepadRightBumper = 0x0004006,
    eGamepadBack = 0x0004007,
    eGamepadStart = 0x0004008,
    eGamepadGuide = 0x0004009,
    eGamepadLeftThumb = 0x000400A,
    eGamepadRightThumb = 0x000400B,
    eGamepadPadUp = 0x000400C,
    eGamepadPadRight = 0x000400D,
    eGamepadPadDown = 0x000400E,
    eGamepadPadLeft = 0x000400F,

    // Gamepad Analog
    eGamepadLeftThumbstick = 0x00005000,
    eGamepadRightThumbstick = 0x00005001,
    eGamepadLeftTrigger = 0x00006001,
    eGamepadRightTrigger = 0x00006002,
};

enum class ButtonState : uint32_t {
    Released = 0x00000000,
    Pressed = 0x00000001,
};

} // namespace bl