// mmsystem.h - the winmm calls of the game for the macOS port (written for the port).
#ifndef PORT_MMSYSTEM_H
#define PORT_MMSYSTEM_H

#include "windef.h"

PORT_BEGIN_C

typedef UINT MMRESULT;
typedef UINT MMVERSION;
#define MMSYSERR_NOERROR 0
#define MMSYSERR_ERROR 1
#define MMSYSERR_BADDEVICEID 2
#define MMSYSERR_NOTENABLED 3
#define MMSYSERR_ALLOCATED 4
#define MMSYSERR_INVALHANDLE 5
#define MMSYSERR_NODRIVER 6
#define MMSYSERR_NOMEM 7
#define MMSYSERR_NOTSUPPORTED 8
#define MMSYSERR_INVALPARAM 11
#define TIMERR_NOERROR 0
#define TIMERR_NOCANDO 97
#define TIME_ONESHOT 0x0000
#define TIME_PERIODIC 0x0001
#define TIME_CALLBACK_FUNCTION 0x0000

typedef struct timecaps_tag {
    UINT wPeriodMin;
    UINT wPeriodMax;
} TIMECAPS, *PTIMECAPS, *LPTIMECAPS;
typedef void (CALLBACK *LPTIMECALLBACK)(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

// Joysticks (RuntimeLib's MfJoystick reads them through winmm).
#define JOYERR_NOERROR 0
#define JOYERR_PARMS 165
#define JOYERR_UNPLUGGED 167
#define JOYSTICKID1 0
#define JOYSTICKID2 1
#define JOY_BUTTON1 0x0001
#define JOY_BUTTON2 0x0002
#define JOY_BUTTON3 0x0004
#define JOY_BUTTON4 0x0008
#define JOY_RETURNX 0x00000001L
#define JOY_RETURNY 0x00000002L
#define JOY_RETURNZ 0x00000004L
#define JOY_RETURNR 0x00000008L
#define JOY_RETURNU 0x00000010L
#define JOY_RETURNV 0x00000020L
#define JOY_RETURNPOV 0x00000040L
#define JOY_RETURNBUTTONS 0x00000080L
#define JOY_RETURNALL (JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNR | JOY_RETURNU | JOY_RETURNV | \
                       JOY_RETURNPOV | JOY_RETURNBUTTONS)
#define JOY_POVCENTERED (WORD)-1
#define MAXPNAMELEN 32
#define MAX_JOYSTICKOEMVXDNAME 260
typedef struct joyinfo_tag {
    UINT wXpos;
    UINT wYpos;
    UINT wZpos;
    UINT wButtons;
} JOYINFO, *PJOYINFO, *LPJOYINFO;
typedef struct joyinfoex_tag {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwXpos;
    DWORD dwYpos;
    DWORD dwZpos;
    DWORD dwRpos;
    DWORD dwUpos;
    DWORD dwVpos;
    DWORD dwButtons;
    DWORD dwButtonNumber;
    DWORD dwPOV;
    DWORD dwReserved1;
    DWORD dwReserved2;
} JOYINFOEX, *PJOYINFOEX, *LPJOYINFOEX;
typedef struct tagJOYCAPSA {
    WORD wMid;
    WORD wPid;
    CHAR szPname[MAXPNAMELEN];
    UINT wXmin;
    UINT wXmax;
    UINT wYmin;
    UINT wYmax;
    UINT wZmin;
    UINT wZmax;
    UINT wNumButtons;
    UINT wPeriodMin;
    UINT wPeriodMax;
    UINT wRmin;
    UINT wRmax;
    UINT wUmin;
    UINT wUmax;
    UINT wVmin;
    UINT wVmax;
    UINT wCaps;
    UINT wMaxAxes;
    UINT wNumAxes;
    UINT wMaxButtons;
    CHAR szRegKey[MAXPNAMELEN];
    CHAR szOEMVxD[MAX_JOYSTICKOEMVXDNAME];
} JOYCAPSA, *PJOYCAPSA, *LPJOYCAPSA;
typedef JOYCAPSA JOYCAPS, *PJOYCAPS, *LPJOYCAPS;

// Wave and MCI basics.
#define WAVE_FORMAT_PCM 1
typedef struct tWAVEFORMATEX {
    WORD wFormatTag;
    WORD nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD nBlockAlign;
    WORD wBitsPerSample;
    WORD cbSize;
} WAVEFORMATEX, *PWAVEFORMATEX, *LPWAVEFORMATEX;
typedef const WAVEFORMATEX *LPCWAVEFORMATEX;
typedef struct waveformat_tag {
    WORD wFormatTag;
    WORD nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD nBlockAlign;
} WAVEFORMAT, *PWAVEFORMAT, *LPWAVEFORMAT;
typedef struct pcmwaveformat_tag {
    WAVEFORMAT wf;
    WORD wBitsPerSample;
} PCMWAVEFORMAT, *PPCMWAVEFORMAT, *LPPCMWAVEFORMAT;
#define SND_SYNC 0x0000
#define SND_ASYNC 0x0001
#define SND_NODEFAULT 0x0002
#define SND_MEMORY 0x0004
#define SND_LOOP 0x0008
#define SND_NOSTOP 0x0010
#define SND_FILENAME 0x00020000L
#define SND_RESOURCE 0x00040004L
#define mmioFOURCC(ch0, ch1, ch2, ch3) \
    ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24))
#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) mmioFOURCC(ch0, ch1, ch2, ch3)
#endif

DWORD WINAPI timeGetTime(void);
MMRESULT WINAPI timeBeginPeriod(UINT uPeriod);
MMRESULT WINAPI timeEndPeriod(UINT uPeriod);
MMRESULT WINAPI timeGetDevCaps(LPTIMECAPS ptc, UINT cbtc);
MMRESULT WINAPI timeSetEvent(UINT uDelay, UINT uResolution, LPTIMECALLBACK fptc, DWORD_PTR dwUser, UINT fuEvent);
MMRESULT WINAPI timeKillEvent(UINT uTimerID);
UINT WINAPI joyGetNumDevs(void);
MMRESULT WINAPI joyGetPos(UINT uJoyID, LPJOYINFO pji);
MMRESULT WINAPI joyGetPosEx(UINT uJoyID, LPJOYINFOEX pji);
MMRESULT WINAPI joyGetDevCapsA(UINT_PTR uJoyID, LPJOYCAPSA pjc, UINT cbjc);
BOOL WINAPI PlaySoundA(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound);
BOOL WINAPI sndPlaySoundA(LPCSTR pszSound, UINT fuSound);
#define joyGetDevCaps joyGetDevCapsA
#define PlaySound PlaySoundA
#define sndPlaySound sndPlaySoundA

PORT_END_C

#endif
