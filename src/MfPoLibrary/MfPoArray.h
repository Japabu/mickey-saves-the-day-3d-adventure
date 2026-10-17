// MfPoArray: the Piccolo array wrapping an array property value
// ("PropertyArray").
#ifndef MFPOLIBRARY_MFPOARRAY_H
#define MFPOLIBRARY_MFPOARRAY_H
#include "MfPoLibrary_api.h"
#include "MfScDataArray.h"
class MfArrayValue;

class MFPO_API MfPoArray : public MfScDataArray {
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

#endif
