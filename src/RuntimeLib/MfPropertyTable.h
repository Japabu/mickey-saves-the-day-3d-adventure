// MfPropertyTable (MfActorLib.dll): a property set kept in a dictionary.
// Add(const MfProperty &) and Replace(const MfProperty &) are new virtuals of
// the derived class: they follow the MfPropertySet slots.
#ifndef RUNTIMELIB_MFPROPERTYTABLE_H
#define RUNTIMELIB_MFPROPERTYTABLE_H
#include "MfDictionary.h"
#include "MfPropertySet.h"
#include "enums.h"

class __declspec(dllimport) MfPropertyTable : public MfPropertySet {
public:
    MfPropertyTable();
    MfPropertyTable(const MfPropertyTable &);
    ~MfPropertyTable();
    MfPropertyTable &operator=(const MfPropertyTable &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int Add(const MfProperty *);
    virtual MfProperty *Find(const MfString &) const;
    virtual MfProperty *Delete(const MfString &);
    virtual void DeleteAll();
    virtual void Replace(MfProperty *);
    virtual void Iterate(MfCallBack &);
    virtual void GetPropertyNames(MfStringList &, int) const;
    virtual int Add(const MfProperty &);
    virtual void Replace(const MfProperty &);
    static const long STREAM_TYPE;

    MfDictionary mDictionary;                               // 0x04
};

#endif
