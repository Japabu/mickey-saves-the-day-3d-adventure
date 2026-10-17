// CMBObject.h - declarations for MBEngine.dll CMBObject.obj
#ifndef CMBOBJECT_H
#define CMBOBJECT_H

#include "../MBCom/MBGUID.h"

#include "MBPointStruct3.h"

#include "MBPoint3.h"

class MBRect;
class IMBSurface;
class IMBObjectGroup;

#include "../MBCom/IMBUnknown.h"

static const MBGUID MBIID_IMBObject = { 0x7ed4fc70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
#include "IMBObject.h"

static const MBGUID MBIID_IMBObjectGroup = { 0x7ed99200, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
#include "IMBObjectGroup.h"

class CMBObject : public IMBObject {
public:
    CMBObject(void);
    ~CMBObject(void);
    virtual MBPoint3 GetOffset(void) const;
    virtual MBPoint3 GetPosition(void) const;
    virtual void Place(const MBPoint3 &point);
    virtual void Move(const MBPoint3 &point);
    virtual MBRect GetBounds(void) const = 0;
    virtual bool IsVisible(void) const;
    virtual long Show(void);
    virtual long Hide(void);
    virtual bool IsDirty(void) const;
    virtual void SetDirty(void);
    virtual void ClearDirty(void);
    virtual long Draw(IMBSurface *, MBRect &, MBRect &) const = 0;
    virtual void GetParent(IMBObjectGroup **parent) const;
    virtual IMBObjectGroup *SetParent(IMBObjectGroup *newParent);
private:
    IMBObjectGroup *mParent;   // +0x04
    MBPoint3 mPosition;        // +0x08
    long mHideCount;           // +0x14
    bool mDirty;               // +0x18
};

#endif
