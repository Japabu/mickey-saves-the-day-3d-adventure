// CMBTextControl.h - declarations for MBEngine.dll CMBTextControl.obj
// Colour header copied from CMBDirectDrawSurface.h (default-argument MBColor ctor:
// `mColor = 0; Set(...)`, the shape the CMBTextControl ctor shows); IMBFont from CMBFont.h.
#ifndef CMBTEXTCONTROL_H
#define CMBTEXTCONTROL_H

#include <string>

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { MBfree_release(p); }

template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;
typedef std::basic_string<unsigned short, std::char_traits<unsigned short>, MBAllocator<unsigned short> > MBString16;

// mbi18n.h
extern "C" {
void MB_utf8_to_ucs2(const MBString &utf8string, MBString16 &unicode);
void MB_ucs2_to_utf8(const MBString16 &unicode, MBString &utf8);
}
// $E numbering: MBString16 is instantiated (npos) ahead of the colour statics, which
// start at $E15 in the original (one above an uninstantiated MBString16)
inline MBString16 MB_utf8_to_ucs2(const MBString &utf8string)
{
    MBString16 unicode;
    MB_utf8_to_ucs2(utf8string, unicode);
    return unicode;
}

#include "../MBCom/MBGUID.h"

// PDB ldata order 0x1008b360..0x1008b430
#include "../MBCom/IMBUnknown.h"
static const MBGUID MBIID_IMBClassFactory = { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBObject = { 0x7ed4fc70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObjectGroup = { 0x7ed99200, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurface = { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFont = { 0x7f4301f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBTextControl = { 0x7f50c2a0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBTextValidator = { 0x8ef5d910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource = { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBGameClock = { 0x04e3c980, 0x9ea1, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodical = { 0x79529260, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodicalManager = { 0x7958af20, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventManager = { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventListener = { 0x799755d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };


extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

#include "MBMinMax.h"

#include "MBPointStruct3.h"

#include "MBPoint3.h"

#include "MBSize.h"

#include "../MBEngine/MBRect.h"

// ---- colors (header statics: every including TU has its own $E initializers) ----
#include "MBColorMask.h"

static const MBColorMask kCM_RGBA32(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
static const MBColorMask kCM_ARGB32(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
static const MBColorMask kCM_ARGB16(0x7c00, 0x03e0, 0x001f, 0x8000);
static const MBColorMask kCM_RGB16(0xf800, 0x07e0, 0x001f, 0);
static const MBColorMask kCM_BGR16(0x001f, 0x07e0, 0xf800, 0);

// own copy: this unit uses the constructor, Set or Convert, whose inline body here differs from MBColor.h (store order / code shape the matches depend on)
class MBColor {
public:
    MBColor(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 0) { mColor = 0; Set(r, g, b, a); }
    void Set(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        mRed = r;
        mBlue = b;
        mGreen = g;
        mAlpha = a;
        Convert(kCM_ARGB32);
    }
    unsigned long Convert(MBColorMask mask) {
        mColor = 0;
        if (mask.mRedShift > 0)
            mColor |= (mRed << mask.mRedShift) & mask.mRedMask;
        else
            mColor |= (mRed >> (char)-mask.mRedShift) & mask.mRedMask;
        if (mask.mGreenShift > 0)
            mColor |= (mGreen << mask.mGreenShift) & mask.mGreenMask;
        else
            mColor |= (mGreen >> (char)-mask.mGreenShift) & mask.mGreenMask;
        if (mask.mBlueShift > 0)
            mColor |= (mBlue << mask.mBlueShift) & mask.mBlueMask;
        else
            mColor |= (mBlue >> (char)-mask.mBlueShift) & mask.mBlueMask;
        if (mask.mAlphaShift > 0)
            mColor |= (mAlpha << mask.mAlphaShift) & mask.mAlphaMask;
        else
            mColor |= (mAlpha >> (char)-mask.mAlphaShift) & mask.mAlphaMask;
        return mColor;
    }
    unsigned char mRed;
    unsigned char mGreen;
    unsigned char mBlue;
    unsigned char mAlpha;
    unsigned long mColor;
};

static const MBColor kWhite(255, 255, 255, 0);
static const MBColor kBlack(0, 0, 0, 0);
static const MBColor kRed(255, 0, 0, 0);
static const MBColor kGreen(0, 255, 0, 0);
static const MBColor kBlue(0, 0, 255, 0);
static const MBColor kYellow(255, 255, 0, 0);
static const MBColor kCyan(0, 255, 255, 0);
static const MBColor kMagenta(255, 0, 255, 0);
static const MBColor kGrey25(64, 64, 64, 0);
static const MBColor kGrey50(128, 128, 128, 0);
static const MBColor kGrey75(192, 192, 192, 0);

class MBPalette;
#include "MBPixelFormat.h"

#include "IMBSurface.h"

#include "IMBFont.h"

class IMBObjectGroup;

#include "IMBObject.h"

#include "IMBObjectGroup.h"

#include "MBTextControlColorFlag.h"

#include "MBTextControlTypeFlag.h"

#include "MBTextControlJustifyFlag.h"

enum TVState {
    kTVValid = 0,
    kTVInvalid = 1,
    kTVAcceptable = 2
};

class IMBTextValidator : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBTextValidator; }
    virtual TVState Validate(MBString16 &) = 0;
};

#include "IMBTextControl.h"

#include "IMBEventListener.h"

#include "IMBEventManager.h"

#include "IMBPeriodical.h"

#include "IMBPeriodicalManager.h"

#include "IMBGameClock.h"

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    ~MBComPtr(void) { if (mInterface) mInterface->Release(); mInterface = 0; }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
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
            long hr = _QueryInterface(unk);
            unk->Release();
            return hr;
        }
        return -2;
    }
private:
    long _QueryInterface(IMBUnknown *const &p) {
        T *pInterface;
        long hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
        if (hr)
            Attach(0);
        else
            Attach(pInterface);
        return hr;
    }
    T *mInterface;
};

class CMBTextControl : public IMBTextControl, public IMBEventListener, public IMBPeriodical {
protected:
    ~CMBTextControl(void);
public:
    CMBTextControl(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void Idle(void);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void GetText(MBString16 &text);
    virtual void GetText(MBString &text);
    virtual void SetText(const MBString16 &text);
    virtual void SetText(const MBString &text);
    virtual void SetFont(IMBFont *font);
    virtual void GetFont(IMBFont **font);
    virtual void SetSize(long width, long height);
    virtual MBSize GetSize(void) const;
    virtual void SetCharacterLimit(unsigned long limit);
    virtual void SetColor(MBTextControlColorFlag colorflag, const MBColor &color);
    virtual void SetColorTransparent(MBTextControlColorFlag colorflag);
    virtual void SetType(MBTextControlTypeFlag type);
    virtual void SetJustification(MBTextControlJustifyFlag justify);
    virtual void SetValidator(IMBTextValidator *validator);
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
    virtual void AddCharacter(unsigned short character);
    IMBObjectGroup *mParent;                    // +0x10
    MBPoint3 mPos;                              // +0x14
    long mHideCount;                            // +0x20
    bool mDirty;                                // +0x24
    MBComPtr<IMBFont> mFont;                    // +0x28
    MBString16 mUniString;                      // +0x2c
    MBSize mSize;                               // +0x3c
    unsigned long mColorFlags;                  // +0x44
    MBColor mBackgroundColor;                   // +0x48
    MBColor mBorderColor;                       // +0x50
    MBColor mActiveBorderColor;                 // +0x58
    unsigned long mCharLimit;                   // +0x60
    unsigned long mCursorBlinkRate;             // +0x64
    bool mCursorBlinkOn;                        // +0x68
    MBTextControlTypeFlag mTextControlType;     // +0x6c
    MBTextControlJustifyFlag mJustification;    // +0x70
    MBComPtr<IMBTextValidator> mValidatorPtr;   // +0x74
    unsigned long mLastTime;                    // +0x78
};

IMBUnknown *MakeCMBTextControl(void);

#endif
