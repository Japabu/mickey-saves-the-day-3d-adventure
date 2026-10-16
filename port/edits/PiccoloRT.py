# PiccoloRT edits for the port (see port/rewrite.py, EDITS).
#
# Inline members of other DLLs' classes: the original imports them from mfutil.dll,
# which emitted and exported every inline member of its dllexport classes. Here
# dllexport is a visibility attribute and unused inlines are not emitted (those that
# are come out hidden), so the views that declare them without a body get the
# exporter's body (src/mfutil/*.h).
#
# PiccoloRT's own inline members that other modules import (MfScClassInfo's
# constructors and GetKey, MfScClassInfoItem::GetKey, MfScClassInfoText's destructor,
# MfScDataObject's GetClassName/GetPropTokenTable/GetPropertyExtensions, MfScHandleDict's
# destructor, with the vtables that follow them): the original emitted and exported
# them from one object each (the one whose unit view has their bodies). The port does
# the same: in that object's view they become declarations, and the object defines them
# out of line on the line of their claim, so they are emitted once, exported. The
# other views keep their inline bodies or declarations; MfScBytecodeRoutine::
# GetBytecodes, which only PiccoloRT calls, gets its body in the view without one.
OUT = "    // PORT: out of line, so it is emitted and exported as in the original"
MFUTIL = "    // PORT: mfutil's inline"

EDITS = {
    'PiccoloRT/MfCallBack.h': [
        ('    virtual int CallBack(void *);\n'
         '    virtual int CallBack(void *, void *);\n'
         '    virtual int CallBack(void *, void *, void *);\n',
         '    virtual int CallBack(void *) { return 0; }' + MFUTIL + '\n'
         '    virtual int CallBack(void *, void *) { return 0; }' + MFUTIL + '\n'
         '    virtual int CallBack(void *, void *, void *) { return 0; }' + MFUTIL + '\n', 1),
    ],
    'PiccoloRT/MfLogger.h': [
        ('    MfLogger();\n    virtual ~MfLogger();\n',
         '    MfLogger() {}' + MFUTIL + '\n    virtual ~MfLogger() {}' + MFUTIL + '\n', 1),
    ],
    'PiccoloRT/MfString.h': [
        ('#pragma once\n\nclass MfString;\n',
         '#pragma once\n#include <stdlib.h>    // PORT: atoi, for operator int below\nclass MfString;\n', 1),
        ('    virtual ~MfStringRef();\n',
         '    virtual ~MfStringRef() { delete[] m_str; }' + MFUTIL + '\n', 1),
        ('    void MakeCopy(const MfStringRef &o);\n',
         '    void MakeCopy(const MfStringRef &o); int SetCharacter(int i, char c);    // PORT: mfutil\'s, for SetCharacter below\n', 1),
        ('    int SetCharacter(int, char);\n',
         '    int SetCharacter(int i, char c) { CopyOnWrite(0); return mfsr->SetCharacter(i, c); }' + MFUTIL + '\n', 1),
        ('    operator int() const;\n',
         '    operator int() const { return atoi(mfsr->m_str); }' + MFUTIL + '\n', 1),
    ],
    'PiccoloRT/MfStringList.h': [
        ('    MfString *First(MfDblListItem *&pli) const;\n'
         '    MfString *Next(MfDblListItem *&pli) const;\n',
         '    MfString *First(MfDblListItem *&pli) const { return (MfString *)MfDblList::GetFirst(pli); }' + MFUTIL + '\n'
         '    MfString *Next(MfDblListItem *&pli) const { return (MfString *)MfDblList::GetNext(pli); }' + MFUTIL + '\n', 1),
    ],
    'PiccoloRT/MfDirectory.h': [
        ('    MfString GetFileName() const;\n',
         '    MfString GetFileName() const { return MfString(m_fd.cFileName); }' + MFUTIL + '\n', 1),
    ],
    'PiccoloRT/MfScBytecodeRoutine.h': [
        ('    unsigned char * GetBytecodes() const;\n',
         '    unsigned char * GetBytecodes() const { return mBytecodes; }    // PORT: the inline of MfScBytecodeRoutine_unit.h\n', 1),
    ],
    # exported out of line (see the top): the object of MfScClassInfo
    'PiccoloRT/MfScClassInfo_unit.h': [
        ('    virtual ~MfScClassInfoText() {}\n', '    virtual ~MfScClassInfoText();' + OUT + '\n', 1),
        ('    MfScClassInfo(LiiExemplar) : m_items(0) {}\n'
         '    MfScClassInfo(const MfString &name) : m_name(name) { m_items = new MfSortedList; }\n',
         '    MfScClassInfo(LiiExemplar);' + OUT + '\n    MfScClassInfo(const MfString &name);' + OUT + '\n', 1),
        ('    virtual MfString GetKey() const { return m_name; }\n', '    virtual MfString GetKey() const;' + OUT + '\n', 2),
    ],
    'PiccoloRT/MfScClassInfo.cpp': [
        ('// 0x10002920 ??1MfScClassInfoText@@UAE@XZ (inline, header)\n',
         'MfScClassInfoText::~MfScClassInfoText() {}' + OUT + '\n', 1),
        ('// 0x10002cf0 ?GetKey@MfScClassInfo@@UBE?AVMfString@@XZ (inline, header; = MfScClassInfo::GetKey)\n',
         'MfString MfScClassInfo::GetKey() const { return m_name; } MfString MfScClassInfoItem::GetKey() const { return m_name; }' + OUT + '\n', 1),
        ('// 0x10003240 ??0MfScClassInfo@@QAE@ABVMfString@@@Z (inline, header)\n',
         'MfScClassInfo::MfScClassInfo(const MfString &name) : m_name(name) { m_items = new MfSortedList; }' + OUT + '\n', 1),
        ('// 0x10003310 ??0MfScClassInfo@@QAE@VLiiExemplar@@@Z (inline, header)\n',
         'MfScClassInfo::MfScClassInfo(LiiExemplar) : m_items(0) {}' + OUT + '\n', 1),
    ],
    # the object of MfScBytecodeRoutine
    'PiccoloRT/MfScBytecodeRoutine_unit.h': [
        ('    virtual MfString GetClassName() { return MfString("Object"); }\n', '    virtual MfString GetClassName();' + OUT + '\n', 1),
        ('    virtual MfScPropTokenTable *GetPropTokenTable() { return 0; }\n', '    virtual MfScPropTokenTable *GetPropTokenTable();' + OUT + '\n', 1),
        ('    virtual MfScPropertyExtList *GetPropertyExtensions() { return 0; }\n', '    virtual MfScPropertyExtList *GetPropertyExtensions();' + OUT + '\n', 1),
    ],
    'PiccoloRT/MfScBytecodeRoutine.cpp': [
        ('void MfScDataObject::FilloutPropertyNameList(MfStringList *) {}\n',
         'void MfScDataObject::FilloutPropertyNameList(MfStringList *) {}'
         ' MfString MfScDataObject::GetClassName() { return MfString("Object"); }'
         ' MfScPropTokenTable *MfScDataObject::GetPropTokenTable() { return 0; }'
         ' MfScPropertyExtList *MfScDataObject::GetPropertyExtensions() { return 0; }' + OUT + '\n', 1),
    ],
    # the object of MfScConsoleManager
    'PiccoloRT/MfScConsoleManager_unit.h': [
        ('    virtual ~MfScHandleDict() { Clear(); }\n', '    virtual ~MfScHandleDict();' + OUT + '\n', 1),
    ],
    'PiccoloRT/MfScConsoleManager.cpp': [
        ('// 0x10003d20 ??1MfScHandleDict@@UAE@XZ (inline, header)\n',
         'MfScHandleDict::~MfScHandleDict() { Clear(); }' + OUT + '\n', 1),
    ],
}
