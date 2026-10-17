// MfPlList: the Piccolo script class "List" of PscStdlib.dll (a list of data
// handles with a cursor).
#ifndef MFPLLIST_H
#define MFPLLIST_H
// The Motion Factory runtime classes it derives from and uses (MfScDataObject,
// MfScDataHandle, MfScHandleList, MfString, ...) and MfPlClassArg, as this plugin
// declares them. PscStdlib.h includes this header in turn, after those.
#include "PscStdlib.h"

class MfPlList : public MfScDataObject {
public:
    MfPlList();
    MfPlList(MfPlClassArg);
    virtual ~MfPlList();
protected:
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *target);
    virtual void ReleaseCircLinks(MfScLinkData *links);
    virtual int GetThroughCount(MfScDataHandle *target);
public:
    virtual MfPropertyErrorCode GetProperty(const MfString &name, MfScDataHandle **value);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *self, const MfString &name, MfScDataHandle *value, int passRef);
    virtual MfString GetClassNameA();
    virtual MfScPropTokenTable *GetPropTokenTable();
    virtual void FilloutPropertyNameList(MfStringList *names);

    MfScDataHandle *List(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Clear(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Copy(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *First(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Next(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Current(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Last(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Prev(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Append(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Prepend(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Find(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *Delete(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *GetItem(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *SetItem(MfScDataHandle *self, int argc, MfScDataHandle **argv);

    MfScHandleList *GetList() const { return mList; }

    static void Initialize();
    static void Shutdown();
    static MfString GetConstants();

private:
    MfDblListItem *FindItem(MfScDataHandle *item, int start);

    enum { kTokenCount = 0, kTokenIndex = 1 };

    MfScHandleList *mList;      // +0x14
};

#endif
