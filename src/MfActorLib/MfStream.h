#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfStream: abstract byte stream with typed operators and object records.
// ReadObject/WriteObject records: -2 = null object; -1, type, size, data for
// sized objects; otherwise type, data.
#include "enums.h"
class MfCallBack;
class MfStreamObject;
class MfStreamSizedObject;
class MfString;

class MFACTOR_API MfStream {
public:
    MfStream();
    ~MfStream();
    virtual MfStreamError ReadBytes(unsigned long, void *) = 0;
    virtual MfStreamError WriteBytes(unsigned long, const void *) = 0;
    virtual MfStreamError Flush() = 0;
    virtual MfStreamError Seek(int) = 0;

    MfStreamError Status();
    void SetStatus(MfStreamError s) { status = s; }
    void SetCallBack(MfCallBack *);

    MfStream &operator>>(bool &);
    MfStream &operator>>(char &);
    MfStream &operator>>(short &);
    MfStream &operator>>(int &);
    MfStream &operator>>(long &);
    MfStream &operator>>(float &);
    MfStream &operator>>(double &);
    MfStream &operator>>(MfString &);
    MfStream &operator>>(MfStreamObject &);
    MfStream &operator<<(bool);
    MfStream &operator<<(char);
    MfStream &operator<<(short);
    MfStream &operator<<(int);
    MfStream &operator<<(long);
    MfStream &operator<<(float);
    MfStream &operator<<(double);
    MfStream &operator<<(const MfString &);
    MfStream &operator<<(const MfStreamObject &);

    static int SizeOf(bool);
    static int SizeOf(char);
    static int SizeOf(short);
    static int SizeOf(int);
    static int SizeOf(long);
    static int SizeOf(float);
    static int SizeOf(double);
    static int SizeOf(const MfString &);
    static int SizeOf(const MfStreamSizedObject &);
    static MfStreamObject *CreateObject(long);
    MfStreamObject *ReadObject(MfStreamObject *(*)());
    MfStreamError WriteObject(MfStreamObject *);

    // member names from the SteamboatApp PDB
    int userFormat;                 // 0x04 format version; the stream objects test it
    long objectType;                // 0x08 record being read/written
    int objectSize;                 // 0x0c
    void *userData;                 // 0x10
protected:
    MfStreamError status;           // 0x14
    MfCallBack *cb;                 // 0x18
};
