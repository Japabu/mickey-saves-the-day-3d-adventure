#pragma once
#include "MfString.h"
class MfScDataHandle;

// PiccoloRT.dll: a compiled module on disk (.pob). Its default constructor and
// ObjectFileName() are inline here.
class __declspec(dllimport) MfScPiccoloObjectFile {   // own copy: inline constructor; src/PiccoloRT/MfScPiccoloObjectFile.h includes PiccoloRT's MfString, which conflicts with the MfString kept here
public:
    enum MfScPOFErrorCode { kOk = 0 };
    MfScPiccoloObjectFile()
        : mFunctionTable(0), mModuleTable(0), mNumFunctions(0), mNumModules(0), mFlags(0), mVersion(0),
          mLastError(0) {}
    virtual ~MfScPiccoloObjectFile();
    MfScPOFErrorCode Open(MfString const &name, int write, int create, unsigned int flags);
    MfScPOFErrorCode AddFunction(MfScDataHandle *function);
    MfScPOFErrorCode WriteClose();
    MfString ECToString();
    MfString const &ObjectFileName() const { return mObjectFileName; }
private:
    void *mFile;                // +0x04
    void *mMapping;             // +0x08
    void *mView;                // +0x0c
    unsigned int mSize;         // +0x10
    unsigned int mPosition;     // +0x14
    MfString mObjectFileName;   // +0x18
    void *mFunctionTable;       // +0x1c
    void *mModuleTable;         // +0x20
    int mNumFunctions;          // +0x24
    int mNumModules;            // +0x28
    unsigned int mFlags;        // +0x2c
    int mVersion;               // +0x30
    int mLastError;             // +0x34
};
