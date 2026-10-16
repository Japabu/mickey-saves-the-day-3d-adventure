// MfActorLib.dll 0x1000ad80-0x1000b050: MfJointKeyList, preceded by the inline
// members of MfKeyList.h this object is the first to emit.
#include "MfJointKeyList.h"
#include "MfJointKeyframe.h"

// 0x1000ad80 ??1MfKeyList@@UAE@XZ (inline, MfKeyList.h; folded with ~MfJointKeyList)
// 0x1000ad90 ??4MfKeyList@@QAEAAV0@ABV0@@Z (compiler-generated)
// 0x1000adc0 ??_EMfKeyList@@UAEPAXI@Z (compiler-generated)

// 0x1000ae20 ??0MfJointKeyList@@QAE@XZ (inline, header)
// 0x1000ae40 ??0MfJointKeyList@@QAE@ABV0@@Z (inline, header)

// 0x1000ae60 ?GetFirst@MfJointKeyList@@QAEPAVMfJointKeyframe@@AAPAUMfKeyListNode@@@Z (inline, header)
// 0x1000ae70 ?GetNext@MfJointKeyList@@QAEPAVMfJointKeyframe@@AAPAUMfKeyListNode@@@Z (inline, header)
// 0x1000ae80 ??_EMfJointKeyList@@UAEPAXI@Z (compiler-generated)

// 0x1000aed0 ?Insert@MfJointKeyList@@QAEPAUMfKeyListNode@@PAVMfJointKeyframe@@H@Z
MfKeyListNode *MfJointKeyList::Insert(MfJointKeyframe *key, int computeBlends)
{
    MfKeyListNode *node = MfKeyList::Insert(key);
    if (computeBlends) {
        key->ComputeBlends(node->m_prev ? (MfJointKeyframe *)node->m_prev->m_keyframe : 0,
                           node->m_next ? (MfJointKeyframe *)node->m_next->m_keyframe : 0, 0);
        if (node->m_prev)
            ((MfJointKeyframe *)node->m_prev->m_keyframe)->ComputeBlends(
                node->m_prev->m_prev ? (MfJointKeyframe *)node->m_prev->m_prev->m_keyframe : 0, key, 0);
        if (node->m_next)
            ((MfJointKeyframe *)node->m_next->m_keyframe)->ComputeBlends(
                key, node->m_next->m_next ? (MfJointKeyframe *)node->m_next->m_next->m_keyframe : 0, 0);
    }
    return node;
}

// EQUIV 0x1000af60 ?SampleAtTime@MfJointKeyList@@QAEHKAAVMfJointKeyframe@@H@Z: reviewed: register choice in the interpolation tail; same Interpolate call and arguments
int MfJointKeyList::SampleAtTime(unsigned long time, MfJointKeyframe &key, int wrap)
{
    if (!mHead || time < mHead->m_keyframe->m_time)
        return 0;
    if (!wrap && time > mTail->m_keyframe->m_time)
        return 0;
    if (!mCurrent || mCurrent->m_keyframe->m_time > time)
        mCurrent = mHead;
    while (mCurrent->m_next && mCurrent->m_next->m_keyframe->m_time <= time)
        mCurrent = mCurrent->m_next;
    MfKeyListNode *node = mCurrent;
    if (node->m_next) {
        MfJointKeyframe *prev = (MfJointKeyframe *)node->m_keyframe;
        unsigned long start = prev->m_time;
        if (start != time) {
            MfJointKeyframe *next = (MfJointKeyframe *)node->m_next->m_keyframe;
            key.Interpolate(prev, next, (float)(time - start) / (float)(next->m_time - start));
            return 1;
        }
    }
    key.Interpolate((MfJointKeyframe *)node->m_keyframe, 0, 0.0f);
    return 1;
}
