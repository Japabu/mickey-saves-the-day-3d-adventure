// CMBWinDraw.h - declarations for MBEngine.dll CMBWinDraw.obj
#ifndef CMBWINDRAW_H
#define CMBWINDRAW_H

#include "CMBDirect3DScreen.h" // colour header statics ($E1..$E47), MBRect, MBColor, IMBUnknown

static const MBGUID MBIID_IMBWinDC = {0x7fb72430, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBDraw = {0x7fbd40f0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};

#include "IMBWinDC.h"

class IMBDraw : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBDraw; }
    virtual long BeginDrawing(IMBUnknown *) = 0;
    virtual long EndDrawing(void) = 0;
    virtual long SetPenInfo(unsigned long, const MBColor &) = 0;
    virtual long SetPenMode(unsigned long) = 0;
    virtual long SetFillColor(const MBColor &) = 0;
    virtual long FrameRect(const MBRect &) = 0;
    virtual long FillRect(const MBRect &) = 0;
    virtual long FrameEllipse(const MBRect &) = 0;
    virtual long FillEllipse(const MBRect &) = 0;
};

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    ~MBComPtr(void) { Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    MBComPtr &operator=(T *p) {
        if (mInterface != p) {
            T *old = mInterface;
            mInterface = p;
            AddRef();
            if (old) old->Release();
        }
        return *this;
    }
    template <class U> MBComPtr &operator=(U *const &p) {
        _QueryInterface(p);
        return *this;
    }
    void Attach(T *p) {
        Release();
        mInterface = p;
    }
    void Release(void) {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
    }
    void AddRef(void) {
        if (mInterface) mInterface->AddRef();
    }
private:
    template <class U> long _QueryInterface(U *const &p) {
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

class CMBWinDraw : public IMBDraw {
public:
    CMBWinDraw(void);
    ~CMBWinDraw(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual long BeginDrawing(IMBUnknown *target);
    virtual long EndDrawing(void);
    virtual long SetPenInfo(unsigned long penSize, const MBColor &penColor);
    virtual long SetPenMode(unsigned long penMode);
    virtual long SetFillColor(const MBColor &fillColor);
    virtual long FrameRect(const MBRect &rect);
    virtual long FillRect(const MBRect &rect);
    virtual long FrameEllipse(const MBRect &boundingRect);
    virtual long FillEllipse(const MBRect &boundingRect);
    void CreateBrush(const MBColor &color);
    void ReleaseBrush(void);
    HBRUSH__ *GetBrush(void);
    void CreatePen(unsigned long penSize, const MBColor &color);
    void ReleasePen(void);
    HPEN__ *GetPen(void);
protected:
    MBComPtr<IMBWinDC> mWinDCPtr;
    HDC__ *m_hDC;
    HBRUSH__ *m_hBrush;
    HBRUSH__ *m_hOldBrush;
    HPEN__ *m_hPen;
    HPEN__ *m_hOldPen;
};

IMBUnknown *MakeCMBWinDraw(void);

#endif
