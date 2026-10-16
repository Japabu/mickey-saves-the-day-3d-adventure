// MfStateEntryAction (RuntimeLib.dll 10085560-10085ba0).
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

// 0x10085560 ??0MfStateEntryAction@@QAE@VMfString@@@Z
MfStateEntryAction::MfStateEntryAction(MfString source)
    : mSource(source), mFunction(0), mOwner(0)
{
    mName = "EntryAction";
    mType = TYPE;
}

// 0x10085600 ?Search@MfStateEntryAction@@UAEXVMfString@@HHHHHHHAAPAVMfFSMMutable@@1@Z

// 0x10085630 ??1MfStateEntryAction@@UAE@XZ
MfStateEntryAction::~MfStateEntryAction()
{
    if (mFunction)
        mFunction->Release();
}

// 0x100856a0 ?Clone@MfStateEntryAction@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfStateEntryAction::Clone()
{
    MfStateEntryAction *copy = new MfStateEntryAction(mSource);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x10085730 ?SetScript@MfStateEntryAction@@UAEXABVMfString@@@Z
void MfStateEntryAction::SetScript(const MfString &source)
{
    mSource = source;
}

// 0x10085750 ?Write@MfStateEntryAction@@UAEXAAVostream@@AAHH@Z
void MfStateEntryAction::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    if (mSource.Length() < 3)
        return;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    if (mSource.Length() < 2)
        return;
    os << tabs;
    os << "entry_action" << endl;
    os << tabs;
    os << "{!*!";
    if (!mode)
        os << mSource;
    os << "!*!}" << endl;
}

// 0x10085850 ?Link@MfStateEntryAction@@UAEXXZ
void MfStateEntryAction::Link()
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

// 0x10085900 ?Load@MfStateEntryAction@@UAEXPAVMfScPiccoloObjectFile@@@Z
void MfStateEntryAction::Load(MfScPiccoloObjectFile *objectFile)
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
        MfString message("Unable to initialize MfStateEntryAction, ");
        message << mPath;
        message << " from the .pobj file.";
        MfLogger::Log(1, "Conductor", "InitCode", message);
    }
}

// EQUIV 0x10085a70 ?Compile@MfStateEntryAction@@UAEXPAVMfScPiccoloObjectFile@@PAVMfPcErrorCallBack@@AAH@Z: reviewed: register choice only, every instruction and stack slot agrees (register choice only: the original loads errors into eax and the vtable into edx for the Compile call)
void MfStateEntryAction::Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures)
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

// the image keeps the identical MfStateExitAction::HasScript (10086770)
int MfStateEntryAction::HasScript()
{
    return mSource.Length() > 3;
}
