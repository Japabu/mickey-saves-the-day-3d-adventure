// MfTransTriggeredBy (RuntimeLib.dll 1008b280-1008be20).
#include <iostream.h>
#include "MfTransitionProperty.h"
#include "MfAiCompiler.h"
#include "MfAiFunctionRegistry.h"
#include "MfFiniteStateMachine.h"
#include "MfInterfaceRegistry.h"
#include "MfLogger.h"
#include "MfMsgTypeTable.h"
#include "MfScDataHandle.h"
#include "MfScHandleList.h"
#include "MfScPiccoloObjectFile.h"

class MfNullString;
extern __declspec(dllimport) MfNullString NULL_STRING;

MfDblList *gTriggers;

// 0x1008b280 ??0MfTransTriggeredBy@@QAE@ABVMfString@@PAV1@@Z
MfTransTriggeredBy::MfTransTriggeredBy(const MfString &event, MfString *guard)
    : mGuard(0), mEvent(-1, -1), mCondition(0)
{
    mName = event;
    if (!gTriggers)
        gTriggers = new MfDblList;
    gTriggers->Add(this);
    mType = TYPE;
    LookupEvent();
    if (guard)
        mGuard = new MfString(*guard);
}

// 0x1008b360 ?SetScript@MfTransTriggeredBy@@UAEXABVMfString@@@Z

// 0x1008b3b0 ??1MfTransTriggeredBy@@UAE@XZ
MfTransTriggeredBy::~MfTransTriggeredBy()
{
    if (mCondition)
        mCondition->Release();
    MfDblListItem *pli = gTriggers->Find(this);
    if (pli)
        gTriggers->Delete(pli);
    delete mGuard;
}

// 0x1008b450 ?Clone@MfTransTriggeredBy@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfTransTriggeredBy::Clone()
{
    MfString *guard;
    if (!mGuard)
        guard = new MfString(NULL_STRING);
    else
        guard = new MfString(*mGuard);
    MfTransTriggeredBy *copy = new MfTransTriggeredBy(mName, guard);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x1008b540 ?SetGuard@MfTransTriggeredBy@@QAEXVMfString@@@Z
void MfTransTriggeredBy::SetGuard(MfString guard)
{
    delete mGuard;
    mGuard = new MfString(guard);
}

// 0x1008b5d0 ?Write@MfTransTriggeredBy@@UAEXAAVostream@@AAHH@Z
void MfTransTriggeredBy::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    os << tabs;
    os << "trig_by " << mName << endl;
    os << tabs;
    os << "{" << endl;
    if (HasGuard() == 1) {
        os << tabs;
        os << "\ttrig_guard" << endl;
        os << tabs;
        os << "\t";
        os << "{!*!";
        if (mGuard && !mode)
            os << *mGuard;
        os << "!*!}";
        os << endl;
    }
    os << tabs;
    os << "}" << endl;
}

// 0x1008b730 ?SetPath@MfTransTriggeredBy@@UAEXVMfString@@@Z
void MfTransTriggeredBy::SetPath(MfString parent)
{
    mPath = parent;
    mPath << '\\';
    mPath << mName;
}

// 0x1008b7a0 ?Link@MfTransTriggeredBy@@UAEXXZ
void MfTransTriggeredBy::Link()
{
    if (mCondition)
        mCondition->Release();
    mCondition = 0;
    MfAiFunctionRegistry *registry = MfScPiccolo::GetFunctionRegistry();
    MfScHandleList functions;
    registry->Find(mPath, &functions);
    mCondition = functions.First();
    if (mCondition)
        mCondition->AddRef();
}

// 0x1008b850 ?Load@MfTransTriggeredBy@@UAEXPAVMfScPiccoloObjectFile@@@Z
void MfTransTriggeredBy::Load(MfScPiccoloObjectFile *objectFile)
{
    if (mCondition)
        mCondition->Release();
    mCondition = 0;
    MfScHandleList functions;
    MfScPiccoloObjectFile::MfScPOFErrorCode error = objectFile->LookupFunctions(mPath, &functions);
    if (error == MfScPiccoloObjectFile::kPOFOk) {
        mCondition = functions.First();
        if (mCondition)
            mCondition->AddRef();
    } else if (!mMachine->IsPlayingBack() && HasGuard() == 1) {
        MfString message("Unable to initialize MfTransTriggeredBy, ");
        message << mPath;
        message << " from the .pobj file.";
        MfLogger::Log(1, "Conductor", "InitCode", message);
    }
}

// 0x1008b9c0 ?Unload@MfTransTriggeredBy@@UAEXXZ
void MfTransTriggeredBy::Unload()
{
    if (mCondition)
        mCondition->Release();
    mCondition = 0;
}

// 0x1008b9e0 ?Search@MfTransTriggeredBy@@UAEXVMfString@@HHHHHHHAAPAVMfFSMMutable@@1@Z
void MfTransTriggeredBy::Search(MfString text, int, int caseSensitive, int, int, int inScripts, int inNames, int,
                                MfFSMMutable *&found, MfFSMMutable *&foundIn)
{
    MfString s;
    if (inNames == 1) {
        s = mName;
        if (!caseSensitive)
            s.ConvertToLower();
        if (s.ContainsSubString(text, 0) >= 0) {
            found = this;
            foundIn = this;
            return;
        }
    }
    if (inScripts == 1 && mGuard) {
        s = *mGuard;
        if (!caseSensitive)
            s.ConvertToLower();
        if (s.ContainsSubString(text, 0) >= 0) {
            found = this;
            foundIn = this;
            return;
        }
    }
    found = 0;
    foundIn = 0;
}

// TODO 0x1008bc70 ?Compile@MfTransTriggeredBy@@UAEXPAVMfScPiccoloObjectFile@@PAVMfPcErrorCallBack@@AAH@Z: (82% similar) the original keeps a separate compiler->Release() on each path; VC5 merges ours
void MfTransTriggeredBy::Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures)
{
    MfAiCompiler *compiler;
    int found = MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler);
    if (found) {
        if (mCondition)
            mCondition->Release();
        mCondition = 0;
        if (mGuard && mGuard->Length() >= 3) {
            if (compiler->Compile(mPath, "SCRIPT", *mGuard, errors, 0x40)) {
                mCondition = compiler->Functions().First();
                if (!mCondition)
                    compiler->Release();
                else {
                    mCondition->AddRef();
                    if (objectFile)
                        objectFile->AddFunction(mCondition);
                    compiler->Release();
                }
            } else {
                failures++;
                compiler->Release();
            }
        } else
            compiler->Release();
    }
}

// 0x1008bdd0 ?HasGuard@MfTransTriggeredBy@@QAEHXZ
int MfTransTriggeredBy::HasGuard()
{
    if (mGuard && mGuard->Length() > 3)
        return 1;
    return 0;
}

// 0x1008bdf0 ?LookupEvent@MfTransTriggeredBy@@QAEXXZ
void MfTransTriggeredBy::LookupEvent()
{
    mEvent = g_msgTypes->Lookup(mName);
}
