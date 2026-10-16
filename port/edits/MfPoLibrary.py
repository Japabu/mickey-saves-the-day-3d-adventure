# MfPoLibrary edits for the port (see port/rewrite.py, EDITS).
#
# Inline members of PiccoloRT's classes: the original imports them from
# PiccoloRT.dll, which emitted and exported every inline member of its dllexport
# classes. Here dllexport is a visibility attribute and unused inlines are not
# emitted (those that are come out hidden), so the views that declare them without
# a body get PiccoloRT's body (src/PiccoloRT/MfScDataObject.h, MfScHandleDict.h,
# MfScClassInfo_unit.h).
PRT = "    // PORT: PiccoloRT's inline"

EDITS = {
    'MfPoLibrary/MfScDataObject.h': [
        ('    virtual MfScPropTokenTable *GetPropTokenTable();\n'
         '    virtual void FilloutPropertyNameList(MfStringList *);\n',
         '    virtual MfScPropTokenTable *GetPropTokenTable() { return 0; }' + PRT + '\n'
         '    virtual void FilloutPropertyNameList(MfStringList *) {}' + PRT + '\n', 1),
        ('    virtual MfScPropertyExtList *GetPropertyExtensions();\n',
         '    virtual MfScPropertyExtList *GetPropertyExtensions() { return 0; }' + PRT + '\n', 1),
    ],
    'MfPoLibrary/MfScHandleDict.h': [
        ('    virtual ~MfScHandleDict();\n', '    virtual ~MfScHandleDict() { Clear(); }' + PRT + '\n', 1),
    ],
    'MfPoLibrary/MfScClassInfo.h': [
        ('    virtual ~MfScClassInfoText();\n', '    virtual ~MfScClassInfoText() {}' + PRT + '\n', 1),
        # mfutil's sorted list, for the MfScClassInfo constructor (its vtable is mfutil's)
        ('};\n\nclass MfSortedList;\n',
         '};\n#include "MfDblList.h"    // PORT: for MfSortedList below\n'
         'class MfSortedList : public MfDblList { public: MfSortedList() {} virtual ~MfSortedList();'
         ' virtual void Insert(void *, EnumListPosition, MfDblListItem *); };    // PORT: mfutil\'s, for the constructor below\n', 1),
        ('    MfScClassInfo(const MfString &name);\n',
         '    MfScClassInfo(const MfString &name) : m_name(name) { m_items = new MfSortedList; }' + PRT + '\n', 1),
    ],
    # typeid(MfPlList) (MfPoActor's argument errors): PscStdlib, which defines the
    # class, is a plugin MfPoLibrary does not link; with no key function here the
    # type_info is emitted locally (it only names the type in a message)
    'MfPoLibrary/MfScHandleList.h': [
        ('    virtual ~MfPlList();\n', '    virtual ~MfPlList() {}    // PORT: no key function here, so the type_info is local\n', 1),
    ],
}
