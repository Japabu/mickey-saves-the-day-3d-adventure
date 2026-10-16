// MBEngine.dll unit CMBWinDebugConsole (.\MBEngineRelease\CMBWinDebugConsole.obj)
#include "CMBWinDebugConsole.h"

// 0x1004abf0 ?MakeCMBWinDebugConsole@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBWinDebugConsole(void)
{
    static IMBUnknown *instance = MBComUtilAddSingleton((IMBDebugConsole *)new CMBWinDebugConsole);
    if (instance)
        instance->AddRef();
    return instance;
}

void *CMBWinDebugConsole::sStdInput;
void *CMBWinDebugConsole::sStdOutput;
void *CMBWinDebugConsole::sStdError;
MBList<char *> CMBWinDebugConsole::sInput;
bool CMBWinDebugConsole::sParentIsActive;
bool CMBWinDebugConsole::sInputThreadIsActive;

// 0x1004ad30 ??0CMBWinDebugConsole@@QAE@XZ
CMBWinDebugConsole::CMBWinDebugConsole(void)
{
    _mRefCnt = 0;
    MBComPtr<IMBPeriodicalManager> _periodicalManager;
    _periodicalManager.CreateObject(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->AddPeriodical(this);
    MBComPtr<IMBEventManager> broadcaster;
    broadcaster.CreateObject(MBIID_IMBEventManager);
    broadcaster->AddListener(-100, this);
    AllocConsole();
    sStdInput = GetStdHandle((unsigned long)-10);
    sStdOutput = GetStdHandle((unsigned long)-11);
    sStdError = GetStdHandle((unsigned long)-12);
    sParentIsActive = true;
    _beginthread(Read, 1024, this);
}

// 0x1004ae70 ?QueryInterface@CMBWinDebugConsole@@UAEJABUMBGUID@@PAPAX@Z
long CMBWinDebugConsole::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBDebugConsole)
        p = (IMBDebugConsole *)this;
    if (iid == MBIID_IMBEventListener)
        p = (IMBEventListener *)this;
    if (iid == MBIID_IMBPeriodical)
        p = (IMBPeriodical *)this;
    if (iid == MBIID_IMBUnknown)
        p = (IMBPeriodical *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1004afd0 ?AddRef@CMBWinDebugConsole@@UAEKXZ
unsigned long CMBWinDebugConsole::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1004afe0 ?Release@CMBWinDebugConsole@@UAEKXZ
unsigned long CMBWinDebugConsole::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}

// 0x1004b010 ??1CMBWinDebugConsole@@IAE@XZ
CMBWinDebugConsole::~CMBWinDebugConsole(void)
{
    sParentIsActive = false;
    MBComPtr<IMBPeriodicalManager> _periodicalManager(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->RemovePeriodical(this);
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(-100, this);
    FreeConsole();
}

// 0x1004b110 ?PositionWindow@CMBWinDebugConsole@@UAEXVMBRect@@@Z
void CMBWinDebugConsole::PositionWindow(MBRect pos)
{
    _SMALL_RECT rect;
    rect.Top = (short)pos.top;
    rect.Left = (short)pos.left;
    rect.Bottom = (short)pos.bottom;
    rect.Right = (short)pos.right;
    SetConsoleWindowInfo(sStdOutput, 1, &rect);
}

// 0x1004b160 ?ListenToEvent@CMBWinDebugConsole@@UAEXJJPAX@Z
void CMBWinDebugConsole::ListenToEvent(long channel, long event, void *data)
{
    if (event == 0)
        Write((const char *)data);
}

// 0x1004b180 ?Idle@CMBWinDebugConsole@@UAEXXZ
void CMBWinDebugConsole::Idle(void)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    while (sInput.size() > 0) {
        char *s = sInput.front();
        sInput.pop_front();
        if (broadcaster)
            broadcaster->BroadcastEvent(-100, 1, s);
        if (s)
            delete s;
    }
}

// 0x1004b240 ?Read@CMBWinDebugConsole@@KAXPAX@Z
void CMBWinDebugConsole::Read(void *context)
{
    unsigned long num_read;
    CMBCriticalSection criticalSection;
    char buf[256];
    sInputThreadIsActive = true;
    while (sParentIsActive) {
        buf[0] = 0;
        if (ReadConsoleA(sStdInput, buf, 255, &num_read, 0) && num_read > 0) {
            char *s = new char[num_read + 1];
            buf[num_read] = 0;
            MBstrcpy(s, buf);
            criticalSection.Enter();
            sInput.push_back(s);
            criticalSection.Leave();
        }
    }
    sInputThreadIsActive = false;
}

// 0x1004b340 ?Write@CMBWinDebugConsole@@KA_NPBD@Z
bool CMBWinDebugConsole::Write(const char *output)
{
    unsigned long num_written;
    return WriteConsoleA(sStdOutput, output, MBstrlen(output), &num_written, 0) ? true : false;
}


// Matched by this code:
// 0x1004ac60 _$E26
// 0x1004ac70 _$E22
// 0x1004aca0 _$E25
// 0x1004acb0 _$E23
// 0x1004b370 ?erase@?$list@PADV?$MBAllocator@PAD@@@std@@QAE?AViterator@12@V312@@Z
// 0x1004b3b0 _$E33
// 0x1004b3d0 _$E32
// 0x1004b3e0 ?QueryInterface@CMBWinDebugConsole@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1004b3f0 ?AddRef@CMBWinDebugConsole@@W3AEKXZ
// 0x1004b400 ?Release@CMBWinDebugConsole@@W3AEKXZ
// 0x1004b410 ?QueryInterface@CMBWinDebugConsole@@W7AEJABUMBGUID@@PAPAX@Z
// 0x1004b420 ?AddRef@CMBWinDebugConsole@@W7AEKXZ
// 0x1004b430 ?Release@CMBWinDebugConsole@@W7AEKXZ
// steamboatapp.dll 0x100f8320 _$E26
// steamboatapp.dll 0x100f8ed0 _$E26
// steamboatapp.dll 0x100fcdb0 _$E26
// steamboatapp.dll 0x100fce50 _$E26
// steamboatapp.dll 0x100fcef0 _$E26
// steamboatapp.dll 0x10112610 _$E26
// steamboatapp.dll 0x10113930 _$E26
// steamboatapp.dll 0x10114670 _$E26
// steamboatapp.dll 0x10118790 _$E26
