# MfActorLib edits that make unit views agree with the complete declarations
# (port/tools/vtablecensus.py). Applied after MfActorLib.py (name order).
FULL_DBLLIST = (
    'enum EnumListPosition { kBefore, kAfter }; virtual ~MfDblList();'
    ' virtual void Prepend(void *const); virtual void Append(void *const);'
    ' virtual void Insert(void *, EnumListPosition, MfDblListItem *); virtual void Merge(MfDblList &, EnumListPosition, MfDblListItem *);'
    ' virtual void Add(void *const); virtual void Copy(const MfDblList &); virtual void Copy(const MfDblList &, MfCallBack &);'
    ' virtual void Copy(const MfDblList &, MfListItemInterface &); virtual void *GetFirst(MfDblListItem *&) const;'
    ' virtual void *GetNext(MfDblListItem *&) const; virtual void *GetCurrent(MfDblListItem *&) const;'
    ' virtual void *GetPrev(MfDblListItem *&) const; virtual void *GetLast(MfDblListItem *&) const;'
    ' virtual void Iterate(MfCallBack &) const; virtual void Iterate(MfListItemInterface &) const;'
    ' virtual void *DeleteFirst(); virtual void *DeleteLast(); virtual void *Delete(MfDblListItem *&);'
    ' virtual void Clear(); virtual void Clear(MfCallBack &); virtual void Clear(MfListItemInterface &);'
    ' virtual MfDblListItem *Find(void *) const; virtual MfDblListItem *Find(void *, MfCallBack &) const;'
    ' virtual MfDblListItem *Find(void *, MfListItemInterface &) const; virtual MfDblListItem *Find(void *, MfDblListItem *, int) const;'
    ' virtual MfDblListItem *Find(void *, MfDblListItem *, int, MfCallBack &) const;'
    ' virtual MfDblListItem *Find(void *, MfDblListItem *, int, MfListItemInterface &) const;')
EDITS = {
    # MfAnimSequence_unit.h's mfutil MfDblList had only its destructor: MfAnimEventList's
    # own slots came out at 4..6 instead of 31..33 for this unit's calls
    'MfActorLib/MfAnimSequence_unit.h': [
        ('class __declspec(dllimport) MfDblList {\npublic:\n    virtual ~MfDblList();\n',
         'class MfCallBack; class MfListItemInterface; class __declspec(dllimport) MfDblList {\npublic:\n    '
         + FULL_DBLLIST + '\n', 1),
    ],
}
