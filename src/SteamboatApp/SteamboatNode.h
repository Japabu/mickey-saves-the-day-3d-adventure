#pragma once
// SteamboatNode.h: the game's base scene node (declaration from SteamboatApp.pdb).
#include "ScriptHelpNode.h"
#include "SBInterfaces.h"
#include "SBDialogue.h"
#include "Vec3.h"

struct HDC__;
struct _STREAM;
struct tagRECT;
struct MfMouseHookData;
struct MfUserHookData;
class MfAiActor;
class MfAiActorNode;
class MfAiSpace;
class MfPoEvent;
class MfPoVector3;
class MfScDataHandle;
class MfString;
class SBItem;
class SBRenderObject;
class CSBRenderManager;
class ViewEventHandler;
namespace Gfx {
class Node;
}

class SteamboatNode : public ScriptHelpNode {
    friend class SBRenderObject;                        // its Draw calls RenderCharacters
public:
    SteamboatNode(const SteamboatNode &);
    SteamboatNode(IMBGameEngine *engine, MBString space);
    SteamboatNode(IMBGameEngine *engine, MfAiSpace *mofaSpace);
    virtual ~SteamboatNode();                           // deleting destructor at vftable+0x20
    virtual void Init();                                // vftable+0xa0
    virtual void SetMfPrimaryActor(const MfString mfActorName); // vftable+0xa4
    virtual MfAiActor *GetMfPrimaryActor();             // vftable+0xa8
    virtual MfAiActorNode *GetMfCameraNode() { return mMfCameraNode; } // vftable+0xac
    virtual void OnStart();                             // vftable+0x0c
    virtual void OnEnd();                               // vftable+0x18
    virtual void OnQuit();                              // vftable+0x1c
    virtual void OnPause();                             // vftable+0x10
    virtual void OnResume();                            // vftable+0x14
    virtual void OnPreRender(MBComPtr<IMBScreen> screen); // vftable+0x24
    virtual void OnPostRender(MBComPtr<IMBScreen> screen); // vftable+0x28
    virtual void OnKeyDown(MBKeyboardEvent &key);       // vftable+0x30
    virtual void OnKeyUp(MBKeyboardEvent &keyEvent);    // vftable+0x34
    virtual void OnLMouseDown(MBMouseEvent &evt);       // vftable+0x38
    virtual void OnLMouseUp(MBMouseEvent &evt);         // vftable+0x3c
    virtual void OnLMouseDblClk(MBMouseEvent &evt);     // vftable+0x40
    virtual void OnMMouseDown(MBMouseEvent &evt);       // vftable+0x44
    virtual void OnMMouseUp(MBMouseEvent &evt);         // vftable+0x48
    virtual void OnMMouseDblClk(MBMouseEvent &evt);     // vftable+0x4c
    virtual void OnRMouseDown(MBMouseEvent &evt);       // vftable+0x50
    virtual void OnRMouseUp(MBMouseEvent &evt);         // vftable+0x54
    virtual void OnRMouseDblClk(MBMouseEvent &evt);     // vftable+0x58
    virtual void OnMouseMove(MBMouseEvent &evt);        // vftable+0x5c
    virtual void ListenToEvent(long channel, long event, void *data); // vftable+0x0c (subobject +0x8), this adjust +0x8
    virtual void Idle();                                // vftable+0x0c (subobject +0x4), this adjust +0x4
    virtual void SetCursor(MBComPtr<IMBAnim> anim, short xOffset, short yOffset); // vftable+0x7c
    virtual void HideCursor();                          // vftable+0x74
    virtual void ShowCursor();                          // vftable+0x78
    virtual void SetCursorSequence(unsigned long seq);  // vftable+0xb0
    void AddDatabase(const char *name);
    void RemoveDatabase(const char *name);
    void VerifyCD();
    virtual void SetView(const MfString &actorName);    // vftable+0xb4
    virtual Vec3<float> &GetCameraDir() { return mCameraDir; } // vftable+0xb8
    virtual float GetCameraAngle() { return mCameraAngle; } // vftable+0xbc
    virtual IMBAnim *GetCursor() { return mCursor; }    // vftable+0xc0
    virtual MBComPtr<IMBMovie> PlayMovie(const char *name, unsigned long flags); // vftable+0xc4
    virtual void KillSound(const long channelID);       // vftable+0xc8
    virtual void PlaySfx(const char *name);             // vftable+0xcc
    virtual void StartDialogue(const char *line, bool force); // vftable+0xd0
    virtual void UserEventHook(MfUserHookData *hook);   // vftable+0xd4
    virtual void OnItemClick(SBItem *item);             // vftable+0xd8
    virtual void On3dLMouseUp();                        // vftable+0xdc
    static MBString __cdecl GetMofaDir();
    virtual void SendExitSceneEvent(const MBString &toSpace); // vftable+0xe0
    void AddItemToInventory(MBString &itemInstanceName, bool bPopUp);
    void RemoveItemFromInventory(MBString &itemInstanceName);
    void ResetCursor();
    static void __cdecl AddDirtyRect(IMBObject *obj);
    static void __cdecl AddDirtyRect(MBRect &rect);
    static void __cdecl DrawDirtyRects(HWND__ *hwnd, HDC__ *hdc, const MBColor &color);
    static SteamboatNode *__cdecl GetActiveNode();
    static IMBAnim *__cdecl GetActiveCursor();
    static IMBWorld *__cdecl GetActiveWorld();
    static IMBMouseMonitor *__cdecl GetActiveMouseMonitor();
    static void __cdecl SetActiveMouseMonitor(IMBMouseMonitor *monitor);
    static long __cdecl AddAnimToWorld(const MBString &strAnimName, IMBAnim *&pAnim);
    static long __cdecl CreateAndLoadAnim(const MBString &strAnimName, IMBAnim *&pAnim);
    static void __cdecl AddToWorld(IMBUnknown *iUnknown);
    static void __cdecl RemoveFromWorld(IMBUnknown *iUnknown);
    static long __cdecl GetStream(unsigned long id, unsigned long type, IMBStream *&pStream);
    static SBItem *__cdecl GetCursorItem();
    static bool __cdecl CreatePrefAndStream(IMBStream *&iStream, IMBPref *&iPref, const MBString &strFile, bool bReadOnly, bool bMustExist, unsigned long flags);
    virtual void SetCursorItem(SBItem *item);           // vftable+0xe4
    static short __cdecl GetSfxVolume() { return sSfxVolume; }
    static short __cdecl GetDialogVolume() { return sDialogVolume; }
    static short __cdecl GetMusicVolume() { return sMusicVolume; }
    static MBString __cdecl GetSpaceCode();
    virtual void EnableHotspots(bool enable);           // vftable+0xe8
    virtual void SaveGame();                            // vftable+0xec
protected:
    virtual void CreateDisplay();                       // vftable+0xf0
    virtual void RenderScene();                         // vftable+0xf4
    virtual void RenderBackground();                    // vftable+0xf8
    virtual void RenderCharacters();                    // vftable+0xfc
    virtual void SetSpaceName();                        // vftable+0x100
    virtual void LoadHotspots();                        // vftable+0x104
    virtual void ClearHotspots();                       // vftable+0x108
    virtual void LoadItems();                           // vftable+0x10c
    virtual void LoadMiscellaneous();                   // vftable+0x110
    virtual void LoadDialogue();                        // vftable+0x114
    virtual void ReadPrefs();                           // vftable+0x118
    virtual void StartListening();                      // vftable+0x88
    virtual void StopListening();                       // vftable+0x8c
    virtual IMBStream *OpenPrefStream();                // vftable+0x120
    virtual IMBStream *OpenPrefStream(const MBString &filename); // vftable+0x11c
    long FindFreeChannel(long start, long end);
    void SetNewBackgroundMusic(MBString name);
    void SetSceneBackgroundMusic(MBString name);
    void SetBackgroundMusicState(bool playing);
    void FeedBackgroundMusic();
    void OnClickWithCursorItem(SBItem *item);
    bool mShowGui;                                      // +0x0028 size 0x1
    MBComPtr<CSBRenderManager> mSBRenderMgr;            // +0x002c size 0x4
    static void __cdecl ClipDirtyRect(const MBRect &bounds, tagRECT &newRect);
    void AddAllObjectDirtyRects();
    void CalculateFrameRate();
    void OnDeleteCameraActor();
    void CopyMBMouseEventToMfMouseHookData(MBMouseEvent &evt, MfMouseHookData &data);
    MfAiSpace *mMotivateSpace;                          // +0x0030 size 0x4
    MfAiActorNode *mMfCameraNode;                       // +0x0034 size 0x4
    static MfAiActor *sMfPrimaryActor;
    Vec3<float> mCameraDir;                             // +0x0038 size 0xc
    float mCameraAngle;                                 // +0x0044 size 0x4
    MBString mSpaceCode;                                // +0x0048 size 0x10
    MBComPtr<IMBScreenPoly> mMotivateHotspot;           // +0x0058 size 0x4
    MBVector<MBComPtr<IMBScreenPoly> > mHotspots;       // +0x005c size 0x10
    MBVector<MBComPtr<ISBAnimButton> > mButtons;        // +0x006c size 0x10
    MBVector<MBComPtr<ISBClickable> > mClickables;      // +0x007c size 0x10
    bool mDisableHotspots;                              // +0x008c size 0x1
    bool mWaitingToPickUpItem;                          // +0x008d size 0x1
    unsigned long mEnableHotspotsDelayTimer;            // +0x0090 size 0x4
    void EnableHotspotsWithDelay();
    SBDialogue mDialogue;                               // +0x0094 size 0x18
    bool mCharacterDeactivatedInputEventChannel;        // +0x00ac size 0x1
    bool mNextSceneIsUI;                                // +0x00ad size 0x1
    static short sSfxVolume;
    static short sDialogVolume;
    static short sMusicVolume;
    static MBString sInventoryItemPicked;
    static Gfx::Node *sSceneRootNode;
    static ViewEventHandler *sMotivateEventHandler;
    static MBString sMofaDir;
    static SBItem *sCursorItem;
    static MBComPtr<IMBTextControl> sTextControl;
    static unsigned long sNextFrameTime;
    static unsigned long sLastFrameTime;
    static char sS[80];
    static int sNumFrames;
    static bool sDisplayFrameRate;
    static unsigned long sRenderFlags;
    static tagRECT sFrameRateRect;
    static tagRECT sBackgroundRect;
    static tagRECT sCursorRect[2];
    static tagRECT sGuiRect[2];
    static int sBackgroundDirty;
    static int sBuffer;
    static SteamboatNode *sCurrentActiveNode;
    static bool sAppInForeground;
    static bool sInputEventChannelActive;
    static _STREAM *sBackgroundMusic;
    static bool sWantBackgroundMusic;
    static MBString sBackgroundMusicName;
    static MBString sLocale;
    SBRenderObject *mRenderObject;                      // +0x00b0 size 0x4
    MBComPtr<ISBWorld> m_sbWorld;                       // +0x00b4 size 0x4
    MBPoint mMousePos;                                  // +0x00b8 size 0x8
    MfPoEvent *m_pMfEvent;                              // +0x00c0 size 0x4
    MfPoVector3 *m_pMfVector;                           // +0x00c4 size 0x4
    MfScDataHandle *m_hpMfVector;                       // +0x00c8 size 0x4
    MfScDataHandle *m_hpMfEvent;                        // +0x00cc size 0x4
    void CleanupExitSceneEvent();
};
