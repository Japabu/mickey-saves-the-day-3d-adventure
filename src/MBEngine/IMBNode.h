// IMBNode.h - a game node (screen of the game) run by the node manager.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBNode.
#ifndef MBENGINE_IMBNODE_H
#define MBENGINE_IMBNODE_H

class IMBNode : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBNode; }
    virtual void OnStart(void) = 0;
    virtual void OnPause(void) = 0;
    virtual void OnResume(void) = 0;
    virtual void OnEnd(void) = 0;
    virtual void OnQuit(void) = 0;
};

#endif
