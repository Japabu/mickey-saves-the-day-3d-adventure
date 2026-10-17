// IMBView.h - a view of a world on a screen.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBView.
#ifndef MBENGINE_IMBVIEW_H
#define MBENGINE_IMBVIEW_H

class IMBScreen;
class IMBSurface;
class IMBWorld;
class MBPoint;
class MBRect;

class IMBView : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBView; }
    virtual bool IsDirty(void) = 0;
    virtual void Draw(IMBSurface *) = 0;
    virtual void SetWorldPosition(MBPoint) = 0;
    virtual MBPoint GetWorldPosition(void) = 0;
    virtual MBRect GetWorldBounds(void) = 0;
    virtual void SetScreenPosition(MBPoint) = 0;
    virtual MBPoint GetScreenPosition(void) = 0;
    virtual MBRect GetScreenBounds(void) = 0;
    virtual void SetSize(unsigned short, unsigned short) = 0;
    virtual unsigned short GetWidth(void) = 0;
    virtual unsigned short GetHeight(void) = 0;
    virtual void AttachToWorld(IMBWorld *) = 0;
    virtual void AttachToScreen(IMBScreen *) = 0;
    virtual bool IsAttachedToScreen(IMBScreen *) = 0;
    virtual void GetWorld(IMBWorld **) = 0;
    virtual MBPoint ScreenToWorldPoint(const MBPoint &) = 0;
};

#endif
