#pragma once
#include <windows.h>
#include "MfString.h"
class MfDblList;
class MfDictionary;
class MfScDataHandle;
class MfScDebugInfo;
class MfScHandleList;
class MfStringList;

// A Piccolo object file (.pobj, .pexe, .plib, .pdbg): compiled script
// functions grouped by module, mapped into memory for reading, collected in
// lists for writing.
class __declspec(dllexport) MfScPiccoloObjectFile {
public:
    enum MfScPOFErrorCode {
        kPOFOk = 0,
        kPOFReadOnly = 1,           // write on a file opened for reading
        kPOFWriteOnly = 2,          // read on a file opened for writing
        kPOFClosed = 3,
        kPOFBadMagic = 4,
        kPOFBadVersion = 5,
        kPOFNoFile = 6,
        kPOFAccessDenied = 7,
        kPOFBadFunction = 8,
        kPOFNoMapping = 9,
        kPOFBadModuleTable = 10,
        kPOFNoModule = 11,
        kPOFNoName = 12
    };
    // the kinds of object file (their extensions)
    enum { kObject = 1, kExecutable = 2, kLibrary = 3, kDebug = 4 };
    struct PObjConstantPoolEntry {
        int mType;                  // +0x00 0x10 int, 0x20 float, 0x100 string
        int mValue;                 // +0x04 the value, or the string's aligned length
    };

    MfScPiccoloObjectFile()
        : mOpen(0), mFile(0), mMapping(0), mBase(0), mFunctions(0), mModules(0), mDebugFile(0) {}
    MfScPiccoloObjectFile(const MfString &name, int write, int type, unsigned int options);
    virtual ~MfScPiccoloObjectFile();

    MfScPOFErrorCode Open(const MfString &name, int write, int type, unsigned int options);
    MfScPOFErrorCode ReadClose();
    MfScPOFErrorCode WriteClose();
    MfScPOFErrorCode AddFunction(MfScDataHandle *function);
    MfScPOFErrorCode GrabFunctions(MfScHandleList *functions);
    MfScPOFErrorCode LookupFunctions(const MfString &module, MfScHandleList *functions);
    MfScDebugInfo *GetDebugInfo(const MfString &module, const MfString &function);
    MfScDebugInfo *GetDebugInfo(unsigned int offset);
    MfScPOFErrorCode GetLastError() const { return mError; }
    int IsOpen() const { return mOpen; }
    const MfString &ObjectFileName() const;
    MfString ECToString();
    void DumpFile(MfStringList *lines, int flags);

    static MfString __cdecl ObjectFileName(const MfString &name, int write, int type);
    static MfString __cdecl ECToString(MfScPOFErrorCode code);
    static MfString __cdecl ECToString(MfScPOFErrorCode code, int systemError);

private:
    MfScPOFErrorCode SetLastError(MfScPOFErrorCode code) { return mError = code; }
    MfScDataHandle *BuildFunction(unsigned long offset);
    PObjConstantPoolEntry *WriteConstantPoolEntry(PObjConstantPoolEntry *entry, const MfString &value);
    PObjConstantPoolEntry *WriteConstantPoolEntry(PObjConstantPoolEntry *entry, float value);
    PObjConstantPoolEntry *WriteConstantPoolEntry(PObjConstantPoolEntry *entry, int value);
    int IsValidVersion();
    int OpenFileMappingA(unsigned long size);
    void CloseFileMapping();
    int ReadModuleNameTable();
    unsigned int ComputeFunctionSectionSize();
    unsigned int ComputeModuleNameSectionSize();
    unsigned int ComputeObjectFileInfoSectionSize();
    void *OffsetToAddress(unsigned long offset);
    unsigned long AddressToOffset(void *address);
    unsigned long AlignedLength(unsigned long length)
    {
        unsigned long rest = length & 3;
        if (rest)
            return length - rest + 4;
        return length;
    }
    // the address of a section of a function record, 0 if it has none
    char *SectionAddress(unsigned long offset)
    {
        if (offset == 0xdbdbdbdb || offset == 0xa0a0a0a0)
            return 0;
        return mBase + offset;
    }
    void Cleanup();
    void DumpData(void *to, const void *from, int size);

    MfScPOFErrorCode mError;        // +0x04
    DWORD mSystemError;             // +0x08 GetLastError of the failed call
    int mWrite;                     // +0x0c
    int mType;                      // +0x10 kObject ...
    unsigned int mOptions;          // +0x14 2: there is a debug file
    MfString mName;                 // +0x18
    int mOpen;                      // +0x1c
    HANDLE mFile;                   // +0x20
    HANDLE mMapping;                // +0x24
    char *mBase;                    // +0x28 the mapped file
    union {                         // +0x2c one list per module:
        MfDblList *mOffsetLists;    //   read: MfIntList of function offsets
        MfDblList *mFunctions;      //   write: MfScHandleList of functions
    };
    MfDictionary *mModules;         // +0x30 module name -> those lists
    MfScPiccoloObjectFile *mDebugFile;  // +0x34 the .pdbg file
};
