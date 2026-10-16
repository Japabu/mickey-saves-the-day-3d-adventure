// MBEngine.dll CMBScript.obj: the script object.
#include "CMBScript.h"

static MBAnimTracker sAnimTracker;
// keeps a reference on the static tracker so that a Release never deletes it
static struct MBAnimTrackerReference {
    MBAnimTrackerReference(void) { sAnimTracker.AddRef(); }
} sAnimTrackerReference;

// the channels a script listens on while it runs
static const long kScriptEventChannels[5] = { -2, -5, -3, -4, -6 };

// 0x1003c540 ?MakeCMBScript@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBScript(void)
{
    IMBUnknown *p = (IMBScript *)new CMBScript;
    if (p)
        p->AddRef();
    return p;
}

// 0x1003c570 ??0CMBScript@@QAE@XZ
CMBScript::CMBScript(void)
    : mID(0), started(false), inBackground(false), exec(0), done(false), mIsWaiting(false),
      paused(false), pauseStartMS(0)
{
    _mRefCnt = 0;
}

// TODO 0x1003c5c0 ?Load@CMBScript@@UAE_NKPAVIMBStream@@@Z: inline budget: original calls CreateObject out of line for outStream/newBlock (and the operator= of mCodeBlock), ours inlines them; block structure of the failure returns unverified (46% similar)
bool CMBScript::Load(unsigned long id, IMBStream *inStream)
{
    if (!inStream)
        return false;
    sAnimTracker.SetupTracker();
    MBComPtr<IMBStream> outStream(MBIID_CMBMemStream);
    MBComPtr<IMBMemStream> memStream(outStream);
    if (!memStream)
        return false;
    MBComPtr<IMBMemoryBlock> newBlock(MBIID_CMBMemoryBlock);
    if (!newBlock)
        return false;
    if (!newBlock->Allocate(0x100))
        return false;
    memStream->SetMemoryBlock(newBlock);
    if (!CompileScript(inStream, outStream, 0))
        return false;
    mCodeBlock = memStream->GetMemoryBlock();
    exec = new TExecutor(this, (char *)mCodeBlock->GetData());
    if (!exec || !exec->ok)
        return false;
    mID = id;
    StartListening();
    MBComPtr<IMBPeriodicalManager> _periodicalManager(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->AddPeriodical(this);
    mClock.CreateObject(MBIID_IMBGameClock);
    SendMessage(-6, 1);
    return true;
}

// 0x1003c810 ??1CMBScript@@QAE@XZ
CMBScript::~CMBScript(void)
{
    MBComPtr<IMBPeriodicalManager> _periodicalManager(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->RemovePeriodical(this);
    StopListening();
    delete exec;
}

// 0x1003c8d0 ?StartListening@CMBScript@@AAEXXZ
void CMBScript::StartListening(void)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster) {
        int i = 0;
        while (i < 5)
            broadcaster->AddListener(kScriptEventChannels[i++], this);
    }
}

// 0x1003c960 ?StopListening@CMBScript@@AAEXXZ
void CMBScript::StopListening(void)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster) {
        int i = 0;
        while (i < 5)
            broadcaster->RemoveListener(kScriptEventChannels[i++], this);
    }
}

// 0x1003c9f0 ?Rewind@CMBScript@@AAE_NXZ
bool CMBScript::Rewind(void)
{
    delete exec;
    exec = new TExecutor(this, (char *)mCodeBlock->GetData());
    if (exec && exec->ok) {
        started = false;
        done = false;
        paused = false;
        StartListening();
        return true;
    }
    return false;
}

// 0x1003ca60 ?Start@CMBScript@@UAEXXZ
void CMBScript::Start(void)
{
    if (IsDone() && !Rewind()) {
        MBDebugStr("ERROR: restarting script %d!\n", GetID());
        return;
    }
    if (!done && !started) {
        started = true;
        SendMessage(-6, 2);
        while (!done && !mIsWaiting)
            Execute();
        if (done) {
            StopListening();
            SendMessage(-6, 6);
        }
    }
}

// 0x1003caf0 ?Stop@CMBScript@@UAEXXZ
void CMBScript::Stop(void)
{
    if (!done) {
        done = true;
        mIsWaiting = false;
        StopListening();
        SendMessage(-6, 6);
    }
}

// 0x1003cb20 ?Pause@CMBScript@@UAEXXZ
void CMBScript::Pause(void)
{
    if (!paused) {
        paused = true;
        pauseStartMS = mClock->GetGameTime();
    }
}

// 0x1003cb40 ?Resume@CMBScript@@UAEXXZ
void CMBScript::Resume(void)
{
    if (paused) {
        long delta = (long)(mClock->GetGameTime() - pauseStartMS);
        if (mIsWaiting && mWaitChannel == 0)
            mWaitInfo.wait.abs_ms += delta;
        pauseStartMS = 0;
        paused = false;
    }
}

// 0x1003cb80 ?Execute@CMBScript@@AAE_NXZ
bool CMBScript::Execute(void)
{
    bool executed = false;
    if (!done && !mIsWaiting && !paused && exec && exec->ok) {
        executed = true;
        mIsWaiting = 0;
        exec->Go();
    }
    if (!exec->ok && !mIsWaiting)
        done = true;
    return executed;
}

// 0x1003cbd0 ?Continue@CMBScript@@AAEXXZ
void CMBScript::Continue(void)
{
    if (!done && started) {
        while (!done && !mIsWaiting && !paused)
            Execute();
        if (done) {
            StopListening();
            SendMessage(-6, 6);
        }
    }
}

// 0x1003cc20 ?Idle@CMBScript@@UAEXXZ
void CMBScript::Idle(void)
{
    if (mIsWaiting && !paused && mWaitChannel == 0 && mWaitInfo.wait.abs_ms <= mClock->GetGameTime())
        mIsWaiting = false;
    if (!mIsWaiting && !done && started && !paused)
        Continue();
}

// 0x1003cc70 ?ListenToEvent@CMBScript@@UAEXJJPAX@Z
void CMBScript::ListenToEvent(long channel, long event, void *data)
{
    if (mIsWaiting && channel == mWaitChannel) {
        switch (channel) {
        case -2: {
            MBAnimEventStruct *animEvent = (MBAnimEventStruct *)data;
            if (animEvent->id == mWaitInfo.anim.id && animEvent->sequence == mWaitInfo.anim.seq)
                mIsWaiting = false;
            break;
        }
        case -4:
            if (event == 1 && ((SoundEndedEvent *)data)->mSoundDescription->GetSoundID() == mWaitInfo.sound.id)
                mIsWaiting = false;
            break;
        }
    }
}

// 0x1003ccd0 ?WaitForActor@CMBScript@@QAEXPAVTSymtabNode@@J@Z
void CMBScript::WaitForActor(TSymtabNode *actor, long path)
{
    unsigned long id = MBHashString(actor->String());
    IMBAnim *anim = sAnimTracker.FindAnimation(id);
    if (anim) {
        if (anim->GetCurrentSequenceIndex() == path && !anim->IsStalled()) {
            mIsWaiting = true;
            exec->paused = 1;
            mWaitChannel = -2;
            mWaitInfo.anim.id = id;
            mWaitInfo.anim.seq = path;
        }
        anim->Release();
    }
}

// EQUIV 0x1003cd60 ?StartActor@CMBScript@@QAEXPAVTSymtabNode@@JJJJ@Z: reviewed: the original copies GetOffset() with the x store scheduled before the y/z loads and duplicates the error MBDebugStr tail into both arms of the inlined String(); calls, arguments, conditions and order agree. Was: scheduling of the GetOffset() copy into loc (x stored before y/z loads) and the error MBDebugStr tail is duplicated per String() arm in the original; if/else, early-return and copy-ctor forms tried (84% similar)
void CMBScript::StartActor(TSymtabNode *actor, long wait, long path, long x, long y)
{
    unsigned long id = MBHashString(actor->String());
    IMBAnim *anim = sAnimTracker.FindAnimation(id);
    if (anim) {
        MBPoint3 loc;
        loc = anim->GetOffset();
        loc.x = x;
        loc.y = y;
        anim->Place(loc);
        anim->SetCurrentSequence(path);
        anim->Play();
    } else {
        MBScriptEvent event;
        event.script = this;
        event.scriptID = GetID();
        event.param.startAnim.id = id;
        event.param.startAnim.seq = path;
        event.param.startAnim.loc.x = 0;
        event.param.startAnim.loc.y = 0;
        SendMessage(-7, 5, event);
        anim = sAnimTracker.FindAnimation(id);
    }
    if (anim) {
        if (wait)
            WaitForActor(actor, path);
        anim->Release();
    } else {
        MBDebugStr("ERROR: Script unable to start anim \"%s\".\n", actor->String());
        return;
    }
}

// 0x1003cea0 ?StopActor@CMBScript@@QAEXPAVTSymtabNode@@@Z
void CMBScript::StopActor(TSymtabNode *actor)
{
    IMBAnim *anim = sAnimTracker.FindAnimation(MBHashString(actor->String()));
    if (anim) {
        anim->Stop();
        anim->Release();
    }
}

// 0x1003cef0 ?CopyActor@CMBScript@@QAEXPAVTSymtabNode@@0JJJ@Z
void CMBScript::CopyActor(TSymtabNode *actor, TSymtabNode *newName, long x, long y, long zplane)
{
}

// 0x1003cf00 ?PlaceActor@CMBScript@@QAEXPAVTSymtabNode@@MMM@Z
void CMBScript::PlaceActor(TSymtabNode *actor, float x, float y, float z)
{
    IMBAnim *anim = sAnimTracker.FindAnimation(MBHashString(actor->String()));
    if (anim)
        anim->Place(MBPoint3((long)x, (long)y, (long)z));
}

// 0x1003cf70 ?MoveActor@CMBScript@@QAEXPAVTSymtabNode@@MMM@Z
void CMBScript::MoveActor(TSymtabNode *actor, float x, float y, float z)
{
    IMBAnim *anim = sAnimTracker.FindAnimation(MBHashString(actor->String()));
    if (anim)
        anim->Move(MBPoint3((long)x, (long)y, (long)z));
}

// 0x1003cfe0 ?StartMovie@CMBScript@@QAEXPAVTSymtabNode@@JJJJJ@Z
void CMBScript::StartMovie(TSymtabNode *movie, long wait, long flags, long x, long y, long z)
{
}

// 0x1003cff0 ?WaitForMovie@CMBScript@@QAEXPAVTSymtabNode@@@Z
void CMBScript::WaitForMovie(TSymtabNode *movie)
{
}

// 0x1003d000 ?StopMovie@CMBScript@@QAEXPAVTSymtabNode@@@Z
void CMBScript::StopMovie(TSymtabNode *movie)
{
}

// 0x1003d010 ?StartSound@CMBScript@@QAEXPAVTSymtabNode@@JJJJJJ@Z
void CMBScript::StartSound(TSymtabNode *sound, long wait, long channel, long volume, long loop, long queue, long force)
{
    long loopCount = !loop;
    if (force)
        MBForceSound(sound->String(), channel, loopCount);
    else if (queue)
        MBForceSound(sound->String(), channel, loopCount);
    else
        MBPlaySound(sound->String(), loopCount);
    if (wait)
        WaitForSound(sound);
}

// EQUIV 0x1003d0a0 ?WaitForSound@CMBScript@@QAEXPAVTSymtabNode@@@Z: reviewed: inline choices of the same helpers only: the original calls MBComPtr::Release and both _QueryInterface out of line where VC6 inlines them; calls, arguments and order agree. Was: inline budget: original calls MBComPtr::Release out of line before MBComCreateInstance and both _QueryInterface (unknown and soundMgr->periodical) out of line (59% similar)
void CMBScript::WaitForSound(TSymtabNode *sound)
{
    MBComPtr<IMBSoundManager> soundMgr(MBIID_IMBSoundManager);
    MBComPtr<IMBPeriodical> soundMgrPeriodical(soundMgr);
    soundMgrPeriodical->Idle();
    unsigned long id = MBHashString(sound->String());
    if (soundMgr->IsSoundPlaying(id)) {
        mIsWaiting = true;
        exec->paused = 1;
        mWaitChannel = -4;
        mWaitInfo.sound.id = id;
    }
}

// 0x1003d180 ?StopSound@CMBScript@@QAEXPAVTSymtabNode@@@Z
void CMBScript::StopSound(TSymtabNode *sound)
{
    MBComPtr<IMBSoundManager> soundMgr(MBIID_IMBSoundManager);
    unsigned long id = MBHashString(sound->String());
    soundMgr->StopSound(id);
}

// EQUIV 0x1003d200 ?StopSound@CMBScript@@QAEXJ@Z: reviewed: the original calls MBComPtr::Release out of line from both Attach paths of the inlined CreateObject; VC6 folds them (the pointer starts null) and selects the QueryInterface result branch-free; the QueryInterface, StopSound and Release calls agree. Was: inline budget: original inlines CreateObject's QI but calls Release out of line from both Attach paths; with this header ours inlines all (guid ctor) or calls _QueryInterface (declared-only); member-template and inline-_QI headers tried (34% similar)
void CMBScript::StopSound(long channel)
{
    MBComPtr<IMBSoundManager> soundMgr;
    soundMgr.CreateObject(MBIID_IMBSoundManager);
    soundMgr->StopSound(channel);
}

// EQUIV 0x1003d290 ?StopAllSound@CMBScript@@QAEXXZ: reviewed: the original calls MBComPtr::Release out of line from both Attach paths of the inlined CreateObject; VC6 folds them (the pointer starts null) and selects the QueryInterface result branch-free; the QueryInterface, StopAllSound and Release calls agree. Was: same as StopSound(long) (36% similar)
void CMBScript::StopAllSound(void)
{
    MBComPtr<IMBSoundManager> soundMgr;
    soundMgr.CreateObject(MBIID_IMBSoundManager);
    soundMgr->StopAllSound();
}

// 0x1003d320 ?StartScript@CMBScript@@QAEXPAVTSymtabNode@@J@Z
void CMBScript::StartScript(TSymtabNode *script, long wait)
{
    if (wait)
        WaitForScript(script);
}

// 0x1003d340 ?WaitForScript@CMBScript@@QAEXPAVTSymtabNode@@@Z
void CMBScript::WaitForScript(TSymtabNode *script)
{
}

// 0x1003d350 ?StopScript@CMBScript@@QAEXPAVTSymtabNode@@@Z
void CMBScript::StopScript(TSymtabNode *script)
{
}

// 0x1003d360 ?StopAllScripts@CMBScript@@QAEXXZ
void CMBScript::StopAllScripts(void)
{
}

// 0x1003d370 ?BackgroundScript@CMBScript@@QAEXJ@Z
void CMBScript::BackgroundScript(long background)
{
}

// 0x1003d380 ?Notify@CMBScript@@QAEXPAVTSymtabNode@@@Z
void CMBScript::Notify(TSymtabNode *msg)
{
    MBScriptEvent event;
    event.script = this;
    event.scriptID = GetID();
    event.param.notifyMsg.msg = msg->String();
    SendMessage(-6, 4, event);
}

// 0x1003d3c0 ?Notify@CMBScript@@QAEXJ@Z
void CMBScript::Notify(long linenum)
{
    MBScriptEvent event;
    event.script = this;
    event.scriptID = GetID();
    event.param.notifyLine.line = linenum;
    SendMessage(-6, 4, event);
}

// 0x1003d400 ?WaitNMilliseconds@CMBScript@@QAEXJ@Z
void CMBScript::WaitNMilliseconds(long ms)
{
    mIsWaiting = true;
    mWaitChannel = 0;
    mWaitInfo.wait.abs_ms = mClock->GetGameTime() + ms;
    exec->paused = 1;
}

// 0x1003d430 ?GotoScene@CMBScript@@QAEXPAVTSymtabNode@@@Z
void CMBScript::GotoScene(TSymtabNode *scene)
{
}

// 0x1003d440 ?PauseEverything@CMBScript@@QAEXJ@Z
void CMBScript::PauseEverything(long pause)
{
}

// 0x1003d450 ?SetScriptVariable@CMBScript@@UAE_NPBD0@Z
bool CMBScript::SetScriptVariable(const char *varname, const char *value)
{
    return exec->SetScriptVariable(varname, value);
}

// 0x1003d470 ?SetScriptVariable@CMBScript@@UAE_NPBDN@Z
bool CMBScript::SetScriptVariable(const char *varname, double value)
{
    return exec->SetScriptVariable(varname, value);
}

// 0x1003d490 ?GetScriptVariable@CMBScript@@UAE_NPBDPAPBD@Z
bool CMBScript::GetScriptVariable(const char *varname, const char **valueDest)
{
    return exec->GetScriptVariable(varname, valueDest);
}

// 0x1003d4b0 ?GetScriptVariable@CMBScript@@UAE_NPBDPAN@Z
bool CMBScript::GetScriptVariable(const char *varname, double *valueDest)
{
    return exec->GetScriptVariable(varname, valueDest);
}

// 0x1003d4d0 ?SendMessage@CMBScript@@AAEXJJ@Z
void CMBScript::SendMessage(long channel, long msg)
{
    MBScriptEvent event;
    event.script = this;
    event.scriptID = GetID();
    SendMessage(channel, msg, event);
}

// 0x1003d500 ?SendMessage@CMBScript@@AAEXJJAAUMBScriptEvent@@@Z
void CMBScript::SendMessage(long channel, long msg, MBScriptEvent &event)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    IMBEventManager *manager = broadcaster;
    if (manager)
        manager->BroadcastEvent(channel, msg, &event);
}

// 0x1003e350 ?QueryInterface@CMBScript@@UAEJABUMBGUID@@PAPAX@Z
long CMBScript::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBScript *)this;
    if (iid == MBIID_IMBEventListener)
        p = (IMBEventListener *)this;
    if (iid == MBIID_IMBPeriodical)
        p = (IMBPeriodical *)this;
    if (iid == MBIID_IMBScript)
        p = (IMBScript *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1003e4a0 ?AddRef@CMBScript@@UAEKXZ
unsigned long CMBScript::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1003e4b0 ?Release@CMBScript@@UAEKXZ
unsigned long CMBScript::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1003e4e0 ?IsStarted@CMBScript@@UBE_NXZ
bool CMBScript::IsStarted(void) const
{
    return started;
}

// 0x1003e4f0 ?IsDone@CMBScript@@UBE_NXZ
bool CMBScript::IsDone(void) const
{
    return done;
}

// 0x1003e500 ?InBackground@CMBScript@@UBE_NXZ
bool CMBScript::InBackground(void) const
{
    return inBackground;
}

// 0x1003e510 ?GetID@CMBScript@@UBEJXZ
long CMBScript::GetID(void) const
{
    return mID;
}

// Matched by this code (static initializers, adjustor thunks):
// 0x1003c4e0 _$E75
// 0x1003c4f0 _$E72
// 0x1003c500 _$E74
// 0x1003c510 _$E73
// 0x1003c520 _$E78
// 0x1003c530 _$E77
// 0x1003d620 _$E22
// 0x1003d630 _$E21
// 0x1003d700 _$E25
// 0x1003d710 _$E24
// 0x1003d7e0 _$E85
// 0x1003d800 _$E84
// 0x1003d810 _$E28
// 0x1003d820 _$E27
// 0x1003d8f0 _$E31
// 0x1003d900 _$E30
// 0x1003d9c0 _$E34
// 0x1003d9d0 _$E33
// 0x1003da90 ?QueryInterface@CMBScript@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1003daa0 ?AddRef@CMBScript@@W3AEKXZ
// 0x1003dab0 ?Release@CMBScript@@W3AEKXZ
// 0x1003dac0 ?QueryInterface@CMBScript@@W7AEJABUMBGUID@@PAPAX@Z
// 0x1003dad0 ?AddRef@CMBScript@@W7AEKXZ
// 0x1003dae0 ?Release@CMBScript@@W7AEKXZ
// 0x1003daf0 _$E37
// 0x1003db00 _$E36
// 0x1003dbb0 _$E40
// 0x1003dbc0 _$E39
// 0x1003dc70 _$E43
// 0x1003dc80 _$E42
// 0x1003dd30 _$E46
// 0x1003dd40 _$E45
// 0x1003de00 _$E49
// 0x1003de10 _$E48
// 0x1003ded0 _$E52
// 0x1003dee0 _$E51
// 0x1003df90 _$E55
// 0x1003dfa0 _$E54
// 0x1003e050 _$E58
// 0x1003e060 _$E57
// 0x1003e110 _$E61
// 0x1003e120 _$E60
// 0x1003e1d0 _$E64
// 0x1003e1e0 _$E63
// 0x1003e290 _$E67
// 0x1003e2a0 _$E66
// TODO 0x1003d570 ??4?$MBComPtr@VIMBMemoryBlock@@@@QAEAAV0@PAVIMBMemoryBlock@@@Z: only emitted out of line once Load matches
// TODO 0x1003d5a0 ?_QueryInterface@?$MBComPtr@VIMBPeriodical@@@@AAEJABV?$MBComPtr@VIMBSoundManager@@@@@Z: only emitted out of line once WaitForSound matches
