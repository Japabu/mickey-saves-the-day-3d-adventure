// MfTransEnterBy (RuntimeLib.dll 10089d00-1008a110).
#include <iostream.h>
#include "MfTransitionProperty.h"
#include "MfState.h"

// 0x10089d00 ??0MfTransEnterBy@@QAE@VMfString@@0@Z
MfTransEnterBy::MfTransEnterBy(MfString state, MfString port)
    : mState(state), mPort(port), mTarget(0)
{
    mType = TYPE;
}

// 0x10089dd0 ??1MfTransEnterBy@@UAE@XZ
MfTransEnterBy::~MfTransEnterBy()
{
}

// 0x10089e40 ?Clone@MfTransEnterBy@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfTransEnterBy::Clone()
{
    MfTransEnterBy *copy = new MfTransEnterBy(mState, mPort);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// TODO 0x10089ef0 ?FormatForExecution@MfTransEnterBy@@UAEXPAVMfFiniteStateMachine@@@Z: same calls; the original clears state and parent right after loading the type and has no zero register, ours keeps 0 in edi from the start
void MfTransEnterBy::FormatForExecution(MfFiniteStateMachine *machine)
{
    mMachine = machine;
    mTarget = 0;
    if (!mTransition)
        return;
    MfStateProperty *owner = mTransition->mParent;
    if (!owner)
        owner = mTransition->GetOwner();
    if (owner) {
        MfState *state = 0;
        MfState *parent = 0;
        if (owner->mType == 0x65) {
            parent = (MfState *)owner->mParent;
            if (parent)
                state = parent->FindState(mState);
            if (state)
                mTarget = state->FindEntryPort(mPort);
        } else if (owner->mType == 0x6f) {
            parent = (MfState *)owner->mParent;
            if (parent)
                state = parent->FindState(mState);
            if (state)
                mTarget = state->FindEntryPort(mPort);
        } else if (owner->mType == 0x6d) {
            MfStateProperty *p = owner->mParent;
            if (p)
                parent = (MfState *)p->mParent;
            if (parent)
                state = parent->FindState(mState);
            if (state)
                mTarget = state->FindEntryPort(mPort);
        } else if (owner->mType == 0x6e) {
            parent = (MfState *)owner->mParent;
            if (parent)
                state = parent->FindState(mState);
            if (state)
                mTarget = state->FindEntryPort(mPort);
        }
    }
    if (mTarget)
        mTargetPath = mTarget->GetPath();
}

// 0x1008a050 ?Write@MfTransEnterBy@@UAEXAAVostream@@AAHH@Z
void MfTransEnterBy::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    os << tabs;
    os << "enterby " << mState << " " << mPort << endl;
}
