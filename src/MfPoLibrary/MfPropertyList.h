#pragma once
// MfPropertyList (MfActorLib.dll): properties in a list ('PLst'). Declaration from the
// SteamboatApp PDB.
#include "MfPropertySet.h"
#include "MfDblList.h"
class MfStream;

// size 0x14
class __declspec(dllimport) MfPropertyList : public MfPropertySet {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &s);
    virtual MfStreamError WriteToStream(MfStream &s) const;
    MfPropertyList(const MfPropertyList &list);
    MfPropertyList();
    ~MfPropertyList();
    virtual int Add(const MfProperty &p);
    virtual int Add(const MfProperty *p);
    // Find moves the property found to the front of the list.
    virtual MfProperty *Find(const MfString &name) const;
    virtual MfProperty *Delete(const MfString &name);
    virtual void DeleteAll();
    virtual void Replace(const MfProperty &p);
    virtual void Replace(MfProperty *p);
    virtual void Iterate(MfCallBack &cb);
    virtual void GetPropertyNames(MfStringList &names, int sorted) const;
protected:
    MfDblList properties;   // +4
};
