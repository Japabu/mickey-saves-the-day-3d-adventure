// MfScBytecodeRoutine: a script routine compiled to Piccolo bytecode.
#pragma once
#include "PiccoloRT_api.h"
#include "MfScDataMethod.h"
class MfScDebugInfo;
class MfString;

// A routine compiled to Piccolo bytecode (0x3c bytes).
class PICCOLORT_API MfScBytecodeRoutine : public MfScDataMethod {
public:
    MfScBytecodeRoutine(int numArgs);
    virtual ~MfScBytecodeRoutine();
    MfScDebugInfo * DebugInfo() const { return mDebugInfo; }
    MfScDebugInfo * Detach();
    MfString & FunctionParams() { return *mFunctionParams; }
    MfString & ModuleName() { return *mModuleName; }
    MfString & ModuleType() { return *mModuleType; }
    MfString & ObjectFileName() { return *mObjectFileName; }
    int & Flags() { return mFlags; }
    int CalcOffsetOfBytecode(unsigned char *);
    int CodeSize() const;
    int HasDebugInfo() const;
    unsigned char *AllocBytecodes(int size)
    {
        mCodeSize = size;
        mBytecodes = new unsigned char[size];
        return mBytecodes;
    }
    unsigned char * GetBytecodeLocAtOffset(int);
    unsigned char * GetBytecodes() const;
    unsigned int MemoryUsage(int);
    void Attach(MfScDebugInfo *);
    void DeleteBytecodes();
    unsigned char * mBytecodes;     // +0x1c
    int mCodeSize;                  // +0x20
    int mFlags;                     // +0x24
    MfString *mModuleName;          // +0x28
    MfString *mModuleType;          // +0x2c
    MfString *mFunctionParams;      // +0x30
    MfScDebugInfo * mDebugInfo;     // +0x34
    MfString *mObjectFileName;      // +0x38
};
