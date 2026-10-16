#pragma once
// The engine's COM-style interfaces (muffin/include/Com, Runtime, Game), declarations from
// SteamboatApp.pdb; the interface ids are header statics (each object holds the ones it uses).
#include "MBContainers.h"
#include "MBGUID.h"
#include "MBRect.h"
#include "MBComPtr.h"
// Names ending in A (DrawTextA, GetObjectA) are the original's DrawText/GetObject, renamed by the
// <windows.h> macros that were in effect where the original declared them.

class IMBAnim;
class IMBDirectDrawSurface;
class IMBDirectXScreen;
class IMBEventListener;
class IMBEventManager;
class IMBFile;
class IMBFileSpec;
class IMBFont;
class IMBGameClock;
class IMBGameEngine;
class IMBMouseMonitor;
class IMBMouseable;
class IMBMovie;
class IMBNode;
class IMBNodeManager;
class IMBObject;
class IMBObjectGroup;
class IMBPeriodical;
class IMBPeriodicalManager;
class IMBPref;
class IMBResource;
class IMBResourceFile;
class IMBResourceFileList;
class IMBScreen;
class IMBScreenManager;
class IMBSoundDescription;
class IMBSoundManager;
class IMBStream;
class IMBStringTable;
class IMBSurface;
class IMBTextControl;
class IMBUnknown;
class IMBView;
class IMBViewManager;
class IMBWinFile;
class IMBWinScreen;
class IMBWorld;
class IMBMemoryBlock;
class IMBStreamLocator;
class IMBTextValidator;
struct tagRECT;

struct HMONITOR__;
struct HWND__;
struct IDirectDraw7;
struct IDirectDrawSurface7;
class MBColor;
class MBLocale;
class MBPalette;
struct MBPixelFormat;
class MBSoundData;
class MfAiActor;
class MfTypedValue;

enum MBAnimationProperty {
    kAnimationDelay = 0,
    kAnimationZPlane = 1,
    kAnimationQuietBackground = 2,
    kAnimationAutoUnload = 3,
    kAnimationDontAdjustZPlane = 4,
    kAnimationExternalCanvas = 5,
    kAnimationExternalSequence = 6
};
enum MBButtonBehavior {
    kPushButtonBehavior = 0,
    kCheckBoxBehavior = 1,
    kRadioButtonBehavior = 2,
    kClickableBehavior = 3,
    kStaticAnim = 4
};
enum MBButtonDrawState {
    kButtonUpDrawState = 0,
    kButtonDownDrawState = 1,
    kButtonRolloverDrawState = 2,
    kButtonDisabledDrawState = 3,
    kButtonDisabledDownDrawState = 4
};
enum MBButtonState {
    kButtonUp = 0,
    kButtonDown = 1
};
enum MBFrameProperty {
    kFrameNotify = 0,
    kFrameDelay = 1,
    kFrameStartSound = 2,
    kFrameStartAnim = 3,
    kFrameStartAnimName = 4,
    kFrameStartAnimSequence = 5,
    kFrameStopAnim = 6,
    kFrameStartScript = 7,
    kFrameStopScript = 8,
    kFrameCastMemberOffset = 9
};
enum MBResFileListAddLoc {
    kResFileListFront = 1,
    kResFileListRear = 2
};
enum MBScreenState {
    kScreenStateActive = 0,
    kScreenStatePaused = 1,
    kScreenStateInactive = 2
};
enum MBSequenceProperty {
    kSequenceComment = 0,
    kSequenceZPlane = 1,
    kSequenceDelay = 2,
    kSequenceInSync = 3,
    kSequenceEndAction = 4,
    kSequenceJumpSequence = 5,
    kSequenceJumpRandomSequenceMin = 6,
    kSequenceJumpRandomSequenceMax = 7,
    kSequenceTileWidth = 8,
    kSequenceTileHeight = 9
};
enum MBTextControlColorFlag {
    kTCFBackgroundColor = 1,
    kTCFCursorColor = 2,
    kTCFBorderColor = 4,
    kTCFActiveBorderColor = 8
};
enum MBTextControlJustifyFlag {
    kTCJLeft = 0,
    kTCJCenter = 1,
    kTCJRight = 2
};
enum MBTextControlTypeFlag {
    kTCTStatic = 1,
    kTCTEditable = 2
};
enum IMBStreamSeekValues {
    kIMBStreamSeekStart = 1,
    kIMBStreamSeekEnd = 2,
    kIMBStreamSeekCurrent = 3
};
enum IMBStreamError {
    kIMBStreamNoError = 0,
    kIMBStreamReadError = 1,
    kIMBStreamWriteError = 2,
    kIMBStreamOpenError = 3,
    kIMBStreamCloseError = 4,
    kIMBStreamSeekError = 5,
    kIMBStreamEOF = 6
};


static const MBGUID MBIID_IMBUnknown = { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory = { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBGameClock = { 0x04e3c980, 0x9ea1, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodical = { 0x79529260, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodicalManager = { 0x7958af20, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBView = { 0x7972a950, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBWorld = { 0x7978c610, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBNode = { 0x795ecbe0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBNodeManager = { 0x7984ff90, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventManager = { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventListener = { 0x799755d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMouseMonitor = { 0x79c52d70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBMouseMonitor = { 0x79cb4a30, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBViewManager = { 0x79a9ac10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBViewManager = { 0x79b15000, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMouseable = { 0x79d166f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreenManager = { 0x79df27a0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSoundManager = { 0x79ee6f80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBPref = { 0x79fc3030, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPref = { 0x7a00c5c0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBGameEngine = { 0x7a1c4720, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreen = { 0x79d783b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObject = { 0x7ed4fc70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObjectGroup = { 0x7ed99200, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBAnim = { 0x7ec2a630, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurface = { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBAnim = { 0x7ecbd150, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBWinFile = { 0x89780490, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFont = { 0x7f4301f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBTextControl = { 0x7f50c2a0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBTextControl = { 0x7f555830, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMovie = { 0x7f5cfc20, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBSmackerMovie = { 0x7f64a010, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBWinScreen = { 0x7f9eb130, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBWinDC = { 0x7fb72430, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBBinkMovie = { 0x7fd5b3f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBDirectXScreen = { 0x7fe4fbd0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSystemFont = { 0x7ff75210, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStream = { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFile = { 0x896bcb10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFileSpec = { 0x897e2150, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource = { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFile = { 0x899e3840, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFileList = { 0x898a5ad0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBResourceMgr = { 0x8991fec0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSoundDescription = { 0x84481ea0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStringTable = { 0x8f1468d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBStringTable = { 0x8f1a8590, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

struct MBKeyboardEvent {
    unsigned short mCharCode;
    unsigned short mSpecialCode;
};

struct MBMouseEvent {
    MBPoint mMousePos;
    IMBScreen *mScreen;
    short mButton;
};

class IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBUnknown; }
    virtual long QueryInterface(const MBGUID &, void **) = 0; // vftable+0x00
    virtual unsigned long AddRef() = 0;                 // vftable+0x04
    virtual unsigned long Release() = 0;                // vftable+0x08
};

class IMBObject : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBObject; }
    virtual MBPoint3 GetOffset() const = 0;             // vftable+0x0c
    virtual MBPoint3 GetPosition() const = 0;           // vftable+0x10
    virtual void Place(const MBPoint3 &) = 0;           // vftable+0x14
    virtual void Move(const MBPoint3 &) = 0;            // vftable+0x18
    virtual MBRect GetBounds() const = 0;               // vftable+0x1c
    virtual bool IsVisible() const = 0;                 // vftable+0x20
    virtual long Show() = 0;                            // vftable+0x24
    virtual long Hide() = 0;                            // vftable+0x28
    virtual bool IsDirty() const = 0;                   // vftable+0x2c
    virtual void SetDirty() = 0;                        // vftable+0x30
    virtual void ClearDirty() = 0;                      // vftable+0x34
    virtual long Draw(IMBSurface *, MBRect &, MBRect &) const = 0; // vftable+0x38
    virtual void GetParent(IMBObjectGroup **) const = 0; // vftable+0x3c
    virtual IMBObjectGroup *SetParent(IMBObjectGroup *) = 0; // vftable+0x40
};

class IMBSurface : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBSurface; }
    virtual unsigned long GetWidth() = 0;               // vftable+0x0c
    virtual unsigned long GetHeight() = 0;              // vftable+0x10
    virtual unsigned long GetPitch() = 0;               // vftable+0x14
    virtual unsigned long GetSize() = 0;                // vftable+0x18
    virtual unsigned long GetDepth() = 0;               // vftable+0x1c
    virtual MBPixelFormat GetFormat() = 0;              // vftable+0x20
    virtual long Lock(void **, unsigned long) = 0;      // vftable+0x24
    virtual long Unlock(void **) = 0;                   // vftable+0x28
    virtual MBColor GetPixelColor(unsigned long, unsigned long) = 0; // vftable+0x2c
    virtual long SetTransparentColor(MBColor) = 0;      // vftable+0x30
    virtual MBColor GetTransparentColor() = 0;          // vftable+0x34
    virtual long SetPalette(MBPalette *) = 0;           // vftable+0x38
    virtual MBPalette *GetPalette() = 0;                // vftable+0x3c
    virtual long ColorFill(MBColor, MBRect *) = 0;      // vftable+0x40
    virtual long Blit(IMBSurface *, MBRect *, MBRect *, unsigned long) = 0; // vftable+0x44
};

class IMBObjectGroup : public IMBObject {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBObjectGroup; }
    virtual void GetBackground(IMBObject **) const = 0; // vftable+0x44
    virtual void SetBackground(IMBObject *) = 0;        // vftable+0x48
    virtual void AddObject(IMBObject *) = 0;            // vftable+0x4c
    virtual void RemoveObject(IMBObject *) = 0;         // vftable+0x50
    virtual void ObjectMoved(IMBObject *) = 0;          // vftable+0x54
};

class IMBWorld : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBWorld; }
    virtual MBRect GetBounds() const = 0;               // vftable+0x0c
    virtual void SetBounds(const MBRect &) = 0;         // vftable+0x10
    virtual long Draw(IMBSurface *, MBRect &, MBRect &) = 0; // vftable+0x14
    virtual IMBObjectGroup &GetRoot() = 0;              // vftable+0x18
    virtual const IMBObjectGroup &GetConstRoot() const = 0; // vftable+0x1c
    void AddObject(IMBObject *);
    void RemoveObject(IMBObject *);
    void GetBackground(IMBObject **) const;
    void SetBackground(IMBObject *);
};

// The world's helpers forward to its root group (no out-of-line copies in the binary: inline). The
// argument is loaded before GetRoot is called (SBSongbirdNode::OnStart).
inline void IMBWorld::AddObject(IMBObject *object) { GetRoot().AddObject(object); }
inline void IMBWorld::RemoveObject(IMBObject *object) { GetRoot().RemoveObject(object); }

class IMBStream : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBStream; }
    virtual bool Read(void *, unsigned long &) = 0;     // vftable+0x0c
    virtual bool Write(const void *, unsigned long) = 0; // vftable+0x10
    virtual bool Seek(long, IMBStreamSeekValues) = 0;   // vftable+0x14
    virtual unsigned long GetPosition() = 0;            // vftable+0x18
    virtual bool Flush() = 0;                           // vftable+0x1c
    virtual bool SetLastError(IMBStreamError) = 0;      // vftable+0x20
    virtual IMBStreamError GetLastError() = 0;          // vftable+0x24
    virtual unsigned long GetStreamSize() = 0;          // vftable+0x28
    virtual void Clone(IMBStream **) = 0;               // vftable+0x2c
};

class IMBNode : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBNode; }
    virtual void OnStart() = 0;                         // vftable+0x0c
    virtual void OnPause() = 0;                         // vftable+0x10
    virtual void OnResume() = 0;                        // vftable+0x14
    virtual void OnEnd() = 0;                           // vftable+0x18
    virtual void OnQuit() = 0;                          // vftable+0x1c
};

class IMBPeriodical : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBPeriodical; }
    virtual void Idle() = 0;                            // vftable+0x0c
};

class IMBEventListener : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBEventListener; }
    virtual void ListenToEvent(long, long, void *) = 0; // vftable+0x0c
};

class IMBGameEngine : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBGameEngine; }
    virtual void Init() = 0;                            // vftable+0x0c
    virtual void ShutDown() = 0;                        // vftable+0x10
    virtual MBComPtr<IMBNodeManager> GetNodeMgr() const = 0; // vftable+0x14
    virtual long CreateScreen(IMBScreenManager *) = 0;  // vftable+0x18
    virtual void Start() = 0;                           // vftable+0x1c
    virtual void Idle() = 0;                            // vftable+0x20
    virtual void UpdateScreen(const MBRect &) = 0;      // vftable+0x24
    virtual bool IsQuitting() const = 0;                // vftable+0x28
    virtual void Quit() = 0;                            // vftable+0x2c
    virtual void SetOnEngineStartupFunction(void (__cdecl *)(IMBGameEngine *)) = 0; // vftable+0x30
    virtual void SetOnEngineShutdownFunction(void (__cdecl *)(IMBGameEngine *)) = 0; // vftable+0x34
    virtual void SetOnEngineCreateScreenFunction(long (__cdecl *)(IMBGameEngine *, IMBScreenManager *)) = 0; // vftable+0x38
    virtual void GetStringTable(IMBStringTable **) = 0; // vftable+0x3c
};

class IMBStringTable : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBStringTable; }
    virtual bool Load(const char *) = 0;                // vftable+0x10
    virtual bool Load(unsigned long, IMBStream *) = 0;  // vftable+0x0c
    virtual bool SetLocale(const MBLocale &) = 0;       // vftable+0x14
    virtual bool GetString(unsigned long, MBString16 &) = 0; // vftable+0x18
    virtual bool GetNamedString(const MBString &, MBString16 &) = 0; // vftable+0x1c
};

// The data of the animation events an IMBAnim broadcasts.
struct MBAnimEventStruct {
    IMBAnim *anim;                                      // +0x0000 size 0x4
    unsigned long id;                                   // +0x0004 size 0x4
    long sequence;                                      // +0x0008 size 0x4
    long frame;                                         // +0x000c size 0x4
};

class IMBAnim : public IMBObject {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBAnim; }
    virtual long Load(unsigned long, IMBStream *, bool) = 0; // vftable+0x44
    virtual long Save(IMBStream *) = 0;                 // vftable+0x48
    virtual void Clone(IMBAnim **) = 0;                 // vftable+0x4c
    virtual long SetAnimationProperty(MBAnimationProperty, const MBString &) = 0; // vftable+0x5c
    virtual long SetAnimationProperty(MBAnimationProperty, unsigned long &) = 0; // vftable+0x58
    virtual long SetAnimationProperty(MBAnimationProperty, unsigned short) = 0; // vftable+0x54
    virtual long SetAnimationProperty(MBAnimationProperty, bool) = 0; // vftable+0x50
    virtual long SetSequenceProperty(long, MBSequenceProperty, unsigned short) = 0; // vftable+0x64
    virtual long SetSequenceProperty(long, MBSequenceProperty, bool) = 0; // vftable+0x60
    virtual long SetFrameProperty(long, long, MBFrameProperty, const MBPoint &) = 0; // vftable+0x78
    virtual long SetFrameProperty(long, long, MBFrameProperty, const MBString &) = 0; // vftable+0x74
    virtual long SetFrameProperty(long, long, MBFrameProperty, unsigned long) = 0; // vftable+0x70
    virtual long SetFrameProperty(long, long, MBFrameProperty, unsigned short) = 0; // vftable+0x6c
    virtual long SetFrameProperty(long, long, MBFrameProperty, bool) = 0; // vftable+0x68
    virtual long GetAnimationProperty(MBAnimationProperty, unsigned long &) const = 0; // vftable+0x84
    virtual long GetAnimationProperty(MBAnimationProperty, unsigned short &) const = 0; // vftable+0x80
    virtual long GetAnimationProperty(MBAnimationProperty, bool &) const = 0; // vftable+0x7c
    virtual long GetSequenceProperty(long, MBSequenceProperty, unsigned short &) const = 0; // vftable+0x8c
    virtual long GetSequenceProperty(long, MBSequenceProperty, bool &) const = 0; // vftable+0x88
    virtual long GetFrameProperty(long, long, MBFrameProperty, MBPoint &) const = 0; // vftable+0x9c
    virtual long GetFrameProperty(long, long, MBFrameProperty, unsigned long &) const = 0; // vftable+0x98
    virtual long GetFrameProperty(long, long, MBFrameProperty, unsigned short &) const = 0; // vftable+0x94
    virtual long GetFrameProperty(long, long, MBFrameProperty, bool &) const = 0; // vftable+0x90
    virtual long UseAnimationProperty(MBAnimationProperty, bool) = 0; // vftable+0xa0
    virtual bool HasAnimationProperty(MBAnimationProperty) const = 0; // vftable+0xa4
    virtual long UseSequenceProperty(long, MBSequenceProperty, bool) = 0; // vftable+0xa8
    virtual bool HasSequenceProperty(long, MBSequenceProperty) const = 0; // vftable+0xac
    virtual long UseFrameProperty(long, long, MBFrameProperty, bool) = 0; // vftable+0xb0
    virtual bool HasFrameProperty(long, long, MBFrameProperty) const = 0; // vftable+0xb4
    virtual void SetCastID(long, long, long) = 0;       // vftable+0xb8
    virtual long GetCastID(long, long) const = 0;       // vftable+0xbc
    virtual void Play() = 0;                            // vftable+0xc0
    virtual void Stop() = 0;                            // vftable+0xc4
    virtual long Pause() = 0;                           // vftable+0xc8
    virtual long Resume() = 0;                          // vftable+0xcc
    virtual bool IsPaused() const = 0;                  // vftable+0xd0
    virtual bool IsStalled() const = 0;                 // vftable+0xd4
    virtual bool IsRendering() const = 0;               // vftable+0xd8
    virtual bool IsPlaying() const = 0;                 // vftable+0xdc
    virtual void Scale(float, float, float) = 0;        // vftable+0xe0
    virtual unsigned long GetNumSequences() const = 0;  // vftable+0xe4
    virtual long AddSequence() = 0;                     // vftable+0xe8
    virtual bool SetCurrentSequence(long) = 0;          // vftable+0xec
    virtual long GetCurrentSequenceIndex() const = 0;   // vftable+0xf0
    virtual unsigned long GetNumFrames(long) const = 0; // vftable+0xf4
    virtual long AddFrame(long) = 0;                    // vftable+0xf8
    virtual bool SetCurrentFrame(long) = 0;             // vftable+0xfc
    virtual long GetCurrentFrameIndex() const = 0;      // vftable+0x100
    virtual unsigned long GetNumCasts() const = 0;      // vftable+0x104
    virtual long AddCastMember(IMBSurface *, const MBPoint &) = 0; // vftable+0x108
    virtual void GetCastMemberSurface(long, IMBSurface **) = 0; // vftable+0x10c
    virtual void SetCastMemberSurface(long, IMBSurface *) = 0; // vftable+0x110
    virtual MBPoint GetCastMemberRegistrationPoint(long) const = 0; // vftable+0x114
    virtual void SetCastMemberRegistrationPoint(long, const MBPoint &) = 0; // vftable+0x118
    virtual MBPoint GetCastMemberAttachmentPoint(long) const = 0; // vftable+0x11c
    virtual void SetCastMemberAttachmentPoint(long, const MBPoint &) = 0; // vftable+0x120
};

class IMBScreen : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBScreen; }
    virtual void SetScreenState(MBScreenState) = 0;     // vftable+0x0c
    virtual MBScreenState GetScreenState() = 0;         // vftable+0x10
    virtual bool CreateSurface(unsigned short, unsigned short, unsigned long, IMBSurface **) = 0; // vftable+0x14
    virtual void GetBackBuffer(IMBSurface **) = 0;      // vftable+0x18
    virtual void Flip() = 0;                            // vftable+0x1c
    virtual unsigned short GetWidth() = 0;              // vftable+0x20
    virtual unsigned short GetHeight() = 0;             // vftable+0x24
    virtual MBRect GetGlobalBounds() = 0;               // vftable+0x28
    virtual MBPoint GlobalToScreenPoint(const MBPoint &) = 0; // vftable+0x2c
};

class IMBWinScreen : public IMBScreen {
public:
    virtual void SetHWND(HWND__ *) = 0;                 // vftable+0x30
    virtual HWND__ *GetHWND() = 0;                      // vftable+0x34
    virtual bool EnterWindowedMode(unsigned short, unsigned short) = 0; // vftable+0x38
    virtual bool EnterFullscreenMode(unsigned short, unsigned short, unsigned short) = 0; // vftable+0x3c
};

class IMBDirectXScreen : public IMBWinScreen {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBDirectXScreen; }
    virtual bool IsFullscreen() = 0;                    // vftable+0x40
    virtual void SetDirectDrawEnumerator(int (__stdcall *)(_GUID *, char *, char *, void *, HMONITOR__ *)) = 0; // vftable+0x44
    virtual IDirectDraw7 *GetDirectDraw7() = 0;         // vftable+0x48
    virtual _GUID *GetDeviceGUID() = 0;                 // vftable+0x4c
    virtual IDirectDrawSurface7 *GetBackBuffer() = 0;   // vftable+0x50
    virtual IDirectDrawSurface7 *GetFrontBuffer() = 0;  // vftable+0x54
    virtual bool IsInGDIMode() = 0;                     // vftable+0x58
    virtual void GDIModeUpdateRect(tagRECT &) = 0;      // vftable+0x5c
};

class IMBPref : public IMBUnknown {
    enum IMBPrefHints {
        kCacheAll = 1,
        kCacheSection = 2,
        kCacheNothing = 4,
        kSaveOneSet = 8,
        kSaveOnExit = 10
    };
public:
    static const MBGUID &GetGUID() { return MBIID_IMBPref; }
    virtual long GetString(IMBStream *, const MBString &, const MBString &, MBString16 &, const MBString16 &) = 0; // vftable+0x10
    virtual long GetString(IMBStream *, const MBString &, const MBString &, MBString &, const MBString &) = 0; // vftable+0x0c
    virtual long GetLong(IMBStream *, const MBString &, const MBString &, long) = 0; // vftable+0x14
    virtual long SetString(IMBStream *, const MBString &, const MBString &, MBString16 &) = 0; // vftable+0x1c
    virtual long SetString(IMBStream *, const MBString &, const MBString &, MBString &) = 0; // vftable+0x18
    virtual long SetLong(IMBStream *, const MBString &, const MBString &, long) = 0; // vftable+0x20
};

class IMBTextControl : public IMBObject {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBTextControl; }
    virtual void SetFont(IMBFont *) = 0;                // vftable+0x44
    virtual void GetFont(IMBFont **) = 0;               // vftable+0x48
    virtual void GetText(MBString &) = 0;               // vftable+0x50
    virtual void GetText(MBString16 &) = 0;             // vftable+0x4c
    virtual void SetText(const MBString &) = 0;         // vftable+0x58
    virtual void SetText(const MBString16 &) = 0;       // vftable+0x54
    virtual void SetSize(long, long) = 0;               // vftable+0x5c
    virtual MBSize GetSize() const = 0;                 // vftable+0x60
    virtual void SetType(MBTextControlTypeFlag) = 0;    // vftable+0x64
    virtual void SetJustification(MBTextControlJustifyFlag) = 0; // vftable+0x68
    virtual void SetCharacterLimit(unsigned long) = 0;  // vftable+0x6c
    virtual void SetColor(MBTextControlColorFlag, const MBColor &) = 0; // vftable+0x70
    virtual void SetColorTransparent(MBTextControlColorFlag) = 0; // vftable+0x74
    virtual void SetValidator(IMBTextValidator *) = 0;  // vftable+0x78
};

class IMBFont : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBFont; }
    virtual long DrawTextA(IMBSurface *, long, long, const unsigned short *const, unsigned long) = 0; // vftable+0x18
    virtual long DrawTextA(IMBSurface *, MBRect &, const unsigned short *const, unsigned long) = 0; // vftable+0x14
    virtual long DrawTextA(IMBSurface *, long, long, const char *const, unsigned long) = 0; // vftable+0x10
    virtual long DrawTextA(IMBSurface *, MBRect &, const char *const, unsigned long) = 0; // vftable+0x0c
    virtual long GetTextSize(IMBSurface *, const unsigned short *, unsigned long, MBSize &) = 0; // vftable+0x20
    virtual long GetTextSize(IMBSurface *, const char *, unsigned long, MBSize &) = 0; // vftable+0x1c
};

class IMBSoundManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBSoundManager; }
    virtual void StartSound(IMBSoundDescription *) = 0; // vftable+0x0c
    virtual void QueueSound(IMBSoundDescription *) = 0; // vftable+0x10
    virtual void StopSound(long) = 0;                   // vftable+0x14
    virtual void StopChannel(long) = 0;                 // vftable+0x18
    virtual void StopAllSound() = 0;                    // vftable+0x1c
    virtual bool IsPaused() = 0;                        // vftable+0x20
    virtual bool IsChannelPaused(long) = 0;             // vftable+0x24
    virtual bool IsChannelPlaying(long) = 0;            // vftable+0x28
    virtual bool IsSoundPlaying(long) = 0;              // vftable+0x30
    virtual bool IsSoundPlaying(long, long) = 0;        // vftable+0x2c
    virtual bool IsSoundPlayingOrQueued(long) = 0;      // vftable+0x34
    virtual bool IsAnySoundPlaying() = 0;               // vftable+0x38
    virtual void Pause() = 0;                           // vftable+0x3c
    virtual void PauseChannel(long) = 0;                // vftable+0x40
    virtual void PauseSound(long) = 0;                  // vftable+0x44
    virtual void Resume() = 0;                          // vftable+0x48
    virtual void ResumeChannel(long) = 0;               // vftable+0x4c
    virtual void ResumeSound(long) = 0;                 // vftable+0x50
    virtual short GetMasterVolume() = 0;                // vftable+0x54
    virtual void SetMasterVolume(short) = 0;            // vftable+0x58
    virtual short GetChannelVolume(long) = 0;           // vftable+0x5c
    virtual void SetChannelVolume(long, short) = 0;     // vftable+0x60
    virtual float GetChannelRateMultiplier(long) = 0;   // vftable+0x64
    virtual void SetChannelRateMultiplier(long, float) = 0; // vftable+0x68
    virtual MBSoundData *GetLockedSound(long) = 0;      // vftable+0x6c
    virtual void LockSound(long) = 0;                   // vftable+0x70
    virtual void UnlockSound(long) = 0;                 // vftable+0x74
};

class IMBSoundDescription : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBSoundDescription; }
    virtual void SetStreamLocator(IMBStreamLocator *) = 0; // vftable+0x0c
    virtual void GetStreamLocator(IMBStreamLocator **) = 0; // vftable+0x10
    virtual unsigned long GetSoundID() const = 0;       // vftable+0x14
    virtual void SetChannel(long) = 0;                  // vftable+0x18
    virtual long GetChannel() const = 0;                // vftable+0x1c
    virtual void SetLoopCount(long) = 0;                // vftable+0x20
    virtual long GetLoopCount() const = 0;              // vftable+0x24
    virtual void SetStreaming(bool) = 0;                // vftable+0x28
    virtual bool IsStreaming() const = 0;               // vftable+0x2c
    virtual void SetRateMultiplier(float) = 0;          // vftable+0x30
    virtual float GetRateMultiplier() const = 0;        // vftable+0x34
};

class IMBResourceFile : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBResourceFile; }
    virtual void LoadResource(unsigned long, unsigned long, IMBMemoryBlock **) = 0; // vftable+0x0c
    virtual void GetStream(unsigned long, unsigned long, IMBStream **) = 0; // vftable+0x10
};

class IMBResourceFileList : public IMBResourceFile {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBResourceFileList; }
    virtual void AddFile(IMBResourceFile *, MBResFileListAddLoc, unsigned long) = 0; // vftable+0x14
    virtual void RemoveFile(IMBResourceFile *) = 0;     // vftable+0x18
};

struct MBScreenAttributes {
    unsigned long width;
    unsigned long height;
    unsigned long depth;
};

class IMBScreenManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBScreenManager; }
    virtual bool RequestScreen(long, MBScreenAttributes &) = 0; // vftable+0x0c
    virtual void Activate() = 0;                        // vftable+0x10
    virtual void Deactivate() = 0;                      // vftable+0x14
    virtual MBPoint GetGlobalMouseLocation() = 0;       // vftable+0x18
    virtual bool ProcessSystemEvent(void *) = 0;        // vftable+0x1c
    virtual void RemoveScreen(long) = 0;                // vftable+0x20
    virtual void RemoveAllScreens() = 0;                // vftable+0x24
    virtual unsigned short GetNumScreens() = 0;         // vftable+0x28
    virtual void GetScreenByIndex(unsigned short, IMBScreen **) = 0; // vftable+0x2c
    virtual void GetScreenByID(long, IMBScreen **) = 0; // vftable+0x30
    virtual void FindScreenContainingPoint(const MBPoint &, IMBScreen **) = 0; // vftable+0x34
};

class IMBView : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBView; }
    virtual bool IsDirty() = 0;                         // vftable+0x0c
    virtual void Draw(IMBSurface *) = 0;                // vftable+0x10
    virtual void SetWorldPosition(MBPoint) = 0;         // vftable+0x14
    virtual MBPoint GetWorldPosition() = 0;             // vftable+0x18
    virtual MBRect GetWorldBounds() = 0;                // vftable+0x1c
    virtual void SetScreenPosition(MBPoint) = 0;        // vftable+0x20
    virtual MBPoint GetScreenPosition() = 0;            // vftable+0x24
    virtual MBRect GetScreenBounds() = 0;               // vftable+0x28
    virtual void SetSize(unsigned short, unsigned short) = 0; // vftable+0x2c
    virtual unsigned short GetWidth() = 0;              // vftable+0x30
    virtual unsigned short GetHeight() = 0;             // vftable+0x34
    virtual void AttachToWorld(IMBWorld *) = 0;         // vftable+0x38
    virtual void AttachToScreen(IMBScreen *) = 0;       // vftable+0x3c
    virtual bool IsAttachedToScreen(IMBScreen *) = 0;   // vftable+0x40
    virtual void GetWorld(IMBWorld **) = 0;             // vftable+0x44
    virtual MBPoint ScreenToWorldPoint(const MBPoint &) = 0; // vftable+0x48
};

class IMBMovie : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBMovie; }
    virtual bool Load(unsigned long, IMBStream *, unsigned long) = 0; // vftable+0x0c
    virtual void Play() = 0;                            // vftable+0x10
    virtual void Stop() = 0;                            // vftable+0x14
    virtual bool IsPlaying() = 0;                       // vftable+0x18
    virtual void Rewind() = 0;                          // vftable+0x1c
    virtual long Pause() = 0;                           // vftable+0x20
    virtual long Unpause() = 0;                         // vftable+0x24
    virtual bool IsPaused() = 0;                        // vftable+0x28
    virtual void SetLoop(bool) = 0;                     // vftable+0x2c
    virtual bool IsLooping() = 0;                       // vftable+0x30
    virtual long CurrentFrameNum() const = 0;           // vftable+0x34
    virtual long NumTotalFrames() const = 0;            // vftable+0x38
    virtual long FrameDelay() const = 0;                // vftable+0x3c
    virtual long Width() const = 0;                     // vftable+0x40
    virtual long Height() const = 0;                    // vftable+0x44
    virtual long Depth() const = 0;                     // vftable+0x48
    virtual void SetTransparentColor(MBColor) = 0;      // vftable+0x4c
    virtual MBColor GetTransparentColor() = 0;          // vftable+0x50
    virtual void SetPalette(MBPalette *) = 0;           // vftable+0x54
    virtual MBPalette *GetPalette() = 0;                // vftable+0x58
};

class IMBResource : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBResource; }
    virtual unsigned long GetID() = 0;                  // vftable+0x0c
    virtual unsigned long GetType() = 0;                // vftable+0x10
};

class IMBMouseMonitor : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBMouseMonitor; }
    virtual void AddMouseable(IMBMouseable *) = 0;      // vftable+0x0c
    virtual void RemoveMouseable(IMBMouseable *) = 0;   // vftable+0x10
    virtual void Activate() = 0;                        // vftable+0x14
    virtual void Deactivate() = 0;                      // vftable+0x18
    virtual bool IsActive() = 0;                        // vftable+0x1c
};

class IMBMouseable : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBMouseable; }
    virtual bool IsRolledOver(const MBPoint &) const = 0; // vftable+0x0c
    virtual void EnterRollover(const MBPoint &) = 0;    // vftable+0x10
    virtual void WithinRollover(const MBPoint &) = 0;   // vftable+0x14
    virtual void LeaveRollover() = 0;                   // vftable+0x18
    virtual bool IsClicked(const MBPoint &, short) const = 0; // vftable+0x1c
    virtual bool IsTrackingClick() const = 0;           // vftable+0x20
    virtual void StartTrackingClick(const MBPoint &, short) = 0; // vftable+0x24
    virtual void TrackClick(const MBPoint &) = 0;       // vftable+0x28
    virtual void StopTrackingClick(const MBPoint &, short) = 0; // vftable+0x2c
    virtual bool IsActive() const = 0;                  // vftable+0x30
    virtual void Activate() = 0;                        // vftable+0x34
    virtual void Deactivate() = 0;                      // vftable+0x38
    virtual void SetPixelTest(bool) = 0;                // vftable+0x3c
    virtual bool GetPixelTest() = 0;                    // vftable+0x40
};

class IMBFileSpec : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBFileSpec; }
    virtual void SetPath(const char *) = 0;             // vftable+0x10
    virtual void SetPath(const IMBFileSpec *, const char *) = 0; // vftable+0x0c
    virtual MBString GetName() const = 0;               // vftable+0x14
    virtual MBString GetExtension() const = 0;          // vftable+0x18
    virtual MBString GetPath() const = 0;               // vftable+0x1c
    virtual MBString GetTitle() const = 0;              // vftable+0x20
    virtual unsigned long GetType() const = 0;          // vftable+0x24
    virtual void GetParent(IMBFileSpec **) const = 0;   // vftable+0x28
    virtual unsigned long Hash() const = 0;             // vftable+0x2c
};

class IMBPeriodicalManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBPeriodicalManager; }
    virtual void AddPeriodical(IMBPeriodical *) = 0;    // vftable+0x0c
    virtual void RemovePeriodical(IMBPeriodical *) = 0; // vftable+0x10
    virtual void DevoteTimeToPeriodicals() = 0;         // vftable+0x14
};

class IMBNodeManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBNodeManager; }
    virtual IMBNode *GetActiveNode() const = 0;         // vftable+0x0c
    virtual void QueuePush(MBComPtr<IMBNode>) = 0;      // vftable+0x10
    virtual void QueuePop() = 0;                        // vftable+0x14
    virtual void QueueSwap(MBComPtr<IMBNode>) = 0;      // vftable+0x18
    virtual void QueueClear() = 0;                      // vftable+0x1c
};

class IMBEventManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBEventManager; }
    virtual void BroadcastEvent(long, long, void *) = 0; // vftable+0x0c
    virtual void AddListener(long, IMBEventListener *) = 0; // vftable+0x10
    virtual void RemoveListener(long, IMBEventListener *) = 0; // vftable+0x18
    virtual void RemoveListener(IMBEventListener *) = 0; // vftable+0x14
};

class IMBGameClock : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBGameClock; }
    virtual unsigned long GetRealTime() = 0;            // vftable+0x0c
    virtual unsigned long GetGameTime() = 0;            // vftable+0x10
    virtual long Pause() = 0;                           // vftable+0x14
    virtual long Unpause() = 0;                         // vftable+0x18
    virtual void SetTime(unsigned long) = 0;            // vftable+0x1c
    virtual void AddTime(unsigned long) = 0;            // vftable+0x20
    virtual void SetMSPerSecond(long) = 0;              // vftable+0x24
};

class IMBDirectDrawSurface : public IMBSurface {
public:
    virtual void SetSurface(IDirectDrawSurface7 *) = 0; // vftable+0x48
    virtual void GetSurface(IDirectDrawSurface7 **) = 0; // vftable+0x4c
    virtual void SetSurfaceCopy(IDirectDrawSurface7 *) = 0; // vftable+0x50
    virtual void GetSurfaceCopy(IDirectDrawSurface7 **) = 0; // vftable+0x54
    virtual void QuerySurfaceInfo() = 0;                // vftable+0x58
};

class IMBWinFile : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBWinFile; }
    virtual void *GetFileHandle() = 0;                  // vftable+0x0c
};

class IMBFile : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBFile; }
    virtual bool Open(const IMBFileSpec *, unsigned long) = 0; // vftable+0x0c
    virtual void GetFileSpec(IMBFileSpec **) = 0;       // vftable+0x10
    virtual bool Close() = 0;                           // vftable+0x14
    virtual bool SetSize(unsigned long) = 0;            // vftable+0x18
};

class IMBViewManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBViewManager; }
    virtual void AddView(IMBView *) = 0;                // vftable+0x0c
    virtual void RemoveView(IMBView *) = 0;             // vftable+0x10
    virtual void FindViewContaingPoint(IMBScreen *, const MBPoint &, IMBView **) = 0; // vftable+0x14
};


// The engine's cursor switch (MBEngine.dll).
extern "C" bool MBShowSystemCursor(bool show);
extern "C" long __cdecl MBOpenFile(IMBFileSpec *spec, unsigned long mode, IMBStream **stream);
extern "C" IMBUnknown *__cdecl MBComUtilAddSingleton(IMBUnknown *unk);

// The engine's message box (MBEngine.dll, MBMsgBoxWin.obj).
enum MBMsgBoxButtonID {
    kMBMsgBoxIDOK = 0,
    kMBMsgBoxIDCancel = -1,
    kMBMsgBoxIDRetry = 1,
    kMBMsgBoxIDYes = 0,
    kMBMsgBoxIDNo = -1
};
extern "C" MBMsgBoxButtonID MBMsgBox(const MBString16 &text, unsigned long flags, const MBString16 &title);
