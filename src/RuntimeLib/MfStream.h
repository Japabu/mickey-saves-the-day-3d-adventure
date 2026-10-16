// MfStream (MfActorLib.dll): a stream of objects. Declaration from the
// SteamboatApp PDB.
#ifndef RUNTIMELIB_MFSTREAM_H
#define RUNTIMELIB_MFSTREAM_H
#include "enums.h"
class MfCallBack;
class MfStreamObject;
class MfStreamSizedObject;
class MfString;

class __declspec(dllimport) MfStream {
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
    MfStream &operator>>(bool &);
    MfStream &operator<<(const MfStreamObject &);
    MfStream &operator<<(const MfString &);
    MfStream &operator<<(double);
    MfStream &operator<<(float);
    MfStream &operator<<(long);
    MfStream &operator<<(int);
    MfStream &operator<<(short);
    MfStream &operator<<(char);
    MfStream &operator<<(unsigned char);
    MfStream &operator<<(bool);
    static MfStreamObject *CreateObject(long);
    MfStreamObject *ReadObject(MfStreamObject *(__cdecl *create)());
    MfStreamError WriteObject(MfStreamObject *);
    virtual MfStreamError ReadBytes(unsigned long, void *) = 0;
    virtual MfStreamError WriteBytes(unsigned long, const void *) = 0;
    virtual MfStreamError Flush() = 0;
    virtual MfStreamError Seek(int) = 0;

    int userFormat;                                         // 0x04
    long objectType;                                        // 0x08
    int objectSize;                                         // 0x0c
    void *userData;                                         // 0x10
protected:
    MfStreamError status;                                   // 0x14
    MfCallBack *cb;                                         // 0x18
#ifdef MF_STREAM_STATUS_ACCESS
    // MfActivity::ReadFromStream sets the status directly (declared only
    // where needed: the friend declaration changes VC5's register choice in
    // MfJointPath::SetKey)
    friend class MfActivity;
#endif
};

// registers the function that creates the objects of a stream type
class __declspec(dllimport) MfStreamConstructorEntry {
public:
    MfStreamConstructorEntry(long type, MfStreamObject *(__cdecl *create)());

    long mType;                                             // 0x00
    MfStreamObject *(__cdecl *mCreate)();                   // 0x04
    MfStreamConstructorEntry *mNext;                        // 0x08
};

#endif
