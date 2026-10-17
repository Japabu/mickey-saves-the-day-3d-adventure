#include "SimpleNode.h"

IMBStringTable *SimpleNode::sStringTable = 0;
MBLocale SimpleNode::sLocale;

// The engine's resource databases: one Database per zip or directory, keyed by name hash.
#include "../MBEngine/DatabaseMgr.h"

// EQUIV 0x10003be0 ??0SimpleNode@@QAE@PAVIMBGameEngine@@_N@Z: reviewed: inline choice of the same helper: the original calls MBComPtr::Release out of line inside mView.CreateObject and in the world pointer assignment, VC6 inlines them and places the blocks differently; the aligned call sequences agree. Was: inline budget: the original calls MBComPtr<IMBView>::Release out of line inside mView.CreateObject (ours inlines it); rest of the body matches
SimpleNode::SimpleNode(IMBGameEngine *engine, bool enableMessageRouting)
    : mEngine(engine), mRouteMessages(enableMessageRouting), mMouseMonitor(MBIID_CMBMouseMonitor)
{
    mEngine = engine;
    _mRefCnt = 0;
    IMBScreenManager *screenManager;
    IMBScreen *tempScreen;
    MBSize bounds(0, 0);
    MBComPtr<IMBScreen> screen;
    MBGetScreenManager(&screenManager);
    screenManager->GetScreenByIndex(0, &tempScreen);
    screen.Attach(tempScreen);
    screenManager->Release();
    if (screen)
        bounds.SetSize(screen->GetWidth(), screen->GetHeight());
    mWorld.CreateObject(MBIID_CMBWorld);
    mView.CreateObject(MBIID_CMBView);
    mWorld->SetBounds(MBRect(MBPoint(0, 0), bounds));
    mView->SetWorldPosition(MBPoint(0, 0));
    mView->SetScreenPosition(MBPoint(0, 0));
    mView->SetSize(bounds.w, bounds.h);
    mView->AttachToWorld(mWorld);
    if (screen)
        mView->AttachToScreen(screen);
    MBComPtr<IMBViewManager> viewManager(MBIID_CMBViewManager);
    viewManager->AddView(mView);
}

// 0x10003e50 ??1SimpleNode@@UAE@XZ
SimpleNode::~SimpleNode()
{
    MBComPtr<IMBPeriodicalManager> _periodicalManager(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->RemovePeriodical(this);
    MBComPtr<IMBViewManager> viewManager(MBIID_CMBViewManager);
    if (viewManager)
        viewManager->RemoveView(mView);
}

// 0x10003f50 ?StartListening@SimpleNode@@MAEXXZ
void SimpleNode::StartListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster) {
        broadcaster->AddListener(-1, this);
        broadcaster->AddListener(-2, this);
        broadcaster->AddListener(-3, this);
        broadcaster->AddListener(-4, this);
    }
}

// 0x10004010 ?StopListening@SimpleNode@@MAEXXZ
void SimpleNode::StopListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster) {
        broadcaster->RemoveListener(-1, this);
        broadcaster->RemoveListener(-2, this);
        broadcaster->RemoveListener(-3, this);
        broadcaster->RemoveListener(-4, this);
    }
}

// 0x100040d0 ?OnStart@SimpleNode@@UAEXXZ
void SimpleNode::OnStart()
{
    MBComPtr<IMBPeriodicalManager> _periodicalManager(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->AddPeriodical(this);
    StartListening();
}

// 0x10004150 ?OnPause@SimpleNode@@UAEXXZ
void SimpleNode::OnPause()
{
    HideCursor();
    StopListening();
}

// 0x10004170 ?OnResume@SimpleNode@@UAEXXZ
void SimpleNode::OnResume()
{
    ShowCursor();
    StartListening();
}

// 0x10004190 ?OnEnd@SimpleNode@@UAEXXZ
void SimpleNode::OnEnd()
{
    StopListening();
}

// 0x100041a0 ?OnQuit@SimpleNode@@UAEXXZ
void SimpleNode::OnQuit()
{
}

// 0x100041b0 ?OnPreRender@SimpleNode@@UAEXV?$MBComPtr@VIMBScreen@@@@@Z
void SimpleNode::OnPreRender(MBComPtr<IMBScreen> screen)
{
    PlaceCursor();
}

// 0x100041d0 ?OnPostRender@SimpleNode@@UAEXV?$MBComPtr@VIMBScreen@@@@@Z
void SimpleNode::OnPostRender(MBComPtr<IMBScreen> screen)
{
    if (screen) {
    }
}

// 0x100041e0 ?OnPostFlip@SimpleNode@@UAEXV?$MBComPtr@VIMBScreen@@@@@Z
void SimpleNode::OnPostFlip(MBComPtr<IMBScreen> screen)
{
}

// 0x100041f0 ?OnKeyDown@SimpleNode@@UAEXAAUMBKeyboardEvent@@@Z
void SimpleNode::OnKeyDown(MBKeyboardEvent &keyEvent)
{
}

// 0x10004200 ?OnKeyUp@SimpleNode@@UAEXAAUMBKeyboardEvent@@@Z
void SimpleNode::OnKeyUp(MBKeyboardEvent &keyEvent)
{
}

// 0x10004210 ?OnLMouseDown@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
void SimpleNode::OnLMouseDown(MBMouseEvent &evt)
{
}

// 0x10004220 ?OnLMouseUp@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
void SimpleNode::OnLMouseUp(MBMouseEvent &evt)
{
}

// 0x10004230 ?OnLMouseDblClk@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
void SimpleNode::OnLMouseDblClk(MBMouseEvent &evt)
{
}

// 0x10004240 ?OnMMouseDown@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
void SimpleNode::OnMMouseDown(MBMouseEvent &evt)
{
}

// 0x10004250 ?OnMMouseUp@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
void SimpleNode::OnMMouseUp(MBMouseEvent &evt)
{
}

// 0x10004260 ?OnMMouseDblClk@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
void SimpleNode::OnMMouseDblClk(MBMouseEvent &evt)
{
}

// 0x10004270 ?OnRMouseDown@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
void SimpleNode::OnRMouseDown(MBMouseEvent &evt)
{
}

// 0x10004280 ?OnRMouseUp@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
void SimpleNode::OnRMouseUp(MBMouseEvent &evt)
{
}

// 0x10004290 ?OnRMouseDblClk@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
void SimpleNode::OnRMouseDblClk(MBMouseEvent &evt)
{
}

// 0x100042a0 ?OnMouseMove@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
void SimpleNode::OnMouseMove(MBMouseEvent &evt)
{
}

// 0x100042b0 ?OnAnimSeqStarted@SimpleNode@@UAEXKK@Z
void SimpleNode::OnAnimSeqStarted(unsigned long id, unsigned long seq)
{
}

// 0x100042c0 ?OnAnimSeqEnded@SimpleNode@@UAEXKK@Z
void SimpleNode::OnAnimSeqEnded(unsigned long id, unsigned long seq)
{
}

// 0x100042d0 ?OnAnimNotify@SimpleNode@@UAEXKKK@Z
void SimpleNode::OnAnimNotify(unsigned long id, unsigned long seq, unsigned long frame)
{
}

// EQUIV 0x100042e0 ?ListenToEvent@SimpleNode@@UAEXJJPAX@Z: reviewed: the original also stores the sound description pointer into a dead stack slot and forms this - 8 with lea (VC6: add); the rest, including the jump table, is the same. Was: sound cases: the original stores the description pointer into the data slot and keeps this-8 in edi; everything else matches
void SimpleNode::ListenToEvent(long channel, long event, void *data)
{
    IMBSoundDescription *desc;
    if (mRouteMessages) {
        switch (channel) {
        case -1:
            switch (event) {
            case 0:
                OnKeyDown(*(MBKeyboardEvent *)data);
                break;
            case 1:
                OnKeyUp(*(MBKeyboardEvent *)data);
                break;
            case 5:
                switch (((MBMouseEvent *)data)->mButton) {
                case 1:
                    OnLMouseDown(*(MBMouseEvent *)data);
                    break;
                case 2:
                    OnRMouseDown(*(MBMouseEvent *)data);
                    break;
                }
                break;
            case 6:
                switch (((MBMouseEvent *)data)->mButton) {
                case 1:
                    OnLMouseUp(*(MBMouseEvent *)data);
                    break;
                case 2:
                    OnRMouseUp(*(MBMouseEvent *)data);
                    break;
                }
                break;
            }
            break;
        case -3:
            switch (event) {
            case 0:
                OnPreRender((IMBScreen *)data);
                break;
            case 1:
                OnPostRender((IMBScreen *)data);
                break;
            case 2:
                OnPostFlip((IMBScreen *)data);
                break;
            case 5:
                OnQuit();
                break;
            }
            break;
        case -2:
            switch (event) {
            case 2:
                OnAnimNotify(((MBAnimEventStruct *)data)->id, ((MBAnimEventStruct *)data)->sequence,
                             ((MBAnimEventStruct *)data)->frame);
                break;
            case 5:
                OnAnimSeqStarted(((MBAnimEventStruct *)data)->id, ((MBAnimEventStruct *)data)->sequence);
                break;
            case 6:
                OnAnimSeqEnded(((MBAnimEventStruct *)data)->id, ((MBAnimEventStruct *)data)->sequence);
                break;
            }
            break;
        case -4:
            switch (event) {
            case 0:
                desc = ((SoundEndedEvent *)data)->mSoundDescription;
                OnSoundStarted(desc->GetChannel(), desc->GetSoundID());
                break;
            case 1:
                desc = ((SoundEndedEvent *)data)->mSoundDescription;
                OnSoundEnded(desc->GetChannel(), desc->GetSoundID());
                break;
            }
            break;
        }
    }
}

// 0x10004540 ?Idle@SimpleNode@@UAEXXZ
void SimpleNode::Idle()
{
}

// 0x10004550 ?HideCursor@SimpleNode@@UAEXXZ
void SimpleNode::HideCursor()
{
    if (mCursor)
        mCursor->Hide();
    else
        MBShowSystemCursor(false);
}

// 0x10004570 ?ShowCursor@SimpleNode@@UAEXXZ
void SimpleNode::ShowCursor()
{
    if (mCursor) {
        while (mCursor->Show()) {}
    }
    else
        MBShowSystemCursor(true);
}

// 0x100045a0 ?PlaceCursor@SimpleNode@@IAEXXZ
void SimpleNode::PlaceCursor()
{
    MBPoint point;
    MBPoint3 point3;
    if (mCursor) {
        IMBScreen *screen;
        if (MBGetScreenMouseLocation(point, &screen)) {
            point3.x = point.x;
            point3.y = point.y;
            point3.z = 10000;
            mCursor->Place(point3);
            screen->Release();
            MBShowSystemCursor(false);
        } else {
            point3.x = 1000;
            point3.y = 1000;
            point3.z = 0;
            mCursor->Place(point3);
            MBShowSystemCursor(true);
        }
    }
}

// 0x10004650 ?SetCursor@SimpleNode@@UAEXV?$MBComPtr@VIMBAnim@@@@FF@Z
void SimpleNode::SetCursor(MBComPtr<IMBAnim> anim, short xOffset, short yOffset)
{
    HideCursor();
    MBShowSystemCursor(false);
    mCursor = anim;
    ShowCursor();
    unsigned long numCasts = mCursor->GetNumCasts();
    for (unsigned long i = 1; i <= numCasts; i++)
        mCursor->SetCastMemberRegistrationPoint(i, MBPoint(xOffset, yOffset));
}

// 0x100046f0 ?SetCursorLocation@SimpleNode@@UAEXVMBPoint@@@Z
void SimpleNode::SetCursorLocation(MBPoint pt)
{
}

static DatabaseMgr sDatabaseMgr;

// 0x10004960 ?SetResourceDirectories@SimpleNode@@QAEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@0@Z
void SimpleNode::SetResourceDirectories(const MBString &cdPath, const MBString &hdPath)
{
    sDatabaseMgr.Initialize(cdPath, hdPath);
}

// 0x10004980 ?AddDatabase@SimpleNode@@QAEXPBD@Z
void SimpleNode::AddDatabase(const char *name)
{
    sDatabaseMgr.AddDatabase(name);
}

// 0x100049a0 ?RemoveDatabase@SimpleNode@@QAEXPBD@Z
void SimpleNode::RemoveDatabase(const char *name)
{
    sDatabaseMgr.RemoveDatabase(name);
}

// 0x100049c0 ?GetStream@SimpleNode@@QAE?AV?$MBComPtr@VIMBStream@@@@KK@Z
MBComPtr<IMBStream> SimpleNode::GetStream(unsigned long id, unsigned long type)
{
    MBComPtr<IMBStream> stream;
    MBComPtr<IMBResourceFileList> mgr(MBIID_CMBResourceMgr);
    if (mgr) {
        IMBStream *streamPtr;
        mgr->GetStream(id, type, &streamPtr);
        stream.Attach(streamPtr);
    }
    return stream;
}

// 0x10004a60 ?CreateAnim@SimpleNode@@QAE?AV?$MBComPtr@VIMBAnim@@@@PBD@Z
MBComPtr<IMBAnim> SimpleNode::CreateAnim(const char *name)
{
    if (MBstrlen(name) > 27)
        MBDebugStr("WARNING: Anim name too long for Mac: %s.\n", name);
    return CreateAnim(MBHashString(name));
}

// 0x10004ab0 ?CreateAnim@SimpleNode@@QAE?AV?$MBComPtr@VIMBAnim@@@@J@Z
MBComPtr<IMBAnim> SimpleNode::CreateAnim(long id)
{
    MBComPtr<IMBAnim> anim;
    MBComPtr<IMBStream> animStream = GetStream(id, 'anm ');
    if (animStream) {
        anim.CreateObject(MBIID_CMBAnim);
        if (anim && anim->Load(id, animStream, false) == 0) {
            anim->SetCurrentSequence(0);
            anim->Play();
            mWorld->AddObject(anim);
        } else
            anim.Release();
    }
    if (!anim)
        MBDebugStr("WARN: anim %d failed to load\n", id);
    return anim;
}

// 0x10004be0 ?CloneAnim@SimpleNode@@QAE?AV?$MBComPtr@VIMBAnim@@@@ABV2@@Z
MBComPtr<IMBAnim> SimpleNode::CloneAnim(const MBComPtr<IMBAnim> &src)
{
    IMBAnim *clone;
    src->Clone(&clone);
    MBComPtr<IMBAnim> anim;
    anim.Attach(clone);
    if (anim) {
        anim->SetCurrentSequence(0);
        anim->Play();
        mWorld->AddObject(anim);
    } else
        MBDebugStr("WARN: anim clone failed\n");
    return anim;
}

// 0x10004c60 ?CreateFont@SimpleNode@@QAE?AV?$MBComPtr@VIMBFont@@@@PBD@Z
MBComPtr<IMBFont> SimpleNode::CreateFont(const char *name)
{
    return CreateFont(MBHashString(name));
}

// 0x10004c90 ?CreateFont@SimpleNode@@QAE?AV?$MBComPtr@VIMBFont@@@@J@Z
MBComPtr<IMBFont> SimpleNode::CreateFont(long id)
{
    MBComPtr<IMBFont> font;
    MBComPtr<IMBStream> fontStream = GetStream(id, 'qsh ');
    if (fontStream) {
        font.CreateObject(MBIID_CMBFont);
        if (font) {
            MBComPtr<IMBBitmapFont> ptr(font);
            ptr->LoadFont(id, fontStream);
            MBComPtr<IMBStream> cmapStream = GetStream(id, 'cmp ');
            if (cmapStream)
                ptr->LoadCharacterMap(id, cmapStream);
            else
                font.Release();
        }
    }
    if (!font)
        MBDebugStr("WARN: font %d failed to load\n", id);
    return font;
}

// 0x10004df0 ?CreateUnicodeFont@SimpleNode@@QAE?AV?$MBComPtr@VIMBFont@@@@PBD@Z
MBComPtr<IMBFont> SimpleNode::CreateUnicodeFont(const char *name)
{
    return CreateUnicodeFont(MBHashString(name));
}

// 0x10004e20 ?CreateUnicodeFont@SimpleNode@@QAE?AV?$MBComPtr@VIMBFont@@@@J@Z
MBComPtr<IMBFont> SimpleNode::CreateUnicodeFont(long id)
{
    MBComPtr<IMBFont> font;
    MBComPtr<IMBStream> fontStream = GetStream(id, 'qsh ');
    if (fontStream) {
        font.CreateObject(MBIID_CMBUniFont);
        if (font) {
            MBComPtr<IMBBitmapFont> ptr(font);
            ptr->LoadFont(id, fontStream);
            MBComPtr<IMBStream> cmapStream = GetStream(id, 'fif ');
            if (cmapStream)
                ptr->LoadCharacterMap(id, cmapStream);
            else
                font.Release();
        }
    }
    if (!font)
        MBDebugStr("WARN: font %d failed to load\n", id);
    return font;
}

// 0x10004f80 ?CreateSystemFont@SimpleNode@@QAE?AV?$MBComPtr@VIMBFont@@@@PBDHABVMBColor@@H@Z
MBComPtr<IMBFont> SimpleNode::CreateSystemFont(const char *name, const int pointsize, const MBColor &color, const int flags)
{
    MBComPtr<IMBFont> font;
    font.CreateObject(MBIID_CMBWinSystemFont);
    if (font) {
        IMBScreenManager *screenManager = 0;
        IMBScreen *screen = 0;
        IMBSurface *surface = 0;
        MBGetScreenManager(&screenManager);
        screenManager->GetScreenByIndex(0, &screen);
        screenManager->Release();
        screen->GetBackBuffer(&surface);
        MBComPtr<IMBSystemFont> ptr(font);
        if (ptr)
            ptr->LoadFont(surface, name, pointsize, color, flags);
        else
            font = 0;
        surface->Release();
        screen->Release();
    }
    return font;
}

// 0x100050a0 ?StartSound@SimpleNode@@QAEXPBD@Z
void SimpleNode::StartSound(const char *name)
{
    MBPlaySound(name, 1);
}

// 0x100050c0 ?OnSoundStarted@SimpleNode@@UAEXJK@Z
void SimpleNode::OnSoundStarted(long channel, unsigned long id)
{
}

// 0x100050d0 ?OnSoundEnded@SimpleNode@@UAEXJK@Z
void SimpleNode::OnSoundEnded(long channel, unsigned long id)
{
}

// 0x100050e0 ?GetString@SimpleNode@@QAEXKAAV?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@@Z
void SimpleNode::GetString(unsigned long id, MBString16 &string)
{
    if (sStringTable)
        sStringTable->GetString(id, string);
}

// 0x100051e0 ?GetNamedString@SimpleNode@@QAEXPBDAAV?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@@Z
void SimpleNode::GetNamedString(const char *name, MBString16 &string)
{
    if (sStringTable)
        sStringTable->GetNamedString(name, string);
}

// 0x10005470 ?SetLocale@SimpleNode@@QAEXAAVMBLocale@@@Z
void SimpleNode::SetLocale(MBLocale &locale)
{
    sLocale = locale;
    if (sStringTable)
        sStringTable->SetLocale(locale);
}

// 0x10005860 ?LoadStringTable@SimpleNode@@UAEXPBD@Z
void SimpleNode::LoadStringTable(const char *name)
{
    if (sStringTable)
        sStringTable->Release();
    MBComCreateInstance(0, MBIID_CMBStringTable, (IMBUnknown **)&sStringTable);
    if (sStringTable)
        sStringTable->Load(MBHashString(name), GetStream(MBHashString(name), 'stl '));
}

// 0x100058f0 ??0Database@DatabaseMgr@@QAE@PAVIMBResourceFile@@@Z
DatabaseMgr::Database::Database(IMBResourceFile *resFile) : mResFile(resFile)
{
    mCount = 0;
    MBComPtr<IMBResourceFileList> mgr;
    mgr.CreateObject(MBIID_CMBResourceMgr);
    mgr->AddFile(resFile, kResFileListFront, 0);
}

// 0x100059a0 ?AddDBRef@Database@DatabaseMgr@@QAEKXZ
unsigned long DatabaseMgr::Database::AddDBRef()
{
    return ++mCount;
}

// 0x100059b0 ?ReleaseDBRef@Database@DatabaseMgr@@QAEKXZ
unsigned long DatabaseMgr::Database::ReleaseDBRef()
{
    if (--mCount == 0) {
        MBComPtr<IMBResourceFileList> mgr(MBIID_CMBResourceMgr);
        if (mgr)
            mgr->RemoveFile(mResFile);
        mResFile.Release();
    }
    return mCount;
}

// TODO 0x10005a40 ?Initialize@DatabaseMgr@@QAE_NABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@0@Z: inline budget: the first guid ctor is expanded inline (Release and _QueryInterface calls) in the original, ours calls CreateObject; control flow not verified past that
bool DatabaseMgr::Initialize(const MBString &cdPath, const MBString &hdPath)
{
    bool success = false;
    MBComPtr<IMBFileSpec> fileSpec(MBIID_IMBFileSpec);
    if (fileSpec) {
        fileSpec->SetPath(cdPath.c_str());
        if (MBFileSpecExists(fileSpec) && MBIsFileSpecAFolder(fileSpec))
            mCDPath = fileSpec;
        else if (cdPath.c_str()[0] == '\\') {
            IMBFileSpec *fullPath;
            MBMakeCDPath(cdPath.c_str(), &fullPath);
            if (fullPath) {
                if (MBIsFileSpecAFolder(fullPath))
                    mCDPath = fullPath;
                fullPath->Release();
            }
        }
    }
    fileSpec.CreateObject(MBIID_IMBFileSpec);
    if (fileSpec) {
        fileSpec->SetPath(hdPath.c_str());
        if (MBFileSpecExists(fileSpec) && MBIsFileSpecAFolder(fileSpec))
            mHDPath = fileSpec;
    }
    fileSpec.Release();
    if (mCDPath || mHDPath)
        success = true;
    return success;
}

// 0x10005f50 ?AddZipDatabase@DatabaseMgr@@IAE_NPAVIMBFileSpec@@PBD@Z
bool DatabaseMgr::AddZipDatabase(IMBFileSpec *parentPath, const char *name)
{
    if (!parentPath || !name)
        return false;
    bool success = false;
    MBComPtr<IMBFileSpec> zipSpec(MBIID_IMBFileSpec);
    if (zipSpec) {
        MBString zipName = name;
        zipName += ".mbd";
        zipSpec->SetPath(parentPath, zipName.c_str());
        if (MBFileSpecExists(zipSpec)) {
            MBComPtr<IMBResourceFile> zipRes(MBIID_CMBResourceZipfile);
            MBComPtr<IMBFile> zipFile(zipRes);
            if (zipRes && zipFile && zipFile->Open(zipSpec, 0)) {
                Database *db = new Database(zipRes);
                db->AddDBRef();
                theMap.insert(DatabaseMap::value_type(MBHashString(name), db));
                success = true;
            }
        }
    }
    return success;
}

// 0x10006660 ?AddDirDatabase@DatabaseMgr@@IAE_NPAVIMBFileSpec@@PBD@Z
bool DatabaseMgr::AddDirDatabase(IMBFileSpec *parentPath, const char *name)
{
    if (!parentPath || !name)
        return false;
    bool success = false;
    MBComPtr<IMBFileSpec> dirSpec(MBIID_IMBFileSpec);
    if (dirSpec) {
        dirSpec->SetPath(parentPath, name);
        if (MBFileSpecExists(dirSpec)) {
            MBComPtr<IMBResourceFile> dirRes(MBIID_CMBResourceTree);
            MBComPtr<IMBFile> dirFile(dirRes);
            if (dirRes && dirFile && dirFile->Open(dirSpec, 0)) {
                Database *db = new Database(dirRes);
                db->AddDBRef();
                theMap.insert(DatabaseMap::value_type(MBHashString(name), db));
                success = true;
            }
        }
    }
    return success;
}

// 0x100067a0 ?AddDatabase@DatabaseMgr@@QAE_NPBD@Z
bool DatabaseMgr::AddDatabase(const char *name)
{
    unsigned long hash = MBHashString(name);
    DatabaseMap::iterator it = theMap.find(hash);
    if (it == theMap.end()) {
        if (mCDPath) {
            if (AddZipDatabase(mCDPath, name))
                MBDebugStr("INFO: Added resource Zip       %s%s.mbd.\n", mCDPath->GetPath().c_str(), name);
            if (AddDirDatabase(mCDPath, name))
                MBDebugStr("INFO: Added resource directory %s\\%s.\n", mCDPath->GetPath().c_str(), name);
        }
        if (mHDPath) {
            if (AddZipDatabase(mHDPath, name))
                MBDebugStr("INFO: Added resource Zip       %s\\%s.mbd.\n", mHDPath->GetPath().c_str(), name);
            if (AddDirDatabase(mHDPath, name))
                MBDebugStr("INFO: Added resource directory %s\\%s.\n", mHDPath->GetPath().c_str(), name);
        }
    } else {
        DatabaseMap::iterator end = theMap.upper_bound(hash);
        while (it != end)
            (*it++).second->AddDBRef();
    }
    return true;
}

// 0x100069e0 ?RemoveDatabase@DatabaseMgr@@QAE_NPBD@Z
bool DatabaseMgr::RemoveDatabase(const char *name)
{
    unsigned long hash = MBHashString(name);
    DatabaseMap::iterator it = theMap.find(hash);
    if (it == theMap.end()) {
        MBDebugStr("ERROR: Database %s not found.\n", name);
        return false;
    }
    DatabaseMap::iterator end = theMap.upper_bound(hash);
    while (it != end) {
        DatabaseMap::iterator cur = it;
        it++;
        if ((*cur).second->ReleaseDBRef() == 0) {
            MBDebugStr("INFO:  Removed resource database %s\n", name);
            delete (*cur).second;
            theMap.erase(cur);
        }
    }
    return true;
}

// 0x10006b00 ?Startup@SimpleNode@@SAXPAVIMBGameEngine@@@Z
void __cdecl SimpleNode::Startup(IMBGameEngine *)
{
}

// 0x10006b10 ?Shutdown@SimpleNode@@SAXPAVIMBGameEngine@@@Z
void __cdecl SimpleNode::Shutdown(IMBGameEngine *engine)
{
    if (sStringTable) {
        sStringTable->Release();
        sStringTable = 0;
    }
}


// Matched by this code:
// 0x10003ae0 _$E70
// 0x10003af0 _$E66
// 0x10003b30 _$E69
// 0x10003b40 _$E67
// 0x10003e20 ??_GSimpleNode@@UAEPAXI@Z
// 0x10004700 _$E77
// 0x10004710 _$E74
// 0x100047b0 _$E76
// 0x100047c0 _$E75
// 0x10005100 _$E2
// 0x10005110 _$E1
// 0x100052b0 _$E5
// 0x100052c0 _$E4
// 0x10005390 _$E8
// 0x100053a0 _$E7
// 0x10005690 _wmemcpy
// 0x100056c0 _$E11
// 0x100056d0 _$E10
// 0x10005790 _$E14
// 0x100057a0 _$E13
// 0x10005c40 _$E17
// 0x10005c50 _$E16
// 0x10005d00 _$E20
// 0x10005d10 _$E19
// 0x10005dc0 _$E23
// 0x10005dd0 _$E22
// 0x10005e80 _$E26
// 0x10005e90 _$E25
// 0x10006110 _$E29
// 0x10006120 _$E28
// 0x100061e0 _$E32
// 0x100061f0 _$E31
// 0x100062a0 _$E35
// 0x100062b0 _$E34
// 0x10006360 _$E38
// 0x10006370 _$E37
// 0x10006420 _$E41
// 0x10006430 _$E40
// 0x100064e0 _$E44
// 0x100064f0 _$E43
// 0x100065a0 _$E47
// 0x100065b0 _$E46
// 0x10006b30 ?Release@?$MBComPtr@VIMBResourceFile@@@@QAEXXZ
// 0x10006b50 ??Eiterator@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@QAE?AV012@H@Z
// 0x10006be0 ??4?$MBComPtr@VIMBFileSpec@@@@QAEAAV0@PAVIMBFileSpec@@@Z
// 0x10006c30 ?CreateObject@?$MBComPtr@VIMBFileSpec@@@@QAEJABUMBGUID@@@Z
// 0x10006cd0 ?_Tidy@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEX_N@Z
// 0x10006d20 ?Release@?$MBComPtr@VIMBScreen@@@@QAEXXZ
// 0x10006d60 ?CreateObject@?$MBComPtr@VIMBMouseMonitor@@@@QAEJABUMBGUID@@@Z
// 0x10006e00 ?Release@?$MBComPtr@VIMBViewManager@@@@QAEXXZ
// 0x10006e20 ?CreateObject@?$MBComPtr@VIMBPeriodicalManager@@@@QAEJABUMBGUID@@@Z
// 0x10006ec0 ?CreateObject@?$MBComPtr@VIMBResourceFile@@@@QAEJABUMBGUID@@@Z
// 0x10006f60 ?begin@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@QAE?AViterator@12@XZ
// 0x10006f70 ?insert@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@_N@2@ABU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@2@@Z
// 0x10007210 ?erase@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10007700 ?erase@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x100077f0 ?find@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@QAE?AViterator@12@ABK@Z
// 0x10007850 ?Release@?$MBComPtr@VIMBResourceFileList@@@@QAEXXZ
// 0x10007870 ?append@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAEAAV12@PBDI@Z
// 0x10007930 ?assign@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAEAAV12@PBDI@Z
// 0x100079d0 ?erase@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAEAAV12@II@Z
// 0x10007af0 ?_Eos@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEXI@Z
// 0x10007b10 ?_Grow@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAE_NI_N@Z
// 0x10007cc0 ?erase@?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@QAEAAV12@II@Z
// 0x10007e10 ?_Tidy@?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@AAEX_N@Z
// 0x10007e60 ?_Eos@?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@AAEXI@Z
// 0x10007e80 ?_Grow@?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@AAE_NI_N@Z
// 0x10008030 ?_Erase@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10008090 ?_Insert@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@2@@Z
// 0x10008370 ?_Lrotate@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@IAEXPAU_Node@12@@Z
// 0x100083d0 ?_Min@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@KAPAU_Node@12@PAU312@@Z
// 0x10008410 ?_Rrotate@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10008470 ?_Ubound@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@IBEPAU_Node@12@ABK@Z
// 0x100084c0 ?_Inc@const_iterator@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@QAEXXZ
// 0x10008550 ?_Copy@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEXI@Z
// 0x10008600 ?_Copy@?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@AAEXI@Z
// 0x100086b0 ?_Lbound@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@IBEPAU_Node@12@ABK@Z
// 0x10008700 ?_Buynode@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@IAEPAU_Node@12@PAU312@W4_Redbl@12@@Z
// 0x10008720 ?_Dec@const_iterator@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@QAEXXZ
// 0x100087c0 ?_QueryInterface@?$MBComPtr@VIMBAnim@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10008840 ?_QueryInterface@?$MBComPtr@VIMBWorld@@@@AAEJABQAVIMBUnknown@@@Z
// 0x100088c0 ?_QueryInterface@?$MBComPtr@VIMBView@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10008940 ?_QueryInterface@?$MBComPtr@VIMBFileSpec@@@@AAEJABQAVIMBUnknown@@@Z
// 0x100089c0 ?_QueryInterface@?$MBComPtr@VIMBFont@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10008a40 ?_QueryInterface@?$MBComPtr@VIMBViewManager@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10008b40 ?_QueryInterface@?$MBComPtr@VIMBResourceFileList@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10008bc0 ?_Construct@std@@YAXPAU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@1@ABU21@@Z
// 0x10008be0 ?_QueryInterface@?$MBComPtr@VIMBBitmapFont@@@@AAEJABV?$MBComPtr@VIMBFont@@@@@Z
// 0x10008ce0 ?_QueryInterface@?$MBComPtr@VIMBFile@@@@AAEJABV?$MBComPtr@VIMBResourceFile@@@@@Z
// 0x10008d60 _$E84
// 0x10008d80 _$E83
// 0x10006d40 ?Release@?$MBComPtr@VIMBView@@@@QAEXXZ
// 0x10008c60 ?_QueryInterface@?$MBComPtr@VIMBSystemFont@@@@AAEJABV?$MBComPtr@VIMBFont@@@@@Z
// TODO 0x10007840 ?AddRef@?$MBComPtr@VIMBFileSpec@@@@QAEXXZ: not emitted: our compile inlines MBComPtr<IMBFileSpec>::AddRef everywhere in this unit (inline budget)
