// MfVariantData: the heap value behind an MfVariant (mfutil.dll). The concrete
// classes (MfVdInt, ..., declared in MfVariant.cpp) convert to the other types.
// Reconstructed (no PDB): virtual order from the vtables, names from the exports
// and RTTI. The inline members' copies are kept by MfStringRef.cpp.
// MfString.h reads this (through MfVariant.h) after the MfString declaration; the
// include below, outside the guard, makes it work when it is included first.
#include "MfString.h"
#ifndef MFUTIL_MFVARIANTDATA_H
#define MFUTIL_MFVARIANTDATA_H

#ifndef MFUTIL_API
#define MFUTIL_API __declspec(dllimport)
#endif

namespace variant_namespace {

// Type ids (m_type).
enum { MFVT_NONE = 0, MFVT_INT = 1, MFVT_UINT = 2, MFVT_FLOAT = 3, MFVT_DOUBLE = 4, MFVT_STRING = 5 };

// vtable 0x10017794
class MFUTIL_API MfVariantData {
public:
    MfVariantData(int type) : m_type(type) {}
    MfVariantData(const MfVariantData &o);
    virtual ~MfVariantData() {}
    virtual MfVariantData *MakeCopy() const { return new MfVariantData(*this); }
    // Assigning a value of another type makes a new data object of that type
    // (the caller deletes the old one); a matching type assigns in place.
    virtual MfVariantData *Assign(int i);
    virtual MfVariantData *Assign(unsigned int u);
    virtual MfVariantData *Assign(float f);
    virtual MfVariantData *Assign(double d);
    virtual MfVariantData *Assign(const MfString &s);
    virtual operator int() const { return 0; }
    virtual operator unsigned int() const { return 0; }
    virtual operator float() const { return 0.0f; }
    virtual operator double() const { return 0.0; }
    virtual MfString ToString() const { return MfString(""); }
    virtual int Compare(const MfVariantData *o) const { return 0; }
    int Type() const { return m_type; }
    MfVariantData &operator=(const MfVariantData &o) { m_type = o.m_type; return *this; }

    int m_type;     // +4
};

}

#endif
