// MfTransChoicePt (RuntimeLib.dll 10089620-10089990).
#include <iostream.h>
#include "MfTransitionProperty.h"
#include "MfState.h"

// 0x10089620 ??0MfTransChoicePt@@QAE@VMfString@@0@Z
MfTransChoicePt::MfTransChoicePt(MfString state, MfString choicePoint)
    : mChoicePoint(choicePoint), mState(state), mTarget(0)
{
    mType = TYPE;
}

// 0x100896f0 ??1MfTransChoicePt@@UAE@XZ
MfTransChoicePt::~MfTransChoicePt()
{
}

// 0x10089760 ?Clone@MfTransChoicePt@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfTransChoicePt::Clone()
{
    MfTransChoicePt *copy = new MfTransChoicePt(mState, mChoicePoint);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x10089810 ?FormatForExecution@MfTransChoicePt@@UAEXPAVMfFiniteStateMachine@@@Z
void MfTransChoicePt::FormatForExecution(MfFiniteStateMachine *machine)
{
    mMachine = machine;
    mTarget = 0;
    if (!mTransition)
        return;
    MfStateProperty *owner = mTransition->mParent;
    if (!owner)
        owner = mTransition->GetOwner();
    if (owner) {
        MfState *state;
        if (owner->mType == 0x65) {
            state = (MfState *)owner->mParent;
            if (state)
                mTarget = state->FindChoicePoint(mChoicePoint);
        } else if (owner->mType == 0x6f) {
            state = (MfState *)owner->mParent;
            if (state)
                mTarget = state->FindChoicePoint(mChoicePoint);
        } else if (owner->mType == 0x6d) {
            state = (MfState *)owner->mParent->mParent;
            if (state)
                mTarget = state->FindChoicePoint(mChoicePoint);
        } else if (owner->mType == 0x6e) {
            state = (MfState *)owner->mParent;
            if (state)
                mTarget = state->FindChoicePoint(mChoicePoint);
        }
    }
    if (mTarget)
        mTargetPath = mTarget->GetPath();
}

// 0x100898d0 ?Write@MfTransChoicePt@@UAEXAAVostream@@AAHH@Z
void MfTransChoicePt::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    os << tabs;
    os << "dest_choice_pt " << mState << " " << mChoicePoint << endl;
}
