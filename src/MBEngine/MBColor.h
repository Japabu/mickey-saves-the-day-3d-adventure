// MBColor.h - an ARGB colour and its value in the screen format.
// Declared first by the includer (still one copy per unit in wave 1): kCM_ARGB32.
// Set converts with the includer's kCM_ARGB32 colour mask constant (still defined per unit).
#ifndef MBENGINE_MBCOLOR_H
#define MBENGINE_MBCOLOR_H

#include "MBColorMask.h"

class MBColor {
public:
    MBColor(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 0) { Set(r, g, b, a); }
    void Set(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        mRed = r;
        mBlue = b;
        mGreen = g;
        mAlpha = a;
        mColor = Convert(kCM_ARGB32);
    }
    void Set(unsigned long color, MBColorMask mask) {
        mColor = color;
        if (mask.mRedMask)
            mRed = (unsigned char)(((color & mask.mRedMask) * 255) / mask.mRedMask);
        if (mask.mGreenMask)
            mGreen = (unsigned char)(((color & mask.mGreenMask) * 255) / mask.mGreenMask);
        if (mask.mBlueMask)
            mBlue = (unsigned char)(((color & mask.mBlueMask) * 255) / mask.mBlueMask);
        if (mask.mAlphaMask)
            mAlpha = (unsigned char)(((color & mask.mAlphaMask) * 255) / mask.mAlphaMask);
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
    unsigned char GetRed(void) const { return mRed; }
    unsigned char GetBlue(void) const { return mBlue; }
    unsigned char GetGreen(void) const { return mGreen; }
    unsigned char GetAlpha(void) const { return mAlpha; }
    bool operator!=(const MBColor &c) const {
        return mRed != c.mRed || mGreen != c.mGreen || mBlue != c.mBlue || mAlpha != c.mAlpha;
    }
    unsigned char mRed;
    unsigned char mGreen;
    unsigned char mBlue;
    unsigned char mAlpha;
    unsigned long mColor;
};

#endif
