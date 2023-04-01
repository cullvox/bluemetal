#pragma once

#include "Math/Vector2.hpp"
#include "Window/Window.hpp"

#include <SDL2/SDL_keycode.h>


namespace bl 
{

enum class Event
{
    eUnknown =                          0x00000000,

    eKey_Return =                       0x00000081,
    eKey_Escape =                       0x00000082,
    eKey_Backspace =                    0x00000083,
    eKey_Tab =                          0x00000084,
    eKey_Space =                        0x00000085,
    eKey_Exclaim =                      0x00000086,
    eKey_QuoteDbl =                     0x00000087,
    eKey_Hash =                         0x00000088,
    eKey_Percent =                      0x00000089,
    eKey_Dollar =                       0x0000008A,
    eKey_Ampersand =                    0x0000008B,
    eKey_Quote =                        0x0000008C,
    eKey_LeftParen =                    0x0000008D,
    eKey_RightParen =                   0x0000008E,
    eKey_Asterisk =                     0x0000008F,
    eKey_Plus =                         0x00000090,
    eKey_Comma =                        0x00000091,
    eKey_Minus =                        0x00000092,
    eKey_Period =                       0x00000093,
    eKey_Slash =                        0x00000094,
    eKey_0 =                            0x00000095,
    eKey_1 =                            0x00000096,
    eKey_2 =                            0x00000097,
    eKey_3 =                            0x00000098,
    eKey_4 =                            0x00000099,
    eKey_5 =                            0x0000009A,
    eKey_6 =                            0x0000009B,
    eKey_7 =                            0x0000009C,
    eKey_8 =                            0x0000009D,
    eKey_9 =                            0x0000009E,
    eKey_Colon =                        0x0000009F,
    eKey_Semicolon =                    0x000000A0,
    eKey_Less =                         0x000000A1,
    eKey_Equals =                       0x000000A2,
    eKey_Greater =                      0x000000A3,
    eKey_Question =                     0x000000A4,
    eKey_At =                           0x000000A5,
    eKey_LeftBracket =                  0x000000A6,
    eKey_Backslash =                    0x000000A7,
    eKey_RightBracket =                 0x000000A8,
    eKey_Caret =                        0x000000A9,
    eKey_Underscore =                   0x000000AA,
    eKey_Backquote =                    0x000000AB,
    eKey_A =                            0x000000AC,
    eKey_B =                            0x000000AD,
    eKey_C =                            0x000000AE,
    eKey_D =                            0x000000AF,
    eKey_E =                            0x000000B0,
    eKey_F =                            0x000000B1,
    eKey_G =                            0x000000B2,
    eKey_H =                            0x000000B3,
    eKey_I =                            0x000000B4,
    eKey_J =                            0x000000B5,
    eKey_K =                            0x000000B6,
    eKey_L =                            0x000000B7,
    eKey_M =                            0x000000B8,
    eKey_N =                            0x000000B9,
    eKey_O =                            0x000000BA,
    eKey_P =                            0x000000BB,
    eKey_Q =                            0x000000BC,
    eKey_R =                            0x000000BD,
    eKey_S =                            0x000000BE,
    eKey_T =                            0x000000BF,
    eKey_U =                            0x000000C0,
    eKey_V =                            0x000000C1,
    eKey_W =                            0x000000C2,
    eKey_X =                            0x000000C3,
    eKey_Y =                            0x000000C4,
    eKey_Z =                            0x000000C5,
    eKey_CapsLock =                     0x000000C6,
    eKey_F1 =                           0x000000C7,
    eKey_F2 =                           0x000000C8,
    eKey_F3 =                           0x000000C9,
    eKey_F4 =                           0x000000CA,
    eKey_F5 =                           0x000000CB,
    eKey_F6 =                           0x000000CC,
    eKey_F7 =                           0x000000CD,
    eKey_F8 =                           0x000000CE,
    eKey_F9 =                           0x000000CF,
    eKey_F10 =                          0x000000D0,
    eKey_F11 =                          0x000000D1,
    eKey_F12 =                          0x000000D2,
    eKey_PrintScreen =                  0x000000D3,
    eKey_ScrollLock =                   0x000000D4,
    eKey_Pause =                        0x000000D5,
    eKey_Insert =                       0x000000D6,
    eKey_Home =                         0x000000D7,
    eKey_PageUp =                       0x000000D8,
    eKey_Delete =                       0x000000D9,
    eKey_End =                          0x000000DA,
    eKey_PageDown =                     0x000000DB,
    eKey_Right =                        0x000000DC,
    eKey_Left =                         0x000000DD,
    eKey_Down =                         0x000000DE,
    eKey_Up =                           0x000000DF,
    eKey_NumlockClear =                 0x000000E0,
    eKey_KpDivide =                     0x000000E1,
    eKey_KpMultiply =                   0x000000E2,
    eKey_KpMinus =                      0x000000E3,
    eKey_KpPlus =                       0x000000E4,
    eKey_KpEnter =                      0x000000E5,
    eKey_Kp_1 =                         0x000000E6,
    eKey_Kp_2 =                         0x000000E7,
    eKey_Kp_3 =                         0x000000E8,
    eKey_Kp_4 =                         0x000000E9,
    eKey_Kp_5 =                         0x000000EA,
    eKey_Kp_6 =                         0x000000EB,
    eKey_Kp_7 =                         0x000000EC,
    eKey_Kp_8 =                         0x000000ED,
    eKey_Kp_9 =                         0x000000EE,
    eKey_Kp_0 =                         0x000000EF,
    eKey_KpPeriod =                     0x000000F0,
    eKey_Application =                  0x000000F1,
    eKey_Power =                        0x000000F2,
    eKey_KpEquals =                     0x000000F3,
    eKey_F13 =                          0x000000F4,
    eKey_F14 =                          0x000000F5,
    eKey_F15 =                          0x000000F6,
    eKey_F16 =                          0x000000F7,
    eKey_F17 =                          0x000000F8,
    eKey_F18 =                          0x000000F9,
    eKey_F19 =                          0x000000FA,
    eKey_F20 =                          0x000000FB,
    eKey_F21 =                          0x000000FC,
    eKey_F22 =                          0x000000FD,
    eKey_F23 =                          0x000000FE,
    eKey_F24 =                          0x000000FF,
    eKey_Execute =                      0x00000100,
    eKey_Help =                         0x00000101,
    eKey_Menu =                         0x00000102,
    eKey_Select =                       0x00000103,
    eKey_Stop =                         0x00000104,
    eKey_Again =                        0x00000105,
    eKey_Undo =                         0x00000106,
    eKey_Cut =                          0x00000107,
    eKey_Copy =                         0x00000108,
    eKey_Paste =                        0x00000109,
    eKey_Find =                         0x0000010A,
    eKey_Mute =                         0x0000010B,
    eKey_VolumeUp =                     0x0000010C,
    eKey_VolumeDown =                   0x0000010D,
    eKey_KpComma =                      0x0000010E,
    eKey_KpEqualSas400 =                0x0000010F,
    eKey_AltErase =                     0x00000110,
    eKey_SysReq =                       0x00000111,
    eKey_Cancel =                       0x00000112,
    eKey_Clear =                        0x00000113,
    eKey_Prior =                        0x00000114,
    eKey_Return2 =                      0x00000115,
    eKey_Separator =                    0x00000116,
    eKey_Out =                          0x00000117,
    eKey_Oper =                         0x00000118,
    eKey_ClearAgain =                   0x00000119,
    eKey_Crsel =                        0x0000011A,
    eKey_Exsel =                        0x0000011B,
    eKey_Kp_00 =                        0x0000011C,
    eKey_Kp_000 =                       0x0000011D,
    eKey_ThousandsSeparator =           0x0000011E,
    eKey_DecimalSeparator =             0x0000011F,
    eKey_CurrencyUnit =                 0x00000120,
    eKey_CurrencySubUnit =              0x00000121,
    eKey_KpLeftParen =                  0x00000122,
    eKey_KpRightParen =                 0x00000123,
    eKey_KpLeftBrace =                  0x00000124,
    eKey_KpRightBrace =                 0x00000125,
    eKey_KpTab =                        0x00000126,
    eKey_KpBackspace =                  0x00000127,
    eKey_KpA =                          0x00000128,
    eKey_KpB =                          0x00000129,
    eKey_KpC =                          0x0000012A,
    eKey_KpD =                          0x0000012B,
    eKey_KpE =                          0x0000012C,
    eKey_KpF =                          0x0000012D,
    eKey_KpXor =                        0x0000012E,
    eKey_KpPower =                      0x0000012F,
    eKey_KpPercent =                    0x00000130,
    eKey_KpLess =                       0x00000131,
    eKey_KpGreater =                    0x00000132,
    eKey_KpAmpersand =                  0x00000133,
    eKey_KpDblAmpersand =               0x00000134,
    eKey_KpVerticalBar =                0x00000135,
    eKey_KpDblVerticalBar =             0x00000136,
    eKey_KpColon =                      0x00000137,
    eKey_KpHash =                       0x00000138,
    eKey_KpSpace =                      0x00000139,
    eKey_KpAt =                         0x0000013A,
    eKey_KpExclam =                     0x0000013B,
    eKey_KpMemStore =                   0x0000013C,
    eKey_KpMemRecall =                  0x0000013D,
    eKey_KpMemClear =                   0x0000013E,
    eKey_KpMemAdd =                     0x0000013F,
    eKey_KpMemSubtract =                0x00000140,
    eKey_KpMemMultiply =                0x00000141,
    eKey_KpMemDivide =                  0x00000142,
    eKey_KpPlusMinus =                  0x00000143,
    eKey_KpClear =                      0x00000144,
    eKey_KpClearEntry =                 0x00000145,
    eKey_KpBinary =                     0x00000146,
    eKey_KpOctal =                      0x00000147,
    eKey_KpDecimal =                    0x00000148,
    eKey_KpHexadecimal =                0x00000149,
    eKey_LCtrl =                        0x0000014A,
    eKey_LShift =                       0x0000014B,
    eKey_LAlt =                         0x0000014C,
    eKey_LGui =                         0x0000014D,
    eKey_RCtrl =                        0x0000014E,
    eKey_RShift =                       0x0000014F,
    eKey_RAlt =                         0x00000150,
    eKey_RGui =                         0x00000151,
    eKey_Mode =                         0x00000152,
    eKey_AudioNext =                    0x00000153,
    eKey_AudioPrev =                    0x00000154,
    eKey_AudioStop =                    0x00000155,
    eKey_AudioPlay =                    0x00000156,
    eKey_AudioMute =                    0x00000157,
    eKey_MediaSelect =                  0x00000158,
    eKey_Www =                          0x00000159,
    eKey_Mail =                         0x0000015A,
    eKey_Calculator =                   0x0000015B,
    eKey_Computer =                     0x0000015C,
    eKey_AcSearch =                     0x0000015D,
    eKey_AcHome =                       0x0000015E,
    eKey_AcBack =                       0x0000015F,
    eKey_AcForward =                    0x00000160,
    eKey_AcStop =                       0x00000161,
    eKey_AcRefresh =                    0x00000162,
    eKey_AcBookmarks =                  0x00000163,
    eKey_BrightnessDown =               0x00000164,
    eKey_BrightnessUp =                 0x00000165,
    eKey_DisplaySwitch =                0x00000166,
    eKey_KbdIllumToggle =               0x00000167,
    eKey_KbdIllumDown =                 0x00000168,
    eKey_KbdIllumUp =                   0x00000169,
    eKey_Eject =                        0x0000016A,
    eKey_Sleep =                        0x0000016B,
    eKey_App1 =                         0x0000016C,
    eKey_App2 =                         0x0000016D,
    eKey_AudioRewind =                  0x0000016E,
    eKey_AudioFastForward =             0x0000016F,
    eKey_SoftLeft =                     0x00000170,
    eKey_SoftRight =                    0x00000171,
    eKey_Call =                         0x00000172,
    eKey_EndCall =                      0x00000173, // Action, Axis
    eKey_StartEnum =                    eKey_Return,
    eKey_EndEnum =                      eKey_EndCall,

    eWindow_Resize =                    0x0001000, // Action, Axis Vector2
    eWindow_Minimize =                  0x0001001, // Action
    eWindow_Maximize =                  0x0001002, // Action
    eWindow_Close =                     0x0001003, // Action
    eWindow_Move =                      0x0001004, // Action, Axis, Vector2
    eWindow_StartEnum =                 eWindow_Resize,
    eWindow_EndEnum =                   eWindow_Move,

// Mouse Events
    eMouse_Left =                       0X0002001, // Action
    eMouse_Right =                      0x0002002, // Action
    eMouse_Middle =                     0x0002003, // Action
    eMouse_SideFront =                  0x0002004, // Action
    eMouse_SideBack =                   0x0002005, // Action
    eMouse_Move =                       0x0002006, // Axis, Vector2


// Gamepad Button Events
    eGamepad_A =                        0x0003001,
    eGamepad_B =                        0x0003002,
    eGamepad_X =                        0x0003003,
    eGamepad_Y =                        0x0003004,
    eGamepad_LeftBumper =               0x0003005,
    eGamepad_RightBumper =              0x0003006,
    eGamepad_Back =                     0x0003007,
    eGamepad_Start =                    0x0003008,
    eGamepad_Guide =                    0x0003009,
    eGamepad_LeftThumb =                0x000300A,
    eGamepad_RightThumb =               0x000300B,
    eGamepad_DPadUp =                   0x000300C,
    eGamepad_DPadRight =                0x000300D,
    eGamepad_DPadDown =                 0x000300E,
    eGamepad_DPadLeft =                 0x000300F,
    eGamepad_LeftThumbstickXAxis =      0x0003010,
    eGamepad_LeftThumbstickYAxis =      0x0003011,
    eGamepad_RightThumbstickXAxis =     0x0003012,
    eGamepad_RightThumbstickYAxis =     0x0003013,
};

enum class InputEvent 
{
    eReleased = SDL_RELEASED,
    ePressed = SDL_PRESSED,
};



static inline Key GetSDLMouseButtonIndex(uint8_t key)
{
    switch (key)
    {
        case SDL_BUTTON_LEFT: return Event::eMouseButton_Left;
        case SDL_BUTTON_RIGHT: return Event::eMouseButton_Right;
        case SDL_BUTTON_MIDDLE: return Event::eMouseButton_Middle;
        case SDL_BUTTON_X1: return Event::eMouseButton_SideFront;
        case SDL_BUTTON_X2: return Event::eMouseButton_SideBack;
        default:
            return Event::eKey_Unknown;
    }
}

static inline bool IsKeyFromWindow(Event key)
{
    switch (key)
    {
    case Event::eWindow_Resize:
    case Event::eWindow_Minimize:
    case Event::eWindow_Maximize:
    case Event::eWindow_Close:
    case Event::eWindow_Move:
        return true;
    default:
        return false;
    }
}

static inline Event GetWindowKeyFromSDL(uint8_t windowEvent)
{
    switch (windowEvent)
    {
    case SDL_WINDOWEVENT_MOVED:     return Event::eWindow_Move;
    case SDL_WINDOWEVENT_RESIZED:   return Event::eWindow_Resize;
    case SDL_WINDOWEVENT_MINIMIZED: return Event::eWindow_Minimize;
    case SDL_WINDOWEVENT_MAXIMIZED: return Event::eWindow_Maximize;
    case SDL_WINDOWEVENT_CLOSE:     return Event::eWindow_Close;
    default:                        return Event::eUnknown;
    }
}

static inline bool IsKeyFromKeyboard(Key key)
{
    switch (key)
    {
        case Event::KeyUnknown:
        case Event::KeyReturn:
        case Event::KeyEscape:
        case Event::KeyBackspace:
        case Event::KeyTab:
        case Event::KeySpace:
        case Event::KeyExclaim:
        case Event::KeyQuoteDbl:
        case Event::KeyHash:
        case Event::KeyPercent:
        case Event::KeyDollar:
        case Event::KeyAmpersand:
        case Event::KeyQuote:
        case Event::KeyLeftParen:
        case Event::KeyRightParen:
        case Event::KeyAsterisk:
        case Event::KeyPlus:
        case Event::KeyComma:
        case Event::KeyMinus:
        case Event::KeyPeriod:
        case Event::KeySlash:
        case Event::Key0:
        case Event::Key1:
        case Event::Key2:
        case Event::Key3:
        case Event::Key4:
        case Event::Key5:
        case Event::Key6:
        case Event::Key7:
        case Event::Key8:
        case Event::Key9:
        case Event::KeyColon:
        case Event::KeySemicolon:
        case Event::KeyLess:
        case Event::KeyEquals:
        case Event::KeyGreater:
        case Event::KeyQuestion:
        case Event::KeyAt:
        case Event::KeyLeftBracket:
        case Event::KeyBackslash:
        case Event::KeyRightBracket:
        case Event::KeyCaret:
        case Event::KeyUnderscore:
        case Event::KeyBackquote:
        case Event::KeyA:
        case Event::KeyB:
        case Event::KeyC:
        case Event::KeyD:
        case Event::KeyE:
        case Event::KeyF:
        case Event::KeyG:
        case Event::KeyH:
        case Event::KeyI:
        case Event::KeyJ:
        case Event::KeyK:
        case Event::KeyL:
        case Event::KeyM:
        case Event::KeyN:
        case Event::KeyO:
        case Event::KeyP:
        case Event::KeyQ:
        case Event::KeyR:
        case Event::KeyS:
        case Event::KeyT:
        case Event::KeyU:
        case Event::KeyV:
        case Event::KeyW:
        case Event::KeyX:
        case Event::KeyY:
        case Event::KeyZ:
        case Event::KeyCapsLock:
        case Event::KeyF1:
        case Event::KeyF2:
        case Event::KeyF3:
        case Event::KeyF4:
        case Event::KeyF5:
        case Event::KeyF6:
        case Event::KeyF7:
        case Event::KeyF8:
        case Event::KeyF9:
        case Event::KeyF10:
        case Event::KeyF11:
        case Event::KeyF12:
        case Event::KeyPrintScreen:
        case Event::KeyScrollLock:
        case Event::KeyPause:
        case Event::KeyInsert:
        case Event::KeyHome:
        case Event::KeyPageUp:
        case Event::KeyDelete:
        case Event::KeyEnd:
        case Event::KeyPageDown:
        case Event::KeyRight:
        case Event::KeyLeft:
        case Event::KeyDown:
        case Event::KeyUp:
        case Event::KeyNumlockClear:
        case Event::KeyKpDivide:
        case Event::KeyKpMultiply:
        case Event::KeyKpMinus:
        case Event::KeyKpPlus:
        case Event::KeyKpEnter:
        case Event::KeyKp_1:
        case Event::KeyKp_2:
        case Event::KeyKp_3:
        case Event::KeyKp_4:
        case Event::KeyKp_5:
        case Event::KeyKp_6:
        case Event::KeyKp_7:
        case Event::KeyKp_8:
        case Event::KeyKp_9:
        case Event::KeyKp_0:
        case Event::KeyKpPeriod:
        case Event::KeyApplication:
        case Event::KeyPower:
        case Event::KeyKpEquals:
        case Event::KeyF13:
        case Event::KeyF14:
        case Event::KeyF15:
        case Event::KeyF16:
        case Event::KeyF17:
        case Event::KeyF18:
        case Event::KeyF19:
        case Event::KeyF20:
        case Event::KeyF21:
        case Event::KeyF22:
        case Event::KeyF23:
        case Event::KeyF24:
        case Event::KeyExecute:
        case Event::KeyHelp:
        case Event::KeyMenu:
        case Event::KeySelect:
        case Event::KeyStop:
        case Event::KeyAgain:
        case Event::KeyUndo:
        case Event::KeyCut:
        case Event::KeyCopy:
        case Event::KeyPaste:
        case Event::KeyFind:
        case Event::KeyMute:
        case Event::KeyVolumeUp:
        case Event::KeyVolumeDown:
        case Event::KeyKpComma:
        case Event::KeyKpEqualSas400:
        case Event::KeyAltErase:
        case Event::KeySysReq:
        case Event::KeyCancel:
        case Event::KeyClear:
        case Event::KeyPrior:
        case Event::KeyReturn2:
        case Event::KeySeparator:
        case Event::KeyOut:
        case Event::KeyOper:
        case Event::KeyClearAgain:
        case Event::KeyCrsel:
        case Event::KeyExsel:
        case Event::KeyKp_00:
        case Event::KeyKp_000:
        case Event::KeyThousandsSeparator:
        case Event::KeyDecimalSeparator:
        case Event::KeyCurrencyUnit:
        case Event::KeyCurrencySubUnit:
        case Event::KeyKpLeftParen:
        case Event::KeyKpRightParen:
        case Event::KeyKpLeftBrace:
        case Event::KeyKpRightBrace:
        case Event::KeyKpTab:
        case Event::KeyKpBackspace:
        case Event::KeyKpA:
        case Event::KeyKpB:
        case Event::KeyKpC:
        case Event::KeyKpD:
        case Event::KeyKpE:
        case Event::KeyKpF:
        case Event::KeyKpXor:
        case Event::KeyKpPower:
        case Event::KeyKpPercent:
        case Event::KeyKpLess:
        case Event::KeyKpGreater:
        case Event::KeyKpAmpersand:
        case Event::KeyKpDblAmpersand:
        case Event::KeyKpVerticalBar:
        case Event::KeyKpDblVerticalBar:
        case Event::KeyKpColon:
        case Event::KeyKpHash:
        case Event::KeyKpSpace:
        case Event::KeyKpAt:
        case Event::KeyKpExclam:
        case Event::KeyKpMemStore:
        case Event::KeyKpMemRecall:
        case Event::KeyKpMemClear:
        case Event::KeyKpMemAdd:
        case Event::KeyKpMemSubtract:
        case Event::KeyKpMemMultiply:
        case Event::KeyKpMemDivide:
        case Event::KeyKpPlusMinus:
        case Event::KeyKpClear:
        case Event::KeyKpClearEntry:
        case Event::KeyKpBinary:
        case Event::KeyKpOctal:
        case Event::KeyKpDecimal:
        case Event::KeyKpHexadecimal:
        case Event::KeyLCtrl:
        case Event::KeyLShift:
        case Event::KeyLAlt:
        case Event::KeyLGui:
        case Event::KeyRCtrl:
        case Event::KeyRShift:
        case Event::KeyRAlt:
        case Event::KeyRGui:
        case Event::KeyMode:
        case Event::KeyAudioNext:
        case Event::KeyAudioPrev:
        case Event::KeyAudioStop:
        case Event::KeyAudioPlay:
        case Event::KeyAudioMute:
        case Event::KeyMediaSelect:
        case Event::KeyWww:
        case Event::KeyMail:
        case Event::KeyCalculator:
        case Event::KeyComputer:
        case Event::KeyAcSearch:
        case Event::KeyAcHome:
        case Event::KeyAcBack:
        case Event::KeyAcForward:
        case Event::KeyAcStop:
        case Event::KeyAcRefresh:
        case Event::KeyAcBookmarks:
        case Event::KeyBrightnessDown:
        case Event::KeyBrightnessUp:
        case Event::KeyDisplaySwitch:
        case Event::KeyKbdIllumToggle:
        case Event::KeyKbdIllumDown:
        case Event::KeyKbdIllumUp:
        case Event::KeyEject:
        case Event::KeySleep:
        case Event::KeyApp1:
        case Event::KeyApp2:
        case Event::KeyAudioRewind:
        case Event::KeyAudioFastForward:
        case Event::KeySoftLeft:
        case Event::KeySoftRight:
        case Event::KeyCall:
        case Event::KeyEndCall:
            return true;
        default:
            return false;
    }
} 

} // namespace bl