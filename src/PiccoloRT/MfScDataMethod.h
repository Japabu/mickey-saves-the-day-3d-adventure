// MfScDataMethod: the base of the callable script data (routines).
#pragma once
#include "PiccoloRT_api.h"
#include "MfScData.h"
#include "MfScDataHandle.h"

// Base of the callable data (bytecode routines, C functions, C++ methods)
// with the routine's constant pool.
class PICCOLORT_API MfScDataMethod : public MfScData {
public:
    enum MfScMethodType { kNone, kFunction, kMethod, kExtMethod };
    MfScDataMethod(MfScMethodType type, int numArgs);
    virtual ~MfScDataMethod();
    MfScDataHandle **AllocConstantPool(int size);
    void DeleteConstantPool();
    void SetConstant(int i, MfScDataHandle *h);
    MfScDataHandle *GetConstant(int i) { return m_constants[i]; }
    MfScDataHandle **GetConstantPool() const { return m_constants; }
    int ConstantPoolSize() const { return m_numConstants; }
    int Params() const { return m_numArgs; }
    int Type() const { return m_type; }
protected:
    int m_type;                     // +0x0c
    MfScDataHandle **m_constants;   // +0x10
    int m_numConstants;             // +0x14
    int m_numArgs;                  // +0x18
};
