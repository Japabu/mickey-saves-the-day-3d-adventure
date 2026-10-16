#include "MfLogger.h"
#include "MfLoggerUI.h"
#include "MfDblList.h"
#include "UtilityApp.h"
#include <stdarg.h>
#include <fstream.h>

// Inline and compiler-generated members of MfStringList.h:
// 0x10005d90 ??1MfStringList@@UAE@XZ
// 0x10005e00 ??HMfStringList@@QAEAAV0@ABVMfString@@@Z
// 0x10005e60 ?GetCurrent@MfDblList@@UBEPAXAAPAVMfDblListItem@@@Z
// 0x10005e80 ??_EMfStringList@@UAEPAXI@Z

// Inline and compiler-generated members of MfLogger.h:
// 0x10005ed0 ??0MfLogger@@QAE@XZ
// 0x10005ee0 ??1MfLogger@@UAE@XZ
// 0x10005ef0 ?SuggestLogWindowTitle@MfLogger@@SAXABVMfString@@@Z
// 0x10005f00 ?DisableLogger@MfLogger@@SAXXZ
// 0x10005f10 ?SetFlags@MfLogger@@SAXH@Z
// 0x10005f30 ?ClearFlags@MfLogger@@SAXH@Z
// 0x10005f50 ?GetFlags@MfLogger@@SAHXZ
// 0x10005f60 ??0LoggerFilter@MfLogger@@QAE@HHABVMfString@@@Z
// 0x10005f90 ?QueryFilter@MfLogger@@SAPBULoggerFilter@1@H@Z
// 0x10005fa0 ??0LoggerFilter@MfLogger@@QAE@ABU01@@Z
// 0x10005fd0 ??4LoggerFilter@MfLogger@@QAEAAU01@ABU01@@Z
// 0x10006000 ??1LoggerFilter@MfLogger@@QAE@XZ
// 0x10006010 ??0MfLogger@@QAE@ABV0@@Z
// 0x10006020 ??_EMfLogger@@UAEPAXI@Z

// Static initializers and destructors of the six objects below, in order:
// 0x10006350 _$E51 (calls $E47 and $E50)
// 0x10006360 _$E47
// 0x10006370 _$E50
// 0x10006380 _$E48
// 0x100063a0 _$E56 (calls $E53 and $E55)
// 0x100063b0 _$E53
// 0x100063c0 _$E55
// 0x100063d0 _$E54
// 0x100063f0 _$E61 (calls $E58 and $E60)
// 0x10006400 _$E58
// 0x10006410 _$E60
// 0x10006420 _$E59
// 0x10006440 _$E66 (calls $E63 and $E65)
// 0x10006450 _$E63
// 0x10006460 _$E65
// 0x10006470 _$E64
// 0x10006490 _$E71 (calls $E68 and $E70)
// 0x100064a0 _$E68
// 0x100064b0 _$E70
// 0x100064c0 _$E69
// 0x100064e0 _$E76 (calls $E73 and $E75)
// 0x100064f0 _$E73
// 0x100066a0 _$E75
// 0x100066b0 _$E74
MfCriticalSection MfLogger::csLoggerLock;
MfCriticalSection MfLogger::csQueueLock;
MfEvent MfLogger::evtStartup;
MfString MfLogger::fsLogWindowTitle("MOFA Event Logger");
MfString MfLogger::fsCWD;
MfLogger::LoggerFilter MfLogger::aFilters[6] = {
    LoggerFilter(MF_LOG_INFORMATION, 1, NULL_STRING),
    LoggerFilter(MF_LOG_WARNING, 1, NULL_STRING),
    LoggerFilter(MF_LOG_ERROR, 1, NULL_STRING),
    LoggerFilter(3, 0, NULL_STRING),
    LoggerFilter(4, 0, NULL_STRING),
    LoggerFilter(5, 0, NULL_STRING)
};
MfDblList *MfLogger::plLogMessages;
MfDblList *MfLogger::plLogQueue;
MfThread *MfLogger::pwthrdLoggerThread;
MfLoggerFrame *MfLogger::pwndLogMessageWindow;
int MfLogger::bClearWindowPending;
int MfLogger::iLoggerFlags;
MfLogMessageDialog *MfLogger::plmdlg;
ofstream *MfLogger::pofsLogFile;
int MfLogger::bFilterChangePending;
int MfLogger::bFilterEnabled;
int MfLogger::bWindowEnabled = 1;
int MfLogger::bEnabled = 1;

// Set while the logger thread runs its message loop.
static int bLoggerThreadRunning;

// 0x100066e0 ?Initialize@MfLogger@@SAHXZ
int MfLogger::Initialize()
{
    if (!bEnabled)
        return 0;
    csLoggerLock.Enter();
    UtilityApp app;
    iLoggerFlags |= app.GetProfileInt(szLoggerSection, szLoggerFlags, 0) & 0xffff0000;
    if (!pwthrdLoggerThread) {
        plLogMessages = new MfDblList;
        csQueueLock.Enter();
        plLogQueue = new MfDblList;
        csQueueLock.Leave();
        MfLoggerFrameUI *pThread = new MfLoggerFrameUI;
        pwthrdLoggerThread = pThread;
        pThread->AssignWork(pThread);
        evtStartup.Wait(INFINITE);
        SetThreadPriority(pwthrdLoggerThread->GetHandle(), THREAD_PRIORITY_ABOVE_NORMAL);
    }
    csLoggerLock.Leave();
    return 1;
}

// Compiler-generated members of MfThreadWork and MfLoggerFrameUI (MfLoggerUI.h):
// 0x100068b0 ??_GMfThreadWork@@UAEPAXI@Z
// 0x100068d0 ??_GMfLoggerFrameUI@@UAEPAXI@Z
// 0x100068f0 ??1MfLoggerFrameUI@@UAE@XZ

// 0x10006920 ?ThreadInitialize@MfLogger@@CAXXZ
// Opens the log file: <LOGDIR or the startup directory>/<application>.log,
// else <application>.log, else <TEMP>/<application>.log.
void MfLogger::ThreadInitialize()
{
    csLoggerLock.Enter();
    if (!pofsLogFile) {
        MfString fsDir(fsCWD);
        fsDir.GetVariable("LOGDIR");
        MfString fsName(UtilityApp::AppName());
        if (fsName == NULL_STRING)
            fsName = "LOGGER";
        MfString fsPath = fsDir + MfString("/") + fsName + MfString(".log");
        pofsLogFile = new ofstream(fsPath, ios::app);
        if (!pofsLogFile->is_open()) {
            fsPath = fsName + MfString(".log");
            delete pofsLogFile;
            pofsLogFile = new ofstream(fsPath, ios::app);
            if (!pofsLogFile->is_open()) {
                fsDir.GetVariable("TEMP");
                fsPath = fsDir + MfString("/") + fsName + MfString(".log");
                delete pofsLogFile;
                pofsLogFile = new ofstream(fsPath, ios::app);
            }
        }
    }
    csLoggerLock.Leave();
}

// 0x10006d10 ??_Gofstream@@UAEPAXI@Z (compiler-generated)

// 0x10006d40 ?GetProgramStartupInfo@MfLogger@@SAXXZ
void MfLogger::GetProgramStartupInfo()
{
    char szDir[MAX_PATH + 1];
    if (fsCWD == NULL_STRING) {
        GetCurrentDirectory(MAX_PATH, szDir);
        fsCWD = szDir;
    }
}

// 0x10006d80 ?Shutdown@MfLogger@@SAXXZ
void MfLogger::Shutdown()
{
    if (bEnabled) {
        UtilityApp app;
        app.WriteProfileInt(szLoggerSection, szLoggerFlags, iLoggerFlags & 0xffff0000);
        if (pwthrdLoggerThread) {
            pwndLogMessageWindow->Close();
            pwndLogMessageWindow = 0;
            pwthrdLoggerThread->PostThreadMessage(WM_QUIT, 0, 0);
            pwthrdLoggerThread = 0;
            Sleep(0);
            Sleep(0);
            Sleep(0);

            csLoggerLock.Enter();
            MfDblListItem *pli;
            MfLogMessage *pMessage;
            for (pMessage = (MfLogMessage *)plLogMessages->GetFirst(pli); pMessage;
                 pMessage = (MfLogMessage *)plLogMessages->GetNext(pli))
                delete pMessage;
            delete plLogMessages;
            plLogMessages = 0;
            csQueueLock.Enter();
            for (pMessage = (MfLogMessage *)plLogQueue->GetFirst(pli); pMessage;
                 pMessage = (MfLogMessage *)plLogQueue->GetNext(pli))
                delete pMessage;
            delete plLogQueue;
            plLogQueue = 0;
            csQueueLock.Leave();
            if (pofsLogFile)
                pofsLogFile->close();
            delete pofsLogFile;
            pofsLogFile = 0;
            csLoggerLock.Leave();
        }
    }
}

// 0x10007020 ?Log@MfLogger@@SAXHABVMfString@@0HZZ
// The variable arguments are MfString pointers, ended by a null pointer; the
// int before them is not used.
void MfLogger::Log(int iType, const MfString &fsSource, const MfString &fsMessage, int iUnused, ...)
{
    if (!bEnabled)
        return;
    if (!pwthrdLoggerThread)
        Initialize();
    MfLogMessage *pMessage = new MfLogMessage(iType, fsSource, fsMessage);
    va_list args;
    va_start(args, iUnused);
    for (const MfString *pfs = va_arg(args, const MfString *); pfs; pfs = va_arg(args, const MfString *))
        pMessage->lstExtra + *pfs;
    va_end(args);
    csQueueLock.Enter();
    plLogQueue->Add(pMessage);
    csQueueLock.Leave();
}

// 0x10007150 ?Log@MfLogger@@SAXHABVMfString@@00@Z
void MfLogger::Log(int iType, const MfString &fsSource, const MfString &fsMessage, const MfString &fsExtra)
{
    if (!bEnabled)
        return;
    if (!pwthrdLoggerThread)
        Initialize();
    MfLogMessage *pMessage = new MfLogMessage(iType, fsSource, fsMessage, fsExtra);
    csQueueLock.Enter();
    plLogQueue->Add(pMessage);
    csQueueLock.Leave();
}

// 0x10007280 ?Log@MfLogger@@SAXHABVMfString@@0ABVMfStringList@@@Z
void MfLogger::Log(int iType, const MfString &fsSource, const MfString &fsMessage, const MfStringList &lstExtra)
{
    if (!bEnabled)
        return;
    if (!pwthrdLoggerThread)
        Initialize();
    MfLogMessage *pMessage = new MfLogMessage(iType, fsSource, fsMessage);
    MfDblListItem *pli;
    for (MfString *pfs = lstExtra.First(pli); pfs; pfs = lstExtra.Next(pli))
        pMessage->lstExtra + *pfs;
    csQueueLock.Enter();
    plLogQueue->Add(pMessage);
    csQueueLock.Leave();
}

// 0x100073d0 ?LoggerWindow@MfLogger@@SAPAUHWND__@@XZ
HWND__ *MfLogger::LoggerWindow()
{
    HWND__ *hWnd = 0;
    if (pwndLogMessageWindow)
        hWnd = pwndLogMessageWindow->GetLoggerHwnd();
    return hWnd;
}

// 0x100073e0 ?EnableWindowOutput@MfLogger@@SAXH@Z
void MfLogger::EnableWindowOutput(int bEnable)
{
    bWindowEnabled = bEnable;
    if (pwndLogMessageWindow) {
        if (bEnable)
            pwndLogMessageWindow->AddTrayIcon();
        else
            pwndLogMessageWindow->RemoveTrayIcon();
    }
}

// 0x10007410 ?DoWork@MfLogger@@CAXXZ
// Runs on the logger thread (timer): moves the queued messages to the message
// list, shows them and writes the new ones to the log file. After a filter
// change the whole list is shown again.
void MfLogger::DoWork()
{
    csLoggerLock.Enter();
    if (pwndLogMessageWindow) {
        if (bClearWindowPending) {
            bClearWindowPending = 0;
            SendMessage(pwndLogMessageWindow->pList->GetHwnd(), LVM_DELETEALLITEMS, 0, 0);
        }
        MfDblListItem *pliNew = 0;
        MfLogMessage *pMessage = 0;
        csQueueLock.Enter();
        if (plLogQueue->GetCount() > 0) {
            pMessage = (MfLogMessage *)plLogQueue->GetFirst(pliNew);
            plLogMessages->Merge(*plLogQueue, MfDblList::LP_AFTER, 0);
        }
        csQueueLock.Leave();
        int bShown = 0;
        int bNew = 1;
        MfDblListItem *pli = pliNew;
        if (bFilterChangePending) {
            SendMessage(pwndLogMessageWindow->pList->GetHwnd(), LVM_DELETEALLITEMS, 0, 0);
            pMessage = (MfLogMessage *)plLogMessages->GetFirst(pli);
            bNew = 0;
        }
        while (pMessage) {
            if (bFilterChangePending && pli == pliNew)
                bNew = 1;
            pMessage->pli = pli;
            if (bWindowEnabled && !bShown
                && ((pMessage->iFlags & MF_LOG_POPUP)
                    || ((iLoggerFlags & MF_LOG_POPUP) && !(pMessage->iFlags & MF_LOG_QUIET))
                    || !((iLoggerFlags | pMessage->iFlags) & (MF_LOG_QUIET | MF_LOG_POPUP)))) {
                bShown = 1;
                DisplayLogMessageWindow();
            }
            FilterMessage(pMessage);
            if (pofsLogFile && bNew)
                pMessage->Write(*pofsLogFile);
            pMessage = (MfLogMessage *)plLogMessages->GetNext(pli);
        }
    }
    bFilterChangePending = 0;
    csLoggerLock.Leave();
}

// 0x100075a0 ?DisplayLogMessageWindow@MfLogger@@CAXXZ
void MfLogger::DisplayLogMessageWindow()
{
    pwndLogMessageWindow->DisplayLogMessageWindow();
}

// 0x100075b0 ?DisplayLogMessageDialog@MfLogger@@CAXPAVMfLogMessage@1@@Z
void MfLogger::DisplayLogMessageDialog(MfLogMessage *pMessage)
{
    plmdlg = new MfLogMessageDialog(pMessage, pwndLogMessageWindow->pList->GetImageList(),
                                    pwndLogMessageWindow->pList, pwndLogMessageWindow->hWnd);
    plmdlg->DoModal();
    delete plmdlg;
    plmdlg = 0;
}

// 0x10007640 ?LmdNext@MfLogger@@CAXAAPAVMfDblListItem@@@Z
void MfLogger::LmdNext(MfDblListItem *&pli)
{
    csLoggerLock.Enter();
    MfLogMessage *pMessage = (MfLogMessage *)plLogMessages->GetNext(pli);
    csLoggerLock.Leave();
    if (pMessage)
        pMessage->Display(plmdlg);
}

// 0x10007680 ?LmdPrev@MfLogger@@CAXAAPAVMfDblListItem@@@Z
void MfLogger::LmdPrev(MfDblListItem *&pli)
{
    csLoggerLock.Enter();
    MfLogMessage *pMessage = (MfLogMessage *)plLogMessages->GetPrev(pli);
    csLoggerLock.Leave();
    if (pMessage)
        pMessage->Display(plmdlg);
}

// 0x100076c0 ?TypeToString@MfLogger@@SA?AVMfString@@H@Z
MfString MfLogger::TypeToString(int iType)
{
    switch (iType) {
    case MF_LOG_INFORMATION:
        return "Information";
    case MF_LOG_WARNING:
        return "Warning";
    case MF_LOG_ERROR:
        return "Error";
    default:
        return "Unknown";
    }
}

// 0x10007730 ?ClearWindow@MfLogger@@SAXXZ
void MfLogger::ClearWindow()
{
    csLoggerLock.Enter();
    bClearWindowPending = 1;
    csLoggerLock.Leave();
}

// 0x10007760 ?Filter@MfLogger@@SAXHHABVMfString@@@Z
void MfLogger::Filter(int iFilter, int iLevel, const MfString &fsName)
{
    if (iFilter < 0 || iFilter >= 6)
        return;
    csLoggerLock.Enter();
    if (!bFilterChangePending
        && (aFilters[iFilter].iLevel != iLevel
            || ((iFilter == 3 || iFilter == 4 || iFilter == 5) && aFilters[iFilter].fsName != fsName)))
        bFilterChangePending = 1;
    aFilters[iFilter].iLevel = iLevel;
    aFilters[iFilter].fsName = fsName;
    bFilterEnabled = 0;
    int i;
    for (i = 0; i <= 2; i++)
        if (aFilters[i].iLevel == 0) {
            bFilterEnabled = 1;
            break;
        }
    for (i = 3; i <= 5; i++)
        if (aFilters[i].iLevel != 0) {
            bFilterEnabled = 1;
            break;
        }
    csLoggerLock.Leave();
}

// EQUIV 0x10007850 ?FilterMessage@MfLogger@@CAXPAVMfLogMessage@1@@Z: reviewed: case 5 loop computes the next string in ecx directly instead of eax then ecx
void MfLogger::FilterMessage(MfLogMessage *pMessage)
{
    if (bFilterEnabled) {
        int bShow = 1;
        for (int i = 0; i < 6; i++) {
            switch (aFilters[i].iType) {
            case MF_LOG_INFORMATION:
            case MF_LOG_WARNING:
            case MF_LOG_ERROR:
                if (pMessage->iType == aFilters[i].iType && aFilters[i].iLevel == 0)
                    bShow = 0;
                break;
            case 3:
                if (aFilters[i].iLevel)
                    bShow = bShow && pMessage->fsSource.CaseCompare(aFilters[i].fsName) == 0;
                break;
            case 4:
                if (aFilters[i].iLevel)
                    bShow = bShow && pMessage->fsMessage.CaseCompare(aFilters[i].fsName) == 0;
                break;
            case 5:
                if (aFilters[i].iLevel && bShow) {
                    MfDblListItem *pli;
                    MfString *pfs = pMessage->lstExtra.First(pli);
                    bShow = 0;
                    while (pfs && !bShow) {
                        if (pfs->CaseContainsSubString(aFilters[i].fsName, 0) >= 0)
                            bShow = 1;
                        pfs = pMessage->lstExtra.Next(pli);
                    }
                }
                break;
            }
        }
        if (bShow)
            pwndLogMessageWindow->pList->AddMessage(pMessage);
    } else
        pwndLogMessageWindow->pList->AddMessage(pMessage);
}

// 0x10007980 ?Write@MfLogMessage@MfLogger@@QAEXAAVostream@@@Z
void MfLogger::MfLogMessage::Write(ostream &os)
{
    MfString fsTime;
    fsTime.FormatTime("%#m/%#d/%y %#I:%M:%S %p", stTime);
    os << fsTime << "  " << MfLogger::TypeToString(iType) << "  " << fsSource << "  " << fsMessage << endl;
    MfDblListItem *pli;
    for (MfString *pfs = lstExtra.First(pli); pfs; pfs = lstExtra.Next(pli))
        os << "\t" << *pfs << endl;
    os << endl;
}

// 0x10007ac0 ?GetDispInfo@MfLogMessage@MfLogger@@QAEXPAUtagLVDISPINFO@@@Z
// The list view's columns: date, time, source, message, first extra line.
void MfLogger::MfLogMessage::GetDispInfo(tagLVDISPINFO *pdi)
{
    if (pdi->item.mask & LVIF_TEXT) {
        MfString fsText;
        switch (pdi->item.iSubItem) {
        case 0:
            fsText.FormatTime("%#m/%#d/%y", stTime);
            lstrcpy(pdi->item.pszText, fsText);
            break;
        case 1:
            fsText.FormatTime("%#I:%M:%S %p", stTime);
            lstrcpy(pdi->item.pszText, fsText);
            break;
        case 2:
            lstrcpy(pdi->item.pszText, fsSource);
            break;
        case 3:
            lstrcpy(pdi->item.pszText, fsMessage);
            break;
        case 4: {
            MfDblListItem *pli;
            MfString *pfs = lstExtra.First(pli);
            if (pfs) {
                MfStringList lstLines;
                pfs->ConvertToLines(&lstLines);
                MfDblListItem *pliLine;
                MfString *pfsLine = lstLines.First(pliLine);
                if (pfsLine)
                    lstrcpy(pdi->item.pszText, *pfsLine);
            }
            break;
        }
        }
    }
    if (pdi->item.mask & LVIF_IMAGE)
        pdi->item.iImage = iType;
}

// 0x10007c70 ?Display@MfLogMessage@MfLogger@@QAEXPAVMfLogMessageDialog@@@Z
void MfLogger::MfLogMessage::Display(MfLogMessageDialog *pDialog)
{
    MfString fsText;
    fsText.FormatTime("%#m/%#d/%y", stTime);
    SetWindowText(pDialog->hDate, fsText);
    fsText.FormatTime("%#I:%M:%S %p", stTime);
    SetWindowText(pDialog->hTime, fsText);
    SetWindowText(pDialog->hType, MfLogger::TypeToString(iType));
    SetWindowText(pDialog->hSource, fsSource);
    SetWindowText(pDialog->hMessage, fsMessage);
    fsText = NULL_STRING;
    MfDblListItem *pli;
    for (MfString *pfs = lstExtra.First(pli); pfs; pfs = lstExtra.Next(pli)) {
        MfStringList lstLines;
        pfs->ConvertToLines(&lstLines);
        MfDblListItem *pliLine;
        for (MfString *pfsLine = lstLines.First(pliLine); pfsLine; pfsLine = lstLines.Next(pliLine))
            fsText << *pfsLine << "\r\n";
    }
    SetWindowText(pDialog->hExtra, fsText);
    pDialog->pMessage = this;
}

// Adjustor thunks of the deleting destructors:
// 0x10007e70 ??_EMfLoggerFrameUI@@WCA@AEPAXI@Z
// 0x10007e80 ??_EMfStringList@@WBA@AEPAXI@Z

// 0x10007e90 ?InitInstance@MfLoggerFrameUI@@UAEHXZ
int MfLoggerFrameUI::InitInstance()
{
    pFrame = new MfLoggerFrame;
    bLoggerThreadRunning = 1;
    MfLogger::ThreadInitialize();
    return 1;
}

// 0x10007f00 ?ExitInstance@MfLoggerFrameUI@@UAEHXZ
int MfLoggerFrameUI::ExitInstance()
{
    bLoggerThreadRunning = 0;
    DestroyWindow(pFrame->hWnd);
    delete pFrame;
    return 0;
}

// 0x10007f30 ?DoWork@MfLoggerFrameUI@@UAEXXZ
void MfLoggerFrameUI::DoWork()
{
    MfLogger::evtStartup.Signal();
    InitInstance();
    MSG msg;
    while (GetMessage(&msg, 0, 0, 0)) {
        if (!pFrame->pOptions || !pFrame->pOptions->IsDialogMessage(&msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    ExitInstance();
    ModState(MF_THREAD_SHUTDOWN);
}
