// CMBChooseSaveFile.cpp (CMBChooseSaveFileWin.obj) - the Windows "Save As" dialog.
#include "CMBChooseSaveFile.h"

// 0x1003e520 ?MakeCMBChooseSaveFile@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBChooseSaveFile(void)
{
    IMBUnknown *p = new CMBChooseSaveFile;
    if (p)
        p->AddRef();
    return p;
}

// 0x1003e550 ??0CMBChooseSaveFile@@QAE@XZ
CMBChooseSaveFile::CMBChooseSaveFile(void)
: mFileSpec(0)
{
    _mRefCnt = 0;
    mFilename[0] = 0;

    // the dialog's owner is the game window
    HWND owner = 0;
    IMBScreenManager *screenManager = 0;
    MBGetScreenManager(&screenManager);
    if (screenManager) {
        IMBScreen *screen = 0;
        screenManager->GetScreenByIndex(0, &screen);
        if (screen) {
            IMBWinScreen *winScreen = 0;
            screen->QueryInterface(MBIID_IMBWinScreen, (void **)&winScreen);
            if (winScreen) {
                owner = winScreen->GetHWND();
                winScreen->Release();
            }
            screen->Release();
        }
        screenManager->Release();
    }

    mOfn.lStructSize = sizeof(OPENFILENAME);
    mOfn.hwndOwner = owner;
    mOfn.hInstance = 0;
    mOfn.lpstrFilter = "";
    mOfn.lpstrCustomFilter = 0;
    mOfn.nMaxCustFilter = 0;
    mOfn.nFilterIndex = 0;
    mOfn.lpstrFile = mFilename;
    mOfn.nMaxFile = MAX_PATH;
    mOfn.lpstrFileTitle = 0;
    mOfn.lpstrInitialDir = 0;
    mOfn.lpstrTitle = 0;
    mOfn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_EXPLORER;
    mOfn.nFileOffset = 0;
    mOfn.nFileExtension = 0;
    mOfn.lpstrDefExt = 0;
    mOfn.lCustData = 0;
    mOfn.lpfnHook = 0;
    mOfn.lpTemplateName = 0;
}

// 0x1003e670 ?QueryInterface@CMBChooseSaveFile@@UAEJABUMBGUID@@PAPAX@Z
long CMBChooseSaveFile::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (iid == MBIID_IMBChooseSaveFile)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1003e740 ?AddRef@CMBChooseSaveFile@@UAEKXZ
unsigned long CMBChooseSaveFile::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1003e750 ?Release@CMBChooseSaveFile@@UAEKXZ
unsigned long CMBChooseSaveFile::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1003e780 ??1CMBChooseSaveFile@@QAE@XZ
CMBChooseSaveFile::~CMBChooseSaveFile(void)
{
}

// 0x1003e790 ?ChooseSaveFile@CMBChooseSaveFile@@UAE_NXZ
// Runs the modal dialog; other components hear about it on the event channel.
bool CMBChooseSaveFile::ChooseSaveFile(void)
{
    bool result = false;
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    eventManager->BroadcastEvent(-3, 3, 0);
    BOOL chosen = GetSaveFileName(&mOfn);
    eventManager->BroadcastEvent(-3, 4, 0);
    if (chosen) {
        result = true;
        CreateFileSpec();
    }
    return result;
}

// 0x1003e830 ?GetFileSpec@CMBChooseSaveFile@@UAEJPAPAVIMBFileSpec@@@Z
long CMBChooseSaveFile::GetFileSpec(IMBFileSpec **fileSpec)
{
    long result = 0;
    *fileSpec = mFileSpec;
    if (mFileSpec)
        mFileSpec->AddRef();
    else
        result = -2;
    return result;
}

// 0x1003e860 ?CompleteSave@CMBChooseSaveFile@@UAEJXZ
long CMBChooseSaveFile::CompleteSave(void)
{
    return 0;
}

// 0x1003e870 ?SetDefaultFilename@CMBChooseSaveFile@@UAEJPBD@Z
long CMBChooseSaveFile::SetDefaultFilename(const char *defaultFilename)
{
    strcpy(mFilename, defaultFilename);
    return 0;
}

// 0x1003e8a0 ?CreateFileSpec@CMBChooseSaveFile@@IAEXXZ
void CMBChooseSaveFile::CreateFileSpec(void)
{
    if (mFileSpec) {
        mFileSpec->Release();
        mFileSpec = 0;
    }
    if (MBstrlen(mFilename) > 0) {
        IMBUnknown *unk;
        if (MBComCreateInstance(0, MBIID_CMBWinFileSpec, &unk) == 0) {
            if (unk->QueryInterface(MBIID_IMBFileSpec, (void **)&mFileSpec) == 0)
                mFileSpec->SetPath(mFilename);
            unk->Release();
        }
    }
}


// Matched by this code:
// 0x1003e910 _$E68
// 0x1003e930 _$E67
// 0x1003e940 _$E16
// 0x1003e950 _$E15
// 0x1003ea20 _$E19
// 0x1003ea30 _$E18
// 0x1003eb00 _$E22
// 0x1003eb10 _$E21
// 0x1003ebe0 _$E25
// 0x1003ebf0 _$E24
// 0x1003ecb0 _$E28
// 0x1003ecc0 _$E27
// 0x1003ed80 _$E31
// 0x1003ed90 _$E30
// 0x1003ee40 _$E34
// 0x1003ee50 _$E33
// 0x1003ef00 _$E37
// 0x1003ef10 _$E36
// 0x1003efc0 _$E40
// 0x1003efd0 _$E39
// 0x1003f090 _$E43
// 0x1003f0a0 _$E42
// 0x1003f160 _$E46
// 0x1003f170 _$E45
// 0x1003f220 _$E49
// 0x1003f230 _$E48
// 0x1003f2e0 _$E52
// 0x1003f2f0 _$E51
// 0x1003f3a0 _$E55
// 0x1003f3b0 _$E54
// 0x1003f460 _$E58
// 0x1003f470 _$E57
// 0x1003f520 _$E61
// 0x1003f530 _$E60
