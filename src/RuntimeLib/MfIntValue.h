// MfIntValue (MfActorLib.dll): an int property value ('VInt'). Declaration
// from the SteamboatApp PDB (src/MfActorLib/MfIntValue.h); RuntimeLib's local
// vtable (after `new MfIntValue`) reaches Copy and Destroy through the import
// thunks and the destructor through the import, so here they are out of line.
#ifndef RUNTIMELIB_MFINTVALUE_H
#define RUNTIMELIB_MFINTVALUE_H
#include "MfTypedValue.h"
class MfStream;

class __declspec(dllimport) MfIntValue : public MfTypedValue {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    MfIntValue() : value(0) {}
    MfIntValue(int val) : value(val) {}
    virtual ~MfIntValue();
    operator int() const { return value; }
    virtual MfTypedValue *Copy() const;
    virtual void Destroy();
    virtual int Compare(const MfTypedValue &);
    virtual MfTypedValue *ConvertTo(long) const;
protected:
    int value;                                              // 0x0c
};

#endif
