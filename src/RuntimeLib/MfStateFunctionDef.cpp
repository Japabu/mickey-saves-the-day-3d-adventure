// MfStateFunctionDef (RuntimeLib.dll 10086f00-10087600): the functions a
// state defines for its scripts ('func_def {!*!...!*!}').
#include <iostream.h>
#include "MfStateAction.h"
#include "MfAiCompiler.h"
#include "MfAiFunctionRegistry.h"
#include "MfFiniteStateMachine.h"
#include "MfInterfaceRegistry.h"
#include "MfLogger.h"
#include "MfScDataHandle.h"
#include "MfScDataMethod.h"
#include "MfScHandleDict.h"
#include "MfScHandleList.h"
#include "MfScPiccoloObjectFile.h"

// 0x10086f00 ??0MfStateFunctionDef@@QAE@VMfString@@@Z
MfStateFunctionDef::MfStateFunctionDef(MfString source)
    : mSource(source)
{
    mName = "StateMethods";
    mType = TYPE;
    mFunctions = new MfScHandleList;
}

// 0x10086fb0 ??_GMfScHandleList@@UAEPAXI@Z

// 0x10086ff0 ??1MfStateFunctionDef@@UAE@XZ
MfStateFunctionDef::~MfStateFunctionDef()
{
    if (mFunctions)
        delete mFunctions;
}

// 0x10087060 ?Clone@MfStateFunctionDef@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfStateFunctionDef::Clone()
{
    MfStateFunctionDef *copy = new MfStateFunctionDef(mSource);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x100870f0 ?Write@MfStateFunctionDef@@UAEXAAVostream@@AAHH@Z
void MfStateFunctionDef::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    if (mSource.Length() < 2)
        return;
    os << tabs;
    os << "func_def ";
    os << "{!*!";
    if (!mode)
        os << mSource;
    os << "!*!}";
    os << endl;
}

// EQUIV 0x100871c0 ?DefineFunctions@MfStateFunctionDef@@QAEXPAVMfScHandleDict@@@Z: reviewed: the original copies First()'s result from eax into esi, VC6 here loads it into esi directly; nothing else differs
void MfStateFunctionDef::DefineFunctions(MfScHandleDict *variables)
{
    for (MfScDataHandle *h = mFunctions->First(); h; h = mFunctions->Next()) {
        MfScDataMethod *method = h ? dynamic_cast<MfScDataMethod *>(h->GetData()) : 0;
        variables->Replace(method->Name(), h, MfScHandleDict::kAddRef);
    }
}

// 0x10087250 ?Link@MfStateFunctionDef@@UAEXXZ
void MfStateFunctionDef::Link()
{
    mFunctions->Clear();
    MfAiFunctionRegistry *registry = MfScPiccolo::GetFunctionRegistry();
    MfScHandleList functions;
    registry->Find(mPath, &functions);
    for (MfScDataHandle *h = functions.First(); h; h = functions.Next())
        mFunctions->Add(h);
}

// EQUIV 0x10087310 ?Load@MfStateFunctionDef@@UAEXPAVMfScPiccoloObjectFile@@@Z: reviewed: this and the path pointer swap ebp/edi (consistently, every push takes the same value), and Next()'s result goes through eax in the original and straight into esi here; same calls, arguments and stores
void MfStateFunctionDef::Load(MfScPiccoloObjectFile *objectFile)
{
    mFunctions->Clear();
    MfScHandleList functions;
    MfScPiccoloObjectFile::MfScPOFErrorCode error = objectFile->LookupFunctions(mPath, &functions);
    if (error == MfScPiccoloObjectFile::kPOFOk) {
        for (MfScDataHandle *h = functions.First(); h; h = functions.Next()) {
            mFunctions->Add(h);
            h->AddRef();
        }
    } else if (!mMachine->IsPlayingBack() && HasScript() == 1) {
        MfString message("Unable to initialize MfStateFunctionDef, ");
        message << mPath;
        message << " from the .pobj file.";
        MfLogger::Log(1, "Conductor", "InitCode", message);
    }
}

// 0x10087490 ?Unload@MfStateFunctionDef@@UAEXXZ
void MfStateFunctionDef::Unload()
{
    mFunctions->Clear();
}

// EQUIV 0x100874a0 ?Compile@MfStateFunctionDef@@UAEXPAVMfScPiccoloObjectFile@@PAVMfPcErrorCallBack@@AAH@Z: reviewed: eax and edx swap for the vtable and argument loads throughout (consistently), and the first list item goes through ecx before esi in the original; same calls, arguments and stores
void MfStateFunctionDef::Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures)
{
    MfAiCompiler *compiler;
    int found = MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler);
    if (found) {
        if (mSource.Length() >= 3) {
            mFunctions->Clear();
            if (compiler->Compile(mPath, "SCRIPT", mSource, errors, 0x80)) {
                if (compiler->CodeType() == 2) {
                    for (MfScDataHandle *h = compiler->Functions().First(); h; h = compiler->Functions().Next()) {
                        mFunctions->Add(h);
                        if (objectFile)
                            objectFile->AddFunction(h);
                    }
                }
            } else
                failures++;
        }
        compiler->Release();
    }
}

// the image keeps the identical MfStateExitAction::HasScript (10086770)
int MfStateFunctionDef::HasScript()
{
    return mSource.Length() > 3;
}
