#pragma once
#include "MfCallBack.h"
class MfDblList;
class MfScDataHandle;
class MfVariant;

// The constant pool of the function being compiled: the distinct literal
// values it uses, in order (8 bytes).
class MfPcConstantTable {
public:
    // Deletes the MfVariant values when the list is cleared.
    class MfPcConstantTableCB : public MfCallBack {
    public:
        virtual int CallBack(void *p);
    };

    MfPcConstantTable();
    virtual ~MfPcConstantTable();
    int Add(MfVariant const &value);
    int Find(MfVariant const &value);
    void Clear();
    void FillConstantPool(MfScDataHandle **pool);
    void Dump();

    MfDblList *mConstants;      // +0x04 MfVariant *
};
