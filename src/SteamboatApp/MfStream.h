#pragma once
// MfStream.h: Motivate's binary streams (declarations from SteamboatApp.pdb; the game calls them
// through the import thunks).
#include "MfTypedValue.h"
#include "MfString.h"

class MfCallBack;

enum MfFileMode {
    MF_FILE_READ = 0,
    MF_FILE_WRITE = 1
};

class MfStream {
public:
    MfStream(const MfStream &);
    MfStream();
    ~MfStream();
    MfStreamError Status();
    void SetStatus(MfStreamError);
    void SetCallBack(MfCallBack *);
    MfStream &operator>>(MfStreamObject &);
    MfStream &operator>>(MfString &);
    MfStream &operator>>(double &);
    MfStream &operator>>(float &);
    MfStream &operator>>(long &);
    MfStream &operator>>(int &);
    MfStream &operator>>(short &);
    MfStream &operator>>(char &);
    MfStream &operator>>(unsigned char &);
    static MfStreamObject *__cdecl CreateObject(long);
    MfStreamObject *ReadObject(MfStreamObject *(__cdecl *)());
    MfStreamError WriteObject(MfStreamObject *);
    virtual MfStreamError ReadBytes(unsigned long, void *) = 0;
    virtual MfStreamError WriteBytes(unsigned long, const void *) = 0;
    virtual MfStreamError Flush() = 0;
    virtual MfStreamError Seek(int) = 0;
    int userFormat;                                     // +0x0004
    long objectType;                                    // +0x0008
    int objectSize;                                     // +0x000c
    void *userData;                                     // +0x0010
protected:
    MfStreamError status;                               // +0x0014
    MfCallBack *cb;                                     // +0x0018
};

class MfFileStream : public MfStream {
public:
    MfFileStream(const MfFileStream &);
    MfFileStream(void *, MfFileMode, bool);
    MfFileStream(const MfString &, MfFileMode, bool);
    ~MfFileStream();
    virtual MfStreamError ReadBytes(unsigned long, void *);
    virtual MfStreamError WriteBytes(unsigned long, const void *);
    virtual MfStreamError Flush();
    virtual MfStreamError Seek(int);
protected:
    void *fileHandle;                                   // +0x001c
    bool closeOnDelete;                                 // +0x0020
    char *buffer;                                       // +0x0024
    unsigned long bytesInBuffer;                        // +0x0028
    unsigned long bufferOffset;                         // +0x002c
    MfFileMode mode;                                    // +0x0030
};

// The prototype of an actor as read from its .mfa file (only what the game uses).
class MfProtoSkill;
class MfProtoActor : public MfStreamSizedObject {
public:
    void AddSkill(MfProtoSkill *);
};
