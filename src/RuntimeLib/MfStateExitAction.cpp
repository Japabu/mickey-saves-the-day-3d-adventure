// MfStateExitAction (RuntimeLib.dll 10086280-100868b0).
#include <iostream.h>
#include "MfStateAction.h"
#include "MfAiCompiler.h"
#include "MfAiFunctionRegistry.h"
#include "MfFiniteStateMachine.h"
#include "MfInterfaceRegistry.h"
#include "MfLogger.h"
#include "MfScDataHandle.h"
#include "MfScHandleList.h"
#include "MfScPiccoloObjectFile.h"

// 0x10086280 ??0MfStateExitAction@@QAE@VMfString@@@Z
MfStateExitAction::MfStateExitAction(MfString source)
    : mSource(source), mFunction(0)
{
    mName = "ExitAction";
    mType = TYPE;
}

// 0x10086350 ??1MfStateExitAction@@UAE@XZ
MfStateExitAction::~MfStateExitAction()
{
    if (mFunction)
        mFunction->Release();
}

// 0x100863c0 ?Clone@MfStateExitAction@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfStateExitAction::Clone()
{
    MfStateExitAction *copy = new MfStateExitAction(mSource);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x10086310 ?SetScript@MfStateExitAction@@UAEXABVMfString@@@Z

// 0x10086450 ?Write@MfStateExitAction@@UAEXAAVostream@@AAHH@Z
void MfStateExitAction::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    if (mSource.Length() < 3)
        return;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    if (mSource.Length() < 2)
        return;
    os << tabs;
    os << "exit_action" << endl;
    os << tabs;
    os << "{!*!";
    if (!mode)
        os << mSource;
    os << "!*!}";
    os << endl;
}

// 0x10086550 ?Link@MfStateExitAction@@UAEXXZ
void MfStateExitAction::Link()
{
    if (mFunction)
        mFunction->Release();
    mFunction = 0;
    MfAiFunctionRegistry *registry = MfScPiccolo::GetFunctionRegistry();
    MfScHandleList functions;
    registry->Find(mPath, &functions);
    mFunction = functions.First();
    if (mFunction)
        mFunction->AddRef();
}

// 0x10086600 ?Load@MfStateExitAction@@UAEXPAVMfScPiccoloObjectFile@@@Z
void MfStateExitAction::Load(MfScPiccoloObjectFile *objectFile)
{
    if (mFunction)
        mFunction->Release();
    mFunction = 0;
    MfScHandleList functions;
    MfScPiccoloObjectFile::MfScPOFErrorCode error = objectFile->LookupFunctions(mPath, &functions);
    if (error == MfScPiccoloObjectFile::kPOFOk) {
        mFunction = functions.First();
        if (mFunction)
            mFunction->AddRef();
    } else if (!mMachine->IsPlayingBack() && HasScript() == 1) {
        MfString message("Unable to initialize MfStateExitAction, ");
        message << mPath;
        message << " from the .pobj file.";
        MfLogger::Log(1, "Conductor", "InitCode", message);
    }
}

// 0x10086770 ?HasScript@MfStateExitAction@@QAEHXZ
int MfStateExitAction::HasScript()
{
    return mSource.Length() > 3;
}

// EQUIV 0x10086780 ?Compile@MfStateExitAction@@UAEXPAVMfScPiccoloObjectFile@@PAVMfPcErrorCallBack@@AAH@Z: reviewed: register choice only, every instruction and stack slot agrees (register choice only: the original loads errors into eax and the vtable into edx for the Compile call)
void MfStateExitAction::Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures)
{
    MfAiCompiler *compiler;
    int found = MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler);
    if (found) {
        if (mFunction)
            mFunction->Release();
        mFunction = 0;
        if (mSource.Length() >= 3) {
            if (compiler->Compile(mPath, "SCRIPT", mSource, errors, 0x40)) {
                mFunction = compiler->Functions().First();
                if (mFunction) {
                    mFunction->AddRef();
                    if (objectFile)
                        objectFile->AddFunction(mFunction);
                }
            } else
                failures++;
        }
        compiler->Release();
    }
}

// Not claimed: in the original this member is folded with an identical function
// (1008b9c0, MfTransTriggeredBy::Unload); the port needs its own definition.
void MfStateExitAction::Unload()
{
    if (mFunction)
        mFunction->Release();
    mFunction = 0;
}
