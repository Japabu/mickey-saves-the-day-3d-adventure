#pragma once
// The engine's COM-style interfaces (muffin/include/Com, Runtime, Game), declarations from
// SteamboatApp.pdb; the interface ids are header statics (each object holds the ones it uses).
#include "MBContainers.h"
#include "MBGUID.h"
#include "MBRect.h"
#include "MBComPtr.h"
// The interfaces, their enums and event structs are MBEngine's (IMBUnknown is MBCom's); MBEngine's
// headers expect the includer to declare MBGUID, MBComPtr, the strings and the MBIID_* statics first.

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

#include "../MBEngine/MBAnimationProperty.h"
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
#include "../MBEngine/MBFrameProperty.h"
#include "../MBEngine/MBResFileListAddLoc.h"
#include "../MBEngine/MBScreenState.h"
#include "../MBEngine/MBSequenceProperty.h"
#include "../MBEngine/MBTextControlColorFlag.h"
#include "../MBEngine/MBTextControlJustifyFlag.h"
#include "../MBEngine/MBTextControlTypeFlag.h"
#include "../MBEngine/IMBStreamSeekValues.h"
#include "../MBEngine/IMBStreamError.h"
#include "../MBCom/IMBUnknown.h"
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

#include "../MBEngine/MBKeyboardEvent.h"

// own copy: MBEngine's MBMouseEvent.h includes MBEngine's MBPoint.h, which conflicts with this
// module's MBPoint.h (the game's points have more inline operators).
struct MBMouseEvent {
    MBPoint mMousePos;
    IMBScreen *mScreen;
    short mButton;
};

#include "../MBEngine/IMBObject.h"
#include "../MBEngine/IMBSurface.h"
#include "../MBEngine/IMBObjectGroup.h"
#include "../MBEngine/IMBWorld.h"
#include "../MBEngine/IMBStream.h"
#include "../MBEngine/IMBNode.h"
#include "../MBEngine/IMBPeriodical.h"
#include "../MBEngine/IMBEventListener.h"
#include "../MBEngine/IMBGameEngine.h"
#include "../MBEngine/IMBStringTable.h"
#include "../MBEngine/MBAnimEventStruct.h"
#include "../MBEngine/IMBAnim.h"
#include "../MBEngine/IMBScreen.h"
#include "../MBEngine/IMBWinScreen.h"
typedef int (__stdcall *MBDDEnumCallback)(_GUID *, char *, char *, void *, HMONITOR__ *);
#include "../MBEngine/IMBDirectXScreen.h"
#include "../MBEngine/IMBPref.h"

// own copy: MBEngine's IMBTextControl.h has no GetGUID, which MBComPtr<IMBTextControl>::_QueryInterface
// (SteamboatNode.cpp) calls.
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

#include "../MBEngine/IMBFont.h"
#include "../MBEngine/IMBSoundManager.h"
#include "../MBEngine/IMBSoundDescription.h"
#include "../MBEngine/IMBResourceFile.h"
#include "../MBEngine/IMBResourceFileList.h"
#include "../MBEngine/MBScreenAttributes.h"
#include "../MBEngine/IMBScreenManager.h"
#include "../MBEngine/IMBView.h"
#include "../MBEngine/IMBMovie.h"
#include "../MBEngine/IMBResource.h"
#include "../MBEngine/IMBMouseMonitor.h"
#include "../MBEngine/IMBMouseable.h"
#include "../MBEngine/IMBFileSpec.h"
#include "../MBEngine/IMBPeriodicalManager.h"
#include "../MBEngine/IMBNodeManager.h"
#include "../MBEngine/IMBEventManager.h"
#include "../MBEngine/IMBGameClock.h"
#include "../MBEngine/IMBDirectDrawSurface.h"
#include "../MBEngine/IMBWinFile.h"
#include "../MBEngine/IMBFile.h"
#include "../MBEngine/IMBViewManager.h"

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
