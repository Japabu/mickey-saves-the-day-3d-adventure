// CMBColorBackgroundObject.h - declarations for MBEngine.dll CMBColorBackgroundObject.obj
#ifndef CMBCOLORBACKGROUNDOBJECT_H
#define CMBCOLORBACKGROUNDOBJECT_H

#include "CMBSurfaceObject.h" // GUIDs, MBColor header constants, IMBObject

static const MBGUID MBIID_IMBColorBackgroundObject =
    { 0x7fc97a70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBColorBackgroundObject =
    { 0x7fcf9730, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBColorBackgroundObject : public IMBObject {
public:
    virtual void SetColor(const MBColor &) = 0;
    virtual void GetColor(MBColor &) = 0;
};

class CMBColorBackgroundObject : public IMBColorBackgroundObject {
public:
    CMBColorBackgroundObject(void);
protected:
    ~CMBColorBackgroundObject(void);
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void SetColor(const MBColor &color);
    virtual void GetColor(MBColor &color);
    virtual MBPoint3 GetOffset(void) const;
    virtual MBPoint3 GetPosition(void) const;
    virtual void Place(const MBPoint3 &point);
    virtual void Move(const MBPoint3 &point);
    virtual MBRect GetBounds(void) const;
    virtual bool IsVisible(void) const;
    virtual long Show(void);
    virtual long Hide(void);
    virtual bool IsDirty(void) const;
    virtual void SetDirty(void);
    virtual void ClearDirty(void);
    virtual long Draw(IMBSurface *dest, MBRect &worldRect, MBRect &destRect) const;
    virtual void GetParent(IMBObjectGroup **parent) const;
protected:
    virtual IMBObjectGroup *SetParent(IMBObjectGroup *newParent);
    IMBObjectGroup *mParent; // +0x08
    MBPoint3 mPos;           // +0x0c
    long mHideCount;         // +0x18
    bool mDirty;             // +0x1c
    MBColor mColor;          // +0x20
};

IMBUnknown *MakeCMBColorBackgroundObject(void);

#endif
