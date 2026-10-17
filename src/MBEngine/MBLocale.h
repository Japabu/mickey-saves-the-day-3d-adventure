// MBLocale.h - a string table locale.
// Declared first by the includer (still one copy per unit in wave 1): MBString, MBString16.
#ifndef MBENGINE_MBLOCALE_H
#define MBENGINE_MBLOCALE_H

class MBLocale {
    friend class CMBStringTable;
public:
    MBLocale(void) {}
    MBLocale(const MBString &lcv) : mLCV(lcv) {}
    void GetLCV(MBString &lcv) { lcv = mLCV; }
protected:
    MBString mLCV;
    MBString16 mDisplayName;
};

#endif
