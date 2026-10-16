// PiccoloRT.dll: MfScDataHandle and the inline members of the VM classes that
// this object emits (0x100016c0-0x10001aed).
#ifndef PICCOLORT_MFSCDATAHANDLE_H
#define PICCOLORT_MFSCDATAHANDLE_H

#include "MfScDataHandle.h"

class MfScHandleDict;

class __declspec(dllexport) MfScVariableStack {
public:
    enum MfScVariableErrorCode {
        kOk = 0,
        kOverflow = 3,
        kUnderflow = 4,
        kBadIndex = 5
    };
    MfScVariableStack();          // out of line (no implicit ctor emitted)
    virtual ~MfScVariableStack(); // 0x10016c30

    MfScVariableErrorCode PushVarPool(MfScHandleDict *pool) {
        if (m_count < m_size - 1) {
            m_pools[m_count] = pool;
            m_count++;
            return kOk;
        }
        return kOverflow;
    }
    MfScVariableErrorCode PopVarPool() {
        if (m_count > 0) {
            m_count--;
            return kOk;
        }
        return kUnderflow;
    }
    MfScVariableErrorCode SetVarPoolAtIndex(int index, MfScHandleDict *pool) {
        if (index >= 0 && index < m_count) {
            m_pools[index] = pool;
            return kOk;
        }
        return kBadIndex;
    }
    MfScVariableErrorCode GetVarPoolAtIndex(int index, MfScHandleDict **pool) {
        if (index >= 0 && index < m_count) {
            *pool = m_pools[index];
            return kOk;
        }
        *pool = 0;
        return kBadIndex;
    }

private:
    int m_size;                // +4
    int m_count;               // +8
    MfScHandleDict **m_pools;  // +c
};

class __declspec(dllexport) MfScVmFrame {
public:
    long m_data[5]; // 0x14 bytes, layout unknown
};

// Partial: more inline members follow at 0x10001af0+ (next unit), including a
// user-visible copy ctor/operator= (0x10001c30/0x10001cf0) over a larger layout.
class __declspec(dllexport) MfScVmEnv {
public:
    MfScDataHandle *GetSPHandle(int i) const { return m_stack[i]; }
    void SetSPHandle(int i, MfScDataHandle *h) { m_stack[i] = h; }
    void ClearScratchREG0() { if (m_reg0) { m_reg0->Release(); m_reg0 = 0; } }
    void ClearScratchREG1() { if (m_reg1) { m_reg1->Release(); m_reg1 = 0; } }
    void ClearScratchREG2() { if (m_reg2) { m_reg2->Release(); m_reg2 = 0; } }
    void ClearScratchREG3() { if (m_reg3) { m_reg3->Release(); m_reg3 = 0; } }
    void ClearScratchRegisters() {
        ClearScratchREG0();
        ClearScratchREG1();
        ClearScratchREG2();
        ClearScratchREG3();
    }
    MfScVariableStack *SetVariableStack(MfScVariableStack *stack) {
        MfScVariableStack *old = m_varStack;
        m_varStack = stack;
        return old;
    }

private:
    long m_unk0[3];              // +0
    MfScDataHandle **m_stack;    // +c
    long m_unk10[4];             // +10
    MfScDataHandle *m_reg0;      // +20
    MfScDataHandle *m_reg1;      // +24
    MfScDataHandle *m_reg2;      // +28
    MfScDataHandle *m_reg3;      // +2c
    MfScVariableStack *m_varStack; // +30
};

#endif
