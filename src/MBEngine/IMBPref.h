// IMBPref.h - preference (ini-style) strings and numbers.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBPref, MBString, MBString16.
#ifndef MBENGINE_IMBPREF_H
#define MBENGINE_IMBPREF_H

class IMBStream;

class IMBPref : public IMBUnknown {
public:
    enum IMBPrefHints {
        kCacheAll = 1,
        kCacheSection = 2,
        kCacheNothing = 4,
        kSaveOneSet = 8,
        kSaveOnExit = 10
    };
    static const MBGUID &GetGUID(void) { return MBIID_IMBPref; }
    // VC6 lays overloaded virtuals out in reverse declaration order
    virtual long GetString(IMBStream *, const MBString &, const MBString &, MBString16 &, const MBString16 &) = 0;
    virtual long GetString(IMBStream *, const MBString &, const MBString &, MBString &, const MBString &) = 0;
    virtual long GetLong(IMBStream *, const MBString &, const MBString &, long) = 0;
    virtual long SetString(IMBStream *, const MBString &, const MBString &, MBString16 &) = 0;
    virtual long SetString(IMBStream *, const MBString &, const MBString &, MBString &) = 0;
    virtual long SetLong(IMBStream *, const MBString &, const MBString &, long) = 0;
};

#endif
