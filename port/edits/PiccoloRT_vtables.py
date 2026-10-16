# PiccoloRT edits that make the unit views of classes agree with the complete
# declarations (port/tools/vtablecensus.py): a unit that emits a class's vtable from an
# incomplete view drops slots for every user. Applied after PiccoloRT.py (name order).
EDITS = {
    'PiccoloRT/MfScConsoleManager_unit.h': [
        ('    virtual void Delete(void *item) {}\n', '    virtual void Destroy(void *item) {}\n', 1),
        # mfutil's MfHashTable/MfDictionary: the four protected virtuals (MfHashTable.h,
        # MfDictionary.h), which MfScHandleDict's vtable (emitted here) needs
        ('class MFIMPORT MfHashTable {\n'
         'public:\n'
         '    virtual ~MfHashTable();\n',
         'enum MfHTErrorCode { MfHTErrorCode_unknown }; class MfCallBack; class MFIMPORT MfHashTable {\n'
         'public: class MfChainNode;\n'
         '    virtual ~MfHashTable(); protected: virtual MfHTErrorCode Search(void *, void **, int *, int *, MfChainNode **) = 0;'
         ' virtual void DeleteDataItem(void *, MfCallBack *) = 0; virtual void *GetKey(void *) = 0; virtual void *GetData(void *) = 0;\n', 1),
        ('    MfDictionary(int size);\n'
         '    virtual ~MfDictionary();\n',
         '    MfDictionary(int size);\n'
         '    virtual ~MfDictionary(); protected: virtual MfHTErrorCode Search(void *, void **, int *, int *, MfHashTable::MfChainNode **);'
         ' virtual void DeleteDataItem(void *, MfCallBack *); virtual void *GetKey(void *); virtual void *GetData(void *); public:\n', 1),
        # MfStringList's own virtuals (MfStringList.h: Add, Delete, Search, CallBack)
        ('    virtual ~MfStringList() { Clear(); }\n'
         '    virtual void Clear();\n',
         '    virtual ~MfStringList() { Clear(); }\n'
         '    virtual void Clear(); virtual MfString *Add(const MfString &s); virtual void Delete(const MfString &s);'
         ' virtual MfDblListItem *Search(const MfString &s) const; virtual int CallBack(const MfString &token, MfString &rest);\n', 1),
        # MfScConsoleManager's second slot (MfScConsoleManager.h); the original's body is
        # unknown, the vtable emitted here needs one
        ('    virtual ~MfScConsoleManager();\n    static MfScConsoleManager *Manager()',
         '    virtual ~MfScConsoleManager(); virtual void _slot1() {}\n    static MfScConsoleManager *Manager()', 1),
    ],
    'PiccoloRT/MfScClassInfo_unit.h': [
        # MfListItemInterface's third slot is Destroy (MfListItemInterface.h)
        ('    virtual void Delete(void *item) {}\n', '    virtual void Destroy(void *item) {}\n', 1),
        # the overloads in declaration order (the view listed MSVC's vtable order, which
        # reverses overloads; mfutil's MfDblList.h declares them this way round)
        ('    virtual void Copy(const MfDblList &list, MfListItemInterface &iface);\n'
         '    virtual void Copy(const MfDblList &list, MfCallBack &cb);\n'
         '    virtual void Copy(const MfDblList &list);\n',
         '    virtual void Copy(const MfDblList &list);\n'
         '    virtual void Copy(const MfDblList &list, MfCallBack &cb);\n'
         '    virtual void Copy(const MfDblList &list, MfListItemInterface &iface);\n', 1),
        ('    virtual void Iterate(MfListItemInterface &iface) const;\n'
         '    virtual void Iterate(MfCallBack &cb) const;\n',
         '    virtual void Iterate(MfCallBack &cb) const;\n'
         '    virtual void Iterate(MfListItemInterface &iface) const;\n', 1),
    ],
    # MfScData's own virtuals (MfScData.h): MfScExtRoutine's vtable is emitted here
    'PiccoloRT/MfScExtRoutine_unit.h': [
        ('class PICAPI MfScData {\npublic:\n    virtual ~MfScData();\n',
         'class MfScLinkData; class PICAPI MfScData {\npublic:\n    virtual ~MfScData(); virtual MfScLinkData *CountCircLinks(MfScDataHandle *);'
         ' virtual void ReleaseCircLinks(MfScLinkData *); virtual int GetThroughCount(MfScDataHandle *); virtual void AddRef(); virtual void Release();\n', 1),
    ],
}
