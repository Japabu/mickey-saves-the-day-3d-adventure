// MfVariant (mfutil.dll 0x10010650-0x10011470) and its data classes.
#include "MfString.h"

namespace variant_namespace {

// 0x10010650 ??0MfVariantData@variant_namespace@@QAE@ABV01@@Z
MfVariantData::MfVariantData(const MfVariantData &o)
{
    m_type = o.m_type;
}

// The concrete data classes (RTTI names MfVdInt ... MfVdString), defined after
// MfVariantData's Assign functions.

// vtable 0x10017944
class MfVdInt : public MfVariantData {
public:
    MfVdInt(int i);
    MfVdInt(const MfVdInt &o);
    virtual MfVariantData *MakeCopy() const;
    virtual MfVariantData *Assign(int i);
    virtual operator int() const;
    virtual operator unsigned int() const;
    virtual operator float() const;
    virtual operator double() const;
    virtual MfString ToString() const;
    virtual int Compare(const MfVariantData *o) const;

    int m_value;    // +8
};

// vtable 0x1001797c
class MfVdUint : public MfVariantData {
public:
    MfVdUint(unsigned int u);
    MfVdUint(const MfVdUint &o);
    virtual MfVariantData *MakeCopy() const;
    virtual MfVariantData *Assign(unsigned int u);
    virtual operator int() const;
    virtual operator unsigned int() const;
    virtual operator float() const;
    virtual operator double() const;
    virtual MfString ToString() const;
    virtual int Compare(const MfVariantData *o) const;

    unsigned int m_value;   // +8
};

// vtable 0x100179b4
class MfVdFloat : public MfVariantData {
public:
    MfVdFloat(float f);
    MfVdFloat(const MfVdFloat &o);
    virtual MfVariantData *MakeCopy() const;
    virtual MfVariantData *Assign(float f);
    virtual operator int() const;
    virtual operator unsigned int() const;
    virtual operator float() const;
    virtual operator double() const;
    virtual MfString ToString() const;
    virtual int Compare(const MfVariantData *o) const;

    float m_value;  // +8
};

// vtable 0x100179ec
class MfVdDouble : public MfVariantData {
public:
    MfVdDouble(double d);
    MfVdDouble(const MfVdDouble &o);
    virtual MfVariantData *MakeCopy() const;
    virtual MfVariantData *Assign(double d);
    virtual operator int() const;
    virtual operator unsigned int() const;
    virtual operator float() const;
    virtual operator double() const;
    virtual MfString ToString() const;
    virtual int Compare(const MfVariantData *o) const;

    double m_value; // +8
};

// vtable 0x10017a24
class MfVdString : public MfVariantData {
public:
    MfVdString(const MfString &s);
    virtual ~MfVdString();
    MfVdString(const MfVdString &o);
    virtual MfVariantData *MakeCopy() const;
    virtual MfVariantData *Assign(const MfString &s);
    virtual operator int() const;
    virtual operator unsigned int() const;
    virtual operator float() const;
    virtual operator double() const;
    virtual MfString ToString() const;
    virtual int Compare(const MfVariantData *o) const;

    MfString m_value;   // +8
};

// 0x10010670 ?Assign@MfVariantData@variant_namespace@@UAEPAV12@H@Z
MfVariantData *MfVariantData::Assign(int i)
{
    return new MfVdInt(i);
}

// 0x100106d0 ?Assign@MfVariantData@variant_namespace@@UAEPAV12@I@Z
MfVariantData *MfVariantData::Assign(unsigned int u)
{
    return new MfVdUint(u);
}

// 0x10010730 ?Assign@MfVariantData@variant_namespace@@UAEPAV12@M@Z
MfVariantData *MfVariantData::Assign(float f)
{
    return new MfVdFloat(f);
}

// 0x10010790 ?Assign@MfVariantData@variant_namespace@@UAEPAV12@N@Z
MfVariantData *MfVariantData::Assign(double d)
{
    return new MfVdDouble(d);
}

// 0x10010800 ?Assign@MfVariantData@variant_namespace@@UAEPAV12@ABVMfString@@@Z
MfVariantData *MfVariantData::Assign(const MfString &s)
{
    return new MfVdString(s);
}

// MfVdInt. Members the linker folded into identical code of a later class are
// claimed there.

// 0x10010860 ??0MfVdInt@variant_namespace@@QAE@H@Z
MfVdInt::MfVdInt(int i) : MfVariantData(MFVT_INT), m_value(i)
{
}

// 0x10010880 ??0MfVdInt@variant_namespace@@QAE@ABV01@@Z
MfVdInt::MfVdInt(const MfVdInt &o) : MfVariantData(o), m_value(o.m_value)
{
}

// 0x100108b0 ?MakeCopy@MfVdInt@variant_namespace@@UBEPAVMfVariantData@2@XZ
MfVariantData *MfVdInt::MakeCopy() const
{
    return new MfVdInt(*this);
}

MfVariantData *MfVdInt::Assign(int i)
{
    m_value = i;
    return this;
}

MfVdInt::operator int() const
{
    return m_value;
}

MfVdInt::operator unsigned int() const
{
    return m_value;
}

// 0x10010910 ??BMfVdInt@variant_namespace@@UBEMXZ
MfVdInt::operator float() const
{
    return (float)m_value;
}

MfVdInt::operator double() const
{
    return m_value;
}

// 0x10010920 ?ToString@MfVdInt@variant_namespace@@UBE?AVMfString@@XZ
MfString MfVdInt::ToString() const
{
    return MfString(m_value);
}

// 0x10010940 ?Compare@MfVdInt@variant_namespace@@UBEHPBVMfVariantData@2@@Z
int MfVdInt::Compare(const MfVariantData *o) const
{
    return m_value == (int)*o;
}

// MfVdUint

// 0x10010960 ??0MfVdUint@variant_namespace@@QAE@I@Z
MfVdUint::MfVdUint(unsigned int u) : MfVariantData(MFVT_UINT), m_value(u)
{
}

// 0x10010980 ??0MfVdUint@variant_namespace@@QAE@ABV01@@Z
MfVdUint::MfVdUint(const MfVdUint &o) : MfVariantData(o), m_value(o.m_value)
{
}

// 0x100109b0 ?MakeCopy@MfVdUint@variant_namespace@@UBEPAVMfVariantData@2@XZ
MfVariantData *MfVdUint::MakeCopy() const
{
    return new MfVdUint(*this);
}

MfVariantData *MfVdUint::Assign(unsigned int u)
{
    m_value = u;
    return this;
}

// 0x10010a10 ??BMfVdUint@variant_namespace@@UBEHXZ
MfVdUint::operator int() const
{
    return m_value;
}

MfVdUint::operator unsigned int() const
{
    return m_value;
}

// 0x10010a20 ??BMfVdUint@variant_namespace@@UBEMXZ
MfVdUint::operator float() const
{
    return (float)m_value;
}

MfVdUint::operator double() const
{
    return m_value;
}

// 0x10010a40 ?ToString@MfVdUint@variant_namespace@@UBE?AVMfString@@XZ
MfString MfVdUint::ToString() const
{
    return MfString(m_value);
}

// 0x10010a60 ?Compare@MfVdUint@variant_namespace@@UBEHPBVMfVariantData@2@@Z
int MfVdUint::Compare(const MfVariantData *o) const
{
    return m_value == (unsigned int)*o;
}

// MfVdFloat

// 0x10010a80 ??0MfVdFloat@variant_namespace@@QAE@M@Z
MfVdFloat::MfVdFloat(float f) : MfVariantData(MFVT_FLOAT), m_value(f)
{
}

// 0x10010aa0 ??0MfVdFloat@variant_namespace@@QAE@ABV01@@Z
MfVdFloat::MfVdFloat(const MfVdFloat &o) : MfVariantData(o), m_value(o.m_value)
{
}

// 0x10010ad0 ?MakeCopy@MfVdFloat@variant_namespace@@UBEPAVMfVariantData@2@XZ
MfVariantData *MfVdFloat::MakeCopy() const
{
    return new MfVdFloat(*this);
}

// 0x10010b30 ?Assign@MfVdFloat@variant_namespace@@UAEPAVMfVariantData@2@M@Z
MfVariantData *MfVdFloat::Assign(float f)
{
    m_value = f;
    return this;
}

// 0x10010b40 ??BMfVdFloat@variant_namespace@@UBEHXZ
MfVdFloat::operator int() const
{
    return (int)m_value;
}

MfVdFloat::operator unsigned int() const
{
    return (unsigned int)m_value;
}

// 0x10010b50 ??BMfVdFloat@variant_namespace@@UBEMXZ
MfVdFloat::operator float() const
{
    return m_value;
}

MfVdFloat::operator double() const
{
    return m_value;
}

// 0x10010b60 ?ToString@MfVdFloat@variant_namespace@@UBE?AVMfString@@XZ
MfString MfVdFloat::ToString() const
{
    return MfString(m_value);
}

// 0x10010b80 ?Compare@MfVdFloat@variant_namespace@@UBEHPBVMfVariantData@2@@Z
int MfVdFloat::Compare(const MfVariantData *o) const
{
    return (float)*o == m_value;
}

// MfVdDouble

// 0x10010bb0 ??0MfVdDouble@variant_namespace@@QAE@N@Z
MfVdDouble::MfVdDouble(double d) : MfVariantData(MFVT_DOUBLE), m_value(d)
{
}

// 0x10010bd0 ??_GMfVdDouble@variant_namespace@@UAEPAXI@Z (compiler generated; all four number classes share it)
// 0x10010bf0 ??0MfVdDouble@variant_namespace@@QAE@ABV01@@Z
MfVdDouble::MfVdDouble(const MfVdDouble &o) : MfVariantData(o), m_value(o.m_value)
{
}

// 0x10010c20 ?MakeCopy@MfVdDouble@variant_namespace@@UBEPAVMfVariantData@2@XZ
MfVariantData *MfVdDouble::MakeCopy() const
{
    return new MfVdDouble(*this);
}

// 0x10010c80 ?Assign@MfVdDouble@variant_namespace@@UAEPAVMfVariantData@2@N@Z
MfVariantData *MfVdDouble::Assign(double d)
{
    m_value = d;
    return this;
}

// 0x10010ca0 ??BMfVdDouble@variant_namespace@@UBEHXZ
MfVdDouble::operator int() const
{
    return (int)m_value;
}

MfVdDouble::operator unsigned int() const
{
    return (unsigned int)m_value;
}

// 0x10010cb0 ??BMfVdDouble@variant_namespace@@UBEMXZ
MfVdDouble::operator float() const
{
    return (float)m_value;
}

MfVdDouble::operator double() const
{
    return m_value;
}

// 0x10010cc0 ?ToString@MfVdDouble@variant_namespace@@UBE?AVMfString@@XZ
MfString MfVdDouble::ToString() const
{
    return MfString(m_value);
}

// 0x10010cf0 ?Compare@MfVdDouble@variant_namespace@@UBEHPBVMfVariantData@2@@Z
int MfVdDouble::Compare(const MfVariantData *o) const
{
    return (double)*o == m_value;
}

// MfVdString

// 0x10010d20 ??0MfVdString@variant_namespace@@QAE@ABVMfString@@@Z
MfVdString::MfVdString(const MfString &s) : MfVariantData(MFVT_STRING), m_value(s)
{
}

// 0x10010d80 ??_GMfVdString@variant_namespace@@UAEPAXI@Z (compiler generated)
// 0x10010da0 ??1MfVdString@variant_namespace@@UAE@XZ
MfVdString::~MfVdString()
{
}

// 0x10010df0 ??0MfVdString@variant_namespace@@QAE@ABV01@@Z
MfVdString::MfVdString(const MfVdString &o) : MfVariantData(o), m_value(o.m_value)
{
}

// 0x10010e50 ?MakeCopy@MfVdString@variant_namespace@@UBEPAVMfVariantData@2@XZ
MfVariantData *MfVdString::MakeCopy() const
{
    return new MfVdString(*this);
}

// 0x10010eb0 ?Assign@MfVdString@variant_namespace@@UAEPAVMfVariantData@2@ABVMfString@@@Z
MfVariantData *MfVdString::Assign(const MfString &s)
{
    m_value = s;
    return this;
}

// 0x10010ed0 ??BMfVdString@variant_namespace@@UBEHXZ
MfVdString::operator int() const
{
    return m_value;
}

// 0x10010ef0 ??BMfVdString@variant_namespace@@UBEIXZ
MfVdString::operator unsigned int() const
{
    return m_value;
}

// 0x10010f00 ??BMfVdString@variant_namespace@@UBEMXZ
MfVdString::operator float() const
{
    return m_value;
}

// 0x10010f10 ??BMfVdString@variant_namespace@@UBENXZ
MfVdString::operator double() const
{
    return m_value;
}

// 0x10010f20 ?ToString@MfVdString@variant_namespace@@UBE?AVMfString@@XZ
MfString MfVdString::ToString() const
{
    return m_value;
}

// 0x10010f40 ?Compare@MfVdString@variant_namespace@@UBEHPBVMfVariantData@2@@Z
int MfVdString::Compare(const MfVariantData *o) const
{
    return m_value == o->ToString();
}

}

using namespace variant_namespace;

// 0x10010fa0 ??0MfVariant@@QAE@XZ
MfVariant::MfVariant()
{
    m_data = new MfVariantData(MFVT_NONE);
}

// 0x10011030 ??0MfVariant@@QAE@H@Z
MfVariant::MfVariant(int i)
{
    m_data = new MfVdInt(i);
}

// 0x10011090 ??0MfVariant@@QAE@I@Z
MfVariant::MfVariant(unsigned int u)
{
    m_data = new MfVdUint(u);
}

// 0x100110f0 ??0MfVariant@@QAE@M@Z
MfVariant::MfVariant(float f)
{
    m_data = new MfVdFloat(f);
}

// 0x10011150 ??0MfVariant@@QAE@N@Z
MfVariant::MfVariant(double d)
{
    m_data = new MfVdDouble(d);
}

// 0x100111c0 ??0MfVariant@@QAE@ABVMfString@@@Z
MfVariant::MfVariant(const MfString &s)
{
    m_data = new MfVdString(s);
}

// 0x10011220 ??0MfVariant@@QAE@ABV0@@Z
MfVariant::MfVariant(const MfVariant &v)
{
    m_data = v.m_data->MakeCopy();
}

// 0x10010fe0 ??_EMfVariant@@UAEPAXI@Z (compiler generated)
// 0x10011240 ??1MfVariant@@UAE@XZ
MfVariant::~MfVariant()
{
    delete m_data;
    m_data = (MfVariantData *)0xdeadbeef;
}

// 0x10011260 ??4MfVariant@@QAEAAV0@H@Z
MfVariant &MfVariant::operator=(int i)
{
    MfVariantData *data = m_data->Assign(i);
    if (data != m_data) {
        delete m_data;
        m_data = data;
    }
    return *this;
}

// 0x10011290 ??4MfVariant@@QAEAAV0@I@Z
MfVariant &MfVariant::operator=(unsigned int u)
{
    MfVariantData *data = m_data->Assign(u);
    if (data != m_data) {
        delete m_data;
        m_data = data;
    }
    return *this;
}

// 0x100112c0 ??4MfVariant@@QAEAAV0@M@Z
MfVariant &MfVariant::operator=(float f)
{
    MfVariantData *data = m_data->Assign(f);
    if (data != m_data) {
        delete m_data;
        m_data = data;
    }
    return *this;
}

// 0x100112f0 ??4MfVariant@@QAEAAV0@N@Z
MfVariant &MfVariant::operator=(double d)
{
    MfVariantData *data = m_data->Assign(d);
    if (data != m_data) {
        delete m_data;
        m_data = data;
    }
    return *this;
}

// 0x10011330 ??4MfVariant@@QAEAAV0@ABVMfString@@@Z
MfVariant &MfVariant::operator=(const MfString &s)
{
    MfVariantData *data = m_data->Assign(s);
    if (data != m_data) {
        delete m_data;
        m_data = data;
    }
    return *this;
}

// 0x10011360 ??4MfVariant@@QAEAAV0@ABV0@@Z
MfVariant &MfVariant::operator=(const MfVariant &v)
{
    MfVariantData *old = m_data;
    m_data = v.m_data->MakeCopy();
    delete old;
    return *this;
}

// 0x10011390 ??BMfVariant@@QBEHXZ
MfVariant::operator int() const
{
    return *m_data;
}

// 0x100113a0 ??BMfVariant@@QBEIXZ
MfVariant::operator unsigned int() const
{
    return *m_data;
}

// 0x100113b0 ??BMfVariant@@QBEMXZ
MfVariant::operator float() const
{
    return *m_data;
}

// 0x100113c0 ??BMfVariant@@QBENXZ
MfVariant::operator double() const
{
    return *m_data;
}

// 0x100113d0 ?ToString@MfVariant@@QBE?AVMfString@@XZ
MfString MfVariant::ToString() const
{
    return m_data->ToString();
}

// 0x100113f0 ??8MfVariant@@QBEHABV0@@Z
int MfVariant::operator==(const MfVariant &v) const
{
    if (IsLike(v))
        return m_data->Compare(v.m_data);
    return 0;
}

// 0x10011420 ?IsLike@MfVariant@@QBEHABV1@@Z
// Both integers, both floating point, or the same type.
int MfVariant::IsLike(const MfVariant &v) const
{
    int t = m_data->Type();
    int vt = v.m_data->Type();
    if ((t == MFVT_INT || t == MFVT_UINT) && (vt == MFVT_INT || vt == MFVT_UINT))
        return 1;
    if ((t == MFVT_FLOAT || t == MFVT_DOUBLE) && (vt == MFVT_FLOAT || vt == MFVT_DOUBLE))
        return 1;
    return t == vt;
}
