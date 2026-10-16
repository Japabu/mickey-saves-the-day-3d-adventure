#pragma once
// MBLocale.h: a locale name (language/country/variant) and its display name (from SteamboatApp.pdb).
// The constructors are inline (SteamboatNode::Init builds a temporary from sLocale); the
// destructor is compiler-generated.
#include "MBContainers.h"

class MBLocale {
public:
    MBLocale(const MBLocale &);
    MBLocale() {}
    MBLocale(const MBString &lcv) : mLCV(lcv) {}
    MBLocale(const MBString &lcv, const MBString16 &displayName);
    bool operator==(const MBLocale &);
    void GetDisplayName(MBString16 &);
    void GetLCVParts(MBString &, MBString &, MBString &);
    void GetLCV(MBString &);
    void GetLC(MBString &);
    void GetL(MBString &);
    void GetLanguage(MBString &);
    void GetCountry(MBString &);
    void GetVariant(MBString &);
protected:
    MBString mLCV;                                      // +0x0000 size 0x10
    MBString16 mDisplayName;                            // +0x0010 size 0x10
};
