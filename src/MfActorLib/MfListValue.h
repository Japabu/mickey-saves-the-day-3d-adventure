#pragma once
// MfListValue: a property value holding a list of values ('VLis'). The SteamboatApp
// PDB names the class only; layout from the code.
#include "MfActorLib_api.h"
#include "MfTypedValue.h"
#include "MfDblList.h"
#include "MfListItemInterface.h"
class MfStream;

// size 0x20: MfTypedValue at 0, MfDblList at 0xc
class MFACTOR_API MfListValue : public MfTypedValue, public MfDblList {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    MfListValue() : flags(0) {}
    MfListValue(const MfListValue &list);
    virtual ~MfListValue();
    virtual MfTypedValue *Copy() const { return new MfListValue(*this); }
    virtual void Destroy() { delete this; }
    // Destroys the values, then empties the list.
    virtual void Clear();
    // The list item holding a value equal to v in pli; 0 if there is none.
    int Search(MfTypedValue *v, MfDblListItem *&pli);
protected:
    int flags;  // +0x1c (cleared by the constructors, otherwise unused)
};
