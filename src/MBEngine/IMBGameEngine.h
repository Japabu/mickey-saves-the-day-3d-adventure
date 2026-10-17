// IMBGameEngine.h - the engine: node manager, screen, string table, startup hooks.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBComPtr, MBGUID, MBIID_IMBGameEngine.
#ifndef MBENGINE_IMBGAMEENGINE_H
#define MBENGINE_IMBGAMEENGINE_H

class IMBNodeManager;
class IMBScreenManager;
class IMBStringTable;
class MBRect;

class IMBGameEngine : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBGameEngine; }
    virtual void Init(void) = 0;
    virtual void ShutDown(void) = 0;
    virtual MBComPtr<IMBNodeManager> GetNodeMgr(void) const = 0;
    virtual long CreateScreen(IMBScreenManager *) = 0;
    virtual void Start(void) = 0;
    virtual void Idle(void) = 0;
    virtual void UpdateScreen(const MBRect &) = 0;
    virtual bool IsQuitting(void) const = 0;
    virtual void Quit(void) = 0;
    virtual void SetOnEngineStartupFunction(void (*)(IMBGameEngine *)) = 0;
    virtual void SetOnEngineShutdownFunction(void (*)(IMBGameEngine *)) = 0;
    virtual void SetOnEngineCreateScreenFunction(long (*)(IMBGameEngine *, IMBScreenManager *)) = 0;
    virtual void GetStringTable(IMBStringTable **) = 0;
};

#endif
