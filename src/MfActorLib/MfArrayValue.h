#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfArrayValue: a property value holding an array of values ('VArr').
#include "MfTypedValue.h"
class MfStream;

// size 0x14
class MFACTOR_API MfArrayValue : public MfTypedValue {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    MfArrayValue() : numItems(0), items(0) {}
    MfArrayValue(const MfArrayValue &a);
    virtual ~MfArrayValue();
    virtual MfTypedValue *Copy() const { return new MfArrayValue(*this); }
    virtual void Destroy() { delete this; }
    int NumItems() const { return numItems; }
    // Resizes the array; values beyond the new size are destroyed, new
    // entries are empty.
    void SetNumItems(int n);
    void SetItem(int i, MfTypedValue *v);
    MfTypedValue *GetItem(int i) const;
    MfTypedValue *operator[](int i) const;
protected:
    int numItems;           // +0xc
    MfTypedValue **items;   // +0x10
};
