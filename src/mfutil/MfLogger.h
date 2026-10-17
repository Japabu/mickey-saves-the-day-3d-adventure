#pragma once
// MfLogger: process-wide log with a window and a log file, run by a logger
// thread. Exported names only; the static members are named in the exports,
// MfLogMessage, MfLoggerFrame and MfLogMessageDialog too.
#include "mfutil_api.h"
#include "MfString.h"     // first: it defines STRICT before windows.h
#include "MfStringList.h"
#include "MfSystemTime.h"
#include "MfCriticalSection.h"
#include "MfEvent.h"
class MfDblList;
class MfDblListItem;
class MfLogMessageDialog;
class MfLoggerFrame;
class MfLoggerWindow;
class MfLoggerOptions;
class MfThread;
class ofstream;
class ostream;
struct tagLVDISPINFO;

// Message types; the high word carries MF_LOG_* flags.
enum { MF_LOG_INFORMATION, MF_LOG_WARNING, MF_LOG_ERROR };
enum {
    MF_LOG_QUIET = 0x10000,         // shown without opening the window
    MF_LOG_POPUP = 0x20000          // opens the window
};

class MFUTIL_API MfLogger {
    friend class MfLoggerFrame;
    friend class MfLoggerWindow;
    friend class MfLogMessageDialog;
    friend class MfLoggerOptions;
    friend class MfLoggerFrameUI;
public:
    MfLogger() {}
    virtual ~MfLogger() {}
    static void SuggestLogWindowTitle(const MfString &fsWindowTitle)
    {
        fsLogWindowTitle = fsWindowTitle;
    }
    static void DisableLogger() { bEnabled = 0; }
    // The low 16 bits are reserved; only the high ones can be set from outside.
    static void SetFlags(int iFlags) { iLoggerFlags |= iFlags & 0xffff0000; }
    static void ClearFlags(int iFlags) { iLoggerFlags &= ~(iFlags & 0xffff0000); }
    static int GetFlags() { return iLoggerFlags; }
    // Filters 0-2 hide one message type (iLevel 0), 3-5 show only the
    // messages whose source, message or extra lines contain fsName.
    struct MFUTIL_API LoggerFilter {
        LoggerFilter(int type, int level, const MfString &name)
            : iType(type), iLevel(level), fsName(name) {}
        int iType;
        int iLevel;
        MfString fsName;
    };

    static const LoggerFilter *QueryFilter(int iFilter) { return &aFilters[iFilter]; }

    class MfLogMessage {
    public:
        MfLogMessage(int type, const MfString &source, const MfString &message)
            : pli(0), iType(type & 0xffff), stTime(0), fsSource(source), fsMessage(message),
              iFlags(type & 0xffff0000)
        {
            stTime.GetLocalTime();
            if (iType == MF_LOG_INFORMATION)
                iFlags |= MF_LOG_QUIET;
        }
        MfLogMessage(int type, const MfString &source, const MfString &message,
                     const MfString &extra)
            : pli(0), iType(type & 0xffff), stTime(0), fsSource(source), fsMessage(message),
              iFlags(type & 0xffff0000)
        {
            stTime.GetLocalTime();
            lstExtra + extra;
            if (iType == MF_LOG_INFORMATION)
                iFlags |= MF_LOG_QUIET;
        }
        void Write(ostream &os);
        void GetDispInfo(tagLVDISPINFO *pdi);
        void Display(MfLogMessageDialog *dialog);

        MfDblListItem *pli;         // +0x00 its item in plLogMessages
        int iType;                  // +0x04
        MfSystemTime stTime;        // +0x08
        MfString fsSource;          // +0x1c
        MfString fsMessage;         // +0x20
        MfStringList lstExtra;      // +0x24
        int iFlags;                 // +0x38
        int iItem;                  // +0x3c in the list view
    };

    static HWND__ *LoggerWindow();
    static MfString TypeToString(int);
    static int Initialize();
    static void ClearWindow();
    static void EnableWindowOutput(int);
    static void Filter(int, int, const MfString &);
    static void GetProgramStartupInfo();
    static void Log(int, const MfString &, const MfString &, const MfString &);
    static void Log(int, const MfString &, const MfString &, const MfStringList &);
    static void Log(int, const MfString &, const MfString &, int, ...);
    static void Shutdown();
private:
    static void DisplayLogMessageDialog(MfLogMessage *);
    static void DisplayLogMessageWindow();
    static void DoWork();
    static void FilterMessage(MfLogMessage *);
    static void LmdNext(MfDblListItem *&);
    static void LmdPrev(MfDblListItem *&);
    static void ThreadInitialize();
    static LoggerFilter aFilters[6];
    static int bClearWindowPending;
    static int bEnabled;
    static int bFilterChangePending;
    static int bFilterEnabled;
    static int bWindowEnabled;
    static MfCriticalSection csLoggerLock;
    static MfCriticalSection csQueueLock;
    static MfEvent evtStartup;
    static MfString fsCWD;
    static MfString fsLogWindowTitle;
    static int iLoggerFlags;
    static MfDblList *plLogMessages;
    static MfDblList *plLogQueue;
    static MfLogMessageDialog *plmdlg;
    static ofstream *pofsLogFile;
    static MfLoggerFrame *pwndLogMessageWindow;
    static MfThread *pwthrdLoggerThread;
};
