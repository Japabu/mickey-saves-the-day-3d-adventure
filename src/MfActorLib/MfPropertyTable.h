#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfPropertyTable: properties in a dictionary keyed by name ('PTab').
// Declaration from the SteamboatApp PDB.
#include "MfPropertySet.h"
#include "MfHashTable.h"
class MfStream;

// size 0x18
class MFACTOR_API MfPropertyTable : public MfPropertySet {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &s);
    virtual MfStreamError WriteToStream(MfStream &s) const;
    MfPropertyTable(const MfPropertyTable &table);
    MfPropertyTable();
    ~MfPropertyTable();
    virtual int Add(const MfProperty &p);
    virtual int Add(const MfProperty *p);
    virtual MfProperty *Find(const MfString &name) const;
    virtual MfProperty *Delete(const MfString &name);
    virtual void DeleteAll();
    virtual void Replace(const MfProperty &p);
    virtual void Replace(MfProperty *p);
    virtual void Iterate(MfCallBack &cb);
    virtual void GetPropertyNames(MfStringList &names, int sorted) const;
protected:
    MfDictionary properties;    // +4
};
