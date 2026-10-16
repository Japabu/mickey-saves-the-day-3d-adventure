// MfVariant: a value that holds an int, unsigned int, float, double or MfString.
// The value lives in a heap MfVariantData whose concrete classes (MfVdInt, ...,
// declared in MfVariant.cpp) convert to the other types. Reconstructed (no PDB):
// virtual order from the vtables, names from the exports and RTTI.
// Like the original MfString.h, this is read after the MfString declaration.
#ifndef MFUTIL_MFVARIANT_H
#define MFUTIL_MFVARIANT_H

namespace variant_namespace {

// Type ids (m_type).
enum { MFVT_NONE = 0, MFVT_INT = 1, MFVT_UINT = 2, MFVT_FLOAT = 3, MFVT_DOUBLE = 4, MFVT_STRING = 5 };

// vtable 0x10017794
class MFAPI MfVariantData {
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

// vtable 0x10017a5c, size 8
class MFAPI MfVariant {
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
