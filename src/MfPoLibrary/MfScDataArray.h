// MfScDataArray (PiccoloRT.dll): a script array; the derived classes provide
// the storage.
#pragma once
#include "MfScDataObject.h"

class __declspec(dllimport) MfScDataArray : public MfScDataObject {
public:
    virtual ~MfScDataArray();
    virtual MfPropertyErrorCode GetProperty(const MfString &name, MfScDataHandle **value);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *self, const MfString &name, MfScDataHandle *value, int release);
    virtual int SetArrayItem(int index, MfScDataHandle *value, int release) = 0;
    virtual int GetArrayItem(int index, MfScDataHandle **value, int addRef) = 0;
    virtual int IsContainer() = 0;
    int GetSize() const { return iSize; }
protected:
    MfScDataArray(int size);
    int iSize;                                              // 0x14
};
