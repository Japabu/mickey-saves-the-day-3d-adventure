# PiccoloRT edits that make the unit views of classes agree with the complete
# declarations (port/tools/vtablecensus.py): a unit that emits a class's vtable from an
# incomplete view drops slots for every user. Applied after PiccoloRT.py (name order).
EDITS = {
    'PiccoloRT/MfScConsoleManager_unit.h': [
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
}
