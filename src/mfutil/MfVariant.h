// MfVariant: a value that holds an int, unsigned int, float, double or MfString.
// The value lives in a heap MfVariantData whose concrete classes (MfVdInt, ...,
// declared in MfVariant.cpp) convert to the other types. Reconstructed (no PDB):
// virtual order from the vtables, names from the exports and RTTI.
// MfString.h reads this after the MfString declaration (as the original did); the
// include below, outside the guard, makes it work when it is included first.
#include "MfString.h"
#ifndef MFUTIL_MFVARIANT_H
#define MFUTIL_MFVARIANT_H

#ifndef MFUTIL_API
#define MFUTIL_API __declspec(dllimport)
#endif
#include "MfVariantData.h"

// vtable 0x10017a5c, size 8
class MFUTIL_API MfVariant {
public:
    MfVariant();
    MfVariant(int i);
    MfVariant(unsigned int u);
    MfVariant(float f);
    MfVariant(double d);
    MfVariant(const MfString &s);
    MfVariant(const MfVariant &v);
    virtual ~MfVariant();

    MfVariant &operator=(int i);
    MfVariant &operator=(unsigned int u);
    MfVariant &operator=(float f);
    MfVariant &operator=(double d);
    MfVariant &operator=(const MfString &s);
    MfVariant &operator=(const MfVariant &v);

    operator int() const;
    operator unsigned int() const;
    operator float() const;
    operator double() const;
    MfString ToString() const;

    int operator==(const MfVariant &v) const;
    int operator!=(const MfVariant &v) const { return !(*this == v); }
    int IsLike(const MfVariant &v) const;
    int Type() const { return m_data->Type(); }
    int IsNumber() const
    {
        int t = Type();
        return t == variant_namespace::MFVT_INT || t == variant_namespace::MFVT_UINT ||
               t == variant_namespace::MFVT_FLOAT || t == variant_namespace::MFVT_DOUBLE;
    }
    int IsInt() const { int t = Type(); return t == variant_namespace::MFVT_INT || t == variant_namespace::MFVT_UINT; }
    int IsFloat() const { int t = Type(); return t == variant_namespace::MFVT_FLOAT || t == variant_namespace::MFVT_DOUBLE; }
    int IsString() const { return Type() == variant_namespace::MFVT_STRING; }

private:
    variant_namespace::MfVariantData *m_data;   // +4
};

#endif
