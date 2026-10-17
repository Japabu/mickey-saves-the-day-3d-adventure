#pragma once
// MfKeyListNode: a node of an MfKeyList (a keyframe and its neighbours).
class MfKeyframe;

struct MfKeyListNode {
    MfKeyframe *m_keyframe;     // +0
    MfKeyListNode *m_prev;      // +4
    MfKeyListNode *m_next;      // +8
};
