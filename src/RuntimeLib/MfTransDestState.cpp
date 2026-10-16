// MfTransDestState (RuntimeLib.dll 10089990-10089d00).
#include <iostream.h>
#include "MfTransitionProperty.h"
#include "MfState.h"

// 0x10089990 ??0MfTransDestState@@QAE@VMfString@@@Z
MfTransDestState::MfTransDestState(MfString state)
    : mTarget(0), mState(state)
{
    mType = TYPE;
}

// Inline members this object emits:
// 0x10089a20 ?GetName@MfTransDestState@@UAEAAVMfString@@XZ

// 0x10089a50 ??1MfTransDestState@@UAE@XZ
MfTransDestState::~MfTransDestState()
{
}

// 0x10089ac0 ?Clone@MfTransDestState@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfTransDestState::Clone()
{
    MfTransDestState *copy = new MfTransDestState(mState);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x10089b50 ?FormatForExecution@MfTransDestState@@UAEXPAVMfFiniteStateMachine@@@Z
void MfTransDestState::FormatForExecution(MfFiniteStateMachine *machine)
{
    mMachine = machine;
    mTarget = 0;
    if (!mTransition)
        return;
    MfStateProperty *owner = mTransition->mParent;
    if (!owner)
        owner = mTransition->GetOwner();
    if (mTransition->mInitial) {
        if (owner && owner->mType == 0x65)
            mTarget = ((MfState *)owner)->FindState(mState);
    } else if (owner) {
        MfState *state;
        if (owner->mType == 0x65) {
            state = (MfState *)owner->mParent;
            if (state)
                mTarget = state->FindState(mState);
        } else if (owner->mType == 0x6f) {
            state = (MfState *)owner->mParent;
            if (state)
                mTarget = state->FindState(mState);
        } else if (owner->mType == 0x6d) {
            MfStateProperty *parent = owner->mParent;
            state = 0;
            if (parent)
                state = (MfState *)parent->mParent;
            if (state)
                mTarget = state->FindState(mState);
        } else if (owner->mType == 0x6e) {
            state = (MfState *)owner->mParent;
            mTarget = state->FindState(mState);
        }
    }
    if (mTarget)
        mTargetPath = mTarget->GetPath();
}

// 0x10089c50 ?Write@MfTransDestState@@UAEXAAVostream@@AAHH@Z
void MfTransDestState::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    os << tabs;
    os << "dest_state " << mState << endl;
}
