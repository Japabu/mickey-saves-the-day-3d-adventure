#pragma once
#include "MfString.h"

// mfutil.dll: typed value (1, 2 integer; 3, 4 float; 5 string).
class MfVariantData {
public:
    virtual ~MfVariantData();
    int m_type;     // +4
};

class __declspec(dllimport) MfVariant {
public:
    enum { kInt = 1, kLong = 2, kFloat = 3, kDouble = 4, kString = 5 };
    MfVariant();
    MfVariant(MfVariant const &);
    virtual ~MfVariant();
    MfVariant &operator=(MfVariant const &);
    MfVariant &operator=(int);
    MfVariant &operator=(float);
    MfVariant &operator=(MfString const &);
    MfString ToString() const;
    operator int() const;
    operator float() const;
    int Type() const { return m_data->m_type; }
    int IsInt() const { int t = Type(); return t == kInt || t == kLong; }
    int IsFloat() const { int t = Type(); return t == kFloat || t == kDouble; }
private:
    MfVariantData *m_data;  // +4
};
