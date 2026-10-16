// MfTransTrigAction (RuntimeLib.dll 1008a9e0-1008b280).
#include <iostream.h>
#include "MfTransitionProperty.h"
#include "MfAiCompiler.h"
#include "MfAiFunctionRegistry.h"
#include "MfFiniteStateMachine.h"
#include "MfInterfaceRegistry.h"
#include "MfLogger.h"
#include "MfScDataHandle.h"
#include "MfScHandleList.h"
#include "MfScPiccoloObjectFile.h"

// 0x1008a9e0 ??0MfTransTrigAction@@QAE@ABVMfString@@@Z
MfTransTrigAction::MfTransTrigAction(const MfString &source)
    : mSource(source), mFunction(0)
{
    mName = "TransitAction";
    mType = TYPE;
}

// 0x1008aa60 ?GetScript@MfTransTrigAction@@UAEPAVMfString@@XZ
// 0x1008aa70 ?SetScript@MfTransTrigAction@@UAEXABVMfString@@@Z

// 0x1008aab0 ??1MfTransTrigAction@@UAE@XZ
MfTransTrigAction::~MfTransTrigAction()
{
    if (mFunction)
        mFunction->Release();
}

// 0x1008ab20 ?Clone@MfTransTrigAction@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfTransTrigAction::Clone()
{
    MfTransTrigAction *copy = new MfTransTrigAction(mSource);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x1008aba0 ?Write@MfTransTrigAction@@UAEXAAVostream@@AAHH@Z
void MfTransTrigAction::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    if (mSource.Length() < 2)
        return;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    os << tabs;
    os << "trig_action" << endl;
    os << tabs;
    os << "{!*!";
    if (!mode)
        os << mSource;
    os << "!*!}";
    os << endl;
}

// 0x1008acb0 ?SetPath@MfTransTrigAction@@UAEXVMfString@@@Z
void MfTransTrigAction::SetPath(MfString parent)
{
    mPath = parent;
    mPath << '\\';
    mPath << "TransitAction";
}

// 0x1008ad20 ?Link@MfTransTrigAction@@UAEXXZ
void MfTransTrigAction::Link()
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

// 0x1008add0 ?Load@MfTransTrigAction@@UAEXPAVMfScPiccoloObjectFile@@@Z
void MfTransTrigAction::Load(MfScPiccoloObjectFile *objectFile)
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
        MfString message("Unable to initialize MfTransTrigAction, ");
        message << mPath;
        message << " from the .pobj file.";
        MfLogger::Log(1, "Conductor", "InitCode", message);
    }
}

// 0x1008af40 ?Unload@MfTransTrigAction@@UAEXXZ
void MfTransTrigAction::Unload()
{
    if (mFunction)
        mFunction->Release();
    mFunction = 0;
}

// 0x1008af60 ?Search@MfTransTrigAction@@UAEXVMfString@@HHHHHHHAAPAVMfFSMMutable@@1@Z
void MfTransTrigAction::Search(MfString text, int a2, int caseSensitive, int a4, int what, int inScripts,
                               int a7, int a8, MfFSMMutable *&found, MfFSMMutable *&foundIn)
{
    if (!inScripts)
        return;
    MfString source = mSource;
    if (!caseSensitive)
        source.ConvertToLower();
    if (source.ContainsSubString(text, 0) <= -1) {
        foundIn = 0;
        found = 0;
        if (what == 1)
            MfFSMMutable::Search(text, a2, caseSensitive, a4, what, inScripts, a7, a8, found, foundIn);
    } else {
        foundIn = this;
        found = this;
    }
}

// EQUIV 0x1008b150 ?Compile@MfTransTrigAction@@UAEXPAVMfScPiccoloObjectFile@@PAVMfPcErrorCallBack@@AAH@Z: reviewed: register choice only
// (as MfStateEntryAction::Compile); the minimum script length is 5 (was 3 here: fixed)
void MfTransTrigAction::Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures)
{
    MfAiCompiler *compiler;
    int found = MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler);
    if (found) {
        if (mFunction)
            mFunction->Release();
        mFunction = 0;
        if (mSource.Length() >= 5) {
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

// 0x1008b140 ?HasScript@MfTransTrigAction@@QAEHXZ
int MfTransTrigAction::HasScript()
{
    return mSource.Length() > 3;
}
