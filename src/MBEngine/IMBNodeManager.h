// IMBNodeManager.h - the stack of game nodes.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBComPtr, MBGUID, MBIID_IMBNodeManager.
#ifndef MBENGINE_IMBNODEMANAGER_H
#define MBENGINE_IMBNODEMANAGER_H

class IMBNode;

class IMBNodeManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBNodeManager; }
    virtual IMBNode *GetActiveNode(void) const = 0;
    virtual void QueuePush(MBComPtr<IMBNode>) = 0;
    virtual void QueuePop(void) = 0;
    virtual void QueueSwap(MBComPtr<IMBNode>) = 0;
    virtual void QueueClear(void) = 0;
};

#endif
