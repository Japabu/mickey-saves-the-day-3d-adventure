// MfPoArray: the Piccolo array wrapping an array property value
// ("PropertyArray").
#pragma once
#include "MfScDataArray.h"
class MfArrayValue;

class __declspec(dllexport) MfPoArray : public MfScDataArray {
public:
    MfPoArray(MfArrayValue *value);
    virtual ~MfPoArray();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    MfArrayValue *ArrayValue() const { return mArray; }
protected:
    virtual int SetArrayItem(int index, MfScDataHandle *value, int release);
    virtual int GetArrayItem(int index, MfScDataHandle **value, int addRef);
    virtual int IsContainer() { return 0; }
private:
    MfPoArray(MfScData::Exemplar);
    virtual MfString GetClassName();
    static MfPoArray *poExemplar;
    MfArrayValue *mArray;                                   // 0x18
};
