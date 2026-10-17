#pragma once
class MfString;

// mfutil.dll: the typed body of an MfVariant.
namespace variant_namespace {
class MfVariantData {   // own copy: src/mfutil/MfVariantData.h includes src/mfutil/MfString.h, which conflicts with the older MfString kept here
public:
    virtual ~MfVariantData();
    int Type() const { return iType; }
    int iType;      // +4: 1 int, 2 unsigned int, 3 float, 4 double, 5 string
};
}

// mfutil.dll: a number or a string. The type tests are inline.
class __declspec(dllimport) MfVariant {   // own copy: src/mfutil/MfVariant.h includes src/mfutil/MfString.h, which conflicts with the older MfString kept here
public:
    MfVariant();
    MfVariant(int);
    MfVariant(unsigned int);
    MfVariant(float);
    MfVariant(double);
    MfVariant(MfString const &);
    MfVariant(MfVariant const &);
    virtual ~MfVariant();
    MfVariant &operator=(MfVariant const &);
    MfVariant &operator=(MfString const &);
    MfVariant &operator=(int);
    MfVariant &operator=(unsigned int);
    MfVariant &operator=(float);
    MfVariant &operator=(double);
    int Type() const { return pvdData->iType; }
    int IsInt() const { return pvdData->iType == 1 || pvdData->iType == 2; }
    int IsFloat() const { return pvdData->iType == 3 || pvdData->iType == 4; }
    int IsNumber() const
    {
        return pvdData->iType == 1 || pvdData->iType == 2 || pvdData->iType == 3 || pvdData->iType == 4;
    }
    int IsString() const { return pvdData->iType == 5; }
    int IsLike(MfVariant const &) const;
    int operator==(MfVariant const &) const;
    int operator!=(MfVariant const &) const;
    operator int() const;
    operator unsigned int() const;
    operator float() const;
    operator double() const;
    MfString ToString() const;
    variant_namespace::MfVariantData *pvdData;  // +4
};
