#pragma once
// MfTypedValue.h: Motivate's streamable values, the base of every property value. The stream
// members live in MfActorLib.dll; the constructors, Copy, Destroy and the default Compare and
// ConvertTo are inline (CSBGameState.obj carries the out-of-line copies). Declarations from
// SteamboatApp.pdb.
#include "MBNew.h"

class MfStream;
class MfString;

enum MfStreamError {
    MF_STREAM_OK = 0,
    MF_STREAM_CREATE_FAILED = 1,
    MF_STREAM_CORRUPT = 2,
    MF_STREAM_READ_FAILED = 3,
    MF_STREAM_WRITE_FAILED = 4,
    MF_STREAM_SEEK_FAILED = 5
};

class MfStreamObject {
public:
    virtual long StreamType() const = 0;
    virtual MfStreamError ReadFromStream(MfStream &) = 0;
    virtual MfStreamError WriteToStream(MfStream &) const = 0;
protected:
    virtual MfStreamError WriteAsObject(MfStream &) const;
};

class MfStreamSizedObject : public MfStreamObject {
public:
    virtual int StreamSize() const = 0;
protected:
    virtual MfStreamError WriteAsObject(MfStream &) const;
};

class MfTypedValue : public MfStreamSizedObject {
public:
    MfTypedValue()
    {
        lModCBackId = 0;
        pModCBack = 0;
    }
    virtual ~MfTypedValue() {}
    virtual MfTypedValue *Copy() const = 0;
    virtual void Destroy() = 0;
    virtual int Compare(const MfTypedValue &) { return -1; }
    virtual MfTypedValue *ConvertTo(long type) const { return 0; }
    int ConvertToInt(int &) const;
    int ConvertToFloat(float &) const;
    int ConvertToString(MfString &) const;
    void SetModCallBack(void (__cdecl *)(MfTypedValue *, long), long);
    long lModCBackId;
    void (__cdecl *pModCBack)(MfTypedValue *, long);
};
