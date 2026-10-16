#include "dllmain.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "SimpleNode.h"

typedef IMBUnknown *(__cdecl *MBClassFactoryFunc)(void);
extern "C" long __cdecl MBComRegisterClassObject(const MBGUID &guid, MBClassFactoryFunc func);

IMBUnknown *MakeCMBScreenPoly();
IMBUnknown *MakeCSBAnimButton();
IMBUnknown *MakeCSBClickable();
IMBUnknown *MakeCSBTrashTile();
IMBUnknown *MakeCSBNodeManager();
IMBUnknown *MakeCSBSongbirdNote();
IMBUnknown *MakeCSBRenderManager();
IMBUnknown *MakeCSBGameState();
IMBUnknown *MakeCSBInventory();
IMBUnknown *MakeCSBWorld();

ISBNodeManager *steamboatNodeMgr;

// 0x1002e920 _MBComRegisterServer
extern "C" unsigned char __cdecl MBComRegisterServer(void)
{
    MBComRegisterClassObject(MBIID_CMBScreenPoly, MakeCMBScreenPoly);
    MBComRegisterClassObject(MBIID_CSBAnimButton, MakeCSBAnimButton);
    MBComRegisterClassObject(MBIID_CSBClickable, MakeCSBClickable);
    MBComRegisterClassObject(MBIID_CSBTrashTile, MakeCSBTrashTile);
    MBComRegisterClassObject(MBIID_CSBNodeManager, MakeCSBNodeManager);
    MBComRegisterClassObject(MBIID_CSBSongbirdNote, MakeCSBSongbirdNote);
    MBComRegisterClassObject(MBIID_CSBRenderManager, MakeCSBRenderManager);
    MBComRegisterClassObject(MBIID_CSBGameState, MakeCSBGameState);
    MBComRegisterClassObject(MBIID_CSBInventory, MakeCSBInventory);
    MBComRegisterClassObject(MBIID_CSBWorld, MakeCSBWorld);
    MBComRegisterClassObject(MBIID_IMBScreenPoly, MakeCMBScreenPoly);
    MBComRegisterClassObject(MBIID_ISBAnimButton, MakeCSBAnimButton);
    MBComRegisterClassObject(MBIID_ISBClickable, MakeCSBClickable);
    MBComRegisterClassObject(MBIID_ISBTrashTile, MakeCSBTrashTile);
    MBComRegisterClassObject(MBIID_ISBNodeManager, MakeCSBNodeManager);
    MBComRegisterClassObject(MBIID_ISBSongbirdNote, MakeCSBSongbirdNote);
    MBComRegisterClassObject(MBIID_ISBGameState, MakeCSBGameState);
    MBComRegisterClassObject(MBIID_ISBInventory, MakeCSBInventory);
    return 1;
}

// 0x1002ea90 _MBComUnregisterServer
extern "C" unsigned char __cdecl MBComUnregisterServer(void)
{
    return 1;
}

// 0x1002eab0 _DDDeviceSelectionCallback@20
extern "C" int __stdcall DDDeviceSelectionCallback(_GUID *lpGUID, char *lpDriverDescription, char *lpDriverName,
                                                   void *lpContext, HMONITOR__ *hm)
{
    return lpGUID == 0;
}

// 0x1002ead0 _OnMBEngineStartup
extern "C" void __cdecl OnMBEngineStartup(IMBGameEngine *engine)
{
    SimpleNode::Startup(engine);
    IMBUnknown *unknown;
    MBComCreateInstance(0, MBIID_CSBNodeManager, &unknown);
    unknown->QueryInterface(MBIID_ISBNodeManager, (void **)&steamboatNodeMgr);
    unknown->Release();
    steamboatNodeMgr->SetEngine(engine);
    steamboatNodeMgr->AddNode("MV");
}

// 0x1002eb50 _OnMBEngineShutdown
extern "C" void __cdecl OnMBEngineShutdown(IMBGameEngine *engine)
{
    SimpleNode::Shutdown(engine);
    if (steamboatNodeMgr)
        steamboatNodeMgr->Release();
}

// 0x1002eb80 _OnMBEngineCreateScreen
extern "C" long __cdecl OnMBEngineCreateScreen(IMBGameEngine *engine, IMBScreenManager *screenManager)
{
    long err = 0;
    MBScreenAttributes attributes;
    attributes.width = 640;
    attributes.height = 480;
    attributes.depth = 16;
    if (!screenManager->RequestScreen(1, attributes))
        err = -2;
    return err;
}

// 0x1002e2e0 _$E18
// 0x1002e300 _$E15
// 0x1002e8d0 _$E17
// 0x1002e8f0 _$E16
// 0x1002ebd0 _$E32
// 0x1002ec10 _$E28
// 0x1002ec30 _$E31
