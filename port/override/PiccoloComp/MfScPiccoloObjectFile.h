// PORT: PiccoloRT's MfScPiccoloObjectFile is declared with the real members (pointers where this view had ints), so stack objects are as big as the real class on 64-bit.
#pragma once
#include "MfString.h"
class MfScDataHandle;

// PiccoloRT.dll: a compiled module on disk (.pob). Its default constructor and
// ObjectFileName() are inline here.
class __declspec(dllimport) MfScPiccoloObjectFile {
public:
    enum MfScPOFErrorCode { kOk = 0 };
    // PORT: the members of the real constructor (PiccoloRT's MfScPiccoloObjectFile.h)
    MfScPiccoloObjectFile()
        : mOpen(0), mFile(0), mMapping(0), mBase(0), mFunctions(0), mModules(0), mDebugFile(0) {}
    virtual ~MfScPiccoloObjectFile();
    MfScPOFErrorCode Open(MfString const &name, int write, int create, unsigned int flags);
    MfScPOFErrorCode AddFunction(MfScDataHandle *function);
    MfScPOFErrorCode WriteClose();
    MfString ECToString();
    MfString const &ObjectFileName() const { return mObjectFileName; }
private:
    // PORT: the members of the real class (src/PiccoloRT/MfScPiccoloObjectFile.h); this
    // view's invented list agreed with it on 32-bit only (96 bytes on 64-bit, not 88)
    int mError;                 // +0x04
    unsigned int mSystemError;  // +0x08
    int mWrite;                 // +0x0c
    int mType;                  // +0x10
    unsigned int mOptions;      // +0x14
    MfString mObjectFileName;   // +0x18 mName
    int mOpen;                  // +0x1c
    void *mFile;                // +0x20
    void *mMapping;             // +0x24
    char *mBase;                // +0x28
    void *mFunctions;           // +0x2c
    void *mModules;             // +0x30
    MfScPiccoloObjectFile *mDebugFile;  // +0x34
};
