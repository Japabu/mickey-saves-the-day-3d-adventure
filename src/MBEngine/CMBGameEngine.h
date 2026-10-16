// CMBGameEngine.h - declarations for MBEngine.dll CMBGameEngine.obj
#ifndef CMBGAMEENGINE_H
#define CMBGAMEENGINE_H

#include <string> // evidence: ctype<unsigned short> id $E70/$E71; the colour $Es start after it ($E16)

// Stand-ins (see NOTES): the PDB numbering has the colour statics at $E16..$E62 and the
// ctype pair at $E70/$E71; <string> first gives $E14 and $E66, so two numbered items
// precede the colour header and two follow it. No evidence names them (the obj's PDB
// data has no extra statics/guards); declared-only static data members stand in.
struct MBGameEngineStandIn1 {
    static int sUnknown1;
    static int sUnknown2;
};

#include "CMBView.h" // MBGUID, colour statics, MBRect, IMBUnknown, IMBScreen

struct MBGameEngineStandIn2 {
    static int sUnknown3;
    static int sUnknown4;
};

// PDB ldata order in CMBGameEngine.obj (0x10089a88..0x10089b98)
static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBPeriodical =
    { 0x79529260, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodicalManager =
    { 0x7958af20, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBNode =
    { 0x795ecbe0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBNodeManager =
    { 0x7984ff90, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBNodeManager =
    { 0x798b1c50, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventManager =
    { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBViewManager =
    { 0x79a9ac10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreenManager =
    { 0x79df27a0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSoundManager =
    { 0x79ee6f80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBGameEngine =
    { 0x7a1c4720, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreen =
    { 0x79d783b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurface =
    { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSoundDescription =
    { 0x84481ea0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource =
    { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStringTable =
    { 0x8f1468d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBStringTable =
    { 0x8f1a8590, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) {
        IMBUnknown *unk;
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            _QueryInterface(unk);
            unk->Release();
        }
    }
    template <class U> MBComPtr(const MBComPtr<U> &p) : mInterface(0) { _QueryInterface(p); }
    MBComPtr(const MBComPtr &p) : mInterface(p.mInterface) { if (mInterface) mInterface->AddRef(); }
    ~MBComPtr(void) { Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    operator bool(void) const { return mInterface != 0; }
    MBComPtr &operator=(T *p) {
        if (mInterface != p) {
            T *old = mInterface;
            mInterface = p;
            if (p) p->AddRef();
            if (old) old->Release();
        }
        return *this;
    }
    void Release(void) {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
    }
    void Attach(T *p) {
        Release();
        mInterface = p;
    }
    long CreateObject(const MBGUID &clsid) {
        IMBUnknown *unk;
        Release();
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            T *pInterface;
            long hr = unk->QueryInterface(T::GetGUID(), (void **)&pInterface);
            if (hr)
                Attach(0);
            else
                Attach(pInterface);
            unk->Release();
            return hr;
        }
        return -2;
    }
private:
    long _QueryInterface(IMBUnknown *const &p); // 0x100039a0 etc., emitted in another object
    template <class U> long _QueryInterface(const MBComPtr<U> &p) {
        long hr = -3;
        if (p) {
            T *pInterface;
            hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
            if (hr) {
                Attach(0);
                return hr;
            }
            Attach(pInterface);
            return hr;
        }
        *this = 0;
        return hr;
    }
    T *mInterface;
};

class IMBPeriodical : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBPeriodical; }
    virtual void Idle(void) = 0;
};

class IMBPeriodicalManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBPeriodicalManager; }
    virtual void AddPeriodical(IMBPeriodical *) = 0;
    virtual void RemovePeriodical(IMBPeriodical *) = 0;
    virtual void DevoteTimeToPeriodicals(void) = 0;
};

class IMBNode : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBNode; }
    virtual void OnStart(void) = 0;
    virtual void OnPause(void) = 0;
    virtual void OnResume(void) = 0;
    virtual void OnEnd(void) = 0;
    virtual void OnQuit(void) = 0;
};

class IMBNodeManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBNodeManager; }
    virtual IMBNode *GetActiveNode(void) const = 0;
    virtual void QueuePush(MBComPtr<IMBNode>) = 0;
    virtual void QueuePop(void) = 0;
    virtual void QueueSwap(MBComPtr<IMBNode>) = 0;
    virtual void QueueClear(void) = 0;
};

class IMBEventListener;

class IMBEventManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBEventManager; }
    virtual void BroadcastEvent(long, long, void *) = 0;
    virtual void AddListener(long, IMBEventListener *) = 0;
    virtual void RemoveListener(long, IMBEventListener *) = 0;
    virtual void RemoveListener(IMBEventListener *) = 0;
};

class IMBViewManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBViewManager; }
    virtual void AddView(IMBView *) = 0;
    virtual void RemoveView(IMBView *) = 0;
    virtual void FindViewContaingPoint(IMBScreen *, const MBPoint &, IMBView **) = 0;
};

struct MBScreenAttributes {
    unsigned long width;
    unsigned long height;
    unsigned long depth;
};

class IMBScreenManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBScreenManager; }
    virtual bool RequestScreen(long, MBScreenAttributes &) = 0;
    virtual void Activate(void) = 0;
    virtual void Deactivate(void) = 0;
    virtual MBPoint GetGlobalMouseLocation(void) = 0;
    virtual bool ProcessSystemEvent(void *) = 0;
    virtual void RemoveScreen(long) = 0;
    virtual void RemoveAllScreens(void) = 0;
    virtual unsigned short GetNumScreens(void) = 0;
    virtual void GetScreenByIndex(unsigned short, IMBScreen **) = 0;
    virtual void GetScreenByID(long, IMBScreen **) = 0;
    virtual void FindScreenContainingPoint(const MBPoint &, IMBScreen **) = 0;
};

class IMBStringTable : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBStringTable; }
    // Load/SetLocale/GetString/GetNamedString: not called here
};

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

class CMBGameEngine : public IMBGameEngine {
public:
    CMBGameEngine(void);
    ~CMBGameEngine(void);
protected:
    unsigned long _mRefCnt;                                      // +0x04
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void Init(void);
    virtual void ShutDown(void);
    virtual MBComPtr<IMBNodeManager> GetNodeMgr(void) const;
    virtual long CreateScreen(IMBScreenManager *screenManager);
    virtual void Start(void);
    virtual void Idle(void);
    virtual void UpdateScreen(const MBRect &bounds);
    virtual bool IsQuitting(void) const;
    virtual void Quit(void);
    virtual void SetOnEngineStartupFunction(void (*func)(IMBGameEngine *));
    virtual void SetOnEngineShutdownFunction(void (*func)(IMBGameEngine *));
    virtual void SetOnEngineCreateScreenFunction(long (*func)(IMBGameEngine *, IMBScreenManager *));
    virtual void GetStringTable(IMBStringTable **table);
protected:
    MBComPtr<IMBNodeManager> mNodeMgr;                           // +0x08
    MBComPtr<IMBStringTable> mStringTable;                       // +0x0c
    void (*mfOnMBEngineStartup)(IMBGameEngine *);                // +0x10
    void (*mfOnMBEngineShutdown)(IMBGameEngine *);               // +0x14
    long (*mOnMBEngineCreateScreen)(IMBGameEngine *, IMBScreenManager *); // +0x18
    bool mIsQuitting;                                            // +0x1c
};

IMBUnknown *MakeCMBGameEngine(void);

#endif
