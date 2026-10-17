// CMBWorld.h - declarations for MBEngine.dll CMBWorld.obj
#ifndef CMBWORLD_H
#define CMBWORLD_H

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { MBfree_release(p); }

#include "../MBCom/MBGUID.h"

// PDB ldata order in CMBWorld.obj: IMBUnknown, IMBClassFactory, IMBObject,
// IMBObjectGroup, CMBObjectGroup, IMBWorld (0x1008b718..0x1008b768)
#include "../MBCom/IMBUnknown.h"
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBObject =
    { 0x7ed4fc70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObjectGroup =
    { 0x7ed99200, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBObjectGroup =
    { 0x7ee135f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBWorld =
    { 0x7978c610, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

#include "MBSize.h"

#include "MBMinMax.h"

#include "../MBEngine/MBRect.h"

#include "MBPointStruct3.h"
#include "MBPoint3.h"

class IMBSurface;
class IMBObjectGroup;


#include "IMBObject.h"

#include "IMBObjectGroup.h"

#include "IMBWorld.h"

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

class CMBWorld : public IMBWorld {
public:
    CMBWorld(void);
    ~CMBWorld(void);
protected:
    unsigned long _mRefCnt;      // +0x04
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual MBRect GetBounds(void) const;
    virtual void SetBounds(const MBRect &bounds);
    virtual long Draw(IMBSurface *dest, MBRect &worldRect, MBRect &destRect);
    virtual IMBObjectGroup &GetRoot(void);
    virtual const IMBObjectGroup &GetConstRoot(void) const;
protected:
    MBRect mBounds;              // +0x08
    IMBObjectGroup *mRoot;       // +0x18
};

IMBUnknown *MakeCMBWorld(void);

#endif
