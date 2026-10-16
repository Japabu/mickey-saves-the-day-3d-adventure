# PiccoloComp edits for the port (see port/rewrite.py, EDITS).
#
# Inline members of mfutil's classes: the original imports them from mfutil.dll,
# which emitted and exported every inline member of its dllexport classes. Here
# dllexport is a visibility attribute and unused inlines are not emitted (those that
# are come out hidden), so the views that declare them without a body get mfutil's
# body (src/mfutil/*.h).
MFUTIL = "    // PORT: mfutil's inline"

EDITS = {
    'PiccoloComp/MfCallBack.h': [
        ('    virtual int CallBack(void *);\n'
         '    virtual int CallBack(void *, void *);\n'
         '    virtual int CallBack(void *, void *, void *);\n',
         '    virtual int CallBack(void *) { return 0; }' + MFUTIL + '\n'
         '    virtual int CallBack(void *, void *) { return 0; }' + MFUTIL + '\n'
         '    virtual int CallBack(void *, void *, void *) { return 0; }' + MFUTIL + '\n', 1),
    ],
    'PiccoloComp/MfDblList.h': [
        ('    virtual void *GetFirst(MfDblListItem *&pli) const;  ',
         '    virtual void *GetFirst(MfDblListItem *&pli) const { pli = pliFirst; return pli ? pli->GetData() : 0; }' + MFUTIL + ' ', 1),
        ('    virtual void *GetNext(MfDblListItem *&pli) const;  ',
         '    virtual void *GetNext(MfDblListItem *&pli) const { if (pli) pli = pli->pliNext; return pli ? pli->pData : 0; }' + MFUTIL + ' ', 1),
        ('    virtual void *GetCurrent(MfDblListItem *&pli) const;  ',
         '    virtual void *GetCurrent(MfDblListItem *&pli) const { return pli ? pli->GetData() : 0; }' + MFUTIL + ' ', 1),
    ],
    'PiccoloComp/MfString.h': [
        ('    void CopyOnWrite(int);\n',
         '    void CopyOnWrite(int extra) { if (mfsr == &mfsrNullInitializer) { mfsr = new MfStringRef(extra); mfsr->m_count++; }'
         ' else if (mfsr->m_count > 1) { mfsr->m_count--; mfsr = mfsr->CopyOnWrite(); mfsr->m_count++; } }' + MFUTIL + '\n', 1),
    ],
    # mfutil defines the constructor out of line (src/mfutil/MfStack.cpp) and exports
    # it; the inline one here would need mfutil's vtable, which is not exported
    'PiccoloComp/MfStack.h': [
        ('    MfStack(int max = -1) : nMax(max) {}\n',
         '    MfStack(int max = -1);    // PORT: mfutil\'s out-of-line constructor (src/mfutil/MfStack.cpp)\n', 1),
    ],
}
