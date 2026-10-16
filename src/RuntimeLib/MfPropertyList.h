// MfPropertyList (MfActorLib.dll): a property set kept in a list.
#ifndef RUNTIMELIB_MFPROPERTYLIST_H
#define RUNTIMELIB_MFPROPERTYLIST_H
#include "MfDblList.h"
#include "MfPropertySet.h"

class __declspec(dllimport) MfPropertyList : public MfPropertySet {
public:
    MfPropertyList();
    MfPropertyList(const MfPropertyList &);
    ~MfPropertyList();
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

    MfDblList mList;                                        // 0x04
};

#endif
