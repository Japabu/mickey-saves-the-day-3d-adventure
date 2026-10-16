# PiccoloRT edits for the port (see port/rewrite.py, EDITS).
#
# Inline members of other DLLs' classes: the original imports them from mfutil.dll,
# which emitted and exported every inline member of its dllexport classes. Here
# dllexport is a visibility attribute and unused inlines are not emitted (those that
# are come out hidden), so the views that declare them without a body get the
# exporter's body (src/mfutil/*.h).
#
# PiccoloRT's own class-info classes: the original emitted their inline members once,
# in the MfScClassInfo object (MfScClassInfo_unit.h), and the other objects called
# them there. Their views declare them without a body (in VC5 an inline body would
# be inlined into the calling functions and change their code), so the port gives
# these views the bodies of MfScClassInfo_unit.h and MfScBytecodeRoutine_unit.h.
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
    'PiccoloRT/MfScClassInfo.h': [
        ('class MfSortedList;\n', '#include "MfSortedList.h"    // PORT: for the constructor below\n', 1),
        ('    MfScClassInfo(LiiExemplar);\n    MfScClassInfo(const MfString &name);\n',
         '    MfScClassInfo(LiiExemplar) : m_items(0) {}    // PORT: the inline of MfScClassInfo_unit.h\n'
         '    MfScClassInfo(const MfString &name) : m_name(name) { m_items = new MfSortedList; }    // PORT: same\n', 1),
        ('    virtual MfString GetKey() const;\n',
         '    virtual MfString GetKey() const { return m_name; }    // PORT: the inline of MfScClassInfo_unit.h\n', 2),
    ],
    'PiccoloRT/MfScConsoleManager_unit.h': [
        ('    MfScClassInfo(LiiExemplar);\n    MfScClassInfo(const MfString &name);\n',
         '    MfScClassInfo(LiiExemplar) : m_items(0) {}    // PORT: the inline of MfScClassInfo_unit.h\n'
         '    MfScClassInfo(const MfString &name) : m_name(name) { m_items = new MfSortedList; }    // PORT: same\n', 1),
        ('    virtual MfString GetKey() const;\n',
         '    virtual MfString GetKey() const { return m_name; }    // PORT: the inline of MfScClassInfo_unit.h\n', 2),
    ],
}
