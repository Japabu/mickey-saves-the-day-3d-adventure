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

# PiccoloRT/MfIntList.h: the list stores ints in its data pointers; they come back through intptr_t (a pointer no longer fits in an int).
EDITS['PiccoloRT/MfIntList.h'] = [
    ('    {\n'
     '        pli = pliFirst;\n'
     '        return pli ? (int)pli->GetData() : 0;\n'
     '    }\n'
     '    int Next(MfDblListItem *&pli) const\n',
     '    {\n'
     '        pli = pliFirst;\n'
     '        return pli ? (int)(intptr_t)pli->GetData() : 0; // PORT: intptr_t\n'
     '    }\n'
     '    int Next(MfDblListItem *&pli) const\n', 1),
    ('            pli = pli->GetNext();\n'
     '            if (pli)\n'
     '                return (int)pli->GetData();\n'
     '        }\n'
     '        return 0;\n',
     '            pli = pli->GetNext();\n'
     '            if (pli)\n'
     '                return (int)(intptr_t)pli->GetData(); // PORT: intptr_t\n'
     '        }\n'
     '        return 0;\n', 1),
]

# PiccoloRT/MfRegularExpression.h: declared with the real member layout of mfutil's MfRegularExpression, which has pointers where this view had a char array (too small on 64-bit).
EDITS['PiccoloRT/MfRegularExpression.h'] = [
    ('    int Matches(const MfString &s);\n'
     'private:\n'
     '    char mData[0x80];\n'
     '};\n',
     '    int Matches(const MfString &s);\n'
     'private:\n'
     '    // PORT: the members of the real class (src/mfutil/MfRegularExpression_unit.h, with\n'
     "    // regexpr.h's re_pattern_buffer and re_registers), so a stack object is as big as\n"
     "    // mfutil's constructor expects (168 bytes on 64-bit; char[0x80] gave 136)\n"
     '    void *mPattern;                 // MfString (one pointer)\n'
     '    void *mError;                   // MfString\n'
     '    int mMatch;\n'
     '    int mRegisterCount;\n'
     '    int mValid;\n'
     '    struct {\n'
     '        char *buffer;\n'
     '        int allocated;\n'
     '        int used;\n'
     '        char *fastmap;\n'
     '        char *translate;\n'
     '        char flags[4];\n'
     '    } mBuf;                         // re_pattern_buffer\n'
     '    struct {\n'
     '        int start[10];\n'
     '        int end[10];\n'
     '    } mRegs;                        // re_registers\n'
     '    char *mFastmap;\n'
     '};\n'
     '#line 14\n', 1),
]

# PiccoloRT/MfScConsoleManager.h: the member is pointer-sized like the real one (MfScConsoleManager_unit.h's m_outputs), so operator= copies the whole pointer on 64-bit.
EDITS['PiccoloRT/MfScConsoleManager.h'] = [
    ('    static MfScConsoleManager * pcmManager;\n'
     'public:\n'
     '    int field_4;\n'
     '};\n',
     '    static MfScConsoleManager * pcmManager;\n'
     'public:\n'
     '    void *field_4; // PORT: a pointer like the real member (the output list, MfDblList *); operator= copied an int\n'
     '};\n', 1),
]

# PiccoloRT/MfScDataHandle_2.cpp: the handle diagnostics print pointers with %p (the source printed their low 32 bits with %x).
EDITS['PiccoloRT/MfScDataHandle_2.cpp'] = [
    ('#include "MfScThreadManager.h"\n'
     '#include "MfScPiccolo.h"\n'
     '#include "MfAiFunctionRegistry.h"\n'
     '#include "MfScLinkData.h"\n'
     '#include "MfScData.h"\n'
     '#include <typeinfo.h>\n'
     '#include <stdio.h>\n'
     '\n'
     'extern __declspec(dllimport) MfNullString NULL_STRING;\n'
     '\n'
     'MfCriticalSection MfScDataHandle::csGCGuard;\n'
     'MfScHandleHeap *MfScDataHandle::aphhp[3];          // 0x10034f90\n'
     'unsigned long MfScDataHandle::dwGCTick;\n'
     'MfThread *MfScDataHandle::ptGCThread;\n'
     'MfScGCWork *MfScDataHandle::pgcwWork;\n'
     'MfThread *MfScDataHandle::ptGCTimerThread;\n'
     'MfScGCTimerWork *MfScDataHandle::pgctwWork;\n'
     'long MfScDataHandle::lGCTimeOut;\n'
     'unsigned long MfScDataHandle::udwHints;\n'
     'long MfScDataHandle::lDisableGC;\n'
     'long MfScDataHandle::lGCrunning;                   // 0x10034fbc\n'
     '\n'
     '// 0x10005140 ??0MfScDataHandle@@QAE@XZ\n'
     'MfScDataHandle::MfScDataHandle()\n'
     '{\n'
     '    m_refCount = 1;\n'
     '    m_flags = 2;\n'
     '    m_data = 0;\n'
     '}\n'
     '\n'
     '// 0x10005160 ??1MfScDataHandle@@QAE@XZ\n'
     'MfScDataHandle::~MfScDataHandle()\n'
     '{\n',
     '#include "MfScThreadManager.h"\n'
     '#include "MfScPiccolo.h"\n'
     '#include "MfAiFunctionRegistry.h"\n'
     '#include "MfScLinkData.h"\n'
     '#include "MfScData.h"\n'
     '#include <typeinfo.h>\n'
     '#include <stdio.h>\n'
     '\n'
     'extern __declspec(dllimport) MfNullString NULL_STRING;\n'
     '\n'
     'MfCriticalSection MfScDataHandle::csGCGuard;\n'
     'MfScHandleHeap *MfScDataHandle::aphhp[3];          // 0x10034f90\n'
     'unsigned int MfScDataHandle::dwGCTick;\n'
     'MfThread *MfScDataHandle::ptGCThread;\n'
     'MfScGCWork *MfScDataHandle::pgcwWork;\n'
     'MfThread *MfScDataHandle::ptGCTimerThread;\n'
     'MfScGCTimerWork *MfScDataHandle::pgctwWork;\n'
     'int MfScDataHandle::lGCTimeOut;\n'
     'unsigned int MfScDataHandle::udwHints;\n'
     'int MfScDataHandle::lDisableGC;\n'
     'int MfScDataHandle::lGCrunning;                   // 0x10034fbc\n'
     '\n'
     '// 0x10005140 ??0MfScDataHandle@@QAE@XZ\n'
     'MfScDataHandle::MfScDataHandle()\n'
     '{\n'
     '    m_refCount = 1;\n'
     '    m_flags = 2;\n'
     '    m_data = 0;\n'
     '}\n'
     '\n'
     '// 0x10005160 ??1MfScDataHandle@@QAE@XZ\n'
     'MfScDataHandle::~MfScDataHandle()\n'
     '{\n', 1),
    ('        *info << *(MfString *)(MfScObjString *)m_data;\n'
     '    else if (IsType(0x40))\n'
     '        *info << ((MfScObjInteger *)m_data)->mValue;\n'
     '    else if (IsType(0x80))\n'
     '        *info << ((MfScObjFloat *)m_data)->Value();\n'
     '    else\n'
     '        *info << "<OBJECT>";\n'
     '}\n'
     '\n'
     '// EQUIV 0x10005460 ?InfoLong@MfScDataHandle@@QAEXPAVMfString@@@Z: reviewed: as InfoShort, the boxed float is passed through fld/fstp in the original and copied as a dword in ours; the later branches differ only in register choice; same calls, arguments and branches\n'
     'void MfScDataHandle::InfoLong(MfString *info)\n'
     '{\n'
     '    char address[16];\n'
     '    info->Format("\\tHandle 0x%08x(%s): Count = %d: ", this, IsReadOnly() ? "RO" : "RW", m_refCount);\n'
     '    if (IsType(0x10))\n'
     '        *info << "DATA_INT: " << (int)m_long;\n'
     '    else if (IsType(0x20))\n'
     '        *info << "DATA_FLOAT: " << m_float;\n'
     '    else if (IsType(0x100))\n'
     '        *info << "DATA_STRING: \'" << *(MfString *)(MfScObjString *)m_data << "\'";\n'
     '    else if (IsType(0x40))\n'
     '        *info << "DATA_OINT: " << ((MfScObjInteger *)m_data)->mValue;\n'
     '    else if (IsType(0x80))\n'
     '        *info << "DATA_OFLOAT: " << ((MfScObjFloat *)m_data)->mValue;\n'
     '    else if (IsType(0x1000)) {\n'
     '        sprintf(address, "0x%08x", m_pointer);\n'
     '        *info << "DATA_POINTER: --> " << address;\n'
     '    } else if (IsType(0x800))\n'
     '        *info << "DATA_METHOD: " << m_data->Name();\n'
     '    else if (IsType(0x200))\n'
     '        *info << "DATA_OBJECT: " << m_data->Name();\n'
     '    *info << " TYPE: " << TypeInfo();\n'
     '}\n'
     '\n'
     '// 0x10005610 ?CreateData@MfScDataHandle@@SAPAV1@H@Z\n'
     'MfScDataHandle * __cdecl MfScDataHandle::CreateData(int value)\n'
     '{\n'
     '    MfScDataHandle *h = new(0) MfScDataHandle;\n',
     '        *info << *(MfString *)(MfScObjString *)m_data;\n'
     '    else if (IsType(0x40))\n'
     '        *info << ((MfScObjInteger *)m_data)->mValue;\n'
     '    else if (IsType(0x80))\n'
     '        *info << ((MfScObjFloat *)m_data)->Value();\n'
     '    else\n'
     '        *info << "<OBJECT>";\n'
     '}\n'
     '\n'
     '// EQUIV 0x10005460 ?InfoLong@MfScDataHandle@@QAEXPAVMfString@@@Z: reviewed: as InfoShort, the boxed float is passed through fld/fstp in the original and copied as a dword in ours; the later branches differ only in register choice; same calls, arguments and branches\n'
     'void MfScDataHandle::InfoLong(MfString *info)\n'
     '{\n'
     '    char address[24]; // PORT: room for a 64-bit %p\n'
     '    info->Format("\\tHandle %p(%s): Count = %d: ", (void *)this, /* PORT: %p */ IsReadOnly() ? "RO" : "RW", m_refCount);\n'
     '    if (IsType(0x10))\n'
     '        *info << "DATA_INT: " << (int)m_long;\n'
     '    else if (IsType(0x20))\n'
     '        *info << "DATA_FLOAT: " << m_float;\n'
     '    else if (IsType(0x100))\n'
     '        *info << "DATA_STRING: \'" << *(MfString *)(MfScObjString *)m_data << "\'";\n'
     '    else if (IsType(0x40))\n'
     '        *info << "DATA_OINT: " << ((MfScObjInteger *)m_data)->mValue;\n'
     '    else if (IsType(0x80))\n'
     '        *info << "DATA_OFLOAT: " << ((MfScObjFloat *)m_data)->mValue;\n'
     '    else if (IsType(0x1000)) {\n'
     '        sprintf(address, "%p", (void *)m_pointer); // PORT: %p\n'
     '        *info << "DATA_POINTER: --> " << address;\n'
     '    } else if (IsType(0x800))\n'
     '        *info << "DATA_METHOD: " << m_data->Name();\n'
     '    else if (IsType(0x200))\n'
     '        *info << "DATA_OBJECT: " << m_data->Name();\n'
     '    *info << " TYPE: " << TypeInfo();\n'
     '}\n'
     '\n'
     '// 0x10005610 ?CreateData@MfScDataHandle@@SAPAV1@H@Z\n'
     'MfScDataHandle * __cdecl MfScDataHandle::CreateData(int value)\n'
     '{\n'
     '    MfScDataHandle *h = new(0) MfScDataHandle;\n', 1),
    ('{\n'
     '    MfScDataHandle *h;\n'
     '    if (array->IsValid())\n'
     '        h = new(2) MfScDataHandle;\n'
     '    else\n'
     '        h = new(1) MfScDataHandle;\n'
     '    h->SetData(array);\n'
     '    h->SetType(0x400);\n'
     '    return h;\n'
     '}\n'
     '\n'
     '// EQUIV 0x10005e20 ??2MfScDataHandle@@SAPAXIH@Z: reviewed: the two throw sites are laid out in another order (the original merges their tails) and its frame is 4 bytes smaller (both throw temporaries share the argument slot); same calls, conditions, EH states and thrown type\n'
     'void * __cdecl MfScDataHandle::operator new(unsigned int, int heapIndex)\n'
     '{\n'
     '    if (!pgctwWork) {\n'
     '        pgctwWork = new MfScGCTimerWork;\n'
     '        ptGCTimerThread = new MfThread(2);\n'
     '        ptGCThread = new MfScGcThread(0);\n'
     '        pgcwWork = new MfScGCWork;\n'
     '        ptGCThread->AssignWork(pgcwWork);\n'
     '        MfHPTimer::Initialize();\n'
     '    }\n'
     '    if (!aphhp[heapIndex]) {\n'
     '        aphhp[0] = new MfScHandleHeap;\n'
     '        aphhp[1] = new MfScHandleHeap;\n',
     '{\n'
     '    MfScDataHandle *h;\n'
     '    if (array->IsValid())\n'
     '        h = new(2) MfScDataHandle;\n'
     '    else\n'
     '        h = new(1) MfScDataHandle;\n'
     '    h->SetData(array);\n'
     '    h->SetType(0x400);\n'
     '    return h;\n'
     '}\n'
     '\n'
     '// EQUIV 0x10005e20 ??2MfScDataHandle@@SAPAXIH@Z: reviewed: the two throw sites are laid out in another order (the original merges their tails) and its frame is 4 bytes smaller (both throw temporaries share the argument slot); same calls, conditions, EH states and thrown type\n'
     'void * __cdecl MfScDataHandle::operator new(size_t, int heapIndex)\n'
     '{\n'
     '    if (!pgctwWork) {\n'
     '        pgctwWork = new MfScGCTimerWork;\n'
     '        ptGCTimerThread = new MfThread(2);\n'
     '        ptGCThread = new MfScGcThread(0);\n'
     '        pgcwWork = new MfScGCWork;\n'
     '        ptGCThread->AssignWork(pgcwWork);\n'
     '        MfHPTimer::Initialize();\n'
     '    }\n'
     '    if (!aphhp[heapIndex]) {\n'
     '        aphhp[0] = new MfScHandleHeap;\n'
     '        aphhp[1] = new MfScHandleHeap;\n', 1),
    ('    released = 0;\n'
     '    if (compact && (!heap->m_nextUnused ||\n'
     '                    heap->m_numFree > MfScVmStack::PageSize() * 16 / sizeof(MfScDataHandle))) {\n'
     '        for (handle = last; handle >= first; handle--) {\n'
     '            if (!handle->IsType(1) && handle->m_flags)\n'
     '                break;\n'
     '        }\n'
     '        if (handle < last - 1) {\n'
     '            handle++;\n'
     '            MfScDataHandle *minimum = first + MfScVmStack::PageSize();\n'
     '            if (handle < minimum)\n'
     '                handle = minimum;\n'
     '            unsigned long decommitted;\n'
     '            unsigned long size = heap->m_stack->CommittedEnd() - (char *)handle;\n'
     '            heap->m_stack->Decommit(size, &decommitted);\n'
     '            if (decommitted > 0) {\n'
     '                released = decommitted / sizeof(MfScDataHandle);\n'
     '                heap->m_numFree -= released;\n'
     '                heap->m_numHandles -= released;\n'
     '                heap->m_nextUnused = handle;\n'
     '                if (heap->m_free == handle)\n'
     '                    heap->m_free = 0;\n'
     '                else {\n'
     '                    MfScDataHandle *free = heap->m_free;\n'
     '                    while (free && free->m_pointer < handle)\n'
     '                        free = free->m_pointer;\n',
     '    released = 0;\n'
     '    if (compact && (!heap->m_nextUnused ||\n'
     '                    heap->m_numFree > MfScVmStack::PageSize() * 16 / sizeof(MfScDataHandle))) {\n'
     '        for (handle = last; handle >= first; handle--) {\n'
     '            if (!handle->IsType(1) && handle->m_flags)\n'
     '                break;\n'
     '        }\n'
     '        if (handle < last - 1) {\n'
     '            handle++;\n'
     '            MfScDataHandle *minimum = first + MfScVmStack::PageSize();\n'
     '            if (handle < minimum)\n'
     '                handle = minimum;\n'
     '            unsigned int decommitted;\n'
     '            unsigned int size = heap->m_stack->CommittedEnd() - (char *)handle;\n'
     '            heap->m_stack->Decommit(size, &decommitted);\n'
     '            if (decommitted > 0) {\n'
     '                released = decommitted / sizeof(MfScDataHandle);\n'
     '                heap->m_numFree -= released;\n'
     '                heap->m_numHandles -= released;\n'
     '                heap->m_nextUnused = handle;\n'
     '                if (heap->m_free == handle)\n'
     '                    heap->m_free = 0;\n'
     '                else {\n'
     '                    MfScDataHandle *free = heap->m_free;\n'
     '                    while (free && free->m_pointer < handle)\n'
     '                        free = free->m_pointer;\n', 1),
    ('    int released = 0;\n'
     '    if (compact && (!heap->m_nextUnused ||\n'
     '                    heap->m_numFree > MfScVmStack::PageSize() * 16 / sizeof(MfScDataHandle))) {\n'
     '        for (handle = last; handle >= first; handle--) {\n'
     '            if (!handle->IsType(1) && handle->m_flags)\n'
     '                break;\n'
     '        }\n'
     '        if (handle < last - 1) {\n'
     '            handle++;\n'
     '            MfScDataHandle *minimum = first + MfScVmStack::PageSize();\n'
     '            if (handle < minimum)\n'
     '                handle = minimum;\n'
     '            unsigned long decommitted;\n'
     '            unsigned long size = heap->m_stack->CommittedEnd() - (char *)handle;\n'
     '            heap->m_stack->Decommit(size, &decommitted);\n'
     '            if (decommitted > 0) {\n'
     '                released = decommitted / sizeof(MfScDataHandle);\n'
     '                heap->m_numFree -= released;\n'
     '                heap->m_numHandles -= released;\n'
     '                heap->m_nextUnused = handle;\n'
     '                if (heap->m_free == handle)\n'
     '                    heap->m_free = 0;\n'
     '                else {\n'
     '                    MfScDataHandle *free = heap->m_free;\n'
     '                    while (free && free->m_pointer < handle)\n'
     '                        free = free->m_pointer;\n',
     '    int released = 0;\n'
     '    if (compact && (!heap->m_nextUnused ||\n'
     '                    heap->m_numFree > MfScVmStack::PageSize() * 16 / sizeof(MfScDataHandle))) {\n'
     '        for (handle = last; handle >= first; handle--) {\n'
     '            if (!handle->IsType(1) && handle->m_flags)\n'
     '                break;\n'
     '        }\n'
     '        if (handle < last - 1) {\n'
     '            handle++;\n'
     '            MfScDataHandle *minimum = first + MfScVmStack::PageSize();\n'
     '            if (handle < minimum)\n'
     '                handle = minimum;\n'
     '            unsigned int decommitted;\n'
     '            unsigned int size = heap->m_stack->CommittedEnd() - (char *)handle;\n'
     '            heap->m_stack->Decommit(size, &decommitted);\n'
     '            if (decommitted > 0) {\n'
     '                released = decommitted / sizeof(MfScDataHandle);\n'
     '                heap->m_numFree -= released;\n'
     '                heap->m_numHandles -= released;\n'
     '                heap->m_nextUnused = handle;\n'
     '                if (heap->m_free == handle)\n'
     '                    heap->m_free = 0;\n'
     '                else {\n'
     '                    MfScDataHandle *free = heap->m_free;\n'
     '                    while (free && free->m_pointer < handle)\n'
     '                        free = free->m_pointer;\n', 1),
]

# PiccoloRT/MfScDataHandle_unit.h: the MfScVmEnv placeholders are pointer-sized like the real members (MfScVmEnv.h), so the inline members use the real offsets on 64-bit.
EDITS['PiccoloRT/MfScDataHandle_unit.h'] = [
    ('class __declspec(dllexport) MfScVmFrame {\n'
     'public:\n'
     '    long m_data[5]; // 0x14 bytes, layout unknown\n'
     '};\n'
     '\n',
     'class __declspec(dllexport) MfScVmFrame {\n'
     'public:\n'
     '    int m_data[5]; // 0x14 bytes, layout unknown\n'
     '};\n'
     '\n', 1),
    ('\n'
     'private:\n'
     '    long m_unk0[3];              // +0\n'
     '    MfScDataHandle **m_stack;    // +c\n'
     '    long m_unk10[4];             // +10\n'
     '    MfScDataHandle *m_reg0;      // +20\n'
     '    MfScDataHandle *m_reg1;      // +24\n',
     '\n'
     'private:\n'
     '    // PORT: pointer-sized placeholders, as the real members (vptr and m_ip; then m_frame,\n'
     '    // m_constants, m_object, m_function), so m_stack and m_reg0 are at the real offsets\n'
     '    void *m_unk0[2];             // +0\n'
     '    MfScDataHandle **m_stack;    // +c\n'
     '    void *m_unk10[4];            // +10\n'
     '    MfScDataHandle *m_reg0;      // +20\n'
     '    MfScDataHandle *m_reg1;      // +24\n'
     '#line 91\n', 1),
]

# PiccoloRT/MfScDebugInfo.cpp: stack words are uintptr_t (pointer-sized): the 32-bit source used unsigned long, which was the size of a pointer there.
EDITS['PiccoloRT/MfScDebugInfo.cpp'] = [
    ('\n'
     '// 0x10009250 ?GetVariable@MfScDebugInfo@@QAEHABVMfString@@PAKHPAPAVMfScDataHandle@@@Z\n'
     'int MfScDebugInfo::GetVariable(MfString const &name, unsigned long *frame, int level, MfScDataHandle **value)\n'
     '{\n'
     '    int found = 0;\n',
     '\n'
     '// 0x10009250 ?GetVariable@MfScDebugInfo@@QAEHABVMfString@@PAKHPAPAVMfScDataHandle@@@Z\n'
     'int MfScDebugInfo::GetVariable(MfString const &name, uintptr_t *frame, /* PORT: uintptr_t */ int level, MfScDataHandle **value)\n'
     '{\n'
     '    int found = 0;\n', 1),
]

# PiccoloRT/MfScDebugInfo.h: stack words are uintptr_t (pointer-sized): the 32-bit source used unsigned long, which was the size of a pointer there.
EDITS['PiccoloRT/MfScDebugInfo.h'] = [
    ('#pragma once\n'
     'class MfScCodeLevelInfo;\n'
     'class MfScConstUsageInfo;\n',
     '#pragma once\n'
     '#include <stdint.h> // PORT: uintptr_t\n'
     'class MfScCodeLevelInfo;\n'
     'class MfScConstUsageInfo;\n'
     '#line 4\n', 1),
    ('    MfScVariableDebugInfo const & GetVarDebugInfo(int) const;\n'
     '    void SetVarDebugInfo(MfScVariableDebugInfo const &, int);\n'
     '    int GetVariable(MfString const &, unsigned long *, int, MfScDataHandle * *);\n'
     '\n'
     '    int InitializeCodeLevelInfo(int);\n',
     '    MfScVariableDebugInfo const & GetVarDebugInfo(int) const;\n'
     '    void SetVarDebugInfo(MfScVariableDebugInfo const &, int);\n'
     '    int GetVariable(MfString const &, uintptr_t *, int, MfScDataHandle * *); // PORT: uintptr_t\n'
     '\n'
     '    int InitializeCodeLevelInfo(int);\n', 1),
]

# PiccoloRT/MfScDebugger.h: stack words are uintptr_t (pointer-sized): the 32-bit source used unsigned long, which was the size of a pointer there.
EDITS['PiccoloRT/MfScDebugger.h'] = [
    ('#pragma once\n'
     '#include "MfAiFunctionRegistry.h"\n'
     'class MfPdbListener;\n',
     '#pragma once\n'
     '#include <stdint.h> // PORT: uintptr_t\n'
     '#include "MfAiFunctionRegistry.h"\n'
     'class MfPdbListener;\n'
     '#line 4\n', 1),
    ('struct MfPdbStepContext {\n'
     '    MfScVmEnv *env;\n'
     '    unsigned long *stackBase;       // the data stack when the VM was entered\n'
     '    MfScBytecodeRoutine *routine;   // the routine that runs, if it is bytecode\n'
     '    MfScVirtualMachine *vm;\n',
     'struct MfPdbStepContext {\n'
     '    MfScVmEnv *env;\n'
     '    uintptr_t *stackBase;           // PORT: uintptr_t; the data stack when the VM was entered\n'
     '    MfScBytecodeRoutine *routine;   // the routine that runs, if it is bytecode\n'
     '    MfScVirtualMachine *vm;\n', 1),
    ('// breakpoint.\n'
     'struct MfPdbBreakpointInfo {\n'
     '    unsigned long data;\n'
     '    unsigned char bytecode;         // the bytecode the breakpoint replaced\n'
     '    void *entry;\n',
     '// breakpoint.\n'
     'struct MfPdbBreakpointInfo {\n'
     '    unsigned int data;\n'
     '    unsigned char bytecode;         // the bytecode the breakpoint replaced\n'
     '    void *entry;\n', 1),
    ('    virtual int UnplantAll() = 0;\n'
     '    // a breakpoint at address was passed: plants it again\n'
     '    virtual int BreakpointHit(unsigned long address, int, MfScVmEnv *env) = 0;\n'
     '    virtual int GetBreakpointInfo(unsigned long address, int, MfPdbBreakpointInfo *info) = 0;\n'
     '    virtual int PlantModule(const MfString &module) = 0;\n'
     '    virtual int Save() = 0;\n',
     '    virtual int UnplantAll() = 0;\n'
     '    // a breakpoint at address was passed: plants it again\n'
     '    // PORT: the bytecode address is uintptr_t (pointer-sized)\n'
     '    virtual int BreakpointHit(uintptr_t address, int, MfScVmEnv *env) = 0;\n'
     '    virtual int GetBreakpointInfo(uintptr_t address, int, MfPdbBreakpointInfo *info) = 0;\n'
     '    virtual int PlantModule(const MfString &module) = 0;\n'
     '    virtual int Save() = 0;\n'
     '#line 63\n', 1),
]

# PiccoloRT/MfScLinkData.cpp: operator new hands out a static buffer big enough for the link data objects (the source returned one pointer-sized global).
EDITS['PiccoloRT/MfScLinkData.cpp'] = [
    ('#include "MfScLinkData.h"\n'
     'int g_MfScLinkDataCount;\n'
     'void *g_MfScLinkDataPool;\n'
     '// 0x10008b30 ?Count@MfScLinkData@@QAEAAHXZ\n'
     'int & MfScLinkData::Count()\n',
     '#include "MfScLinkData.h"\n'
     'int g_MfScLinkDataCount;\n'
     '// PORT: the one live link data object needs room for an MfScListLinkData (24 bytes on\n'
     '// 64-bit); the source had a single pointer-sized slot\n'
     'alignas(16) static unsigned char g_MfScLinkDataPool[64];\n'
     '// 0x10008b30 ?Count@MfScLinkData@@QAEAAHXZ\n'
     'int & MfScLinkData::Count()\n'
     '#line 6\n', 1),
    ('\n'
     '// 0x1000bd80 ??2MfScLinkData@@SAPAXI@Z\n'
     'void * __cdecl MfScLinkData::operator new(unsigned int)\n'
     '{\n'
     '    g_MfScLinkDataCount = 1;\n'
     '    return &g_MfScLinkDataPool;\n'
     '}\n'
     '\n',
     '\n'
     '// 0x1000bd80 ??2MfScLinkData@@SAPAXI@Z\n'
     'void * __cdecl MfScLinkData::operator new(size_t)\n'
     '{\n'
     '    g_MfScLinkDataCount = 1;\n'
     '    return g_MfScLinkDataPool; // PORT: the buffer above\n'
     '}\n'
     '\n', 1),
]

# PiccoloRT/MfScPiccoloObjectFile.cpp: the .pobj object file is a 32-bit format: its words are read and written as DWORD (32 bits) where the source has `unsigned long` (64 bits on macOS); offsets are computed from char pointers, not through int casts.
EDITS['PiccoloRT/MfScPiccoloObjectFile.cpp'] = [
    ('\n'
     '// 0x1000f0c0 ?OffsetToAddress@MfScPiccoloObjectFile@@AAEPAXK@Z\n'
     'void * MfScPiccoloObjectFile::OffsetToAddress(unsigned long offset)\n'
     '{\n'
     '    return (void *)((int)mBase + (int)offset);\n'
     '}\n'
     '\n'
     '// 0x1000f0d0 ?AddressToOffset@MfScPiccoloObjectFile@@AAEKPAX@Z\n'
     'unsigned long MfScPiccoloObjectFile::AddressToOffset(void * address)\n'
     '{\n'
     '    return (unsigned long)((int)address - (int)mBase);\n'
     '}\n'
     '\n',
     '\n'
     '// 0x1000f0c0 ?OffsetToAddress@MfScPiccoloObjectFile@@AAEPAXK@Z\n'
     'void * MfScPiccoloObjectFile::OffsetToAddress(DWORD offset)\n'
     '{\n'
     '    return (void *)(mBase + offset);\n'
     '}\n'
     '\n'
     '// 0x1000f0d0 ?AddressToOffset@MfScPiccoloObjectFile@@AAEKPAX@Z\n'
     'DWORD MfScPiccoloObjectFile::AddressToOffset(void * address)\n'
     '{\n'
     '    return (DWORD)((char *)address - mBase);\n'
     '}\n'
     '\n', 1),
    ('        if (!OpenFileMappingA(0))\n'
     '            return SetLastError(kPOFNoMapping);\n'
     '        unsigned long *header = (unsigned long *)mBase;\n'
     '        if (header[0] != 0x50534a4d) {\n'
     '            mOpen = 1;\n',
     '        if (!OpenFileMappingA(0))\n'
     '            return SetLastError(kPOFNoMapping);\n'
     '        DWORD *header = (DWORD *)mBase;\n'
     '        if (header[0] != 0x50534a4d) {\n'
     '            mOpen = 1;\n', 1),
    ('            return SetLastError(kPOFBadModuleTable);\n'
     '        }\n'
     '        if (header[4] != (unsigned long)-1) {\n'
     '            unsigned long *info = (unsigned long *)(header[4] + mBase + 0x10);\n'
     '            mType = info[0];\n'
     '            mOptions = info[1];\n',
     '            return SetLastError(kPOFBadModuleTable);\n'
     '        }\n'
     '        if (header[4] != (DWORD)-1) {\n'
     '            DWORD *info = (DWORD *)(header[4] + mBase + 0x10);\n'
     '            mType = info[0];\n'
     '            mOptions = info[1];\n', 1),
    ('{\n'
     '    union {\n'
     '        unsigned long all;\n'
     '        unsigned char part[4];\n'
     '    } version;\n'
     '    version.all = *(unsigned long *)(mBase + 4);\n'
     '    int minor = version.part[2];\n'
     '    if ((version.all & 0xff000000) == 0x2000000 && minor <= 99)\n',
     '{\n'
     '    union {\n'
     '        DWORD all;\n'
     '        unsigned char part[4];\n'
     '    } version;\n'
     '    version.all = *(DWORD *)(mBase + 4);\n'
     '    int minor = version.part[2];\n'
     '    if ((version.all & 0xff000000) == 0x2000000 && minor <= 99)\n', 1),
    ('\n'
     '// 0x10013cc0 ?OpenFileMappingA@MfScPiccoloObjectFile@@AAEHK@Z\n'
     'int MfScPiccoloObjectFile::OpenFileMappingA(unsigned long size)\n'
     '{\n'
     '    if (!mWrite)\n',
     '\n'
     '// 0x10013cc0 ?OpenFileMappingA@MfScPiccoloObjectFile@@AAEHK@Z\n'
     'int MfScPiccoloObjectFile::OpenFileMappingA(DWORD size)\n'
     '{\n'
     '    if (!mWrite)\n', 1),
    ('    if (mWrite == 1)\n'
     '        return SetLastError(kPOFWriteOnly);\n'
     '    char *section = mBase + *(unsigned long *)(mBase + 0xc);\n'
     '    char *record = section + 0xc;\n'
     '    for (unsigned long i = 0; i < *(unsigned long *)(section + 8); i++) {\n'
     '        MfScDataHandle *function = BuildFunction(record - mBase + 4);\n'
     '        functions->Add(function);\n'
     '        function->Release();\n'
     '        record += *(unsigned long *)record;\n'
     '    }\n'
     '    mError = kPOFOk;\n',
     '    if (mWrite == 1)\n'
     '        return SetLastError(kPOFWriteOnly);\n'
     '    char *section = mBase + *(DWORD *)(mBase + 0xc);\n'
     '    char *record = section + 0xc;\n'
     '    for (DWORD i = 0; i < *(DWORD *)(section + 8); i++) {\n'
     '        MfScDataHandle *function = BuildFunction(record - mBase + 4);\n'
     '        functions->Add(function);\n'
     '        function->Release();\n'
     '        record += *(DWORD *)record;\n'
     '    }\n'
     '    mError = kPOFOk;\n', 1),
    ('// functions\n'
     'struct PObjModuleRecord {\n'
     '    unsigned long mSize;            // +0x00 to the next record\n'
     '    unsigned long mNameSize;        // +0x04 aligned\n'
     '    char mName[4];                  // +0x08\n'
     '};\n',
     '// functions\n'
     'struct PObjModuleRecord {\n'
     '    DWORD mSize;            // +0x00 to the next record\n'
     '    DWORD mNameSize;        // +0x04 aligned\n'
     '    char mName[4];                  // +0x08\n'
     '};\n', 1),
    ('{\n'
     '    mModules->Clear();\n'
     '    char *section = mBase + *(unsigned long *)(mBase + 8);\n'
     '    PObjModuleRecord *record = (PObjModuleRecord *)(section + 0xc);\n'
     '    for (unsigned long i = 0; i < *(unsigned long *)(section + 8); i++) {\n'
     '        MfString name(record->mName);\n'
     '        int *offsets = (int *)(record->mName + record->mNameSize);\n',
     '{\n'
     '    mModules->Clear();\n'
     '    char *section = mBase + *(DWORD *)(mBase + 8);\n'
     '    PObjModuleRecord *record = (PObjModuleRecord *)(section + 0xc);\n'
     '    for (DWORD i = 0; i < *(DWORD *)(section + 8); i++) {\n'
     '        MfString name(record->mName);\n'
     '        int *offsets = (int *)(record->mName + record->mNameSize);\n', 1),
    ('\n'
     '// EQUIV 0x100140f0 ?BuildFunction@MfScPiccoloObjectFile@@AAEPAVMfScDataHandle@@K@Z: reviewed: the original computes both name addresses before constructing the strings, lays out the debug-info branches in another order and keeps the registry on the stack; same calls, constants and flow\n'
     'MfScDataHandle *MfScPiccoloObjectFile::BuildFunction(unsigned long offset)\n'
     '{\n'
     '    if (MfScPiccolo::bIsRunning) {\n'
     '        MfAiFunctionRegistry *registry = MfScPiccolo::GetFunctionRegistry();\n'
     '        if (registry) {\n'
     '            unsigned long *record = (unsigned long *)(mBase + offset);\n'
     '            MfString name(mBase + record[1] + 4);\n'
     '            MfString module(mBase + record[2] + 4);\n',
     '\n'
     '// EQUIV 0x100140f0 ?BuildFunction@MfScPiccoloObjectFile@@AAEPAVMfScDataHandle@@K@Z: reviewed: the original computes both name addresses before constructing the strings, lays out the debug-info branches in another order and keeps the registry on the stack; same calls, constants and flow\n'
     'MfScDataHandle *MfScPiccoloObjectFile::BuildFunction(DWORD offset)\n'
     '{\n'
     '    if (MfScPiccolo::bIsRunning) {\n'
     '        MfAiFunctionRegistry *registry = MfScPiccolo::GetFunctionRegistry();\n'
     '        if (registry) {\n'
     '            DWORD *record = (DWORD *)(mBase + offset);\n'
     '            MfString name(mBase + record[1] + 4);\n'
     '            MfString module(mBase + record[2] + 4);\n', 1),
    ('        }\n'
     '    }\n'
     '    unsigned long *record = (unsigned long *)(mBase + offset);\n'
     '    MfScDataHandle *function = MfScDataHandle::CreateBytecodeFunction(MfString(mBase + record[1] + 4), record[0]);\n'
     '    MfScBytecodeRoutine *routine = function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;\n',
     '        }\n'
     '    }\n'
     '    DWORD *record = (DWORD *)(mBase + offset);\n'
     '    MfScDataHandle *function = MfScDataHandle::CreateBytecodeFunction(MfString(mBase + record[1] + 4), record[0]);\n'
     '    MfScBytecodeRoutine *routine = function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;\n', 1),
    ('    routine->FunctionParams() = mBase + record[4] + 4;\n'
     '    routine->ObjectFileName() = mName;\n'
     '    unsigned long *code = (unsigned long *)(mBase + record[5]);\n'
     '    memcpy(routine->AllocBytecodes(code[0]), code + 1, code[0]);\n'
     '    unsigned long *pool = (unsigned long *)(mBase + record[6]);\n'
     '    routine->AllocConstantPool(pool[0]);\n'
     '    MfScDataHandle **constants = routine->GetConstantPool();\n'
     '    PObjConstantPoolEntry *entry = (PObjConstantPoolEntry *)(pool + 1);\n'
     '    for (unsigned long i = 0; i < pool[0]; i++) {\n'
     '        if (entry->mType == 0) {\n'
     '            constants[i] = 0;\n',
     '    routine->FunctionParams() = mBase + record[4] + 4;\n'
     '    routine->ObjectFileName() = mName;\n'
     '    DWORD *code = (DWORD *)(mBase + record[5]);\n'
     '    memcpy(routine->AllocBytecodes(code[0]), code + 1, code[0]);\n'
     '    DWORD *pool = (DWORD *)(mBase + record[6]);\n'
     '    routine->AllocConstantPool(pool[0]);\n'
     '    MfScDataHandle **constants = routine->GetConstantPool();\n'
     '    PObjConstantPoolEntry *entry = (PObjConstantPoolEntry *)(pool + 1);\n'
     '    for (DWORD i = 0; i < pool[0]; i++) {\n'
     '        if (entry->mType == 0) {\n'
     '            constants[i] = 0;\n', 1),
    ('    MfDblListItem *pos;\n'
     '    for (int offset = offsets->First(pos); pos && !info; offset = offsets->Next(pos)) {\n'
     '        if (function == mBase + *(unsigned long *)(mBase + offset + 4) + 4)\n'
     '            info = GetDebugInfo(offset);\n'
     '    }\n',
     '    MfDblListItem *pos;\n'
     '    for (int offset = offsets->First(pos); pos && !info; offset = offsets->Next(pos)) {\n'
     '        if (function == (const char *)(mBase + *(DWORD *)(mBase + offset + 4) + 4))\n'
     '            info = GetDebugInfo(offset);\n'
     '    }\n', 1),
    ('MfScDebugInfo *MfScPiccoloObjectFile::GetDebugInfo(unsigned int offset)\n'
     '{\n'
     '    unsigned long *record = (unsigned long *)(mBase + offset);\n'
     '    int i;\n'
     '    int *lines = (int *)SectionAddress(record[7]);\n',
     'MfScDebugInfo *MfScPiccoloObjectFile::GetDebugInfo(unsigned int offset)\n'
     '{\n'
     '    DWORD *record = (DWORD *)(mBase + offset);\n'
     '    int i;\n'
     '    int *lines = (int *)SectionAddress(record[7]);\n', 1),
    ('    }\n'
     '\n'
     '    unsigned long *header = (unsigned long *)mBase;\n'
     '    header[3] = 0x24 + 0xc + moduleSize;\n'
     '    header[4] = header[3] + 0xc + functionSize;\n',
     '    }\n'
     '\n'
     '    DWORD *header = (DWORD *)mBase;\n'
     '    header[3] = 0x24 + 0xc + moduleSize;\n'
     '    header[4] = header[3] + 0xc + functionSize;\n', 1),
    ('    header[1] = 0x20a0000;      // 2.10.0\n'
     '    header[2] = 0x24;\n'
     '    memset(&header[5], 0xff, 4 * sizeof(unsigned long));\n'
     '\n'
     '    unsigned long *moduleSection = (unsigned long *)(mBase + header[2]);\n'
     '    moduleSection[0] = 1;\n'
     '    moduleSection[1] = moduleSize + 0xc;\n'
     '    moduleSection[2] = 0;\n'
     '    unsigned long *functionSection = (unsigned long *)(mBase + header[3]);\n'
     '    functionSection[0] = 2;\n'
     '    functionSection[1] = functionSize + 0xc;\n'
     '    functionSection[2] = 0;\n'
     '    PObjModuleRecord *module = (PObjModuleRecord *)(moduleSection + 3);\n'
     '    unsigned long *entry = functionSection + 3;\n'
     '\n'
     '    MfDblListItem *pos;\n',
     '    header[1] = 0x20a0000;      // 2.10.0\n'
     '    header[2] = 0x24;\n'
     '    memset(&header[5], 0xff, 4 * sizeof(DWORD));\n'
     '\n'
     '    DWORD *moduleSection = (DWORD *)(mBase + header[2]);\n'
     '    moduleSection[0] = 1;\n'
     '    moduleSection[1] = moduleSize + 0xc;\n'
     '    moduleSection[2] = 0;\n'
     '    DWORD *functionSection = (DWORD *)(mBase + header[3]);\n'
     '    functionSection[0] = 2;\n'
     '    functionSection[1] = functionSize + 0xc;\n'
     '    functionSection[2] = 0;\n'
     '    PObjModuleRecord *module = (PObjModuleRecord *)(moduleSection + 3);\n'
     '    DWORD *entry = functionSection + 3;\n'
     '\n'
     '    MfDblListItem *pos;\n', 1),
    ('        module->mNameSize = AlignedLength(routine->ModuleName().Length() + 1);\n'
     '        DumpData(module->mName, (const char *)routine->ModuleName(), routine->ModuleName().Length() + 1);\n'
     '        unsigned long *offsets = (unsigned long *)(module->mName + module->mNameSize);\n'
     '        offsets[0] = list->GetCount();\n'
     '        moduleSection[2]++;\n'
     '        module = (PObjModuleRecord *)((char *)module + module->mSize);\n'
     '\n'
     '        unsigned long *offset = offsets + 1;\n'
     '        for (MfScDataHandle *function = list->First(); function;\n'
     '             function = (MfScDataHandle *)list->MfDblList::GetNext(list->CurrentPos()), offset++) {\n'
     '            routine = function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;\n'
     '            unsigned long *record = entry + 1;\n'
     '            *offset = (char *)record - mBase;\n'
     '            char *p = (char *)(record + 12);\n'
     '            record[11] = 0xa0a0a0a0;\n'
     '            record[0] = routine->Params();\n'
     '            *(unsigned long *)p = AlignedLength(routine->Name().Length() + 1);\n'
     '            DumpData(p + 4, (const char *)routine->Name(), routine->Name().Length() + 1);\n'
     '            record[1] = p - mBase;\n'
     '            p += *(unsigned long *)p + 4;\n'
     '            *(unsigned long *)p = AlignedLength(routine->ModuleName().Length() + 1);\n'
     '            DumpData(p + 4, (const char *)routine->ModuleName(), routine->ModuleName().Length() + 1);\n'
     '            record[2] = p - mBase;\n',
     '        module->mNameSize = AlignedLength(routine->ModuleName().Length() + 1);\n'
     '        DumpData(module->mName, (const char *)routine->ModuleName(), routine->ModuleName().Length() + 1);\n'
     '        DWORD *offsets = (DWORD *)(module->mName + module->mNameSize);\n'
     '        offsets[0] = list->GetCount();\n'
     '        moduleSection[2]++;\n'
     '        module = (PObjModuleRecord *)((char *)module + module->mSize);\n'
     '\n'
     '        DWORD *offset = offsets + 1;\n'
     '        for (MfScDataHandle *function = list->First(); function;\n'
     '             function = (MfScDataHandle *)list->MfDblList::GetNext(list->CurrentPos()), offset++) {\n'
     '            routine = function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;\n'
     '            DWORD *record = entry + 1;\n'
     '            *offset = (char *)record - mBase;\n'
     '            char *p = (char *)(record + 12);\n'
     '            record[11] = 0xa0a0a0a0;\n'
     '            record[0] = routine->Params();\n'
     '            *(DWORD *)p = AlignedLength(routine->Name().Length() + 1);\n'
     '            DumpData(p + 4, (const char *)routine->Name(), routine->Name().Length() + 1);\n'
     '            record[1] = p - mBase;\n'
     '            p += *(DWORD *)p + 4;\n'
     '            *(DWORD *)p = AlignedLength(routine->ModuleName().Length() + 1);\n'
     '            DumpData(p + 4, (const char *)routine->ModuleName(), routine->ModuleName().Length() + 1);\n'
     '            record[2] = p - mBase;\n', 1),
    ('                // a debug file keeps only names and the code size\n'
     '                MfString empty;\n'
     '                p += *(unsigned long *)p + 4;\n'
     '                *(unsigned long *)p = AlignedLength(1);\n'
     '                DumpData(p + 4, (const char *)empty, empty.Length() + 1);\n'
     '                record[3] = p - mBase;\n'
     '                p += *(unsigned long *)p + 4;\n'
     '                *(unsigned long *)p = AlignedLength(1);\n'
     '                DumpData(p + 4, (const char *)empty, empty.Length() + 1);\n'
     '                record[4] = p - mBase;\n'
     '                p += *(unsigned long *)p + 4;\n'
     '                *(unsigned long *)p = routine->mCodeSize;\n'
     '                record[5] = p - mBase;\n'
     '                p += 4;\n'
     '                *(unsigned long *)p = 0;\n'
     '                record[6] = p - mBase;\n'
     '                p += 4;\n'
     '            } else {\n'
     '                p += *(unsigned long *)p + 4;\n'
     '                *(unsigned long *)p = AlignedLength(routine->ModuleType().Length() + 1);\n'
     '                DumpData(p + 4, (const char *)routine->ModuleType(), routine->ModuleType().Length() + 1);\n'
     '                record[3] = p - mBase;\n'
     '                p += *(unsigned long *)p + 4;\n'
     '                *(unsigned long *)p = AlignedLength(routine->FunctionParams().Length() + 1);\n'
     '                DumpData(p + 4, (const char *)routine->FunctionParams(), routine->FunctionParams().Length() + 1);\n'
     '                record[4] = p - mBase;\n'
     '                p += *(unsigned long *)p + 4;\n'
     '                *(unsigned long *)p = AlignedLength(routine->mCodeSize);\n'
     '                DumpData(p + 4, routine->mBytecodes, routine->mCodeSize);\n'
     '                record[5] = p - mBase;\n'
     '                unsigned long *pool = (unsigned long *)(p + *(unsigned long *)p + 4);\n'
     '                *pool = routine->ConstantPoolSize();\n'
     '                PObjConstantPoolEntry *constant = (PObjConstantPoolEntry *)(pool + 1);\n',
     '                // a debug file keeps only names and the code size\n'
     '                MfString empty;\n'
     '                p += *(DWORD *)p + 4;\n'
     '                *(DWORD *)p = AlignedLength(1);\n'
     '                DumpData(p + 4, (const char *)empty, empty.Length() + 1);\n'
     '                record[3] = p - mBase;\n'
     '                p += *(DWORD *)p + 4;\n'
     '                *(DWORD *)p = AlignedLength(1);\n'
     '                DumpData(p + 4, (const char *)empty, empty.Length() + 1);\n'
     '                record[4] = p - mBase;\n'
     '                p += *(DWORD *)p + 4;\n'
     '                *(DWORD *)p = routine->mCodeSize;\n'
     '                record[5] = p - mBase;\n'
     '                p += 4;\n'
     '                *(DWORD *)p = 0;\n'
     '                record[6] = p - mBase;\n'
     '                p += 4;\n'
     '            } else {\n'
     '                p += *(DWORD *)p + 4;\n'
     '                *(DWORD *)p = AlignedLength(routine->ModuleType().Length() + 1);\n'
     '                DumpData(p + 4, (const char *)routine->ModuleType(), routine->ModuleType().Length() + 1);\n'
     '                record[3] = p - mBase;\n'
     '                p += *(DWORD *)p + 4;\n'
     '                *(DWORD *)p = AlignedLength(routine->FunctionParams().Length() + 1);\n'
     '                DumpData(p + 4, (const char *)routine->FunctionParams(), routine->FunctionParams().Length() + 1);\n'
     '                record[4] = p - mBase;\n'
     '                p += *(DWORD *)p + 4;\n'
     '                *(DWORD *)p = AlignedLength(routine->mCodeSize);\n'
     '                DumpData(p + 4, routine->mBytecodes, routine->mCodeSize);\n'
     '                record[5] = p - mBase;\n'
     '                DWORD *pool = (DWORD *)(p + *(DWORD *)p + 4);\n'
     '                *pool = routine->ConstantPoolSize();\n'
     '                PObjConstantPoolEntry *constant = (PObjConstantPoolEntry *)(pool + 1);\n', 1),
    ('            // debug tables: each is its entry count and the entries\n'
     '            MfScDebugInfo *debug = routine->DebugInfo();\n'
     '            unsigned long *table = (unsigned long *)p;\n'
     '            MfScLineInfo *line = (MfScLineInfo *)(table + 1);\n'
     '            record[7] = (char *)table - mBase;\n',
     '            // debug tables: each is its entry count and the entries\n'
     '            MfScDebugInfo *debug = routine->DebugInfo();\n'
     '            DWORD *table = (DWORD *)p;\n'
     '            MfScLineInfo *line = (MfScLineInfo *)(table + 1);\n'
     '            record[7] = (char *)table - mBase;\n', 1),
    ('            }\n'
     '\n'
     '            table = (unsigned long *)line;\n'
     '            unsigned long *var = table + 1;\n'
     '            record[8] = (char *)table - mBase;\n'
     '            if (debug && (~mOptions & 1) && (~mOptions & 2)) {\n',
     '            }\n'
     '\n'
     '            table = (DWORD *)line;\n'
     '            DWORD *var = table + 1;\n'
     '            record[8] = (char *)table - mBase;\n'
     '            if (debug && (~mOptions & 1) && (~mOptions & 2)) {\n', 1),
    ('                    var[2] = AlignedLength(info.mName.Length() + 1);\n'
     '                    DumpData(var + 3, (const char *)info.mName, info.mName.Length() + 1);\n'
     '                    var = (unsigned long *)((char *)var + var[2] + 0xc);\n'
     '                }\n'
     '            } else {\n',
     '                    var[2] = AlignedLength(info.mName.Length() + 1);\n'
     '                    DumpData(var + 3, (const char *)info.mName, info.mName.Length() + 1);\n'
     '                    var = (DWORD *)((char *)var + var[2] + 0xc);\n'
     '                }\n'
     '            } else {\n', 1),
    ('            // without debug info the end stays at the (zero) count: the\n'
     "            // record's size leaves it out, the next record overwrites it\n"
     '            table = (unsigned long *)level;\n'
     '            p = (char *)table;\n'
     '            record[10] = (char *)table - mBase;\n',
     '            // without debug info the end stays at the (zero) count: the\n'
     "            // record's size leaves it out, the next record overwrites it\n"
     '            table = (DWORD *)level;\n'
     '            p = (char *)table;\n'
     '            record[10] = (char *)table - mBase;\n', 1),
    ('                    const MfScConstUsageInfo &info = debug->GetConstUsageInfo(i);\n'
     '                    MfScConstUsageInfo usage(info.Name(), info.Value());\n'
     '                    *(unsigned long *)p = AlignedLength(usage.mName.Length() + 1);\n'
     '                    DumpData(p + 4, (const char *)usage.mName, usage.mName.Length() + 1);\n'
     '                    p += *(unsigned long *)p + 4;\n'
     '                    switch (usage.mValue.Type()) {\n'
     '                    case MfVariant::kInt:\n',
     '                    const MfScConstUsageInfo &info = debug->GetConstUsageInfo(i);\n'
     '                    MfScConstUsageInfo usage(info.Name(), info.Value());\n'
     '                    *(DWORD *)p = AlignedLength(usage.mName.Length() + 1);\n'
     '                    DumpData(p + 4, (const char *)usage.mName, usage.mName.Length() + 1);\n'
     '                    p += *(DWORD *)p + 4;\n'
     '                    switch (usage.mValue.Type()) {\n'
     '                    case MfVariant::kInt:\n', 1),
    ('\n'
     '            entry[0] = p - (char *)record + 4;\n'
     '            entry = (unsigned long *)((char *)entry + entry[0]);\n'
     '            functionSection[2]++;\n'
     '        }\n'
     '    }\n'
     '\n'
     '    unsigned long *info = (unsigned long *)(mBase + header[4]);\n'
     '    info[0] = 3;\n'
     '    info[1] = infoSize + 0xc;\n',
     '\n'
     '            entry[0] = p - (char *)record + 4;\n'
     '            entry = (DWORD *)((char *)entry + entry[0]);\n'
     '            functionSection[2]++;\n'
     '        }\n'
     '    }\n'
     '\n'
     '    DWORD *info = (DWORD *)(mBase + header[4]);\n'
     '    info[0] = 3;\n'
     '    info[1] = infoSize + 0xc;\n', 1),
    ('    now.Update();\n'
     '    info[6] = now.Seconds();\n'
     '    memset(&info[7], 0xb0, 10 * sizeof(unsigned long));\n'
     '\n'
     '    // the debug information goes to a .pdbg file of its own\n',
     '    now.Update();\n'
     '    info[6] = now.Seconds();\n'
     '    memset(&info[7], 0xb0, 10 * sizeof(DWORD));\n'
     '\n'
     '    // the debug information goes to a .pdbg file of its own\n', 1),
    ('        return;\n'
     '    MfString line;\n'
     '    unsigned long *header = (unsigned long *)mBase;\n'
     '    if (flags & 1) {\n'
     '        line.Format("OBJECT FILE HEADER:");\n',
     '        return;\n'
     '    MfString line;\n'
     '    DWORD *header = (DWORD *)mBase;\n'
     '    if (flags & 1) {\n'
     '        line.Format("OBJECT FILE HEADER:");\n', 1),
    ('        lines->Add(line);\n'
     '        union {\n'
     '            unsigned long all;\n'
     '            unsigned char part[4];\n'
     '        } version;\n',
     '        lines->Add(line);\n'
     '        union {\n'
     '            DWORD all;\n'
     '            unsigned char part[4];\n'
     '        } version;\n', 1),
    ('        lines->Add(line);\n'
     '    }\n'
     '    if ((flags & 8) && header[4] != (unsigned long)-1 && header[4] != 0) {\n'
     '        if (lines->GetCount() > 0)\n'
     '            lines->Add(MfString(""));\n'
     '        line.Format("OBJECT FILE INFOMATION:");\n'
     '        lines->Add(line);\n'
     '        unsigned long *entry = (unsigned long *)(mBase + header[4] + 0xc);\n'
     '        line.Format(" Section Entry %3d Offset: %08x", 1, (char *)entry - mBase);\n'
     '        lines->Add(line);\n'
     '        line.Format(" Section Entry %3d   Size: %08x", 1, entry[0]);\n'
     '        lines->Add(line);\n'
     '        unsigned long *info = (unsigned long *)(header[4] + mBase + 0x10);\n'
     '        line.Format("  Object File Type: %d", info[0]);\n'
     '        lines->Add(line);\n',
     '        lines->Add(line);\n'
     '    }\n'
     '    if ((flags & 8) && header[4] != (DWORD)-1 && header[4] != 0) {\n'
     '        if (lines->GetCount() > 0)\n'
     '            lines->Add(MfString(""));\n'
     '        line.Format("OBJECT FILE INFOMATION:");\n'
     '        lines->Add(line);\n'
     '        DWORD *entry = (DWORD *)(mBase + header[4] + 0xc);\n'
     '        line.Format(" Section Entry %3d Offset: %08x", 1, (char *)entry - mBase);\n'
     '        lines->Add(line);\n'
     '        line.Format(" Section Entry %3d   Size: %08x", 1, entry[0]);\n'
     '        lines->Add(line);\n'
     '        DWORD *info = (DWORD *)(header[4] + mBase + 0x10);\n'
     '        line.Format("  Object File Type: %d", info[0]);\n'
     '        lines->Add(line);\n', 1),
    ('        line.Format("MODULE NAME SECTION:");\n'
     '        lines->Add(line);\n'
     '        unsigned long *section = (unsigned long *)(mBase + header[2]);\n'
     '        line.Format("     Size: %08x", section[1]);\n'
     '        lines->Add(line);\n',
     '        line.Format("MODULE NAME SECTION:");\n'
     '        lines->Add(line);\n'
     '        DWORD *section = (DWORD *)(mBase + header[2]);\n'
     '        line.Format("     Size: %08x", section[1]);\n'
     '        lines->Add(line);\n', 1),
    ('        lines->Add(line);\n'
     '        PObjModuleRecord *record = (PObjModuleRecord *)(section + 3);\n'
     '        for (unsigned long i = 0; i < section[2]; i++) {\n'
     '            line.Format(" Section Entry %3d Offset: %08x", i + 1, (char *)record - mBase);\n'
     '            lines->Add(line);\n',
     '        lines->Add(line);\n'
     '        PObjModuleRecord *record = (PObjModuleRecord *)(section + 3);\n'
     '        for (DWORD i = 0; i < section[2]; i++) {\n'
     '            line.Format(" Section Entry %3d Offset: %08x", i + 1, (char *)record - mBase);\n'
     '            lines->Add(line);\n', 1),
    ('            for (int j = 0; j < offsets[0]; j++) {\n'
     '                int offset = offsets[j + 1];\n'
     '                line.Format("   Offset %3d: %08x (%s)", j + 1, offset, mBase + *(unsigned long *)(mBase + offset + 4) + 4);\n'
     '                lines->Add(line);\n'
     '            }\n',
     '            for (int j = 0; j < offsets[0]; j++) {\n'
     '                int offset = offsets[j + 1];\n'
     '                line.Format("   Offset %3d: %08x (%s)", j + 1, offset, mBase + *(DWORD *)(mBase + offset + 4) + 4);\n'
     '                lines->Add(line);\n'
     '            }\n', 1),
    ('        line.Format("FUNCTION BODY SECTION:");\n'
     '        lines->Add(line);\n'
     '        unsigned long *section = (unsigned long *)(mBase + header[3]);\n'
     '        line.Format("     Size: %08x", section[1]);\n'
     '        lines->Add(line);\n',
     '        line.Format("FUNCTION BODY SECTION:");\n'
     '        lines->Add(line);\n'
     '        DWORD *section = (DWORD *)(mBase + header[3]);\n'
     '        line.Format("     Size: %08x", section[1]);\n'
     '        lines->Add(line);\n', 1),
    ('        line.Format("");\n'
     '        lines->Add(line);\n'
     '        unsigned long *record = section + 3;\n'
     '        for (unsigned long i = 0; i < section[2]; i++) {\n'
     '            line.Format(" Section Entry %3d Offset: %08x", i + 1, (char *)record - mBase);\n'
     '            lines->Add(line);\n',
     '        line.Format("");\n'
     '        lines->Add(line);\n'
     '        DWORD *record = section + 3;\n'
     '        for (DWORD i = 0; i < section[2]; i++) {\n'
     '            line.Format(" Section Entry %3d Offset: %08x", i + 1, (char *)record - mBase);\n'
     '            lines->Add(line);\n', 1),
    ('            line.Format("      Param String Offset: %08x (%s)", record[5], mBase + record[5] + 4);\n'
     '            lines->Add(line);\n'
     '            line.Format("          Bytecode Offset: %08x (%d bytecodes)", record[6], *(unsigned long *)(mBase + record[6]));\n'
     '            lines->Add(line);\n'
     '            line.Format("     Constant Pool Offset: %08x (%d constants)", record[7], *(unsigned long *)(mBase + record[7]));\n'
     '            lines->Add(line);\n'
     '            if (record[8] != 0xa0a0a0a0 && record[8] != 0xdbdbdbdb)\n'
     '                line.Format("      Line Numbers Offset: %08x (%d entries)", record[8], *(unsigned long *)(mBase + record[8]));\n'
     '            else\n'
     '                line.Format("      Line Numbers Offset: %s", record[8] == 0xa0a0a0a0 ? "UNIMPLEMENTED" : "DEBUG FILE");\n'
     '            lines->Add(line);\n'
     '            if (record[9] != 0xa0a0a0a0 && record[9] != 0xdbdbdbdb)\n'
     '                line.Format("   Debug Variables Offset: %08x (%d entries)", record[9], *(unsigned long *)(mBase + record[9]));\n'
     '            else\n'
     '                line.Format("   Debug Variables Offset: %s", record[9] == 0xa0a0a0a0 ? "UNIMPLEMENTED" : "DEBUG FILE");\n'
     '            lines->Add(line);\n'
     '            if (record[10] != 0xa0a0a0a0 && record[10] != 0xdbdbdbdb)\n'
     '                line.Format("       Code Levels Offset: %08x (%d entries)", record[10], *(unsigned long *)(mBase + record[10]));\n'
     '            else\n'
     '                line.Format("       Code Levels Offset: %s", record[10] == 0xa0a0a0a0 ? "UNIMPLEMENTED" : "DEBUG FILE");\n'
     '            lines->Add(line);\n'
     '            if (record[11] != 0xa0a0a0a0 && record[11] != 0xdbdbdbdb)\n'
     '                line.Format("  Constant Symbols Offset: %08x (%d entries)", record[11], *(unsigned long *)(mBase + record[11]));\n'
     '            else\n'
     '                line.Format("  Constant Symbols Offset: %s", record[11] == 0xa0a0a0a0 ? "UNIMPLEMENTED" : "DEBUG FILE");\n',
     '            line.Format("      Param String Offset: %08x (%s)", record[5], mBase + record[5] + 4);\n'
     '            lines->Add(line);\n'
     '            line.Format("          Bytecode Offset: %08x (%d bytecodes)", record[6], *(DWORD *)(mBase + record[6]));\n'
     '            lines->Add(line);\n'
     '            line.Format("     Constant Pool Offset: %08x (%d constants)", record[7], *(DWORD *)(mBase + record[7]));\n'
     '            lines->Add(line);\n'
     '            if (record[8] != 0xa0a0a0a0 && record[8] != 0xdbdbdbdb)\n'
     '                line.Format("      Line Numbers Offset: %08x (%d entries)", record[8], *(DWORD *)(mBase + record[8]));\n'
     '            else\n'
     '                line.Format("      Line Numbers Offset: %s", record[8] == 0xa0a0a0a0 ? "UNIMPLEMENTED" : "DEBUG FILE");\n'
     '            lines->Add(line);\n'
     '            if (record[9] != 0xa0a0a0a0 && record[9] != 0xdbdbdbdb)\n'
     '                line.Format("   Debug Variables Offset: %08x (%d entries)", record[9], *(DWORD *)(mBase + record[9]));\n'
     '            else\n'
     '                line.Format("   Debug Variables Offset: %s", record[9] == 0xa0a0a0a0 ? "UNIMPLEMENTED" : "DEBUG FILE");\n'
     '            lines->Add(line);\n'
     '            if (record[10] != 0xa0a0a0a0 && record[10] != 0xdbdbdbdb)\n'
     '                line.Format("       Code Levels Offset: %08x (%d entries)", record[10], *(DWORD *)(mBase + record[10]));\n'
     '            else\n'
     '                line.Format("       Code Levels Offset: %s", record[10] == 0xa0a0a0a0 ? "UNIMPLEMENTED" : "DEBUG FILE");\n'
     '            lines->Add(line);\n'
     '            if (record[11] != 0xa0a0a0a0 && record[11] != 0xdbdbdbdb)\n'
     '                line.Format("  Constant Symbols Offset: %08x (%d entries)", record[11], *(DWORD *)(mBase + record[11]));\n'
     '            else\n'
     '                line.Format("  Constant Symbols Offset: %s", record[11] == 0xa0a0a0a0 ? "UNIMPLEMENTED" : "DEBUG FILE");\n', 1),
    ('            line.Format("");\n'
     '            lines->Add(line);\n'
     '            record = (unsigned long *)((char *)record + record[0]);\n'
     '        }\n'
     '    }\n',
     '            line.Format("");\n'
     '            lines->Add(line);\n'
     '            record = (DWORD *)((char *)record + record[0]);\n'
     '        }\n'
     '    }\n', 1),
]

# PiccoloRT/MfScPiccoloObjectFile.h: the .pobj object file is a 32-bit format: its words are read and written as DWORD (32 bits) where the source has `unsigned long` (64 bits on macOS); offsets are computed from char pointers, not through int casts.
EDITS['PiccoloRT/MfScPiccoloObjectFile.h'] = [
    ('private:\n'
     '    MfScPOFErrorCode SetLastError(MfScPOFErrorCode code) { return mError = code; }\n'
     '    MfScDataHandle *BuildFunction(unsigned long offset);\n'
     '    PObjConstantPoolEntry *WriteConstantPoolEntry(PObjConstantPoolEntry *entry, const MfString &value);\n'
     '    PObjConstantPoolEntry *WriteConstantPoolEntry(PObjConstantPoolEntry *entry, float value);\n'
     '    PObjConstantPoolEntry *WriteConstantPoolEntry(PObjConstantPoolEntry *entry, int value);\n'
     '    int IsValidVersion();\n'
     '    int OpenFileMappingA(unsigned long size);\n'
     '    void CloseFileMapping();\n'
     '    int ReadModuleNameTable();\n',
     'private:\n'
     '    MfScPOFErrorCode SetLastError(MfScPOFErrorCode code) { return mError = code; }\n'
     '    MfScDataHandle *BuildFunction(DWORD offset);\n'
     '    PObjConstantPoolEntry *WriteConstantPoolEntry(PObjConstantPoolEntry *entry, const MfString &value);\n'
     '    PObjConstantPoolEntry *WriteConstantPoolEntry(PObjConstantPoolEntry *entry, float value);\n'
     '    PObjConstantPoolEntry *WriteConstantPoolEntry(PObjConstantPoolEntry *entry, int value);\n'
     '    int IsValidVersion();\n'
     '    int OpenFileMappingA(DWORD size);\n'
     '    void CloseFileMapping();\n'
     '    int ReadModuleNameTable();\n', 1),
    ('    unsigned int ComputeModuleNameSectionSize();\n'
     '    unsigned int ComputeObjectFileInfoSectionSize();\n'
     '    void *OffsetToAddress(unsigned long offset);\n'
     '    unsigned long AddressToOffset(void *address);\n'
     '    unsigned long AlignedLength(unsigned long length)\n'
     '    {\n'
     '        unsigned long rest = length & 3;\n'
     '        if (rest)\n'
     '            return length - rest + 4;\n',
     '    unsigned int ComputeModuleNameSectionSize();\n'
     '    unsigned int ComputeObjectFileInfoSectionSize();\n'
     '    void *OffsetToAddress(DWORD offset);\n'
     '    DWORD AddressToOffset(void *address);\n'
     '    DWORD AlignedLength(DWORD length)\n'
     '    {\n'
     '        DWORD rest = length & 3;\n'
     '        if (rest)\n'
     '            return length - rest + 4;\n', 1),
    ('    }\n'
     '    // the address of a section of a function record, 0 if it has none\n'
     '    char *SectionAddress(unsigned long offset)\n'
     '    {\n'
     '        if (offset == 0xdbdbdbdb || offset == 0xa0a0a0a0)\n',
     '    }\n'
     '    // the address of a section of a function record, 0 if it has none\n'
     '    char *SectionAddress(DWORD offset)\n'
     '    {\n'
     '        if (offset == 0xdbdbdbdb || offset == 0xa0a0a0a0)\n', 1),
]

# PiccoloRT/MfScVirtualMachine.cpp: stack words are uintptr_t (pointer-sized); pointers handed to the debugger as int go through intptr_t.
EDITS['PiccoloRT/MfScVirtualMachine.cpp'] = [
    ('            env->m_sp[i] = args[i - 1];\n'
     '    }\n'
     '    env->PushWord((unsigned long)MfScDataHandle::CreateData(numArgs));\n'
     '    env->PushData(function);\n'
     '    // call the function, then return\n',
     '            env->m_sp[i] = args[i - 1];\n'
     '    }\n'
     '    env->PushWord((uintptr_t)MfScDataHandle::CreateData(numArgs)); // PORT: uintptr_t\n'
     '    env->PushData(function);\n'
     '    // call the function, then return\n', 1),
    ('\n'
     '// 0x1001b2b0 ?HasType@@YAHPAVMfScDataHandle@@K@Z\n'
     'inline int HasType(MfScDataHandle *h, unsigned long type)\n'
     '{\n'
     '    return h->GetType() & type;\n',
     '\n'
     '// 0x1001b2b0 ?HasType@@YAHPAVMfScDataHandle@@K@Z\n'
     'inline int HasType(MfScDataHandle *h, unsigned int type)\n'
     '{\n'
     '    return h->GetType() & type;\n', 1),
    ('    env->m_ip = code;\n'
     '    env->m_constants = constants;\n'
     '    unsigned long *stackBase = (unsigned long *)env->m_sp;\n'
     '    MfAiPdbDebugger *debugger = MfScDebugger::pdbgTheDebugger;\n'
     '    env->m_frame = 0;\n',
     '    env->m_ip = code;\n'
     '    env->m_constants = constants;\n'
     '    uintptr_t *stackBase = (uintptr_t *)env->m_sp; // PORT: uintptr_t\n'
     '    MfAiPdbDebugger *debugger = MfScDebugger::pdbgTheDebugger;\n'
     '    env->m_frame = 0;\n', 1),
    ('                        if (debugger->GetInterface(kPdbBreakpointManager, (MfAi **)&manager) == 0) {\n'
     '                            MfPdbBreakpointInfo info;\n'
     '                            manager->GetBreakpointInfo((unsigned long)env->m_ip, (int)env, &info);\n'
     '                            manager->Release();\n'
     '                            if (i > 0)\n',
     '                        if (debugger->GetInterface(kPdbBreakpointManager, (MfAi **)&manager) == 0) {\n'
     '                            MfPdbBreakpointInfo info;\n'
     '                            manager->GetBreakpointInfo((uintptr_t)env->m_ip, (int)(intptr_t)env, &info); // PORT: uintptr_t, intptr_t\n'
     '                            manager->Release();\n'
     '                            if (i > 0)\n', 1),
    ('                    if (debugger) {\n'
     '                        if (debugger->GetInterface(kPdbBreakpointManager, (MfAi **)&manager) == 0) {\n'
     '                            manager->BreakpointHit((unsigned long)jumps, (int)env->m_ip, env);\n'
     '                            manager->Release();\n'
     '                        } else {\n',
     '                    if (debugger) {\n'
     '                        if (debugger->GetInterface(kPdbBreakpointManager, (MfAi **)&manager) == 0) {\n'
     '                            manager->BreakpointHit((uintptr_t)jumps, (int)(intptr_t)env->m_ip, env); // PORT: uintptr_t, intptr_t\n'
     '                            manager->Release();\n'
     '                        } else {\n', 1),
    ('            MfString where;\n'
     '            where.Format("%s(): ", (const char *)function->Name());\n'
     '            int offset = frame.mIP - (unsigned long)routine->mBytecodes;\n'
     '            if (routine->DebugInfo())\n'
     '                line.Format("Line %d in Module: %s", routine->DebugInfo()->FindLine(offset),\n',
     '            MfString where;\n'
     '            where.Format("%s(): ", (const char *)function->Name());\n'
     '            int offset = frame.mIP - (uintptr_t)routine->mBytecodes; // PORT: uintptr_t\n'
     '            if (routine->DebugInfo())\n'
     '                line.Format("Line %d in Module: %s", routine->DebugInfo()->FindLine(offset),\n', 1),
]

# PiccoloRT/MfScVmEnv.h: stack words are uintptr_t (pointer-sized): the 32-bit source used unsigned long, which was the size of a pointer there.
EDITS['PiccoloRT/MfScVmEnv.h'] = [
    ('#pragma once\n'
     '#include "MfScVariableStack.h"\n'
     '#include "MfString.h"\n',
     '#pragma once\n'
     '#include <stdint.h> // PORT: uintptr_t\n'
     '#include "MfScVariableStack.h"\n'
     '#include "MfString.h"\n'
     '#line 4\n', 1),
    ('    int PushFunctionFrame();\n'
     '    void PopFunctionFrame();\n'
     '    void ReleaseStack(unsigned long *to);\n'
     '    int FirstFrame(MfScVmFrame *frame) const;\n'
     '    int NextFrame(MfScVmFrame *frame) const;\n',
     '    int PushFunctionFrame();\n'
     '    void PopFunctionFrame();\n'
     '    void ReleaseStack(uintptr_t *to); // PORT: uintptr_t stack words\n'
     '    int FirstFrame(MfScVmFrame *frame) const;\n'
     '    int NextFrame(MfScVmFrame *frame) const;\n', 1),
    ('    __int64 GetAccumProcessTime() { return m_accumTime; }\n'
     '\n'
     '    void PushWord(unsigned long w)\n'
     '    {\n'
     '        *(unsigned long *)m_sp = w;\n'
     '        m_sp--;\n'
     '    }\n'
     '    unsigned long PopWord() { return *(unsigned long *)++m_sp; }\n'
     '    void PushData(MfScDataHandle *h)\n'
     '    {\n',
     '    __int64 GetAccumProcessTime() { return m_accumTime; }\n'
     '\n'
     '    // PORT: a stack word is uintptr_t\n'
     '    void PushWord(uintptr_t w)\n'
     '    {\n'
     '        *(uintptr_t *)m_sp = w;\n'
     '        m_sp--;\n'
     '    }\n'
     '    uintptr_t PopWord() { return *(uintptr_t *)++m_sp; }\n'
     '    void PushData(MfScDataHandle *h)\n'
     '    {\n'
     '#line 104\n', 1),
    ('    unsigned char *m_ip;            // +0x08 the next bytecode\n'
     '    MfScDataHandle **m_sp;          // +0x0c the data stack (grows down, points at the free slot)\n'
     '    unsigned long *m_frame;         // +0x10 the current call frame (FP)\n'
     "    MfScDataHandle **m_constants;   // +0x14 the running routine's constant pool (CPP)\n"
     '    MfScDataHandle *m_object;       // +0x18 the object the routine runs on ("this")\n',
     '    unsigned char *m_ip;            // +0x08 the next bytecode\n'
     '    MfScDataHandle **m_sp;          // +0x0c the data stack (grows down, points at the free slot)\n'
     '    uintptr_t *m_frame;             // PORT: uintptr_t; +0x10 the current call frame (FP)\n'
     "    MfScDataHandle **m_constants;   // +0x14 the running routine's constant pool (CPP)\n"
     '    MfScDataHandle *m_object;       // +0x18 the object the routine runs on ("this")\n', 1),
    ('    MfScDataHandle *m_reg3;         // +0x2c\n'
     '    MfScVariableStack *m_varStack;  // +0x30\n'
     '    unsigned long *m_stackBase;     // +0x34\n'
     '    long m_stackSize;               // +0x38\n'
     '    MfScVmStack *m_stack;           // +0x3c\n'
     '    MfScVmException *m_exception;   // +0x40\n',
     '    MfScDataHandle *m_reg3;         // +0x2c\n'
     '    MfScVariableStack *m_varStack;  // +0x30\n'
     '    uintptr_t *m_stackBase;         // PORT: uintptr_t; +0x34\n'
     '    int m_stackSize;               // +0x38\n'
     '    MfScVmStack *m_stack;           // +0x3c\n'
     '    MfScVmException *m_exception;   // +0x40\n', 1),
]

# PiccoloRT/MfScVmEnv_3.cpp: stack words are uintptr_t (pointer-sized), and the initial stack pointer is counted in words of that size (the source divided by 4).
EDITS['PiccoloRT/MfScVmEnv_3.cpp'] = [
    ('      m_executing(0), m_id(0), m_accumTime(0)\n'
     '{\n'
     '    unsigned long committed;\n'
     '    m_stack = new MfScVmStack(MfScVmStack::PageSize() << 5, 1, MfScVmStack::PageSize() << 4,\n'
     '                              (void **)&m_stackBase, &committed);\n'
     '    m_sp = (MfScDataHandle **)&m_stackBase[committed / 4 - 1];\n'
     '    m_stackSize += committed;\n'
     '    INIT_VARPOOL();\n',
     '      m_executing(0), m_id(0), m_accumTime(0)\n'
     '{\n'
     '    unsigned int committed;\n'
     '    m_stack = new MfScVmStack(MfScVmStack::PageSize() << 5, 1, MfScVmStack::PageSize() << 4,\n'
     '                              (void **)&m_stackBase, &committed);\n'
     '    m_sp = (MfScDataHandle **)&m_stackBase[committed / sizeof(*m_stackBase) - 1]; // PORT: was committed / 4\n'
     '    m_stackSize += committed;\n'
     '    INIT_VARPOOL();\n', 1),
    ('int MfScVmEnv::PushFunctionFrame()\n'
     '{\n'
     '    PushWord((unsigned long)m_ip);\n'
     '    PushWord((unsigned long)m_object);\n'
     '    PushWord((unsigned long)m_function);\n'
     '    PushWord((unsigned long)m_constants);\n'
     '    PushWord((unsigned long)m_frame);\n'
     '    m_frame = (unsigned long *)m_sp;\n'
     '    return 1;\n'
     '}\n',
     'int MfScVmEnv::PushFunctionFrame()\n'
     '{\n'
     '    PushWord((uintptr_t)m_ip); // PORT: uintptr_t stack words (here and below)\n'
     '    PushWord((uintptr_t)m_object);\n'
     '    PushWord((uintptr_t)m_function);\n'
     '    PushWord((uintptr_t)m_constants);\n'
     '    PushWord((uintptr_t)m_frame);\n'
     '    m_frame = (uintptr_t *)m_sp;\n'
     '    return 1;\n'
     '}\n', 1),
    ('{\n'
     '    ReleaseStack(m_frame);\n'
     '    m_frame = (unsigned long *)PopWord();\n'
     '    m_constants = (MfScDataHandle **)PopWord();\n'
     '    if (m_function)\n',
     '{\n'
     '    ReleaseStack(m_frame);\n'
     '    m_frame = (uintptr_t *)PopWord();\n'
     '    m_constants = (MfScDataHandle **)PopWord();\n'
     '    if (m_function)\n', 1),
    ('\n'
     '// 0x1001bba0 ?ReleaseStack@MfScVmEnv@@QAEXPAK@Z\n'
     'void MfScVmEnv::ReleaseStack(unsigned long *to)\n'
     '{\n'
     '    while ((unsigned long *)m_sp < to)\n'
     '        PopAndReleaseData();\n'
     '}\n',
     '\n'
     '// 0x1001bba0 ?ReleaseStack@MfScVmEnv@@QAEXPAK@Z\n'
     'void MfScVmEnv::ReleaseStack(uintptr_t *to)\n'
     '{\n'
     '    while ((uintptr_t *)m_sp < to)\n'
     '        PopAndReleaseData();\n'
     '}\n', 1),
    ('{\n'
     '    frame->ReleaseHandles();\n'
     '    frame->mIP = (unsigned long)m_ip;\n'
     '    frame->mObject = m_object;\n'
     '    if (frame->mObject)\n',
     '{\n'
     '    frame->ReleaseHandles();\n'
     '    frame->mIP = (uintptr_t)m_ip;\n'
     '    frame->mObject = m_object;\n'
     '    if (frame->mObject)\n', 1),
    ('    if (frame->mFunction)\n'
     '        frame->mFunction->AddRef();\n'
     '    frame->mConstants = (unsigned long)m_constants;\n'
     '    frame->mFrame = m_frame;\n'
     '    return frame->mFrame != 0;\n',
     '    if (frame->mFunction)\n'
     '        frame->mFunction->AddRef();\n'
     '    frame->mConstants = (uintptr_t)m_constants;\n'
     '    frame->mFrame = m_frame;\n'
     '    return frame->mFrame != 0;\n', 1),
    ('        frame->mFunction->AddRef();\n'
     '    frame->mConstants = frame->mFrame[2];\n'
     '    frame->mFrame = (unsigned long *)frame->mFrame[1];\n'
     '    return frame->mFrame != 0;\n'
     '}\n',
     '        frame->mFunction->AddRef();\n'
     '    frame->mConstants = frame->mFrame[2];\n'
     '    frame->mFrame = (uintptr_t *)frame->mFrame[1];\n'
     '    return frame->mFrame != 0;\n'
     '}\n', 1),
    ('        if (routine) {\n'
     '            entry->mModule << routine->ModuleName();\n'
     '            entry->mOffset = frame.mIP - (unsigned long)routine->mBytecodes;\n'
     '            if (routine->mDebugInfo)\n'
     '                entry->mLine = routine->mDebugInfo->FindLine(entry->mOffset);\n',
     '        if (routine) {\n'
     '            entry->mModule << routine->ModuleName();\n'
     '            entry->mOffset = frame.mIP - (uintptr_t)routine->mBytecodes;\n'
     '            if (routine->mDebugInfo)\n'
     '                entry->mLine = routine->mDebugInfo->FindLine(entry->mOffset);\n', 1),
]

# PiccoloRT/MfScVmFrame.h: stack words are uintptr_t (pointer-sized): the 32-bit source used unsigned long, which was the size of a pointer there.
EDITS['PiccoloRT/MfScVmFrame.h'] = [
    ('#pragma once\n'
     'class MfScDataHandle;\n'
     '\n',
     '#pragma once\n'
     '#include <stdint.h> // PORT: uintptr_t\n'
     'class MfScDataHandle;\n'
     '\n'
     '#line 4\n', 1),
    ('    void ReleaseHandles();\n'
     '\n'
     '    unsigned long mIP;          // +0x00 (MfScVmEnv +0x08)\n'
     '    MfScDataHandle *mObject;    // +0x04 (MfScVmEnv +0x18)\n'
     '    MfScDataHandle *mFunction;  // +0x08 (MfScVmEnv +0x1c)\n'
     '    unsigned long mConstants;   // +0x0c (MfScVmEnv +0x14)\n'
     '    unsigned long *mFrame;      // +0x10 (MfScVmEnv +0x10)\n'
     '};\n',
     '    void ReleaseHandles();\n'
     '\n'
     '    uintptr_t mIP;              // PORT: uintptr_t; +0x00 (MfScVmEnv +0x08)\n'
     '    MfScDataHandle *mObject;    // +0x04 (MfScVmEnv +0x18)\n'
     '    MfScDataHandle *mFunction;  // +0x08 (MfScVmEnv +0x1c)\n'
     '    uintptr_t mConstants;       // PORT: uintptr_t; +0x0c (MfScVmEnv +0x14)\n'
     '    uintptr_t *mFrame;          // PORT: uintptr_t; +0x10 (MfScVmEnv +0x10)\n'
     '};\n', 1),
]

# PiccoloRT/MfThread.h: the members have the types of the real class in mfutil (pointers where the 32-bit view had int), so sizes and offsets agree on 64-bit.
EDITS['PiccoloRT/MfThread.h'] = [
    ('    MfThread(int);\n'
     '    int AssignWork(MfThreadWork *);\n'
     '    int PostThreadMessageA(unsigned int, unsigned int, long);\n'
     '    static MfThread * __cdecl GetThread();\n'
     '    static void __cdecl ReleaseTls();\n'
     '    unsigned int GetState() const { return mState; }\n'
     '    unsigned long GetId() const;\n'
     '    void * GetHandle() const;\n'
     '    void ModState(int);\n',
     '    MfThread(int);\n'
     '    int AssignWork(MfThreadWork *);\n'
     '    int PostThreadMessageA(unsigned int, unsigned int, int);\n'
     '    static MfThread * __cdecl GetThread();\n'
     '    static void __cdecl ReleaseTls();\n'
     '    unsigned int GetState() const { return mState; }\n'
     '    unsigned int GetId() const;\n'
     '    void * GetHandle() const;\n'
     '    void ModState(int);\n', 1),
    ('    static void __cdecl ThreadInit(void *);\n'
     '    void SetState(int);\n'
     '    static unsigned long iThreadDataIndex;\n'
     'public:\n'
     '    int field_4;\n'
     '    int field_8;\n'
     '    int field_c;\n'
     "    void *mHandle;                  // +0x10 the thread's handle\n"
     '    unsigned int mState;\n'
     '    int field_18;\n'
     '    int field_1c;\n'
     '};\n',
     '    static void __cdecl ThreadInit(void *);\n'
     '    void SetState(int);\n'
     '    static unsigned int iThreadDataIndex;\n'
     'public:\n'
     "    // PORT: the member types of mfutil's MfThread (src/mfutil/MfThread.h), so the\n"
     '    // object is as big as the real one (64 bytes) and MfScThread::mVmEnv lands at +64\n'
     '    unsigned int field_4;          // dwThreadId\n'
     '    int field_8;                    // iFlags\n'
     '    MfThread *field_c;              // pThread\n'
     "    void *mHandle;                  // +0x10 the thread's handle\n"
     '    unsigned int mState;\n'
     '    void *field_18;                 // pevtWork\n'
     '    void *field_1c;                 // pWork\n'
     '};\n'
     '#line 41\n', 1),
]
