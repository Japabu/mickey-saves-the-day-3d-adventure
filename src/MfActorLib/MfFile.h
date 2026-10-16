// MfFile.h - binary object files of MfActorLib (exported classes MfFileType,
// MfFileObject, MfFile, MfFileProgress). Shared by the object that emits the
// inline members (MfFile.cpp, 0x10001880-) and the one that defines MfFile
// (MfFile_2.cpp, 0x10006c70-0x10007c10).
#pragma once

// <windows.h> comes first: the export is RegisterClassA, renamed by its macro.
#include <windows.h>
#include "MfString.h"
#include "MfDblList.h"
#include "enums.h"

extern __declspec(dllimport) MfNullString NULL_STRING;

// mfutil.dll; every member is inline.
class __declspec(dllimport) MfCriticalSection {
public:
    MfCriticalSection() { InitializeCriticalSection(&m_cs); }
    ~MfCriticalSection() { DeleteCriticalSection(&m_cs); }
    void Enter() { EnterCriticalSection(&m_cs); }
    void Leave() { LeaveCriticalSection(&m_cs); }
private:
    CRITICAL_SECTION m_cs;
};

class MfFile;

// A four-character code ('    ' when unset).
class __declspec(dllexport) MfFileType {
public:
    MfFileType(long t) { m_type = t; }
    MfFileType(MfString);
    ~MfFileType() {}
    int operator==(MfFileType t) { return m_type == t.m_type; }
    int operator!=(MfFileType t) { return m_type != t.m_type; }
    operator long() { return m_type; }
    MfFileType &operator=(const MfFileType &t) { m_type = t.m_type; return *this; }
private:
    long m_type;
};

class __declspec(dllexport) MfFileObject {
public:
    MfFileObject() {}
    virtual ~MfFileObject() {}
    static MfFileType ClassTypeInFile() { return MfFileType(0x20202020); }
    virtual MfFileType TypeInFile() { return ClassTypeInFile(); }
    virtual MfString NameInFile() { return NULL_STRING; }
    static MfFileObject *ConstructFromFile(MfFile &) { return 0; }
    virtual int ReadFromFile(MfFile &) = 0;
    virtual int WriteToFile(MfFile &) = 0;
};

typedef MfFileObject *(*MfFileObjectConstructor)(MfFile &);


// m_lastError values
enum MfFileError {
    MF_FILE_OK = 0,
    MF_FILE_NOT_OPEN = 1,       // also: wrong mode, cannot create
    MF_FILE_BAD_FORMAT = 2,     // also: bad mode
    MF_FILE_ALREADY_OPEN = 3,
    MF_FILE_IO_ERROR = 4,
    MF_FILE_WRONG_TYPE = 5
};

#define MF_FILE_MAGIC 'BFM '
#define MF_FILE_BUFFER_SIZE 0x800

class __declspec(dllexport) MfFile {
public:
    MfFile();
    virtual ~MfFile();
    virtual int Open(const MfString &, int);
    virtual int Open(int, char *, int);
    virtual int Close();
    virtual unsigned long ReadBytes(unsigned long, void *);
    virtual unsigned long WriteBytes(unsigned long, const void *);

    MfFileType GetType() { return m_type; }
    void SetType(MfFileType t) { m_type = t; }
    unsigned short GetVersion() { return m_version; }
    void SetVersion(unsigned short v) { m_version = v; }
    MfString Filename() { return m_filename; }
    MfFileType GetObjectType() { return m_objectType; }
    int GetLastError() { return m_lastError; }
    char *GetBuffer(int &n) { n = m_pos; return m_buffer; }

    int PeekType(MfFileType &);
    int ReadType(MfFileType &);
    int Read(MfFileObject &);
    int Read(int, MfFileObject *);
    MfFileObject *ReadObject();
    int Read(short &);
    int Read(int &);
    int Read(float &);
    int Read(MfString &);
    int Write(MfFileObject &);
    int Write(int, MfFileObject *);
    int WriteTypeOf(MfFileObject &);
    int Write(short);
    int Write(int);
    int Write(float);
    int Write(MfString);
    int Compress();
    int Uncompress();
    static void RegisterClass(MfFileObjectConstructor, MfFileType);
protected:
    int SetError(int e) { m_lastError = e; return e == 0; }
    int ReadHeader();
    int WriteHeader();
    unsigned long GetFilePointer();
    void SetFilePointer(unsigned long, unsigned long);
    MfFileObjectConstructor FindConstructor(MfFileType);

    static MfDblList lRegisteredClasses;

    HANDLE m_handle;            // 0x04
    int m_isOpen;               // 0x08
    int m_inMemory;             // 0x0c
    int m_lastError;            // 0x10
    int m_mode;                 // 0x14 MfFileMode
    MfFileType m_type;          // 0x18
    unsigned short m_format;    // 0x1c (2)
    unsigned short m_version;   // 0x1e
    MfString m_filename;        // 0x20
    MfFileType m_objectType;    // 0x24
    char *m_buffer;             // 0x28
    unsigned long m_bufferSize; // 0x2c
    unsigned long m_pos;        // 0x30
};

class __declspec(dllexport) MfFileProgress {
public:
    static void GetProgress(int &, MfString &);
    static void SetProgress(int, MfString);
    static void SetProgressCB(void (*)());
    static void UpdateProgress();
protected:
    static MfCriticalSection lock;
    static void (*callback)();
};
