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

// PDB ldata order in CMBGameEngine.obj (0x10089a88..0x10089b98); MBIID_IMBUnknown comes first, from IMBUnknown.h via CMBView.h
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

#include "../MBEngine/MBComPtr.h"

#include "IMBPeriodical.h"

#include "IMBPeriodicalManager.h"

#include "IMBNode.h"

#include "IMBNodeManager.h"

class IMBEventListener;

#include "IMBEventManager.h"

#include "IMBViewManager.h"

#include "MBScreenAttributes.h"

#include "IMBScreenManager.h"

// own copy: a GetGUID-only view: this unit declares no MBString, MBString16 or MBLocale
class IMBStringTable : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBStringTable; }
    // Load/SetLocale/GetString/GetNamedString: not called here
};

#include "IMBGameEngine.h"

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
