// MfTransExitBy (RuntimeLib.dll 1008a110-1008a430).
#include <iostream.h>
#include "MfTransitionProperty.h"
#include "MfState.h"

// 0x1008a110 ??0MfTransExitBy@@QAE@VMfString@@@Z
MfTransExitBy::MfTransExitBy(MfString port)
    : mPort(port), mTarget(0)
{
    mType = TYPE;
}

// 0x1008a1c0 ??1MfTransExitBy@@UAE@XZ
MfTransExitBy::~MfTransExitBy()
{
}

// 0x1008a230 ?Clone@MfTransExitBy@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfTransExitBy::Clone()
{
    MfTransExitBy *copy = new MfTransExitBy(mPort);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x1008a2c0 ?FormatForExecution@MfTransExitBy@@UAEXPAVMfFiniteStateMachine@@@Z
void MfTransExitBy::FormatForExecution(MfFiniteStateMachine *machine)
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
                mTarget = state->FindExitPort(mPort);
        } else if (owner->mType == 0x6f) {
            state = (MfState *)owner->mParent;
            if (state)
                mTarget = state->FindExitPort(mPort);
        } else if (owner->mType == 0x6d) {
            MfStateProperty *parent = owner->mParent;
            if (parent) {
                state = (MfState *)parent->mParent;
                if (state)
                    mTarget = state->FindExitPort(mPort);
            }
        } else if (owner->mType == 0x6e) {
            mTarget = 0;
        }
    }
    if (mTarget)
        mTargetPath = mTarget->GetPath();
}

// 0x1008a380 ?Write@MfTransExitBy@@UAEXAAVostream@@AAHH@Z
void MfTransExitBy::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    os << tabs;
    os << "exitby " << mPort << endl;
}
