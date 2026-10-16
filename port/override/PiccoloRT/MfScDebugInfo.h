// PORT: stack words are uintptr_t (pointer-sized): the 32-bit source used unsigned long, which was the size of a pointer there.
#pragma once
#include <stdint.h> // PORT: uintptr_t
class MfScCodeLevelInfo;
class MfScConstUsageInfo;
class MfScDataHandle;
class MfScLineInfo;
class MfScVariableDebugInfo;
class MfString;

// Debug information of a bytecode routine: line table, local variables,
// code levels and the constants it uses.
class __declspec(dllexport) MfScDebugInfo {
public:
    MfScDebugInfo();
    virtual ~MfScDebugInfo();
    int GetCodeSize() const { return mCodeSize; }
    void SetCodeSize(int size) { mCodeSize = size; }

    int InitializeLineInfo(int);
    void DeleteLineInfo();
    int NumLineInfo() const { return mNumLineInfo; }
    MfScLineInfo const & GetLineInfo(int) const;
    void SetLineInfo(MfScLineInfo const &, int);
    int FindLine(int);
    int FindOffset(int);
    int FindOffsetOfFirstLine();
    int FindOffsetOfNextLine(int, int *);
    int FindOffsetOfNextLine(int);
    int ComputeOffsetRange(int, int *, int *);
    void GetRange(int &, int &);

    int InitializeVarDebugInfo(int);
    void DeleteVarDebugInfo();
    int NumVarDebugInfo() const { return mNumVarDebugInfo; }
    MfScVariableDebugInfo const & GetVarDebugInfo(int) const;
    void SetVarDebugInfo(MfScVariableDebugInfo const &, int);
    int GetVariable(MfString const &, uintptr_t *, int, MfScDataHandle * *); // PORT: uintptr_t

    int InitializeCodeLevelInfo(int);
    void DeleteCodeLevelInfo();
    int NumCodeLevelInfo() const { return mNumCodeLevelInfo; }
    MfScCodeLevelInfo const & GetCodeLevelInfo(int) const;
    void SetCodeLevelInfo(MfScCodeLevelInfo const &, int);
    int FindLevel(int);

    int InitializeConstUsageInfo(int);
    void DeleteConstUsageInfo();
    int NumConstUsageInfo() const { return mNumConstUsageInfo; }
    MfScConstUsageInfo const & GetConstUsageInfo(int) const;
    void SetConstUsageInfo(MfScConstUsageInfo const &, int);

    unsigned int MemoryUsage();
private:
    int FindLineIndex(int);

    int mCodeSize;                          // +0x04
    MfScLineInfo *mLineInfo;                // +0x08
    int mNumLineInfo;                       // +0x0c
    MfScVariableDebugInfo *mVarDebugInfo;   // +0x10
    int mNumVarDebugInfo;                   // +0x14
    MfScCodeLevelInfo *mCodeLevelInfo;      // +0x18
    int mNumCodeLevelInfo;                  // +0x1c
    MfScConstUsageInfo *mConstUsageInfo;    // +0x20
    int mNumConstUsageInfo;                 // +0x24
};
