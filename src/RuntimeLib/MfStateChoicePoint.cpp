// MfStateChoicePoint (RuntimeLib.dll 10084630-10084df0).
#include <iostream.h>
#include "MfStateChoicePoint.h"
#include "MfState.h"
#include "MfStateAction.h"
#include "MfStateDisplayPos.h"
#include "MfFiniteStateMachine.h"
#include "MfScDataHandle.h"
#include "MfScThread.h"
#include "MfScVirtualMachine.h"
#include "MfScVmEnv.h"

// what the machine's choice is reset to
static char sNoChoice[4];

// 0x10084630 ??0MfStateChoicePoint@@QAE@ABVMfString@@PAVMfStatePropertyList@@@Z
MfStateChoicePoint::MfStateChoicePoint(const MfString &name, MfStatePropertyList *properties)
    : mTransitions(1), mChosen(0), mDefault(0), mAction(0)
{
    mName = name;
    mType = TYPE;
    mProperties = properties;
}

// 0x100846d0 ??1MfStateChoicePoint@@UAE@XZ
MfStateChoicePoint::~MfStateChoicePoint()
{
    if (mAction)
        delete mAction;
    mAction = 0;
}

// 0x10084740 ?Clone@MfStateChoicePoint@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfStateChoicePoint::Clone()
{
    MfStatePropertyList *properties = new MfStatePropertyList(0);
    MfStateChoicePoint *copy = new MfStateChoicePoint(mName, properties);
    copy->SetProperties(mProperties);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x10084800 ?GetVariableStack@MfStateChoicePoint@@UAEPAVMfScVariableStack@@XZ
MfScVariableStack *MfStateChoicePoint::GetVariableStack()
{
    return mParent->GetVariableStack();
}

// 0x10084810 ?FormatForExecution@MfStateChoicePoint@@UAEXPAVMfFiniteStateMachine@@@Z
void MfStateChoicePoint::FormatForExecution(MfFiniteStateMachine *machine)
{
    MfString defaultName("default");
    MfDblListItem *pli;
    MfStateProperty *p;
    mMachine = machine;
    mDefault = 0;
    mChosen = 0;
    for (p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli)) {
        switch (p->mType) {
        case MfStateDisplayPos::TYPE:
            mDisplayPos = p;
            break;
        case MfStateTransition::TYPE:
            mChosen = (MfStateTransition *)p;
            mChosen->SetSourceState(this);
            if (!mTransitions.Contains(mChosen))
                mTransitions.AddProperty(mChosen);
            if (mChosen->GetName() == defaultName)
                mDefault = mChosen;
            break;
        case MfStateEntryAction::TYPE:
            mAction = (MfStateEntryAction *)p;
            mAction->mOwner = this;
            break;
        default:
            cout << "MfStateChoicePoint::FormatForExecution(): Unknown transition type:" << p->mType << endl;
            break;
        }
    }
    for (p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->FormatForExecution(mMachine);
}

// EQUIV 0x10084970 ?Choose@MfStateChoicePoint@@QAEPAVMfStateTransition@@PAVMfScVirtualMachine@@@Z: reviewed: register choice and the order of independent loads/pushes only: the original loads vm before storing the variable stack and swaps eax/edx from there on
MfStateTransition *MfStateChoicePoint::Choose(MfScVirtualMachine *vm)
{
    mChosen = 0;
    if (!mAction)
        return 0;
    if (mAction->mFunction) {
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        MfScVariableStack *stack = mParent->GetVariableStack();
        MfScVariableStack *outer = env->mVarStack;
        env->mVarStack = stack;
        MfScDataHandle *result = vm->ExecuteFunction(mAction->mFunction, 0);
        if (result)
            result->Release();
        env->mVarStack = outer;
        MfString choice = mMachine->GetChoice();
        if (choice.Length() > 0) {
            MfDblListItem *pli;
            MfStatePropertyList *list = &mTransitions;
            for (MfStateTransition *t = (MfStateTransition *)list->GetFirst(pli); t; t = (MfStateTransition *)list->GetNext(pli)) {
                if (t->GetName() == choice) {
                    mChosen = t;
                    break;
                }
            }
        }
        mMachine->mChoice = sNoChoice;
    }
    if (!mChosen)
        mChosen = mDefault;
    return mChosen;
}

// 0x10084ab0 ?GetChoice@MfFiniteStateMachine@@QAE?AVMfString@@XZ

// 0x10084ae0 ?CanConnectTo@MfStateChoicePoint@@UAEHPAVMfStateProperty@@@Z
int MfStateChoicePoint::CanConnectTo(MfStateProperty *target)
{
    if (target->mType == MfState::TYPE) {
        if (mParent != target->mParent) {
            mMachine->ShowMessage(2, "Illegal connection attempted. If you want a CHOICE point directly connected to a STATE the STATE has to be a sibling of that CHOICE point!");
            return 0;
        }
    } else if (target->mType == 0x6d) {
        if (mParent != target->mParent->mParent) {
            mMachine->ShowMessage(2, "Illegal connection attempted. To connect to an EXIT port to a CHOICE point the EXIT ports owner state has to be siblings with the choice point!");
            return 0;
        }
    } else if (target->mType == 0x6e) {
        if (mParent != target->mParent) {
            mMachine->ShowMessage(2, "Illegal connection attempted. To connect to an entry port from a choice point the entry port has to be on a state that is a sibling of the choice point!");
            return 0;
        }
    } else if (target->mType == MfStateChoicePoint::TYPE) {
        if (mParent != target->mParent || target == this) {
            mMachine->ShowMessage(2, "Choice points can only be directly connected together if they are siblings!");
            return 0;
        }
    }
    return 1;
}

// 0x10084b80 ?Write@MfStateChoicePoint@@UAEXAAVostream@@AAHH@Z
void MfStateChoicePoint::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    indent++;
    os << tabs;
    os << "choice_pt " << mName << endl;
    os << tabs;
    os << "{" << endl;
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Write(os, indent, mode);
    os << tabs;
    os << "}" << endl;
    indent--;
}

// 0x10084cb0 ?SetPath@MfStateChoicePoint@@UAEXVMfString@@@Z
void MfStateChoicePoint::SetPath(MfString parent)
{
    mPath = parent;
    mPath << '\\';
    mPath << mName;
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->SetPath(mPath);
}

// 0x10084d60 ?Link@MfStateChoicePoint@@UAEXXZ
void MfStateChoicePoint::Link()
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Link();
}

// 0x10084d90 ?Unload@MfStateChoicePoint@@UAEXXZ
void MfStateChoicePoint::Unload()
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Unload();
}

// 0x10084dc0 ?GetScript@MfStateChoicePoint@@UAEPAVMfString@@XZ
MfString *MfStateChoicePoint::GetScript()
{
    if (mAction)
        return mAction->GetScript();
    return 0;
}

// 0x10084dd0 ?SetScript@MfStateChoicePoint@@UAEXABVMfString@@@Z
void MfStateChoicePoint::SetScript(const MfString &source)
{
    if (mAction)
        mAction->SetScript(source);
}

// the slots that pass a call on to all properties (the image keeps the
// copies of MfState and MfStateEntryPort)
void MfStateChoicePoint::Load(MfScPiccoloObjectFile *objectFile)
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Load(objectFile);
}

void MfStateChoicePoint::Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures)
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Compile(objectFile, errors, failures);
}
