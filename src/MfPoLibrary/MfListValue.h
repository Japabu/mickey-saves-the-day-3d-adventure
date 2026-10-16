// MfListValue (MfActorLib.dll): a property value holding a list of values
// ('VLis'); MfTypedValue at 0, MfDblList at 0xc.
#pragma once
#include "MfTypedValue.h"
#include "MfDblList.h"
#include "MfListItemInterface.h"
class MfStream;

class __declspec(dllimport) MfListValue : public MfTypedValue, public MfDblList {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    MfListValue() : pliCursor(0) {}
    MfListValue(const MfListValue &list);
    virtual ~MfListValue();
    virtual MfTypedValue *Copy() const;
    virtual void Destroy();
    virtual void Clear();
    int Search(MfTypedValue *v, MfDblListItem *&pli);
    // the script list's iteration position (MfActorLib only clears it)
    MfDblListItem *pliCursor;                               // 0x1c
};

// matches the list values that compare equal to the key
class __declspec(dllimport) MfListValueSearchItr : public MfListItemInterface {
public:
    MfListValueSearchItr() {}
    virtual int Matches(void *pKey, void *pData);
};
