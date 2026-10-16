#pragma once
// muffin/include/Runtime/MBColor.h (layout from SteamboatApp.pdb; the bodies are inline, as in the
// engine's copy).

class MBColorMask {
public:
    MBColorMask(unsigned long r, unsigned long g, unsigned long b, unsigned long a) { Set(r, g, b, a); }
    void Set(unsigned long redmask, unsigned long greenmask, unsigned long bluemask, unsigned long alphamask)
    {
        mRedMask = redmask;
        mBlueMask = bluemask;
        mGreenMask = greenmask;
        mAlphaMask = alphamask;
        mRedShift = GetShiftCount(redmask) + GetBitCount(redmask) - 8;
        mBlueShift = GetShiftCount(bluemask) + GetBitCount(bluemask) - 8;
        mGreenShift = GetShiftCount(greenmask) + GetBitCount(greenmask) - 8;
        mAlphaShift = GetShiftCount(alphamask) + GetBitCount(alphamask) - 8;
    }
    unsigned char GetRedBitCount() const;
    unsigned char GetBlueBitCount() const;
    unsigned char GetGreenBitCount() const;
    unsigned char GetAlphaBitCount() const;
    unsigned long mRedMask;
    unsigned long mBlueMask;
    unsigned long mGreenMask;
    unsigned long mAlphaMask;
    char mRedShift;
    char mBlueShift;
    char mGreenShift;
    char mAlphaShift;
    static unsigned char __cdecl GetShiftCount(unsigned long mask)
    {
        unsigned char count = 0;
        while (!(mask & 1)) {
            if (count >= 32)
                break;
            mask >>= 1;
            count++;
        }
        return count;
    }
    static unsigned char __cdecl GetBitCount(unsigned long mask)
    {
        unsigned char count = 0;
        while (mask) {
            mask &= mask - 1;
            count++;
        }
        return count;
    }
};

// The colour constants belong to the precompiled header (Steamboat_pch.obj initializes them all);
// a unit re-emits only the ones it uses, so the units define those themselves (SteamboatNode.cpp:
// kCM_ARGB32 and kYellow). MBColor's constructor converts through kCM_ARGB32.
extern const MBColorMask kCM_ARGB32;

class MBColor {
public:
    MBColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) { Set(r, g, b, a); }
    void Set(unsigned long, MBColorMask);
    void Set(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        mRed = r;
        mBlue = b;
        mGreen = g;
        mAlpha = a;
        mColor = Convert(kCM_ARGB32);
    }
    bool operator==(MBColor &c) { return mRed == c.mRed && mGreen == c.mGreen && mBlue == c.mBlue && mAlpha == c.mAlpha; }
    bool operator!=(MBColor &c) { return mRed != c.mRed || mGreen != c.mGreen || mBlue != c.mBlue || mAlpha != c.mAlpha; }
    unsigned char GetRed() const { return mRed; }
    unsigned char GetBlue() const { return mBlue; }
    unsigned char GetGreen() const { return mGreen; }
    unsigned char GetAlpha() const { return mAlpha; }
    void SetRed(unsigned char red) { mRed = red; }
    void SetBlue(unsigned char blue) { mBlue = blue; }
    void SetGreen(unsigned char green) { mGreen = green; }
    void SetAlpha(unsigned char alpha) { mAlpha = alpha; }
    unsigned long Convert(const MBColorMask &mask)
    {
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
