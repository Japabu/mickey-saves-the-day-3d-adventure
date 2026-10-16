#include "MfScPiccoloObjectFile.h"
#include "MfDictionary.h"
#include "MfIntList.h"
#include "MfScHandleList.h"
#include "MfScDataHandle.h"
#include "MfScBytecodeRoutine.h"
#include "MfScObject.h"
#include "MfScPiccolo.h"
#include "MfAiFunctionRegistry.h"
#include "MfScDebugInfo.h"
#include "MfScLineInfo.h"
#include "MfScCodeLevelInfo.h"
#include "MfScVariableDebugInfo.h"
#include "MfScConstUsageInfo.h"
#include "MfVariant.h"
#include "MfStringList.h"
#include "MfTime.h"

extern __declspec(dllimport) MfNullString NULL_STRING;
#include "MfString.h"

// separates the directories of PICCOLOPATH and PICCOLOLIB
extern const char *const gPathSeparator = ";";

// 0x1000f0b0 ?ObjectFileName@MfScPiccoloObjectFile@@QBEABVMfString@@XZ
const MfString &MfScPiccoloObjectFile::ObjectFileName() const
{
    return mName;
}

// 0x1000f0c0 ?OffsetToAddress@MfScPiccoloObjectFile@@AAEPAXK@Z
void * MfScPiccoloObjectFile::OffsetToAddress(unsigned long offset)
{
    return (void *)((int)mBase + (int)offset);
}

// 0x1000f0d0 ?AddressToOffset@MfScPiccoloObjectFile@@AAEKPAX@Z
unsigned long MfScPiccoloObjectFile::AddressToOffset(void * address)
{
    return (unsigned long)((int)address - (int)mBase);
}

// 0x10015a20 ?ComputeObjectFileInfoSectionSize@MfScPiccoloObjectFile@@AAEIXZ
unsigned int MfScPiccoloObjectFile::ComputeObjectFileInfoSectionSize()
{
    return 0x38;
}

// 0x1000f080 ??0MfScPiccoloObjectFile@@QAE@XZ
// 0x1000f0e0 ?AlignedLength@MfScPiccoloObjectFile@@AAEKK@Z
// 0x1000f1e0 ??_EMfScPiccoloObjectFile@@UAEPAXI@Z
// 0x1000f100 ??0MfScPiccoloObjectFile@@QAE@ABV0@@Z
// 0x1000f170 ??4MfScPiccoloObjectFile@@QAEAAV0@ABV0@@Z

// 0x100132f0 ??0MfScPiccoloObjectFile@@QAE@ABVMfString@@HHI@Z
MfScPiccoloObjectFile::MfScPiccoloObjectFile(const MfString &name, int write, int type, unsigned int options)
{
    mOpen = 0;
    mFile = 0;
    mMapping = 0;
    mBase = 0;
    mModules = 0;
    mDebugFile = 0;
    mFunctions = 0;
    Open(name, write, type, options);
}

// 0x10013370 ??1MfScPiccoloObjectFile@@UAE@XZ
MfScPiccoloObjectFile::~MfScPiccoloObjectFile()
{
    if (mOpen) {
        if (!mWrite)
            ReadClose();
        else
            WriteClose();
    }
    Cleanup();
}

// 0x100133e0 ?Cleanup@MfScPiccoloObjectFile@@AAEXXZ
void MfScPiccoloObjectFile::Cleanup()
{
    delete mModules;
    mModules = 0;
    MfDblListItem *pos;
    if (!mWrite && mFunctions) {
        for (MfIntList *list = (MfIntList *)mFunctions->GetFirst(pos); list;
             list = (MfIntList *)mFunctions->GetNext(pos))
            delete list;
        delete mFunctions;
        mFunctions = 0;
    } else if (mWrite == 1 && mFunctions) {
        for (MfScHandleList *list = (MfScHandleList *)mFunctions->GetFirst(pos); list;
             list = (MfScHandleList *)mFunctions->GetNext(pos))
            delete list;
        delete mFunctions;
        mFunctions = 0;
    }
}

// 0x10013490 ?ObjectFileName@MfScPiccoloObjectFile@@SA?AVMfString@@ABV2@HH@Z
MfString MfScPiccoloObjectFile::ObjectFileName(const MfString &name, int write, int type)
{
    MfString result(name);
    if (write == 1) {
        MfString drive;
        MfString dir;
        MfString base;
        MfString ext;
        name.SplitPath(&drive, &dir, &base, &ext);
        switch (type) {
        case kExecutable:
            ext = ".pexe";
            break;
        case kLibrary:
            ext = ".plib";
            break;
        case kDebug:
            ext = ".pdbg";
            break;
        default:
            ext = ".pobj";
            break;
        }
        result.MakePath(drive, dir, base, ext);
    }
    return result;
}

// 0x100139b0 ??_GMfDictionary@@UAEPAXI@Z

// EQUIV 0x100135e0 ?Open@MfScPiccoloObjectFile@@QAE?AW4MfScPOFErrorCode@1@ABVMfString@@HHI@Z: reviewed: the original stores mWrite after the name test (a const call) and loads NULL_STRING earlier; same calls (the recursive Open included) and flow
MfScPiccoloObjectFile::MfScPOFErrorCode MfScPiccoloObjectFile::Open(const MfString &name, int write, int type,
                                                                    unsigned int options)
{
    mWrite = write;
    if (name == NULL_STRING)
        return SetLastError(kPOFNoName);
    mType = type;
    mOptions = options;
    if (type == kDebug)
        mOptions &= ~3;
    mName = ObjectFileName(name, write, type);
    if (!mWrite) {
        HANDLE file = CreateFile(mName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (file == INVALID_HANDLE_VALUE) {
            if (mError == 2 || mError == 3)
                return SetLastError(kPOFNoFile);
            if (mError == 5)
                return SetLastError(kPOFAccessDenied);
            return SetLastError(kPOFNoMapping);
        }
        CloseHandle(file);
        if (!OpenFileMappingA(0))
            return SetLastError(kPOFNoMapping);
        unsigned long *header = (unsigned long *)mBase;
        if (header[0] != 0x50534a4d) {
            mOpen = 1;
            ReadClose();
            return SetLastError(kPOFBadMagic);
        }
        if (!IsValidVersion()) {
            mOpen = 1;
            ReadClose();
            return SetLastError(kPOFBadVersion);
        }
        mFunctions = new MfDblList;
        mModules = new MfDictionary(0x35);
        if (!ReadModuleNameTable()) {
            mOpen = 1;
            ReadClose();
            return SetLastError(kPOFBadModuleTable);
        }
        if (header[4] != (unsigned long)-1) {
            unsigned long *info = (unsigned long *)(header[4] + mBase + 0x10);
            mType = info[0];
            mOptions = info[1];
            if (mOptions & 2) {
                mDebugFile = new MfScPiccoloObjectFile;
                MfString drive;
                MfString dir;
                MfString base;
                MfString ext;
                mName.SplitPath(&drive, &dir, &base, &ext);
                ext = ".pdbg";
                MfString path;
                path.MakePath(drive, dir, base, ext);
                mDebugFile->Open(path, 0, kDebug, 0);
                if (mDebugFile->mError) {
                    delete mDebugFile;
                    mDebugFile = 0;
                }
            }
        } else {
            mType = kObject;
            mOptions = 0;
        }
    } else {
        mFunctions = new MfDblList;
        mModules = new MfDictionary(0x35);
    }
    mOpen = 1;
    mError = kPOFOk;
    return kPOFOk;
}

// EQUIV 0x100139d0 ?IsValidVersion@MfScPiccoloObjectFile@@AAEHXZ: reviewed: the original keeps the version in a one-dword frame and loads the minor byte with xor/mov cl; ours masks a dword with 0xff and compares signed (jg vs ja), same result for a 0..255 value
int MfScPiccoloObjectFile::IsValidVersion()
{
    union {
        unsigned long all;
        unsigned char part[4];
    } version;
    version.all = *(unsigned long *)(mBase + 4);
    int minor = version.part[2];
    if ((version.all & 0xff000000) == 0x2000000 && minor <= 99)
        return 1;
    return 0;
}

// 0x10013a00 ?ECToString@MfScPiccoloObjectFile@@SA?AVMfString@@W4MfScPOFErrorCode@1@@Z
MfString MfScPiccoloObjectFile::ECToString(MfScPOFErrorCode code)
{
    return ECToString(code, 0);
}

// 0x10013a30 ?ECToString@MfScPiccoloObjectFile@@SA?AVMfString@@W4MfScPOFErrorCode@1@H@Z
MfString MfScPiccoloObjectFile::ECToString(MfScPOFErrorCode code, int systemError)
{
    switch (code) {
    case kPOFOk:
        return "There was no Piccolo object file error.";
    case kPOFReadOnly:
        return "Write operation attempted on read-only file.";
    case kPOFWriteOnly:
        return "Read operation attempted on read-only file.";
    case kPOFClosed:
        return "Operation attempted on closed file.";
    case kPOFBadMagic:
        return "Incorrect Magic number. The object file is invalid.";
    case kPOFBadVersion:
        return "Incorrect file version. The object file is invalid.";
    case kPOFNoFile:
        return "The file does not exist.";
    case kPOFAccessDenied:
        return "File permission is denied.";
    case kPOFBadFunction:
        return "Invalid function was added.";
    case kPOFNoMapping: {
        MfString message("Cannot create file mapping: ");
        char *text;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      0, systemError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char *)&text, 0, 0);
        MfString reason(text);
        reason.NoTrailingWhiteSpace();
        LocalFree(text);
        message << reason;
        return message;
    }
    case kPOFBadModuleTable:
        return "Object file module name section is corrupt.";
    case kPOFNoModule:
        return "Cannot file module name in object file.";
    case kPOFNoName:
        return "The file name is an empty string.";
    }
    return "Unknown Piccolo object file error.";
}

// 0x10013c90 ?ECToString@MfScPiccoloObjectFile@@QAE?AVMfString@@XZ
MfString MfScPiccoloObjectFile::ECToString()
{
    return ECToString(mError, mSystemError);
}

// 0x10013cc0 ?OpenFileMappingA@MfScPiccoloObjectFile@@AAEHK@Z
int MfScPiccoloObjectFile::OpenFileMappingA(unsigned long size)
{
    if (!mWrite)
        mFile = CreateFile(mName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    else {
        SetFileAttributes(mName, GetFileAttributes(mName) & ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM));
        mFile = CreateFile(mName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    }
    if (mFile == INVALID_HANDLE_VALUE) {
        mSystemError = ::GetLastError();
        mFile = 0;
        return 0;
    }
    mMapping = CreateFileMapping(mFile, 0, mWrite == 1 ? PAGE_READWRITE : PAGE_READONLY, 0, size, 0);
    if (!mMapping) {
        mSystemError = ::GetLastError();
        mFile = 0;
        CloseHandle(0);
        return 0;
    }
    mBase = (char *)MapViewOfFile(mMapping, mWrite == 1 ? FILE_MAP_WRITE : FILE_MAP_READ, 0, 0, 0);
    if (!mBase) {
        mSystemError = ::GetLastError();
        CloseHandle(mMapping);
        CloseHandle(mFile);
        mMapping = 0;
        mFile = 0;
        return 0;
    }
    return 1;
}

// 0x10013de0 ?CloseFileMapping@MfScPiccoloObjectFile@@AAEXXZ
void MfScPiccoloObjectFile::CloseFileMapping()
{
    if (mBase) {
        UnmapViewOfFile(mBase);
        mBase = 0;
    }
    if (mMapping) {
        CloseHandle(mMapping);
        mMapping = 0;
    }
    if (mFile) {
        CloseHandle(mFile);
        mFile = 0;
    }
}

// 0x10013e30 ?GrabFunctions@MfScPiccoloObjectFile@@QAE?AW4MfScPOFErrorCode@1@PAVMfScHandleList@@@Z
MfScPiccoloObjectFile::MfScPOFErrorCode MfScPiccoloObjectFile::GrabFunctions(MfScHandleList *functions)
{
    if (!mOpen)
        return SetLastError(kPOFClosed);
    if (mWrite == 1)
        return SetLastError(kPOFWriteOnly);
    char *section = mBase + *(unsigned long *)(mBase + 0xc);
    char *record = section + 0xc;
    for (unsigned long i = 0; i < *(unsigned long *)(section + 8); i++) {
        MfScDataHandle *function = BuildFunction(record - mBase + 4);
        functions->Add(function);
        function->Release();
        record += *(unsigned long *)record;
    }
    mError = kPOFOk;
    return kPOFOk;
}

// 0x10013ee0 ?LookupFunctions@MfScPiccoloObjectFile@@QAE?AW4MfScPOFErrorCode@1@ABVMfString@@PAVMfScHandleList@@@Z
MfScPiccoloObjectFile::MfScPOFErrorCode MfScPiccoloObjectFile::LookupFunctions(const MfString &module,
                                                                               MfScHandleList *functions)
{
    functions->Clear();
    if (!mOpen)
        return SetLastError(kPOFClosed);
    if (mWrite == 1)
        return SetLastError(kPOFWriteOnly);
    MfIntList *offsets = (MfIntList *)mModules->Find(module);
    if (!offsets)
        return SetLastError(kPOFNoModule);
    MfDblListItem *pos;
    for (int offset = offsets->First(pos); pos; offset = offsets->Next(pos)) {
        MfScDataHandle *function = BuildFunction(offset);
        functions->Add(function);
        function->Release();
    }
    mError = kPOFOk;
    return kPOFOk;
}

// one module of the module name section: its name and the offsets of its
// functions
struct PObjModuleRecord {
    unsigned long mSize;            // +0x00 to the next record
    unsigned long mNameSize;        // +0x04 aligned
    char mName[4];                  // +0x08
};

// EQUIV 0x10013fa0 ?ReadModuleNameTable@MfScPiccoloObjectFile@@AAEHXZ: reviewed: one lea has base and index swapped ([edi+ecx+8] vs [ecx+edi+8]), same address
int MfScPiccoloObjectFile::ReadModuleNameTable()
{
    mModules->Clear();
    char *section = mBase + *(unsigned long *)(mBase + 8);
    PObjModuleRecord *record = (PObjModuleRecord *)(section + 0xc);
    for (unsigned long i = 0; i < *(unsigned long *)(section + 8); i++) {
        MfString name(record->mName);
        int *offsets = (int *)(record->mName + record->mNameSize);
        MfIntList *list = new MfIntList;
        mFunctions->Add(list);
        for (int j = 0; j < offsets[0]; j++)
            list->Append((void *)offsets[j + 1]);
        mModules->Add(name, list);
        record = (PObjModuleRecord *)((char *)record + record->mSize);
    }
    return 1;
}

// 0x100140d0 ??_GMfIntList@@UAEPAXI@Z

// EQUIV 0x100140f0 ?BuildFunction@MfScPiccoloObjectFile@@AAEPAVMfScDataHandle@@K@Z: reviewed: the original computes both name addresses before constructing the strings, lays out the debug-info branches in another order and keeps the registry on the stack; same calls, constants and flow
MfScDataHandle *MfScPiccoloObjectFile::BuildFunction(unsigned long offset)
{
    if (MfScPiccolo::bIsRunning) {
        MfAiFunctionRegistry *registry = MfScPiccolo::GetFunctionRegistry();
        if (registry) {
            unsigned long *record = (unsigned long *)(mBase + offset);
            MfString name(mBase + record[1] + 4);
            MfString module(mBase + record[2] + 4);
            MfScDataHandle *found = 0;
            if (registry->Find(module, name, &found))
                return found;
        }
    }
    unsigned long *record = (unsigned long *)(mBase + offset);
    MfScDataHandle *function = MfScDataHandle::CreateBytecodeFunction(MfString(mBase + record[1] + 4), record[0]);
    MfScBytecodeRoutine *routine = function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;
    routine->ModuleName() = mBase + record[2] + 4;
    routine->ModuleType() = mBase + record[3] + 4;
    routine->FunctionParams() = mBase + record[4] + 4;
    routine->ObjectFileName() = mName;
    unsigned long *code = (unsigned long *)(mBase + record[5]);
    memcpy(routine->AllocBytecodes(code[0]), code + 1, code[0]);
    unsigned long *pool = (unsigned long *)(mBase + record[6]);
    routine->AllocConstantPool(pool[0]);
    MfScDataHandle **constants = routine->GetConstantPool();
    PObjConstantPoolEntry *entry = (PObjConstantPoolEntry *)(pool + 1);
    for (unsigned long i = 0; i < pool[0]; i++) {
        if (entry->mType == 0) {
            constants[i] = 0;
            entry++;
        } else if (entry->mType == 0x10) {
            constants[i] = MfScDataHandle::CreateData(entry->mValue);
            entry++;
        } else if (entry->mType == 0x20) {
            constants[i] = MfScDataHandle::CreateData(*(float *)&entry->mValue);
            entry++;
        } else if (entry->mType == 0x100) {
            constants[i] = MfScDataHandle::CreateData(MfString((char *)(entry + 1)), 1);
            entry = (PObjConstantPoolEntry *)((char *)(entry + 1) + entry->mValue);
        }
    }
    MfScDebugInfo *info;
    if (!(mOptions & 2))
        info = GetDebugInfo(offset);
    else if (mDebugFile)
        info = mDebugFile->GetDebugInfo(routine->ModuleName(), routine->Name());
    else
        info = 0;
    routine->Attach(info);
    MfAiFunctionRegistry *registry = MfScPiccolo::GetFunctionRegistry();
    if (registry)
        registry->Add(function);
    return function;
}

// 0x10014410 ?GetDebugInfo@MfScPiccoloObjectFile@@QAEPAVMfScDebugInfo@@ABVMfString@@0@Z
MfScDebugInfo *MfScPiccoloObjectFile::GetDebugInfo(const MfString &module, const MfString &function)
{
    MfScDebugInfo *info = 0;
    MfIntList *offsets = (MfIntList *)mModules->Find(module);
    if (!offsets)
        return 0;
    MfDblListItem *pos;
    for (int offset = offsets->First(pos); pos && !info; offset = offsets->Next(pos)) {
        if (function == mBase + *(unsigned long *)(mBase + offset + 4) + 4)
            info = GetDebugInfo(offset);
    }
    return info;
}

// 0x100144a0 ?GetDebugInfo@MfScPiccoloObjectFile@@QAEPAVMfScDebugInfo@@I@Z
MfScDebugInfo *MfScPiccoloObjectFile::GetDebugInfo(unsigned int offset)
{
    unsigned long *record = (unsigned long *)(mBase + offset);
    int i;
    int *lines = (int *)SectionAddress(record[7]);
    int *vars = (int *)SectionAddress(record[8]);
    int *levels = (int *)SectionAddress(record[9]);
    int *consts = (int *)SectionAddress(record[10]);
    MfScDebugInfo *info = 0;
    if (lines && lines[0]) {
        info = info ? info : new MfScDebugInfo;
        if ((unsigned)lines[0] > 0)
            info->InitializeLineInfo(lines[0]);
        MfScLineInfo *line = (MfScLineInfo *)(lines + 1);
        for (i = 0; i < lines[0]; i++) {
            MfScLineInfo copy(*line);
            info->SetLineInfo(copy, i);
            line++;
        }
    }
    if (vars && vars[0]) {
        info = info ? info : new MfScDebugInfo;
        if ((unsigned)vars[0] > 0)
            info->InitializeVarDebugInfo(vars[0]);
        int *var = vars + 1;
        for (i = 0; i < vars[0]; i++) {
            MfScVariableDebugInfo debug(MfString((char *)(var + 3)), var[0], var[1]);
            info->SetVarDebugInfo(debug, i);
            var = (int *)((char *)var + var[2] + 0xc);
        }
    }
    if (levels && levels[0]) {
        info = info ? info : new MfScDebugInfo;
        if ((unsigned)levels[0] > 0)
            info->InitializeCodeLevelInfo(levels[0]);
        MfScCodeLevelInfo *level = (MfScCodeLevelInfo *)(levels + 1);
        for (i = 0; i < levels[0]; i++) {
            MfScCodeLevelInfo copy(*level);
            info->SetCodeLevelInfo(copy, i);
            level++;
        }
    }
    if (consts && consts[0]) {
        info = info ? info : new MfScDebugInfo;
        if ((unsigned)consts[0] > 0)
            info->InitializeConstUsageInfo(consts[0]);
        int *entry = consts + 1;
        for (i = 0; i < consts[0]; i++) {
            MfString name((char *)(entry + 1));
            PObjConstantPoolEntry *value = (PObjConstantPoolEntry *)((char *)entry + entry[0] + 4);
            MfVariant variant;
            if (value->mType == 0) {
                variant = 0;
                entry = (int *)(value + 1);
            } else if (value->mType == 0x10) {
                variant = value->mValue;
                entry = (int *)(value + 1);
            } else if (value->mType == 0x20) {
                variant = *(float *)&value->mValue;
                entry = (int *)(value + 1);
            } else if (value->mType == 0x100) {
                variant = MfString((char *)(value + 1));
                entry = (int *)((char *)(value + 1) + value->mValue);
            }
            MfScConstUsageInfo usage(name, variant);
            info->SetConstUsageInfo(usage, i);
        }
    }
    return info;
}

// 0x10014930 ?ReadClose@MfScPiccoloObjectFile@@QAE?AW4MfScPOFErrorCode@1@XZ
MfScPiccoloObjectFile::MfScPOFErrorCode MfScPiccoloObjectFile::ReadClose()
{
    if (!mOpen)
        return SetLastError(kPOFClosed);
    if (mWrite == 1)
        return SetLastError(kPOFWriteOnly);
    if (mDebugFile) {
        delete mDebugFile;
        mDebugFile = 0;
    }
    CloseFileMapping();
    mOpen = 0;
    Cleanup();
    mError = kPOFOk;
    return kPOFOk;
}

// 0x10014990 ?AddFunction@MfScPiccoloObjectFile@@QAE?AW4MfScPOFErrorCode@1@PAVMfScDataHandle@@@Z
MfScPiccoloObjectFile::MfScPOFErrorCode MfScPiccoloObjectFile::AddFunction(MfScDataHandle *function)
{
    if (!mOpen)
        return SetLastError(kPOFClosed);
    if (!mWrite)
        return SetLastError(kPOFReadOnly);
    if (function->IsType(0x800)) {
        MfScBytecodeRoutine *routine = function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;
        if (routine && routine->Type() == MfScDataMethod::kNone) {
            MfScHandleList *list = (MfScHandleList *)mModules->Find(routine->ModuleName());
            if (!list) {
                list = new MfScHandleList;
                mFunctions->Add(list);
                mModules->Add(routine->ModuleName(), list);
            }
            list->Add(function);
            mError = kPOFOk;
            return kPOFOk;
        }
    }
    return SetLastError(kPOFBadFunction);
}

// 0x10014a80 ?WriteConstantPoolEntry@MfScPiccoloObjectFile@@AAEPAUPObjConstantPoolEntry@1@PAU21@H@Z
MfScPiccoloObjectFile::PObjConstantPoolEntry *MfScPiccoloObjectFile::WriteConstantPoolEntry(PObjConstantPoolEntry *entry, int value)
{
    entry->mType = 0x10;
    entry->mValue = value;
    return entry + 1;
}

// 0x10014aa0 ?WriteConstantPoolEntry@MfScPiccoloObjectFile@@AAEPAUPObjConstantPoolEntry@1@PAU21@M@Z
MfScPiccoloObjectFile::PObjConstantPoolEntry *MfScPiccoloObjectFile::WriteConstantPoolEntry(PObjConstantPoolEntry *entry, float value)
{
    entry->mType = 0x20;
    *(float *)&entry->mValue = value;
    return entry + 1;
}

// 0x10014ac0 ?WriteConstantPoolEntry@MfScPiccoloObjectFile@@AAEPAUPObjConstantPoolEntry@1@PAU21@ABVMfString@@@Z
MfScPiccoloObjectFile::PObjConstantPoolEntry *MfScPiccoloObjectFile::WriteConstantPoolEntry(PObjConstantPoolEntry *entry,
                                                                                            const MfString &value)
{
    entry->mType = 0x100;
    entry->mValue = AlignedLength(value.Length() + 1);
    DumpData(entry + 1, (const char *)value, value.Length() + 1);
    return (PObjConstantPoolEntry *)((char *)(entry + 1) + entry->mValue);
}

// EQUIV 0x10014b10 ?WriteClose@MfScPiccoloObjectFile@@QAE?AW4MfScPOFErrorCode@1@XZ: reviewed: register/slot choice and scheduling; the original calls the imported MfDblList::GetFirst/GetNext (inline in our MfDblList.h, same as mfutil's) and _ftime (ours ftime, the oldnames alias); same calls, constants and flow
//
// Layout of the file written here:
//   header (0x24 bytes): magic "MJSP", version, the offsets of the module
//     name, function and object file info sections, four reserved -1 words
//   each section: type (1 modules, 2 functions, 3 info), size, entries
//   module record: PObjModuleRecord, then the count and the offsets of the
//     module's functions (each points to a function record + 4)
//   function record: size, then 12 words (params, the offsets of name,
//     module, type and parameter strings, bytecode, constant pool, line,
//     variable, code level and constant symbol tables, 0xa0a0a0a0), then
//     the data they point to; a string is its aligned length + the text.
//     A debug table of a function whose debug info went to the .pdbg file is
//     0xdbdbdbdb.
MfScPiccoloObjectFile::MfScPOFErrorCode MfScPiccoloObjectFile::WriteClose()
{
    if (!mOpen)
        return SetLastError(kPOFClosed);
    if (!mWrite)
        return SetLastError(kPOFReadOnly);
    unsigned int moduleSize = ComputeModuleNameSectionSize();
    unsigned int functionSize = ComputeFunctionSectionSize();
    unsigned int infoSize = ComputeObjectFileInfoSectionSize();
    if (!OpenFileMappingA(0x48 + moduleSize + functionSize + infoSize)) {
        CloseFileMapping();
        mOpen = 0;
        Cleanup();
        return SetLastError(kPOFNoMapping);
    }

    unsigned long *header = (unsigned long *)mBase;
    header[3] = 0x24 + 0xc + moduleSize;
    header[4] = header[3] + 0xc + functionSize;
    header[0] = 0x50534a4d;
    header[1] = 0x20a0000;      // 2.10.0
    header[2] = 0x24;
    memset(&header[5], 0xff, 4 * sizeof(unsigned long));

    unsigned long *moduleSection = (unsigned long *)(mBase + header[2]);
    moduleSection[0] = 1;
    moduleSection[1] = moduleSize + 0xc;
    moduleSection[2] = 0;
    unsigned long *functionSection = (unsigned long *)(mBase + header[3]);
    functionSection[0] = 2;
    functionSection[1] = functionSize + 0xc;
    functionSection[2] = 0;
    PObjModuleRecord *module = (PObjModuleRecord *)(moduleSection + 3);
    unsigned long *entry = functionSection + 3;

    MfDblListItem *pos;
    for (MfScHandleList *list = (MfScHandleList *)mFunctions->GetFirst(pos); list;
         list = (MfScHandleList *)mFunctions->GetNext(pos)) {
        MfScDataHandle *first = (MfScDataHandle *)list->MfDblList::GetFirst(list->CurrentPos());
        MfScBytecodeRoutine *routine = first ? dynamic_cast<MfScBytecodeRoutine *>(first->GetData()) : 0;
        module->mSize = AlignedLength(routine->ModuleName().Length() + 1) + list->GetCount() * 4 + 0xc;
        module->mNameSize = AlignedLength(routine->ModuleName().Length() + 1);
        DumpData(module->mName, (const char *)routine->ModuleName(), routine->ModuleName().Length() + 1);
        unsigned long *offsets = (unsigned long *)(module->mName + module->mNameSize);
        offsets[0] = list->GetCount();
        moduleSection[2]++;
        module = (PObjModuleRecord *)((char *)module + module->mSize);

        unsigned long *offset = offsets + 1;
        for (MfScDataHandle *function = list->First(); function;
             function = (MfScDataHandle *)list->MfDblList::GetNext(list->CurrentPos()), offset++) {
            routine = function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;
            unsigned long *record = entry + 1;
            *offset = (char *)record - mBase;
            char *p = (char *)(record + 12);
            record[11] = 0xa0a0a0a0;
            record[0] = routine->Params();
            *(unsigned long *)p = AlignedLength(routine->Name().Length() + 1);
            DumpData(p + 4, (const char *)routine->Name(), routine->Name().Length() + 1);
            record[1] = p - mBase;
            p += *(unsigned long *)p + 4;
            *(unsigned long *)p = AlignedLength(routine->ModuleName().Length() + 1);
            DumpData(p + 4, (const char *)routine->ModuleName(), routine->ModuleName().Length() + 1);
            record[2] = p - mBase;
            if (mType == kDebug) {
                // a debug file keeps only names and the code size
                MfString empty;
                p += *(unsigned long *)p + 4;
                *(unsigned long *)p = AlignedLength(1);
                DumpData(p + 4, (const char *)empty, empty.Length() + 1);
                record[3] = p - mBase;
                p += *(unsigned long *)p + 4;
                *(unsigned long *)p = AlignedLength(1);
                DumpData(p + 4, (const char *)empty, empty.Length() + 1);
                record[4] = p - mBase;
                p += *(unsigned long *)p + 4;
                *(unsigned long *)p = routine->mCodeSize;
                record[5] = p - mBase;
                p += 4;
                *(unsigned long *)p = 0;
                record[6] = p - mBase;
                p += 4;
            } else {
                p += *(unsigned long *)p + 4;
                *(unsigned long *)p = AlignedLength(routine->ModuleType().Length() + 1);
                DumpData(p + 4, (const char *)routine->ModuleType(), routine->ModuleType().Length() + 1);
                record[3] = p - mBase;
                p += *(unsigned long *)p + 4;
                *(unsigned long *)p = AlignedLength(routine->FunctionParams().Length() + 1);
                DumpData(p + 4, (const char *)routine->FunctionParams(), routine->FunctionParams().Length() + 1);
                record[4] = p - mBase;
                p += *(unsigned long *)p + 4;
                *(unsigned long *)p = AlignedLength(routine->mCodeSize);
                DumpData(p + 4, routine->mBytecodes, routine->mCodeSize);
                record[5] = p - mBase;
                unsigned long *pool = (unsigned long *)(p + *(unsigned long *)p + 4);
                *pool = routine->ConstantPoolSize();
                PObjConstantPoolEntry *constant = (PObjConstantPoolEntry *)(pool + 1);
                MfScDataHandle **constants = routine->GetConstantPool();
                for (int i = 0; i < routine->ConstantPoolSize(); i++) {
                    MfScDataHandle *h = constants[i];
                    if (!h)
                        constant = WriteConstantPoolEntry(constant, 0);
                    else if (h->IsType(0x10))
                        constant = WriteConstantPoolEntry(constant, h->GetInt());
                    else if (h->IsType(0x20))
                        constant = WriteConstantPoolEntry(constant, h->GetFloat());
                    else if (h->IsType(0x100))
                        constant = WriteConstantPoolEntry(constant, *(MfString *)(MfScObjString *)h->GetData());
                }
                record[6] = (char *)pool - mBase;
                p = (char *)constant;
            }

            // debug tables: each is its entry count and the entries
            MfScDebugInfo *debug = routine->DebugInfo();
            unsigned long *table = (unsigned long *)p;
            MfScLineInfo *line = (MfScLineInfo *)(table + 1);
            record[7] = (char *)table - mBase;
            if (debug && (~mOptions & 1) && (~mOptions & 2)) {
                *table = debug->NumLineInfo();
                for (int i = 0; i < debug->NumLineInfo(); i++) {
                    *line = debug->GetLineInfo(i);
                    line++;
                }
            } else {
                *table = 0;
                if (debug && (mOptions & 2))
                    record[7] = 0xdbdbdbdb;
            }

            table = (unsigned long *)line;
            unsigned long *var = table + 1;
            record[8] = (char *)table - mBase;
            if (debug && (~mOptions & 1) && (~mOptions & 2)) {
                *table = debug->NumVarDebugInfo();
                for (int i = 0; i < debug->NumVarDebugInfo(); i++) {
                    MfScVariableDebugInfo info(debug->GetVarDebugInfo(i));
                    var[0] = info.mSlot;
                    var[1] = info.mLevel;
                    var[2] = AlignedLength(info.mName.Length() + 1);
                    DumpData(var + 3, (const char *)info.mName, info.mName.Length() + 1);
                    var = (unsigned long *)((char *)var + var[2] + 0xc);
                }
            } else {
                *table = 0;
                if (debug && (mOptions & 2))
                    record[8] = 0xdbdbdbdb;
            }

            table = var;
            MfScCodeLevelInfo *level = (MfScCodeLevelInfo *)(table + 1);
            record[9] = (char *)table - mBase;
            if (debug && (~mOptions & 1) && (~mOptions & 2)) {
                *table = debug->NumCodeLevelInfo();
                for (int i = 0; i < debug->NumCodeLevelInfo(); i++) {
                    const MfScCodeLevelInfo &info = debug->GetCodeLevelInfo(i);
                    level->mOffset = info.mOffset;
                    level->mLevel = info.mLevel;
                    level++;
                }
            } else {
                *table = 0;
                if (debug && (mOptions & 2))
                    record[9] = 0xdbdbdbdb;
            }

            // without debug info the end stays at the (zero) count: the
            // record's size leaves it out, the next record overwrites it
            table = (unsigned long *)level;
            p = (char *)table;
            record[10] = (char *)table - mBase;
            if (debug && (~mOptions & 1) && (~mOptions & 2)) {
                p = (char *)(table + 1);
                *table = debug->NumConstUsageInfo();
                for (int i = 0; i < debug->NumConstUsageInfo(); i++) {
                    const MfScConstUsageInfo &info = debug->GetConstUsageInfo(i);
                    MfScConstUsageInfo usage(info.Name(), info.Value());
                    *(unsigned long *)p = AlignedLength(usage.mName.Length() + 1);
                    DumpData(p + 4, (const char *)usage.mName, usage.mName.Length() + 1);
                    p += *(unsigned long *)p + 4;
                    switch (usage.mValue.Type()) {
                    case MfVariant::kInt:
                    case MfVariant::kLong:
                        p = (char *)WriteConstantPoolEntry((PObjConstantPoolEntry *)p, (int)usage.mValue);
                        break;
                    case MfVariant::kFloat:
                    case MfVariant::kDouble:
                        p = (char *)WriteConstantPoolEntry((PObjConstantPoolEntry *)p, (float)usage.mValue);
                        break;
                    case MfVariant::kString:
                        p = (char *)WriteConstantPoolEntry((PObjConstantPoolEntry *)p, usage.mValue.ToString());
                        break;
                    default:
                        p = (char *)WriteConstantPoolEntry((PObjConstantPoolEntry *)p, 0);
                        break;
                    }
                }
            } else {
                *table = 0;
                if (debug && (mOptions & 2))
                    record[10] = 0xdbdbdbdb;
            }

            entry[0] = p - (char *)record + 4;
            entry = (unsigned long *)((char *)entry + entry[0]);
            functionSection[2]++;
        }
    }

    unsigned long *info = (unsigned long *)(mBase + header[4]);
    info[0] = 3;
    info[1] = infoSize + 0xc;
    info[2] = 1;
    info[3] = 0x38;
    info[4] = mType;
    info[5] = mOptions;
    MfTime now;
    now.Update();
    info[6] = now.Seconds();
    memset(&info[7], 0xb0, 10 * sizeof(unsigned long));

    // the debug information goes to a .pdbg file of its own
    if (mOptions & 2) {
        MfScPiccoloObjectFile debugFile;
        debugFile.Open(mName, 1, kDebug, 0);
        for (MfScHandleList *list = (MfScHandleList *)mFunctions->GetFirst(pos); list;
             list = (MfScHandleList *)mFunctions->GetNext(pos))
            debugFile.AddFunction(list->First());
        debugFile.WriteClose();
    }
    CloseFileMapping();
    mOpen = 0;
    Cleanup();
    mError = kPOFOk;
    return kPOFOk;
}

// 0x100155e0 ?DumpData@MfScPiccoloObjectFile@@AAEXPAXPBXH@Z
void MfScPiccoloObjectFile::DumpData(void *to, const void *from, int size)
{
    memcpy(to, from, size);
    char *end = (char *)to + size;
    int aligned = AlignedLength(size);
    if (size < aligned)
        memset(end, 0, aligned - size);
}

// EQUIV 0x10015630 ?ComputeModuleNameSectionSize@MfScPiccoloObjectFile@@AAEIXZ: reviewed: register choice only (the original reuses eax for the module name)
unsigned int MfScPiccoloObjectFile::ComputeModuleNameSectionSize()
{
    unsigned int size = 0;
    MfDblListItem *pos;
    for (MfScHandleList *list = (MfScHandleList *)mFunctions->GetFirst(pos); list;
         list = (MfScHandleList *)mFunctions->GetNext(pos)) {
        MfScDataHandle *first = list->First();
        MfScBytecodeRoutine *routine = first ? dynamic_cast<MfScBytecodeRoutine *>(first->GetData()) : 0;
        size += 0xc + AlignedLength(routine->ModuleName().Length() + 1) + list->GetCount() * 4;
    }
    return size;
}

// EQUIV 0x100156d0 ?ComputeFunctionSectionSize@MfScPiccoloObjectFile@@AAEIXZ: reviewed: the original calls the imported MfDblList::GetFirst/GetNext where ours inlines them (same bodies as mfutil's MfDblList.h); otherwise slots and scheduling
unsigned int MfScPiccoloObjectFile::ComputeFunctionSectionSize()
{
    unsigned int size = 0;
    MfDblListItem *pos;
    for (MfScHandleList *list = (MfScHandleList *)mFunctions->GetFirst(pos); list;
         list = (MfScHandleList *)mFunctions->GetNext(pos)) {
        for (MfScDataHandle *function = (MfScDataHandle *)list->MfDblList::GetFirst(list->CurrentPos()); function;
             function = (MfScDataHandle *)list->MfDblList::GetNext(list->CurrentPos())) {
            MfScBytecodeRoutine *routine = dynamic_cast<MfScBytecodeRoutine *>(function->GetData());
            size += 0x34;
            size += AlignedLength(routine->Name().Length() + 1) + 4;
            size += AlignedLength(routine->ModuleName().Length() + 1) + 4;
            if (mType == kDebug)
                size += 0x18;
            else {
                size += AlignedLength(routine->ModuleType().Length() + 1) + 4;
                size += AlignedLength(routine->FunctionParams().Length() + 1) + 4;
                size += AlignedLength(routine->mCodeSize) + 8;
                MfScDataHandle **constants = routine->GetConstantPool();
                for (int i = 0; i < routine->ConstantPoolSize(); i++) {
                    MfScDataHandle *constant = constants[i];
                    size += 4;
                    if (!constant)
                        size += 4;
                    else if (constant->IsType(0x10))
                        size += 4;
                    else if (constant->IsType(0x20))
                        size += 4;
                    else if (constant->IsType(0x100))
                        size += AlignedLength(((MfScObjString *)constant->GetData())->Length() + 1) + 4;
                }
            }
            MfScDebugInfo *debug = routine->DebugInfo();
            size += 0x10;
            if (debug && (~mOptions & 1) && (~mOptions & 2)) {
                int i;
                size += debug->NumLineInfo() * 8;
                size += debug->NumVarDebugInfo() * 8;
                for (i = 0; i < debug->NumVarDebugInfo(); i++) {
                    MfScVariableDebugInfo var;
                    var = debug->GetVarDebugInfo(i);
                    size += AlignedLength(var.mName.Length() + 1) + 4;
                }
                size += debug->NumCodeLevelInfo() * 8;
                for (i = 0; i < debug->NumConstUsageInfo(); i++) {
                    const MfScConstUsageInfo &info = debug->GetConstUsageInfo(i);
                    MfScConstUsageInfo usage(info.Name(), info.Value());
                    size += AlignedLength(usage.mName.Length() + 1) + 8;
                    int type = usage.mValue.Type();
                    if (type < MfVariant::kFloat)
                        size += 4;
                    else if (type <= MfVariant::kDouble)
                        size += 4;
                    else if (type == MfVariant::kString)
                        size += AlignedLength(usage.mValue.ToString().Length() + 1) + 4;
                    else
                        size += 4;
                }
            }
        }
    }
    return size;
}

// EQUIV 0x10015a30 ?DumpFile@MfScPiccoloObjectFile@@QAEXPAVMfStringList@@H@Z: reviewed: the original frame is 4 bytes smaller and loads the minor version byte with xor/mov (ours masks a dword); register choice and addressing otherwise; same calls and strings
void MfScPiccoloObjectFile::DumpFile(MfStringList *lines, int flags)
{
    lines->Clear();
    if (mWrite == 1 || !mOpen)
        return;
    MfString line;
    unsigned long *header = (unsigned long *)mBase;
    if (flags & 1) {
        line.Format("OBJECT FILE HEADER:");
        lines->Add(line);
        line.Format("             Magic Number: %08x", header[0]);
        lines->Add(line);
        union {
            unsigned long all;
            unsigned char part[4];
        } version;
        version.all = header[1];
        line.Format("                  Version: %d.%02d.%04d", version.all >> 24, version.part[2], version.all & 0xffff);
        lines->Add(line);
        line.Format("    Module Section Offset: %08x", header[2]);
        lines->Add(line);
        line.Format("  Function Section Offset: %08x", header[3]);
        lines->Add(line);
        line.Format("  Object File Info Offset: %08x", header[4]);
        lines->Add(line);
    }
    if ((flags & 8) && header[4] != (unsigned long)-1 && header[4] != 0) {
        if (lines->GetCount() > 0)
            lines->Add(MfString(""));
        line.Format("OBJECT FILE INFOMATION:");
        lines->Add(line);
        unsigned long *entry = (unsigned long *)(mBase + header[4] + 0xc);
        line.Format(" Section Entry %3d Offset: %08x", 1, (char *)entry - mBase);
        lines->Add(line);
        line.Format(" Section Entry %3d   Size: %08x", 1, entry[0]);
        lines->Add(line);
        unsigned long *info = (unsigned long *)(header[4] + mBase + 0x10);
        line.Format("  Object File Type: %d", info[0]);
        lines->Add(line);
        line.Format(" Object File Flags: %08x", info[1]);
        lines->Add(line);
        MfString time;
        MfTime stamp(info[2], 0);
        time.FormatTime("%H:%M:%S %a %b %d, %Y", stamp);
        line.Format("        Time Stamp: %08x ", info[2]);
        line << "(" << time << ")";
        lines->Add(line);
    }
    if (flags & 2) {
        if (lines->GetCount() > 0)
            lines->Add(MfString(""));
        line.Format("MODULE NAME SECTION:");
        lines->Add(line);
        unsigned long *section = (unsigned long *)(mBase + header[2]);
        line.Format("     Size: %08x", section[1]);
        lines->Add(line);
        line.Format("  Entries: %d", section[2]);
        lines->Add(line);
        line.Format("");
        lines->Add(line);
        PObjModuleRecord *record = (PObjModuleRecord *)(section + 3);
        for (unsigned long i = 0; i < section[2]; i++) {
            line.Format(" Section Entry %3d Offset: %08x", i + 1, (char *)record - mBase);
            lines->Add(line);
            line.Format(" Section Entry %3d   Size: %08x", i + 1, record->mSize);
            lines->Add(line);
            line.Format("       Module: %s", record->mName);
            lines->Add(line);
            int *offsets = (int *)(record->mName + record->mNameSize);
            line.Format("      Offsets: %d", offsets[0]);
            lines->Add(line);
            for (int j = 0; j < offsets[0]; j++) {
                int offset = offsets[j + 1];
                line.Format("   Offset %3d: %08x (%s)", j + 1, offset, mBase + *(unsigned long *)(mBase + offset + 4) + 4);
                lines->Add(line);
            }
            line.Format("");
            lines->Add(line);
            record = (PObjModuleRecord *)((char *)record + record->mSize);
        }
    }
    if (flags & 4) {
        if (lines->GetCount() > 0)
            lines->Add(MfString(""));
        line.Format("FUNCTION BODY SECTION:");
        lines->Add(line);
        unsigned long *section = (unsigned long *)(mBase + header[3]);
        line.Format("     Size: %08x", section[1]);
        lines->Add(line);
        line.Format("  Entries: %d", section[2]);
        lines->Add(line);
        line.Format("");
        lines->Add(line);
        unsigned long *record = section + 3;
        for (unsigned long i = 0; i < section[2]; i++) {
            line.Format(" Section Entry %3d Offset: %08x", i + 1, (char *)record - mBase);
            lines->Add(line);
            line.Format(" Section Entry %3d   Size: %08x", i + 1, record[0]);
            lines->Add(line);
            line.Format("     Number of Parameters: %d", record[1]);
            lines->Add(line);
            line.Format("              Name Offset: %08x (%s)", record[2], mBase + record[2] + 4);
            lines->Add(line);
            line.Format("            Module Offset: %08x (%s)", record[3], mBase + record[3] + 4);
            lines->Add(line);
            line.Format("              Type Offset: %08x (%s)", record[4], mBase + record[4] + 4);
            lines->Add(line);
            line.Format("      Param String Offset: %08x (%s)", record[5], mBase + record[5] + 4);
            lines->Add(line);
            line.Format("          Bytecode Offset: %08x (%d bytecodes)", record[6], *(unsigned long *)(mBase + record[6]));
            lines->Add(line);
            line.Format("     Constant Pool Offset: %08x (%d constants)", record[7], *(unsigned long *)(mBase + record[7]));
            lines->Add(line);
            if (record[8] != 0xa0a0a0a0 && record[8] != 0xdbdbdbdb)
                line.Format("      Line Numbers Offset: %08x (%d entries)", record[8], *(unsigned long *)(mBase + record[8]));
            else
                line.Format("      Line Numbers Offset: %s", record[8] == 0xa0a0a0a0 ? "UNIMPLEMENTED" : "DEBUG FILE");
            lines->Add(line);
            if (record[9] != 0xa0a0a0a0 && record[9] != 0xdbdbdbdb)
                line.Format("   Debug Variables Offset: %08x (%d entries)", record[9], *(unsigned long *)(mBase + record[9]));
            else
                line.Format("   Debug Variables Offset: %s", record[9] == 0xa0a0a0a0 ? "UNIMPLEMENTED" : "DEBUG FILE");
            lines->Add(line);
            if (record[10] != 0xa0a0a0a0 && record[10] != 0xdbdbdbdb)
                line.Format("       Code Levels Offset: %08x (%d entries)", record[10], *(unsigned long *)(mBase + record[10]));
            else
                line.Format("       Code Levels Offset: %s", record[10] == 0xa0a0a0a0 ? "UNIMPLEMENTED" : "DEBUG FILE");
            lines->Add(line);
            if (record[11] != 0xa0a0a0a0 && record[11] != 0xdbdbdbdb)
                line.Format("  Constant Symbols Offset: %08x (%d entries)", record[11], *(unsigned long *)(mBase + record[11]));
            else
                line.Format("  Constant Symbols Offset: %s", record[11] == 0xa0a0a0a0 ? "UNIMPLEMENTED" : "DEBUG FILE");
            lines->Add(line);
            line.Format("");
            lines->Add(line);
            record = (unsigned long *)((char *)record + record[0]);
        }
    }
}
