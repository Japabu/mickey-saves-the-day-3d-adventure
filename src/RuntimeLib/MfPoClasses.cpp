// The script classes RuntimeLib defines (RuntimeLib.dll 0x1006c040-
// 0x1006c270): MfRuntime::Initialize creates their class objects and compiles
// their constants, MfRuntime::Shutdown removes them again.
#include "MfPoApp.h"
#include "MfPoHFSM.h"
#include "MfPoJoystick.h"
#include "MfPoSound.h"
#include "MfPoSoundDriver.h"
#include "MfPoSoundResource.h"
#include "MfPoSpace.h"
#include "MfPoView.h"
#include "MfPoWorld.h"
#include "MfAiCompiler.h"
#include "MfInterfaceRegistry.h"

// 0x1006c040 ?MfPoInitializeClasses@@YAXXZ
// Creates the class objects; with the Piccolo compiler loaded, the constants
// of the joystick, sound, world and space classes become script globals.
void MfPoInitializeClasses()
{
    MfConstInitFunction initConstants = 0;
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        initConstants = compiler->GetConstInitFunction();
        compiler->Release();
    }
    MfPoHFSM::Initialize();
    MfPoSound::Initialize();
    MfPoSoundResource::Initialize();
    MfPoSoundDriver::Initialize();
    MfPoSpace::Init();
    MfPoWorld::Initialize();
    MfPoApp::Initialize();
    MfPoJoystick::Initialize();
    MfPoView::CreateClassObject();
    if (initConstants) {
        MfString errors;
        {
            MfString name("<Joystick Constants>");
            initConstants(name, MfPoJoystick::InitConstants(), &errors);
        }
        {
            MfString name("<Sound Constants>");
            initConstants(name, MfPoSound::InitConstants(), &errors);
        }
        {
            MfString name("<World Constants>");
            initConstants(name, MfPoWorld::InitConstants(), &errors);
        }
        {
            MfString name("<Space Constants>");
            initConstants(name, MfPoSpace::InitConstants(), &errors);
        }
    }
}

// 0x1006c240 ?MfPoShutdownClasses@@YAXXZ
void MfPoShutdownClasses()
{
    MfPoSpace::Exit();
    MfPoWorld::Shutdown();
    MfPoApp::Shutdown();
    MfPoView::DestroyClassObject();
    MfPoJoystick::Shutdown();
    MfPoHFSM::Shutdown();
    MfPoSound::Shutdown();
    MfPoSoundResource::Shutdown();
    MfPoSoundDriver::Shutdown();
}
