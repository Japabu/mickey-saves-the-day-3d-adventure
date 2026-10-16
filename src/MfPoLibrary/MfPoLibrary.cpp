// MfPoLibrary (MfPoLibrary.dll 0x1002ff60-0x10030690)
#include "MfPoLibrary.h"
#include "MfAiCompiler.h"
#include "MfInterfaceRegistry.h"
#include "MfPoActor.h"
#include "MfPoActorNode.h"
#include "MfPoArray.h"
#include "MfPoCamera.h"
#include "MfPoColor.h"
#include "MfPoEvent.h"
#include "MfPoFilter.h"
#include "MfPoKChain.h"
#include "MfPoKeyboard.h"
#include "MfPoLight.h"
#include "MfPoList.h"
#include "MfPoMaterial.h"
#include "MfPoMouse.h"
#include "MfPoPath2.h"
#include "MfPoRotation3.h"
#include "MfPoSkill.h"
#include "MfPoSystem.h"
#include "MfPoTexture.h"
#include "MfPoTransform.h"
#include "MfPoVector3.h"
#include "MfPoActivity.h"
#include "MfPoActivityWrapper.h"

// 0x1002ff60 ??0MfPoLibrary@@AAE@XZ
// 0x1002ff70 ??4MfPoLibrary@@QAEAAV0@ABV0@@Z (implicit)

// the application interfaces the script classes use
MfAiMouse *g_paiMouse;
MfAiWorld *g_paiWorld;
MfAiAppWorld *g_paiAppWorld;
MfAiKeyboard *g_paiKeyboard;

// 0x100301f0 ?Initialize@MfPoLibrary@@SAXXZ
void MfPoLibrary::Initialize()
{
    MfAiCompiler *compiler;
    MfConstInitFunction initConstants = 0;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        initConstants = compiler->GetConstInitFunction();
        compiler->Release();
    }
    MfPoActor::Initialize();
    MfPoActorNode::Initialize();
    MfPoActivity::Initialize();
    MfPoKeyboard::Initialize();
    MfPoVector3::Initialize();
    MfPoRotation3::Initialize();
    MfPoTransform::Initialize();
    MfPoActorNode::Initialize();
    MfPoSkill::Initialize();
    MfPoKChain::Initialize();
    MfPoMouse::Initialize();
    MfPoSystem::Initialize();
    MfPoMaterial::Initialize();
    MfPoPath2::Initialize();
    MfPoActivityWrapper::Initialize();
    MfPoArray::Initialize();
    MfPoList::Initialize();
    MfPoEvent::Initialize();
    MfPoCamera::Initialize();
    MfPoColor::Initialize();
    MfPoLight::Initialize();
    MfPoTexture::Initialize();
    MfPoFilter::Initialize();
    if (initConstants) {
        MfString errors;
        initConstants("<MfPoActivity Constants>", MfPoActivity::InitConstants(), &errors);
        initConstants("<MfPoActor Constants>", MfPoActor::InitConstants(), &errors);
        initConstants("<MfPoActorNode Constants>", MfPoActorNode::InitConstants(), &errors);
        initConstants("<MfPoKeyboard Constants>", MfPoKeyboard::InitConstants(), &errors);
        initConstants("<MfPoMouse Constants>", MfPoMouse::InitConstants(), &errors);
        initConstants("<MfPoSystem Constants>", MfPoSystem::InitConstants(), &errors);
        initConstants("<MfPoCamera Constants>", MfPoCamera::InitConstants(), &errors);
        initConstants("<MfPoLight Constants>", MfPoLight::InitConstants(), &errors);
        initConstants("<MfPoMaterial Constants>", MfPoMaterial::InitConstants(), &errors);
        initConstants("<MfPoTexture Constants>", MfPoTexture::InitConstants(), &errors);
        initConstants("<MfPoFilter Constants>", MfPoFilter::InitConstants(), &errors);
    }
}

// 0x10030620 ?Shutdown@MfPoLibrary@@SAXXZ
void MfPoLibrary::Shutdown()
{
    MfPoActivityWrapper::Shutdown();
    MfPoList::Shutdown();
    MfPoArray::Shutdown();
    MfPoActivity::Shutdown();
    MfPoActor::Shutdown();
    MfPoActorNode::Shutdown();
    MfPoKeyboard::Shutdown();
    MfPoRotation3::Shutdown();
    MfPoSkill::Shutdown();
    MfPoKChain::Shutdown();
    MfPoTransform::Shutdown();
    MfPoVector3::Shutdown();
    MfPoMouse::Shutdown();
    MfPoMaterial::Shutdown();
    MfPoPath2::Shutdown();
    MfPoColor::Shutdown();
    MfPoTexture::Shutdown();
    MfPoLight::Shutdown();
    MfPoCamera::Shutdown();
    MfPoEvent::Shutdown();
    MfPoFilter::Shutdown();
}
