#ifndef __WINDOWS_HPP__
#define __WINDOWS_HPP__

#pragma comment(lib, "libcmt.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "d3d11.lib")

#include "core.hpp"

#define WINAPI      __stdcall
#define WINCALLBACK __stdcall
#define DLLIMPORT no_mangle __declspec(dllimport)

typedef struct HWND__ {
	int unused;
}* Handle;
typedef Handle HModule;
typedef Handle HInstance;
typedef Handle HWnd;
typedef Handle HMonitor;
typedef Handle HIcon;
typedef Handle HCursor;
typedef Handle HBrush;
typedef Handle HMenu;

typedef s32 HResult;
#define SUCCEEDED(result) ((bool)((HResult)(result) >= 0))
#define FAILED(   result) ((bool)((HResult)(result) <  0))

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-exitprocess
 */
DLLIMPORT void WINAPI ExitProcess(
	u32 exit_code
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/windef/ns-windef-point
 */
struct Point {
	s32 x;
	s32 y;
};

#define MONITOR_DEFAULTTONULL       0x00000000
#define MONITOR_DEFAULTTOPRIMARY    0x00000001
#define MONITOR_DEFAULTTONEAREST    0x00000002

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-monitorfrompoint
 */
DLLIMPORT HMonitor MonitorFromPoint(
	Point point,
	u32   flags
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nc-winuser-wndproc
 */
typedef s64 (WINCALLBACK* WndProc)(HWnd window, u32 msg, u64 w_param, s64 l_param);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassexw
 */
struct WndClassExW {
	u32             size;
	u32             style;
	WndProc         wnd_proc;
	s32             cls_extra;
	s32             wnd_extra;
	HInstance       instance;
	HIcon           icon;
	HCursor         cursor;
	HBrush          background;
	const wchar_t*  menu_name;
	const wchar_t*  class_name;
	HIcon           icon_sm;
};

/**
 * Class Styles
 */

#define CS_VREDRAW         0x00000001
#define CS_HREDRAW         0x00000002
#define CS_DBLCLKS         0x00000008
#define CS_OWNDC           0x00000020
#define CS_CLASSDC         0x00000040
#define CS_PARENTDC        0x00000080
#define CS_NOCLOSE         0x00000200
#define CS_SAVEBITS        0x00000800
#define CS_BYTEALIGNCLIENT 0x00001000
#define CS_BYTEALIGNWINDOW 0x00002000
#define CS_GLOBALCLASS     0x00004000
#define CS_IME             0x00010000
#define CS_DROPSHADOW      0x00020000

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassexw
 */
DLLIMPORT u16 WINAPI RegisterClassExW(WndClassExW* wnd_class);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
 */
DLLIMPORT HWnd WINAPI CreateWindowExW(
	u32            ex_style,
	const wchar_t* class_name,
	const wchar_t* window_name,
	u32            style,
	s32            x,
	s32            y,
	s32            width,
	s32            height,
	HWnd           wnd_parent,
	HMenu          menu,
	HInstance      instance,
	void*          param
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-destroywindow
 */
DLLIMPORT b32 WINAPI DestroyWindow(
  HWnd window
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-postquitmessage
 */
DLLIMPORT void WINAPI PostQuitMessage(
	int exit_code
);

/**
 * Window Styles
 */

#define WS_OVERLAPPED       0x00000000L
#define WS_POPUP            0x80000000L
#define WS_CHILD            0x40000000L
#define WS_MINIMIZE         0x20000000L
#define WS_VISIBLE          0x10000000L
#define WS_DISABLED         0x08000000L
#define WS_CLIPSIBLINGS     0x04000000L
#define WS_CLIPCHILDREN     0x02000000L
#define WS_MAXIMIZE         0x01000000L
#define WS_BORDER           0x00800000L
#define WS_DLGFRAME         0x00400000L
#define WS_CAPTION          (WS_BORDER | WS_DLGFRAME)
#define WS_VSCROLL          0x00200000L
#define WS_HSCROLL          0x00100000L
#define WS_SYSMENU          0x00080000L
#define WS_THICKFRAME       0x00040000L
#define WS_GROUP            0x00020000L
#define WS_TABSTOP          0x00010000L
#define WS_MINIMIZEBOX      0x00020000L
#define WS_MAXIMIZEBOX      0x00010000L
#define WS_TILED            WS_OVERLAPPED
#define WS_ICONIC           WS_MINIMIZE
#define WS_SIZEBOX          WS_THICKFRAME
#define WS_OVERLAPPEDWINDOW ( \
	WS_OVERLAPPED  |          \
	WS_CAPTION     |          \
	WS_SYSMENU     |          \
	WS_THICKFRAME  |          \
	WS_MINIMIZEBOX |          \
	WS_MAXIMIZEBOX            \
)
#define WS_TILEDWINDOW      WS_OVERLAPPEDWINDOW
#define WS_POPUPWINDOW      ( \
	WS_POPUP  |               \
	WS_BORDER |               \
	WS_SYSMENU                \
)
#define WS_CHILDWINDOW          (WS_CHILD)
#define WS_EX_DLGMODALFRAME     0x00000001L
#define WS_EX_NOPARENTNOTIFY    0x00000004L
#define WS_EX_TOPMOST           0x00000008L
#define WS_EX_ACCEPTFILES       0x00000010L
#define WS_EX_TRANSPARENT       0x00000020L
#define WS_EX_MDICHILD          0x00000040L
#define WS_EX_TOOLWINDOW        0x00000080L
#define WS_EX_WINDOWEDGE        0x00000100L
#define WS_EX_CLIENTEDGE        0x00000200L
#define WS_EX_CONTEXTHELP       0x00000400L
#define WS_EX_RIGHT             0x00001000L
#define WS_EX_LEFT              0x00000000L
#define WS_EX_RTLREADING        0x00002000L
#define WS_EX_LTRREADING        0x00000000L
#define WS_EX_LEFTSCROLLBAR     0x00004000L
#define WS_EX_RIGHTSCROLLBAR    0x00000000L
#define WS_EX_CONTROLPARENT     0x00010000L
#define WS_EX_STATICEDGE        0x00020000L
#define WS_EX_APPWINDOW         0x00040000L
#define WS_EX_OVERLAPPEDWINDOW  (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE)
#define WS_EX_PALETTEWINDOW     (WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)
#define WS_EX_LAYERED           0x00080000
#define WS_EX_NOINHERITLAYOUT   0x00100000L // Disable inheritence of mirroring by children
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#define WS_EX_LAYOUTRTL         0x00400000L // Right to left mirroring
#define WS_EX_COMPOSITED        0x02000000L
#define WS_EX_NOACTIVATE        0x08000000L

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow
 */
enum ShowWindowCommand {
	ShowWindowCommand_HIDE            = 0,
	ShowWindowCommand_SHOWNORMAL      = 1,
	ShowWindowCommand_NORMAL          = 1,
	ShowWindowCommand_SHOWMINIMIZED   = 2,
	ShowWindowCommand_SHOWMAXIMIZED   = 3,
	ShowWindowCommand_MAXIMIZE        = 3,
	ShowWindowCommand_SHOWNOACTIVATE  = 4,
	ShowWindowCommand_SHOW            = 5,
	ShowWindowCommand_MINIMIZE        = 6,
	ShowWindowCommand_SHOWMINNOACTIVE = 7,
	ShowWindowCommand_SHOWNA          = 8,
	ShowWindowCommand_RESTORE         = 9,
	ShowWindowCommand_SHOWDEFAULT     = 10,
	ShowWindowCommand_FORCEMINIMIZE   = 11,
	ShowWindowCommand_MAX             = 11,
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow
 */
DLLIMPORT b32 WINAPI ShowWindow(
	HWnd              window,
	ShowWindowCommand cmd_show
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-updatewindow
 */
DLLIMPORT b32 WINAPI UpdateWindow(
	HWnd window
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-msg
 */
struct Msg {
  HWnd  wnd;
  u32   message;
  u64   w_param;
  s64   l_param;
  u32   time;
  Point pt;
  u32   l_private;
};

/*
 * PeekMessage remove options
 */

#define PM_NOREMOVE         0x0000
#define PM_REMOVE           0x0001
#define PM_NOYIELD          0x0002

/**
 * Window Messages
 */

#define WM_NULL                           0x0000
#define WM_CREATE                         0x0001
#define WM_DESTROY                        0x0002
#define WM_MOVE                           0x0003
#define WM_SIZE                           0x0005
#define WM_ACTIVATE                       0x0006
#define WM_SETFOCUS                       0x0007
#define WM_KILLFOCUS                      0x0008
#define WM_ENABLE                         0x000A
#define WM_SETREDRAW                      0x000B
#define WM_SETTEXT                        0x000C
#define WM_GETTEXT                        0x000D
#define WM_GETTEXTLENGTH                  0x000E
#define WM_PAINT                          0x000F
#define WM_CLOSE                          0x0010
#define WM_QUERYENDSESSION                0x0011
#define WM_QUERYOPEN                      0x0013
#define WM_ENDSESSION                     0x0016
#define WM_QUIT                           0x0012
#define WM_ERASEBKGND                     0x0014
#define WM_SYSCOLORCHANGE                 0x0015
#define WM_SHOWWINDOW                     0x0018
#define WM_WININICHANGE                   0x001A
#define WM_SETTINGCHANGE                  WM_WININICHANGE
#define WM_DEVMODECHANGE                  0x001B
#define WM_ACTIVATEAPP                    0x001C
#define WM_FONTCHANGE                     0x001D
#define WM_TIMECHANGE                     0x001E
#define WM_CANCELMODE                     0x001F
#define WM_SETCURSOR                      0x0020
#define WM_MOUSEACTIVATE                  0x0021
#define WM_CHILDACTIVATE                  0x0022
#define WM_QUEUESYNC                      0x0023
#define WM_GETMINMAXINFO                  0x0024
#define WM_PAINTICON                      0x0026
#define WM_ICONERASEBKGND                 0x0027
#define WM_NEXTDLGCTL                     0x0028
#define WM_SPOOLERSTATUS                  0x002A
#define WM_DRAWITEM                       0x002B
#define WM_MEASUREITEM                    0x002C
#define WM_DELETEITEM                     0x002D
#define WM_VKEYTOITEM                     0x002E
#define WM_CHARTOITEM                     0x002F
#define WM_SETFONT                        0x0030
#define WM_GETFONT                        0x0031
#define WM_SETHOTKEY                      0x0032
#define WM_GETHOTKEY                      0x0033
#define WM_QUERYDRAGICON                  0x0037
#define WM_COMPAREITEM                    0x0039
#define WM_GETOBJECT                      0x003D
#define WM_COMPACTING                     0x0041
#define WM_COMMNOTIFY                     0x0044  /* no longer suported */
#define WM_WINDOWPOSCHANGING              0x0046
#define WM_WINDOWPOSCHANGED               0x0047
#define WM_POWER                          0x0048
#define WM_COPYDATA                       0x004A
#define WM_CANCELJOURNAL                  0x004B
#define WM_NOTIFY                         0x004E
#define WM_INPUTLANGCHANGEREQUEST         0x0050
#define WM_INPUTLANGCHANGE                0x0051
#define WM_TCARD                          0x0052
#define WM_HELP                           0x0053
#define WM_USERCHANGED                    0x0054
#define WM_NOTIFYFORMAT                   0x0055
#define WM_CONTEXTMENU                    0x007B
#define WM_STYLECHANGING                  0x007C
#define WM_STYLECHANGED                   0x007D
#define WM_DISPLAYCHANGE                  0x007E
#define WM_GETICON                        0x007F
#define WM_SETICON                        0x0080
#define WM_NCCREATE                       0x0081
#define WM_NCDESTROY                      0x0082
#define WM_NCCALCSIZE                     0x0083
#define WM_NCHITTEST                      0x0084
#define WM_NCPAINT                        0x0085
#define WM_NCACTIVATE                     0x0086
#define WM_GETDLGCODE                     0x0087
#define WM_SYNCPAINT                      0x0088
#define WM_NCMOUSEMOVE                    0x00A0
#define WM_NCLBUTTONDOWN                  0x00A1
#define WM_NCLBUTTONUP                    0x00A2
#define WM_NCLBUTTONDBLCLK                0x00A3
#define WM_NCRBUTTONDOWN                  0x00A4
#define WM_NCRBUTTONUP                    0x00A5
#define WM_NCRBUTTONDBLCLK                0x00A6
#define WM_NCMBUTTONDOWN                  0x00A7
#define WM_NCMBUTTONUP                    0x00A8
#define WM_NCMBUTTONDBLCLK                0x00A9
#define WM_NCXBUTTONDOWN                  0x00AB
#define WM_NCXBUTTONUP                    0x00AC
#define WM_NCXBUTTONDBLCLK                0x00AD
#define WM_INPUT_DEVICE_CHANGE            0x00FE
#define WM_INPUT                          0x00FF
#define WM_KEYFIRST                       0x0100
#define WM_KEYDOWN                        0x0100
#define WM_KEYUP                          0x0101
#define WM_CHAR                           0x0102
#define WM_DEADCHAR                       0x0103
#define WM_SYSKEYDOWN                     0x0104
#define WM_SYSKEYUP                       0x0105
#define WM_SYSCHAR                        0x0106
#define WM_SYSDEADCHAR                    0x0107
#define WM_UNICHAR                        0x0109
// TODO(hhammon) Figure out what the hell happened here
// #define WM_KEYLAST                        0x0109
// #define WM_KEYLAST                        0x0108
#define WM_IME_STARTCOMPOSITION           0x010D
#define WM_IME_ENDCOMPOSITION             0x010E
#define WM_IME_COMPOSITION                0x010F
#define WM_IME_KEYLAST                    0x010F
#define WM_INITDIALOG                     0x0110
#define WM_COMMAND                        0x0111
#define WM_SYSCOMMAND                     0x0112
#define WM_TIMER                          0x0113
#define WM_HSCROLL                        0x0114
#define WM_VSCROLL                        0x0115
#define WM_INITMENU                       0x0116
#define WM_INITMENUPOPUP                  0x0117
#define WM_GESTURE                        0x0119
#define WM_GESTURENOTIFY                  0x011A
#define WM_MENUSELECT                     0x011F
#define WM_MENUCHAR                       0x0120
#define WM_ENTERIDLE                      0x0121
#define WM_MENURBUTTONUP                  0x0122
#define WM_MENUDRAG                       0x0123
#define WM_MENUGETOBJECT                  0x0124
#define WM_UNINITMENUPOPUP                0x0125
#define WM_MENUCOMMAND                    0x0126
#define WM_CHANGEUISTATE                  0x0127
#define WM_UPDATEUISTATE                  0x0128
#define WM_QUERYUISTATE                   0x0129
#define WM_CTLCOLORMSGBOX                 0x0132
#define WM_CTLCOLOREDIT                   0x0133
#define WM_CTLCOLORLISTBOX                0x0134
#define WM_CTLCOLORBTN                    0x0135
#define WM_CTLCOLORDLG                    0x0136
#define WM_CTLCOLORSCROLLBAR              0x0137
#define WM_CTLCOLORSTATIC                 0x0138
#define WM_MOUSEFIRST                     0x0200
#define WM_MOUSEMOVE                      0x0200
#define WM_LBUTTONDOWN                    0x0201
#define WM_LBUTTONUP                      0x0202
#define WM_LBUTTONDBLCLK                  0x0203
#define WM_RBUTTONDOWN                    0x0204
#define WM_RBUTTONUP                      0x0205
#define WM_RBUTTONDBLCLK                  0x0206
#define WM_MBUTTONDOWN                    0x0207
#define WM_MBUTTONUP                      0x0208
#define WM_MBUTTONDBLCLK                  0x0209
#define WM_MOUSEWHEEL                     0x020A
#define WM_XBUTTONDOWN                    0x020B
#define WM_XBUTTONUP                      0x020C
#define WM_XBUTTONDBLCLK                  0x020D
#define WM_MOUSEHWHEEL                    0x020E
// TODO(hhammon) Figure out what the hell happened here
// #define WM_MOUSELAST                      0x020E
// #define WM_MOUSELAST                      0x020D
// #define WM_MOUSELAST                      0x020A
// #define WM_MOUSELAST                      0x0209
#define WM_PARENTNOTIFY                   0x0210
#define WM_ENTERMENULOOP                  0x0211
#define WM_EXITMENULOOP                   0x0212
#define WM_NEXTMENU                       0x0213
#define WM_SIZING                         0x0214
#define WM_CAPTURECHANGED                 0x0215
#define WM_MOVING                         0x0216
#define WM_POWERBROADCAST                 0x0218
#define WM_DEVICECHANGE                   0x0219
#define WM_MDICREATE                      0x0220
#define WM_MDIDESTROY                     0x0221
#define WM_MDIACTIVATE                    0x0222
#define WM_MDIRESTORE                     0x0223
#define WM_MDINEXT                        0x0224
#define WM_MDIMAXIMIZE                    0x0225
#define WM_MDITILE                        0x0226
#define WM_MDICASCADE                     0x0227
#define WM_MDIICONARRANGE                 0x0228
#define WM_MDIGETACTIVE                   0x0229
#define WM_MDISETMENU                     0x0230
#define WM_ENTERSIZEMOVE                  0x0231
#define WM_EXITSIZEMOVE                   0x0232
#define WM_DROPFILES                      0x0233
#define WM_MDIREFRESHMENU                 0x0234
#define WM_POINTERDEVICECHANGE            0x238
#define WM_POINTERDEVICEINRANGE           0x239
#define WM_POINTERDEVICEOUTOFRANGE        0x23A
#define WM_TOUCH                          0x0240
#define WM_NCPOINTERUPDATE                0x0241
#define WM_NCPOINTERDOWN                  0x0242
#define WM_NCPOINTERUP                    0x0243
#define WM_POINTERUPDATE                  0x0245
#define WM_POINTERDOWN                    0x0246
#define WM_POINTERUP                      0x0247
#define WM_POINTERENTER                   0x0249
#define WM_POINTERLEAVE                   0x024A
#define WM_POINTERACTIVATE                0x024B
#define WM_POINTERCAPTURECHANGED          0x024C
#define WM_TOUCHHITTESTING                0x024D
#define WM_POINTERWHEEL                   0x024E
#define WM_POINTERHWHEEL                  0x024F
#define WM_POINTERROUTEDTO                0x0251
#define WM_POINTERROUTEDAWAY              0x0252
#define WM_POINTERROUTEDRELEASED          0x0253
#define WM_IME_SETCONTEXT                 0x0281
#define WM_IME_NOTIFY                     0x0282
#define WM_IME_CONTROL                    0x0283
#define WM_IME_COMPOSITIONFULL            0x0284
#define WM_IME_SELECT                     0x0285
#define WM_IME_CHAR                       0x0286
#define WM_IME_REQUEST                    0x0288
#define WM_IME_KEYDOWN                    0x0290
#define WM_IME_KEYUP                      0x0291
#define WM_MOUSEHOVER                     0x02A1
#define WM_MOUSELEAVE                     0x02A3
#define WM_NCMOUSEHOVER                   0x02A0
#define WM_NCMOUSELEAVE                   0x02A2
#define WM_WTSSESSION_CHANGE              0x02B1
#define WM_TABLET_FIRST                   0x02c0
#define WM_TABLET_LAST                    0x02df
#define WM_DPICHANGED                     0x02E0
#define WM_DPICHANGED_BEFOREPARENT        0x02E2
#define WM_DPICHANGED_AFTERPARENT         0x02E3
#define WM_GETDPISCALEDSIZE               0x02E4
#define WM_CUT                            0x0300
#define WM_COPY                           0x0301
#define WM_PASTE                          0x0302
#define WM_CLEAR                          0x0303
#define WM_UNDO                           0x0304
#define WM_RENDERFORMAT                   0x0305
#define WM_RENDERALLFORMATS               0x0306
#define WM_DESTROYCLIPBOARD               0x0307
#define WM_DRAWCLIPBOARD                  0x0308
#define WM_PAINTCLIPBOARD                 0x0309
#define WM_VSCROLLCLIPBOARD               0x030A
#define WM_SIZECLIPBOARD                  0x030B
#define WM_ASKCBFORMATNAME                0x030C
#define WM_CHANGECBCHAIN                  0x030D
#define WM_HSCROLLCLIPBOARD               0x030E
#define WM_QUERYNEWPALETTE                0x030F
#define WM_PALETTEISCHANGING              0x0310
#define WM_PALETTECHANGED                 0x0311
#define WM_HOTKEY                         0x0312
#define WM_PRINT                          0x0317
#define WM_PRINTCLIENT                    0x0318
#define WM_APPCOMMAND                     0x0319
#define WM_THEMECHANGED                   0x031A
#define WM_CLIPBOARDUPDATE                0x031D
#define WM_DWMCOMPOSITIONCHANGED          0x031E
#define WM_DWMNCRENDERINGCHANGED          0x031F
#define WM_DWMCOLORIZATIONCOLORCHANGED    0x0320
#define WM_DWMWINDOWMAXIMIZEDCHANGE       0x0321
#define WM_DWMSENDICONICTHUMBNAIL         0x0323
#define WM_DWMSENDICONICLIVEPREVIEWBITMAP 0x0326
#define WM_GETTITLEBARINFOEX              0x033F
#define WM_HANDHELDFIRST                  0x0358
#define WM_HANDHELDLAST                   0x035F
#define WM_AFXFIRST                       0x0360
#define WM_AFXLAST                        0x037F
#define WM_PENWINFIRST                    0x0380
#define WM_PENWINLAST                     0x038F
#define WM_APP                            0x8000
#define WM_USER                           0x0400
#define WM_INTERCEPTED_WINDOW_ACTION      0x0346
#define WM_TOOLTIPDISMISS                 0x0345
#define WM_CLOAKED_STATE_CHANGED          0x0347

/*
 * WM_SIZE message wParam values
 */
#define SIZE_RESTORED  0
#define SIZE_MINIMIZED 1
#define SIZE_MAXIMIZED 2
#define SIZE_MAXSHOW   3
#define SIZE_MAXHIDE   4

/*
 * System Menu Command Values
 */
#define SC_SIZE         0xF000
#define SC_MOVE         0xF010
#define SC_MINIMIZE     0xF020
#define SC_MAXIMIZE     0xF030
#define SC_NEXTWINDOW   0xF040
#define SC_PREVWINDOW   0xF050
#define SC_CLOSE        0xF060
#define SC_VSCROLL      0xF070
#define SC_HSCROLL      0xF080
#define SC_MOUSEMENU    0xF090
#define SC_KEYMENU      0xF100
#define SC_ARRANGE      0xF110
#define SC_RESTORE      0xF120
#define SC_TASKLIST     0xF130
#define SC_SCREENSAVE   0xF140
#define SC_HOTKEY       0xF150
#define SC_DEFAULT      0xF160
#define SC_MONITORPOWER 0xF170
#define SC_CONTEXTHELP  0xF180
#define SC_SEPARATOR    0xF00F

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-peekmessagea
 */
DLLIMPORT b32 WINAPI PeekMessageW(
	Msg* msg,
	HWnd wnd,
	u32  msg_filter_min,
	u32  msg_filter_max,
	u32  remove_msg
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-translatemessage
 */
DLLIMPORT b32 WINAPI TranslateMessage(
	Msg* msg
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-dispatchmessagew
 */
DLLIMPORT s64 WINAPI DispatchMessageW(
	Msg* msg
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-defwindowprocw
 */
DLLIMPORT s64 WINAPI DefWindowProcW(
	HWnd wnd,
	u32  msg,
	u64  w_param,
	s64  l_param
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-sleep
 */
DLLIMPORT void WINAPI Sleep(
	u32 milliseconds
);

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgicommon/ns-dxgicommon-dxgi_rational
 */
struct DxgiRational {
	u32 numerator;
	u32 denominator;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format
 */
enum DxgiFormat : s32 {
	DxgiFormat_UNKNOWN                                 = 0,
	DxgiFormat_R32G32B32A32_TYPELESS                   = 1,
	DxgiFormat_R32G32B32A32_FLOAT                      = 2,
	DxgiFormat_R32G32B32A32_UINT                       = 3,
	DxgiFormat_R32G32B32A32_SINT                       = 4,
	DxgiFormat_R32G32B32_TYPELESS                      = 5,
	DxgiFormat_R32G32B32_FLOAT                         = 6,
	DxgiFormat_R32G32B32_UINT                          = 7,
	DxgiFormat_R32G32B32_SINT                          = 8,
	DxgiFormat_R16G16B16A16_TYPELESS                   = 9,
	DxgiFormat_R16G16B16A16_FLOAT                      = 10,
	DxgiFormat_R16G16B16A16_UNORM                      = 11,
	DxgiFormat_R16G16B16A16_UINT                       = 12,
	DxgiFormat_R16G16B16A16_SNORM                      = 13,
	DxgiFormat_R16G16B16A16_SINT                       = 14,
	DxgiFormat_R32G32_TYPELESS                         = 15,
	DxgiFormat_R32G32_FLOAT                            = 16,
	DxgiFormat_R32G32_UINT                             = 17,
	DxgiFormat_R32G32_SINT                             = 18,
	DxgiFormat_R32G8X24_TYPELESS                       = 19,
	DxgiFormat_D32_FLOAT_S8X24_UINT                    = 20,
	DxgiFormat_R32_FLOAT_X8X24_TYPELESS                = 21,
	DxgiFormat_X32_TYPELESS_G8X24_UINT                 = 22,
	DxgiFormat_R10G10B10A2_TYPELESS                    = 23,
	DxgiFormat_R10G10B10A2_UNORM                       = 24,
	DxgiFormat_R10G10B10A2_UINT                        = 25,
	DxgiFormat_R11G11B10_FLOAT                         = 26,
	DxgiFormat_R8G8B8A8_TYPELESS                       = 27,
	DxgiFormat_R8G8B8A8_UNORM                          = 28,
	DxgiFormat_R8G8B8A8_UNORM_SRGB                     = 29,
	DxgiFormat_R8G8B8A8_UINT                           = 30,
	DxgiFormat_R8G8B8A8_SNORM                          = 31,
	DxgiFormat_R8G8B8A8_SINT                           = 32,
	DxgiFormat_R16G16_TYPELESS                         = 33,
	DxgiFormat_R16G16_FLOAT                            = 34,
	DxgiFormat_R16G16_UNORM                            = 35,
	DxgiFormat_R16G16_UINT                             = 36,
	DxgiFormat_R16G16_SNORM                            = 37,
	DxgiFormat_R16G16_SINT                             = 38,
	DxgiFormat_R32_TYPELESS                            = 39,
	DxgiFormat_D32_FLOAT                               = 40,
	DxgiFormat_R32_FLOAT                               = 41,
	DxgiFormat_R32_UINT                                = 42,
	DxgiFormat_R32_SINT                                = 43,
	DxgiFormat_R24G8_TYPELESS                          = 44,
	DxgiFormat_D24_UNORM_S8_UINT                       = 45,
	DxgiFormat_R24_UNORM_X8_TYPELESS                   = 46,
	DxgiFormat_X24_TYPELESS_G8_UINT                    = 47,
	DxgiFormat_R8G8_TYPELESS                           = 48,
	DxgiFormat_R8G8_UNORM                              = 49,
	DxgiFormat_R8G8_UINT                               = 50,
	DxgiFormat_R8G8_SNORM                              = 51,
	DxgiFormat_R8G8_SINT                               = 52,
	DxgiFormat_R16_TYPELESS                            = 53,
	DxgiFormat_R16_FLOAT                               = 54,
	DxgiFormat_D16_UNORM                               = 55,
	DxgiFormat_R16_UNORM                               = 56,
	DxgiFormat_R16_UINT                                = 57,
	DxgiFormat_R16_SNORM                               = 58,
	DxgiFormat_R16_SINT                                = 59,
	DxgiFormat_R8_TYPELESS                             = 60,
	DxgiFormat_R8_UNORM                                = 61,
	DxgiFormat_R8_UINT                                 = 62,
	DxgiFormat_R8_SNORM                                = 63,
	DxgiFormat_R8_SINT                                 = 64,
	DxgiFormat_A8_UNORM                                = 65,
	DxgiFormat_R1_UNORM                                = 66,
	DxgiFormat_R9G9B9E5_SHAREDEXP                      = 67,
	DxgiFormat_R8G8_B8G8_UNORM                         = 68,
	DxgiFormat_G8R8_G8B8_UNORM                         = 69,
	DxgiFormat_BC1_TYPELESS                            = 70,
	DxgiFormat_BC1_UNORM                               = 71,
	DxgiFormat_BC1_UNORM_SRGB                          = 72,
	DxgiFormat_BC2_TYPELESS                            = 73,
	DxgiFormat_BC2_UNORM                               = 74,
	DxgiFormat_BC2_UNORM_SRGB                          = 75,
	DxgiFormat_BC3_TYPELESS                            = 76,
	DxgiFormat_BC3_UNORM                               = 77,
	DxgiFormat_BC3_UNORM_SRGB                          = 78,
	DxgiFormat_BC4_TYPELESS                            = 79,
	DxgiFormat_BC4_UNORM                               = 80,
	DxgiFormat_BC4_SNORM                               = 81,
	DxgiFormat_BC5_TYPELESS                            = 82,
	DxgiFormat_BC5_UNORM                               = 83,
	DxgiFormat_BC5_SNORM                               = 84,
	DxgiFormat_B5G6R5_UNORM                            = 85,
	DxgiFormat_B5G5R5A1_UNORM                          = 86,
	DxgiFormat_B8G8R8A8_UNORM                          = 87,
	DxgiFormat_B8G8R8X8_UNORM                          = 88,
	DxgiFormat_R10G10B10_XR_BIAS_A2_UNORM              = 89,
	DxgiFormat_B8G8R8A8_TYPELESS                       = 90,
	DxgiFormat_B8G8R8A8_UNORM_SRGB                     = 91,
	DxgiFormat_B8G8R8X8_TYPELESS                       = 92,
	DxgiFormat_B8G8R8X8_UNORM_SRGB                     = 93,
	DxgiFormat_BC6H_TYPELESS                           = 94,
	DxgiFormat_BC6H_UF16                               = 95,
	DxgiFormat_BC6H_SF16                               = 96,
	DxgiFormat_BC7_TYPELESS                            = 97,
	DxgiFormat_BC7_UNORM                               = 98,
	DxgiFormat_BC7_UNORM_SRGB                          = 99,
	DxgiFormat_AYUV                                    = 100,
	DxgiFormat_Y410                                    = 101,
	DxgiFormat_Y416                                    = 102,
	DxgiFormat_NV12                                    = 103,
	DxgiFormat_P010                                    = 104,
	DxgiFormat_P016                                    = 105,
	DxgiFormat_420_OPAQUE                              = 106,
	DxgiFormat_YUY2                                    = 107,
	DxgiFormat_Y210                                    = 108,
	DxgiFormat_Y216                                    = 109,
	DxgiFormat_NV11                                    = 110,
	DxgiFormat_AI44                                    = 111,
	DxgiFormat_IA44                                    = 112,
	DxgiFormat_P8                                      = 113,
	DxgiFormat_A8P8                                    = 114,
	DxgiFormat_B4G4R4A4_UNORM                          = 115,
	DxgiFormat_P208                                    = 130,
	DxgiFormat_V208                                    = 131,
	DxgiFormat_V408                                    = 132,
	DxgiFormat_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE         = 189,
	DxgiFormat_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,
	DxgiFormat_FORCE_UINT                              = -1,
};

/**
 * https://learn.microsoft.com/en-us/previous-versions/windows/desktop/legacy/bb173067(v=vs.85)
 */
enum DxgiModeScanlineOrder : u32 {
	DxgiModeScanlineOrder_UNSPECIFIED        = 0,
	DxgiModeScanlineOrder_PROGRESSIVE        = 1,
	DxgiModeScanlineOrder_UPPER_FIELD_FIRST  = 2,
	DxgiModeScanlineOrder_LOWER_FIELD_FIRST  = 3,
};

/**
 * https://learn.microsoft.com/en-us/previous-versions/windows/desktop/legacy/bb173066(v=vs.85)
 */
enum DxgiModeScaling : u32 {
	DxgiModeScaling_UNSPECIFIED = 0,
	DxgiModeScaling_CENTERED    = 1,
	DxgiModeScaling_STRETCHED   = 2,
};

/**
 * https://learn.microsoft.com/en-us/previous-versions/windows/desktop/legacy/bb173064(v=vs.85)
 */
struct DxgiModeDesc {
	u32                   width;
	u32                   height;
	DxgiRational          refresh_rate;
	DxgiFormat            format;
	DxgiModeScanlineOrder scanline_ordering;
	DxgiModeScaling       scaling;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgicommon/ns-dxgicommon-dxgi_sample_desc
 */
struct DxgiSampleDesc {
	u32 count;
	u32 quality;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/dxgi-usage
 */
enum_flags(DxgiUsage, u32) {
	DxgiUsage_CPU_ACCESS_NONE       =  0,
	DxgiUsage_CPU_ACCESS_DYNAMIC    =  1,
	DxgiUsage_CPU_ACCESS_READ_WRITE =  2,
	DxgiUsage_CPU_ACCESS_SCRATCH    =  3,
	DxgiUsage_CPU_ACCESS_FIELD      = 15,
	DxgiUsage_SHADER_INPUT          = (1 << (0 + 4)),
	DxgiUsage_RENDER_TARGET_OUTPUT  = (1 << (1 + 4)),
	DxgiUsage_BACK_BUFFER           = (1 << (2 + 4)),
	DxgiUsage_SHARED                = (1 << (3 + 4)),
	DxgiUsage_READ_ONLY             = (1 << (4 + 4)),
	DxgiUsage_DISCARD_ON_PRESENT    = (1 << (5 + 4)),
	DxgiUsage_UNORDERED_ACCESS      = (1 << (6 + 4)),
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/ne-dxgi-dxgi_swap_effect
 */
enum DxgiSwapEffect : u32 {
	DxgiSwapEffect_DISCARD         = 0,
	DxgiSwapEffect_SEQUENTIAL      = 1,
	DxgiSwapEffect_FLIP_SEQUENTIAL = 3,
	DxgiSwapEffect_FLIP_DISCARD    = 4,
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/ne-dxgi-dxgi_swap_chain_flag
 */
enum_flags(DxgiSwapChainFlags, u32) {
	DxgiSwapChainFlag_NONPREROTATED                          =   1,
	DxgiSwapChainFlag_ALLOW_MODE_SWITCH                      =   2,
	DxgiSwapChainFlag_GDI_COMPATIBLE                         =   4,
	DxgiSwapChainFlag_RESTRICTED_CONTENT                     =   8,
	DxgiSwapChainFlag_RESTRICT_SHARED_RESOURCE_DRIVER        =  16,
	DxgiSwapChainFlag_DISPLAY_ONLY                           =  32,
	DxgiSwapChainFlag_FRAME_LATENCY_WAITABLE_OBJECT          =  64,
	DxgiSwapChainFlag_FOREGROUND_LAYER                       =  128,
	DxgiSwapChainFlag_FULLSCREEN_VIDEO                       =  256,
	DxgiSwapChainFlag_YUV_VIDEO                              =  512,
	DxgiSwapChainFlag_HW_PROTECTED                           = 1024,
	DxgiSwapChainFlag_ALLOW_TEARING                          = 2048,
	DxgiSwapChainFlag_RESTRICTED_TO_ALL_HOLOGRAPHIC_DISPLAYS = 4096,
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/ns-dxgi-dxgi_swap_chain_desc
 */
struct DxgiSwapChainDesc {
	DxgiModeDesc       buffer_desc;
	DxgiSampleDesc     sample_desc;
	DxgiUsage          buffer_usage;
	u32                buffer_count;
	HWnd               output_window;
	b32                windowed;
	DxgiSwapEffect     swap_effect;
	DxgiSwapChainFlags flags;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_driver_type
 */
enum D3DDriverType {
  D3DDriverType_UNKNOWN = 0,
  D3DDriverType_HARDWARE,
  D3DDriverType_REFERENCE,
  D3DDriverType_NULL,
  D3DDriverType_SOFTWARE,
  D3DDriverType_WARP,
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_create_device_flag
 */
enum_flags(D3D11CreateDeviceFlags, u32) {
	D3D11CreateDeviceFlag_SINGLETHREADED                                = 0x1,
	D3D11CreateDeviceFlag_DEBUG                                         = 0x2,
	D3D11CreateDeviceFlag_SWITCH_TO_REF                                 = 0x4,
	D3D11CreateDeviceFlag_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS      = 0x8,
	D3D11CreateDeviceFlag_BGRA_SUPPORT                                  = 0x20,
	D3D11CreateDeviceFlag_DEBUGGABLE                                    = 0x40,
	D3D11CreateDeviceFlag_PREVENT_ALTERING_LAYER_SETTINGS_FROM_REGISTRY = 0x80,
	D3D11CreateDeviceFlag_DISABLE_GPU_TIMEOUT                           = 0x100,
	D3D11CreateDeviceFlag_VIDEO_SUPPORT                                 = 0x800
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_feature_level
 */
enum D3DFeatureLevel {
	D3DFeatureLevel_1_0_GENERIC = 0x100,
	D3DFeatureLevel_1_0_CORE    = 0x1000,
	D3DFeatureLevel_9_1         = 0x9100,
	D3DFeatureLevel_9_2         = 0x9200,
	D3DFeatureLevel_9_3         = 0x9300,
	D3DFeatureLevel_10_0        = 0xa000,
	D3DFeatureLevel_10_1        = 0xa100,
	D3DFeatureLevel_11_0        = 0xb000,
	D3DFeatureLevel_11_1        = 0xb100,
	D3DFeatureLevel_12_0        = 0xc000,
	D3DFeatureLevel_12_1        = 0xc100,
	D3DFeatureLevel_12_2        = 0xc200,
};

#define	D3D11_SDK_VERSION (7)

#define COM_UNUSED uprocptr

struct COM_Guid {
	u32 data1;
	u16 data2;
	u16 data3;
	u8  data4[8];
};

#define COM_DECLARE_INTERFACE(IName, d1, d2, d3, d4_1, d4_2, d4_3, d4_4, d4_5, d4_6, d4_7, d4_8)     \
struct IName##_VTable;                                                                               \
struct IName {                                                                                       \
	IName##_VTable* vtbl;                                                                            \
};                                                                                                   \
global const COM_Guid IName##_Guid = {d1, d2, d3, {d4_1, d4_2, d4_3, d4_4, d4_5, d4_6, d4_7, d4_8}}; \
struct IName##_VTable

#define COM_EXTEND_INTERFACE(IBase) IBase##_VTable _

#define COM_INTERFACE_GUID(IName) (&IName##_Guid)

#define COM_INTERFACE_OUT(IName, out_var) COM_INTERFACE_GUID(IName), (void**)(out_var)

#define COM_INTERFACE_OUT_PARAM(out_var) const COM_Guid* out_var##_riid, void** out_var

// Forward declarations to make errors go away
struct ID3D11Resource;
struct ID3D11RenderTargetView;

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nn-unknwn-iunknown
 */
COM_DECLARE_INTERFACE(
	IUnknown,
	0x00000000, 0x0000, 0x0000, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46
) {
	/**
	 * https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-queryinterface(refiid_void)
	 */
	HResult (WINAPI* query_interface)(
		void* self,
		COM_INTERFACE_OUT_PARAM(object)
	);

	COM_UNUSED add_ref;

	/**
	 * https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-release
	 */
	u32 (WINAPI* release)(
		void* self
	);
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nn-dxgi-idxgiobject
 */
COM_DECLARE_INTERFACE(
	IDXGIObject,
	0xaec22fb8, 0x76f3, 0x4639, 0x9b, 0xe0, 0x28, 0xeb, 0x43, 0xa6, 0x7a, 0x2e
) {
	COM_EXTEND_INTERFACE(IUnknown);

	COM_UNUSED set_private_data;
	COM_UNUSED set_private_data_interface;
	COM_UNUSED get_private_data;
	COM_UNUSED get_parent;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nn-dxgi-idxgidevicesubobject
 */
COM_DECLARE_INTERFACE(
	IDXGIDeviceSubObject,
	0x3d3e0379, 0xf9de, 0x4d58, 0xbb, 0x6c, 0x18, 0xd6, 0x29, 0x92, 0xf1, 0xa6
) {
	COM_EXTEND_INTERFACE(IDXGIObject);

	COM_UNUSED get_device;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/dxgi-present
 */
enum_flags(DxgiPresentFlags, u32) {
	DxgiPresentFlag_TEST                   = 0x00000001UL,
	DxgiPresentFlag_DO_NOT_SEQUENCE        = 0x00000002UL,
	DxgiPresentFlag_RESTART                = 0x00000004UL,
	DxgiPresentFlag_DO_NOT_WAIT            = 0x00000008UL,
	DxgiPresentFlag_STEREO_PREFER_RIGHT    = 0x00000010UL,
	DxgiPresentFlag_STEREO_TEMPORARY_MONO  = 0x00000020UL,
	DxgiPresentFlag_RESTRICT_TO_OUTPUT     = 0x00000040UL,
	DxgiPresentFlag_USE_DURATION           = 0x00000100UL,
	DxgiPresentFlag_ALLOW_TEARING          = 0x00000200UL,
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nn-dxgi-idxgiswapchain
 */
COM_DECLARE_INTERFACE(
	IDXGISwapChain,
	0x310d36a0, 0xd2e7, 0x4c0a, 0xaa, 0x04, 0x6a, 0x9d, 0x23, 0xb8, 0x88, 0x6a
) {
	COM_EXTEND_INTERFACE(IDXGIDeviceSubObject);

	/**
	 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-present
	 */
	HResult (WINAPI* present)(
		void*            self,
		u32              sync_interval,
		DxgiPresentFlags flags
	);

	/**
	 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-getbuffer
	 */
	HResult (WINAPI* get_buffer)(
		void* self,
		u32   buffer_idx,
		COM_INTERFACE_OUT_PARAM(surface)
	);

	COM_UNUSED set_fullscreen_state;
	COM_UNUSED get_fullscreen_state;
	COM_UNUSED get_desc;

	/**
	 * https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-resizebuffers
	 */
	HResult (WINAPI* resize_buffers)(
		void*              self,
		u32                buffer_count,
		u32                width,
		u32                height,
		DxgiFormat         format,
		DxgiSwapChainFlags flags
	);

	COM_UNUSED resize_target;
	COM_UNUSED get_containing_output;
	COM_UNUSED get_frame_statistics;
	COM_UNUSED get_last_present_count;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nn-d3d11-id3d11device
 */
COM_DECLARE_INTERFACE(
	ID3D11Device,
	0xdb6f6ddb, 0xac77, 0x4e88, 0x82, 0x53, 0x81, 0x9d, 0xf9, 0xbb, 0xf1, 0x40
) {
	COM_EXTEND_INTERFACE(IUnknown);

	COM_UNUSED create_buffer;
	COM_UNUSED create_texture1d;
	COM_UNUSED create_texture2d;
	COM_UNUSED create_texture3d;
	COM_UNUSED create_shader_resource_view;
	COM_UNUSED create_unordered_access_view;

	/**
	 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createrendertargetview
	 */
	HResult (WINAPI* create_render_target_view)(
		void*                    self,
		ID3D11Resource*          resource,
		void*                    desc,
		ID3D11RenderTargetView** render_target_view
	);

	COM_UNUSED create_depth_stencil_view;
	COM_UNUSED create_input_layout;
	COM_UNUSED create_vertex_shader;
	COM_UNUSED create_geometry_shader;
	COM_UNUSED create_geometry_shader_with_stream_output;
	COM_UNUSED create_pixel_shader;
	COM_UNUSED create_hull_shader;
	COM_UNUSED create_domain_shader;
	COM_UNUSED create_compute_shader;
	COM_UNUSED create_class_linkage;
	COM_UNUSED create_blend_state;
	COM_UNUSED create_depth_stencil_state;
	COM_UNUSED create_rasterizer_state;
	COM_UNUSED create_sampler_state;
	COM_UNUSED create_query;
	COM_UNUSED create_predicate;
	COM_UNUSED create_counter;
	COM_UNUSED create_deferred_context;
	COM_UNUSED open_shared_resource;
	COM_UNUSED check_format_support;
	COM_UNUSED check_multisample_quality_levels;
	COM_UNUSED check_counter_info;
	COM_UNUSED check_counter;
	COM_UNUSED check_feature_support;
	COM_UNUSED get_private_data;
	COM_UNUSED set_private_data;
	COM_UNUSED set_private_data_interface;
	COM_UNUSED get_feature_level;
	COM_UNUSED get_creation_flags;
	COM_UNUSED get_device_removed_reason;
	COM_UNUSED get_immediate_context;
	COM_UNUSED set_exception_mode;
	COM_UNUSED get_exception_mode;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nn-d3d11-id3d11devicechild
 */
COM_DECLARE_INTERFACE(
	ID3D11DeviceChild,
	0x1841e5c8, 0x16b0, 0x489b, 0xbc, 0xc8, 0x44, 0xcf, 0xb0, 0xd5, 0xde, 0xae
) {
	COM_EXTEND_INTERFACE(IUnknown);

	COM_UNUSED get_device;
	COM_UNUSED get_private_data;
	COM_UNUSED set_private_data;
	COM_UNUSED set_private_data_interface;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nn-d3d11-id3d11devicecontext
 */
COM_DECLARE_INTERFACE(
	ID3D11DeviceContext,
	0xc0bfa96c, 0xe089, 0x44fb, 0x8e, 0xaf, 0x26, 0xf8, 0x79, 0x61, 0x90, 0xda
) {
	COM_EXTEND_INTERFACE(ID3D11DeviceChild);

	COM_UNUSED vs_set_constant_buffers;
	COM_UNUSED ps_set_shader_resources;
	COM_UNUSED ps_set_shader;
	COM_UNUSED ps_set_samplers;
	COM_UNUSED vs_set_shader;
	COM_UNUSED draw_indexed;
	COM_UNUSED draw;
	COM_UNUSED map;
	COM_UNUSED unmap;
	COM_UNUSED ps_set_constant_buffers;
	COM_UNUSED ia_set_input_layout;
	COM_UNUSED ia_set_vertex_buffers;
	COM_UNUSED ia_set_index_buffer;
	COM_UNUSED draw_indexed_instanced;
	COM_UNUSED draw_instanced;
	COM_UNUSED gs_set_constant_buffers;
	COM_UNUSED gs_set_shader;
	COM_UNUSED ia_set_primitive_topology;
	COM_UNUSED vs_set_shader_resources;
	COM_UNUSED vs_set_samplers;
	COM_UNUSED begin;
	COM_UNUSED end;
	COM_UNUSED get_data;
	COM_UNUSED set_predication;
	COM_UNUSED gs_set_shader_resources;
	COM_UNUSED gs_set_samplers;

	/**
	 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-omsetrendertargets
	 */
	void (WINAPI* om_set_render_targets)(
		void*                    self,
		u32                      num_views,
		ID3D11RenderTargetView** render_target_views,
		void*                    depth_stencil_view
	);

	COM_UNUSED om_set_render_targets_and_unordered_access_views;
	COM_UNUSED om_set_blend_state;
	COM_UNUSED om_set_depth_stencil_state;
	COM_UNUSED so_set_targets;
	COM_UNUSED draw_auto;
	COM_UNUSED draw_indexed_instanced_indirect;
	COM_UNUSED draw_instanced_indirect;
	COM_UNUSED dispatch;
	COM_UNUSED dispatch_indirect;
	COM_UNUSED rs_set_state;
	COM_UNUSED rs_set_viewports;
	COM_UNUSED rs_set_scissor_rects;
	COM_UNUSED copy_subresource_region;
	COM_UNUSED copy_resource;
	COM_UNUSED update_subresource;
	COM_UNUSED copy_structure_count;

	/**
	 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-clearrendertargetview
	 */
	void (WINAPI* clear_render_target_view)(
		void*                   self,
		ID3D11RenderTargetView* render_target_view,
		f32                     color_rgba[4]
	);

	COM_UNUSED clear_unordered_access_view_uint;
	COM_UNUSED clear_unordered_access_view_float;
	COM_UNUSED clear_depth_stencil_view;
	COM_UNUSED generate_mips;
	COM_UNUSED set_resource_min_lod;
	COM_UNUSED get_resource_min_lod;
	COM_UNUSED resolve_subresource;
	COM_UNUSED execute_command_list;
	COM_UNUSED hs_set_shader_resources;
	COM_UNUSED hs_set_shader;
	COM_UNUSED hs_set_samplers;
	COM_UNUSED hs_set_constant_buffers;
	COM_UNUSED ds_set_shader_resources;
	COM_UNUSED ds_set_shader;
	COM_UNUSED ds_set_samplers;
	COM_UNUSED ds_set_constant_buffers;
	COM_UNUSED cs_set_shader_resources;
	COM_UNUSED cs_set_unordered_access_views;
	COM_UNUSED cs_set_shader;
	COM_UNUSED cs_set_samplers;
	COM_UNUSED cs_set_constant_buffers;
	COM_UNUSED vs_get_constant_buffers;
	COM_UNUSED ps_get_shader_resources;
	COM_UNUSED ps_get_shader;
	COM_UNUSED ps_get_samplers;
	COM_UNUSED vs_get_shader;
	COM_UNUSED ps_get_constant_buffers;
	COM_UNUSED ia_get_input_layout;
	COM_UNUSED ia_get_vertex_buffers;
	COM_UNUSED ia_get_index_buffer;
	COM_UNUSED gs_get_constant_buffers;
	COM_UNUSED gs_get_shader;
	COM_UNUSED ia_get_primitive_topology;
	COM_UNUSED vs_get_shader_resources;
	COM_UNUSED vs_get_samplers;
	COM_UNUSED get_predication;
	COM_UNUSED gs_get_shader_resources;
	COM_UNUSED gs_get_samplers;
	COM_UNUSED om_get_render_targets;
	COM_UNUSED om_get_render_targets_and_unordered_access_views;
	COM_UNUSED om_get_blend_state;
	COM_UNUSED om_get_depth_stencil_state;
	COM_UNUSED so_get_targets;
	COM_UNUSED rs_get_state;
	COM_UNUSED rs_get_viewports;
	COM_UNUSED rs_get_scissor_rects;
	COM_UNUSED hs_get_shader_resources;
	COM_UNUSED hs_get_shader;
	COM_UNUSED hs_get_samplers;
	COM_UNUSED hs_get_constant_buffers;
	COM_UNUSED ds_get_shader_resources;
	COM_UNUSED ds_get_shader;
	COM_UNUSED ds_get_samplers;
	COM_UNUSED ds_get_constant_buffers;
	COM_UNUSED cs_get_shader_resources;
	COM_UNUSED cs_get_unordered_access_views;
	COM_UNUSED cs_get_shader;
	COM_UNUSED cs_get_samplers;
	COM_UNUSED cs_get_constant_buffers;
	COM_UNUSED clear_state;
	COM_UNUSED flush;
	COM_UNUSED get_type;
	COM_UNUSED get_context_flags;
	COM_UNUSED finish_command_list;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nn-d3d11-id3d11view
 */
COM_DECLARE_INTERFACE(
	ID3D11View,
	0x839d1216, 0xbb2e, 0x412b, 0xb7, 0xf4, 0xa9, 0xdb, 0xeb, 0xe0, 0x8e, 0xd1
) {
	COM_EXTEND_INTERFACE(ID3D11DeviceChild);

	COM_UNUSED get_resource;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nn-d3d11-id3d11rendertargetview
 */
COM_DECLARE_INTERFACE(
	ID3D11RenderTargetView,
	0xdfdba067, 0x0b8d, 0x4865, 0x87, 0x5b, 0xd7, 0xb4, 0x51, 0x6c, 0xc1, 0x64
) {
	COM_EXTEND_INTERFACE(ID3D11View);

	COM_UNUSED get_desc;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-d3d11createdeviceandswapchain
 */
DLLIMPORT HResult WINAPI D3D11CreateDeviceAndSwapChain(
	void*                  adapter,
	D3DDriverType          driver_type,
	HModule                software,
	D3D11CreateDeviceFlags flags,
	D3DFeatureLevel*       feature_levels,
	u32                    feature_levels_len,
	u32                    sdk_version,
	DxgiSwapChainDesc*     swap_chain_desc,
	IDXGISwapChain**       swap_chain,
	ID3D11Device**         device,
	D3DFeatureLevel*       feature_level,
	ID3D11DeviceContext**  immediate_context
);

#define DXGI_ERROR(error_val) ((HResult)(0x887a0000 | ((error_val) & 0xffff)))
enum DxgiError : HResult {
	DxgiError_OK                            = 0,
	DxgiError_INVALID_CALL                  = DXGI_ERROR(0x0001),
	DxgiError_NOT_FOUND                     = DXGI_ERROR(0x0002),
	DxgiError_MORE_DATA                     = DXGI_ERROR(0x0003),
	DxgiError_UNSUPPORTED                   = DXGI_ERROR(0x0004),
	DxgiError_DEVICE_REMOVED                = DXGI_ERROR(0x0005),
	DxgiError_DEVICE_HUNG                   = DXGI_ERROR(0x0006),
	DxgiError_DEVICE_RESET                  = DXGI_ERROR(0x0007),
	DxgiError_WAS_STILL_DRAWING             = DXGI_ERROR(0x000a),
	DxgiError_FRAME_STATISTICS_DISJOINT     = DXGI_ERROR(0x000b),
	DxgiError_GRAPHICS_VIDPN_SOURCE_IN_USE  = DXGI_ERROR(0x000c),
	DxgiError_DRIVER_INTERNAL_ERROR         = DXGI_ERROR(0x0020),
	DxgiError_NONEXCLUSIVE                  = DXGI_ERROR(0x0021),
	DxgiError_NOT_CURRENTLY_AVAILABLE       = DXGI_ERROR(0x0022),
	DxgiError_REMOTE_CLIENT_DISCONNECTED    = DXGI_ERROR(0x0023),
	DxgiError_REMOTE_OUTOFMEMORY            = DXGI_ERROR(0x0024),
	DxgiError_ACCESS_LOST                   = DXGI_ERROR(0x0026),
	DxgiError_WAIT_TIMEOUT                  = DXGI_ERROR(0x0027),
	DxgiError_SESSION_DISCONNECTED          = DXGI_ERROR(0x0028),
	DxgiError_RESTRICT_TO_OUTPUT_STALE      = DXGI_ERROR(0x0029),
	DxgiError_CANNOT_PROTECT_CONTENT        = DXGI_ERROR(0x002a),
	DxgiError_ACCESS_DENIED                 = DXGI_ERROR(0x002b),
	DxgiError_NAME_ALREADY_EXISTS           = DXGI_ERROR(0x002c),
	DxgiError_SDK_COMPONENT_MISSING         = DXGI_ERROR(0x002d),
	DxgiError_NOT_CURRENT                   = DXGI_ERROR(0x002e),
	DxgiError_HW_PROTECTION_OUTOFMEMORY     = DXGI_ERROR(0x0030),
	DxgiError_DYNAMIC_CODE_POLICY_VIOLATION = DXGI_ERROR(0x0031),
	DxgiError_NON_COMPOSITED_UI             = DXGI_ERROR(0x0032),
	DxgiError_MODE_CHANGE_IN_PROGRESS       = DXGI_ERROR(0x0025),
	DxgiError_CACHE_CORRUPT                 = DXGI_ERROR(0x0033),
	DxgiError_CACHE_FULL                    = DXGI_ERROR(0x0034),
	DxgiError_CACHE_HASH_COLLISION          = DXGI_ERROR(0x0035),
	DxgiError_ALREADY_EXISTS                = DXGI_ERROR(0x0036),
};

#define DXGI_STATUS(status_val) ((HResult)(0x087a0000 | ((status_val) & 0xffff)))
enum DxgiStatus : HResult {
	DxgiStatus_OCCLUDED                     = DXGI_STATUS(0x0001),
	DxgiStatus_CLIPPED                      = DXGI_STATUS(0x0002),
	DxgiStatus_NO_REDIRECTION               = DXGI_STATUS(0x0004),
	DxgiStatus_NO_DESKTOP_ACCESS            = DXGI_STATUS(0x0005),
	DxgiStatus_GRAPHICS_VIDPN_SOURCE_IN_USE = DXGI_STATUS(0x0006),
	DxgiStatus_MODE_CHANGED                 = DXGI_STATUS(0x0007),
	DxgiStatus_MODE_CHANGE_IN_PROGRESS      = DXGI_STATUS(0x0008),
	DxgiStatus_UNOCCLUDED                   = DXGI_STATUS(0x0009),
	DxgiStatus_DDA_WAS_STILL_DRAWING        = DXGI_STATUS(0x000A),
	DxgiStatus_PRESENT_REQUIRED             = DXGI_STATUS(0x002F),
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nn-d3d11-id3d11resource
 */
COM_DECLARE_INTERFACE(
	ID3D11Resource,
	0xdc8e63f3, 0xd12b, 0x4952, 0xb4, 0x7b, 0x5e, 0x45, 0x02, 0x6a, 0x86, 0x2d
) {
	COM_EXTEND_INTERFACE(ID3D11DeviceChild);

	COM_UNUSED get_type;
	COM_UNUSED set_eviction_priority;
	COM_UNUSED get_eviction_priority;
};

/**
 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nn-d3d11-id3d11texture2d
 */
COM_DECLARE_INTERFACE(
	ID3D11Texture2D,
	0x6f15aaf2, 0xd208, 0x4e89, 0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c
) {
	COM_EXTEND_INTERFACE(ID3D11Resource);

	COM_UNUSED get_desc;
};

#endif // __WINDOWS_HPP__
