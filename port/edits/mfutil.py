# mfutil edits for the port (see port/rewrite.py, EDITS).
#
# The logger shows its messages in a list view window, which the port has no control
# for, and writes them to steamboat.log only from its window thread. Each message is
# also passed to OutputDebugString when it is logged (on the line that queues it), so
# script errors and Logger.Log lines reach the port's log (PORT_LOG=1) in order with
# everything else.
_ECHO = ('    csQueueLock.Enter();\n    plLogQueue->Add(pMessage);',
         '    { MfString fsLine = MfString("[MfLogger] ") + fsSource + MfString(": ") + fsMessage + MfString("\\n");'
         ' OutputDebugString(fsLine); MfDblListItem *pliExtra;'
         ' for (MfString *pfs = pMessage->lstExtra.First(pliExtra); pfs; pfs = pMessage->lstExtra.Next(pliExtra))'
         ' { OutputDebugString("    "); OutputDebugString(*pfs); OutputDebugString("\\n"); } }'
         ' csQueueLock.Enter();\n    plLogQueue->Add(pMessage);', 3)

# "%#m" (no leading zero) is Visual C++'s strftime; the C library prints it literally.
_TIME = [('"%#m/%#d/%y %#I:%M:%S %p"', '"%m/%d/%y %I:%M:%S %p"', 1),
         ('"%#m/%#d/%y"', '"%m/%d/%y"', 2),
         ('"%#I:%M:%S %p"', '"%I:%M:%S %p"', 2)]

EDITS = {
    'mfutil/MfLogger.cpp': [_ECHO] + _TIME,
}

# mfutil/MfKeyDialog.cpp: the window keeps its C++ object in GWLP_USERDATA through Set/GetWindowLongPtr (a LONG truncates the pointer).
EDITS['mfutil/MfKeyDialog.cpp'] = [
    ('    if (msg == WM_INITDIALOG) {\n'
     '        MfKeyDialog *dlg = (MfKeyDialog *)lParam;\n'
     '        SetWindowLongA(hwnd, GWL_USERDATA, lParam);\n'
     '        dlg->m_hDlg = hwnd;\n'
     '        result = dlg->OnInitDialog();\n',
     '    if (msg == WM_INITDIALOG) {\n'
     '        MfKeyDialog *dlg = (MfKeyDialog *)lParam;\n'
     '        SetWindowLongPtrA(hwnd, GWLP_USERDATA, lParam);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)\n'
     '        dlg->m_hDlg = hwnd;\n'
     '        result = dlg->OnInitDialog();\n', 1),
    ('        return result;\n'
     '    }\n'
     '    MfKeyDialog *dlg = (MfKeyDialog *)GetWindowLongA(hwnd, GWL_USERDATA);\n'
     '    if (msg == WM_COMMAND) {\n'
     '        WORD id = LOWORD(wParam);\n',
     '        return result;\n'
     '    }\n'
     '    MfKeyDialog *dlg = (MfKeyDialog *)GetWindowLongPtrA(hwnd, GWLP_USERDATA);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)\n'
     '    if (msg == WM_COMMAND) {\n'
     '        WORD id = LOWORD(wParam);\n', 1),
]

# mfutil/MfLoggerUI.cpp: the window keeps its C++ object in GWLP_USERDATA through Set/GetWindowLongPtr (a LONG truncates the pointer).
EDITS['mfutil/MfLoggerUI.cpp'] = [
    ('    if (uiMsg == WM_CREATE) {\n'
     '        CREATESTRUCT *pcs = (CREATESTRUCT *)lParam;\n'
     '        SetWindowLong(hwnd, GWL_USERDATA, (LONG)pcs->lpCreateParams);\n'
     '        pFrame = (MfLoggerFrame *)pcs->lpCreateParams;\n'
     '        pFrame->hWnd = hwnd;\n'
     '        lResult = pFrame->OnCreate(pcs);\n'
     '    } else {\n'
     '        pFrame = (MfLoggerFrame *)GetWindowLong(hwnd, GWL_USERDATA);\n'
     '        if (pFrame) {\n'
     '            pFrame->uiMsg = uiMsg;\n',
     '    if (uiMsg == WM_CREATE) {\n'
     '        CREATESTRUCT *pcs = (CREATESTRUCT *)lParam;\n'
     '        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pcs->lpCreateParams);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)\n'
     '        pFrame = (MfLoggerFrame *)pcs->lpCreateParams;\n'
     '        pFrame->hWnd = hwnd;\n'
     '        lResult = pFrame->OnCreate(pcs);\n'
     '    } else {\n'
     '        pFrame = (MfLoggerFrame *)GetWindowLongPtr(hwnd, GWLP_USERDATA);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)\n'
     '        if (pFrame) {\n'
     '            pFrame->uiMsg = uiMsg;\n', 1),
    ('    if (uiMsg == WM_INITDIALOG) {\n'
     '        pDialog = (MfLogMessageDialog *)lParam;\n'
     '        SetWindowLong(hDlg, GWL_USERDATA, lParam);\n'
     '        pDialog->hDlg = hDlg;\n'
     '        bResult = pDialog->OnInitDialog();\n',
     '    if (uiMsg == WM_INITDIALOG) {\n'
     '        pDialog = (MfLogMessageDialog *)lParam;\n'
     '        SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)\n'
     '        pDialog->hDlg = hDlg;\n'
     '        bResult = pDialog->OnInitDialog();\n', 1),
    ('                   rc.right - rc.left, rc.bottom - rc.top, TRUE);\n'
     '    } else {\n'
     '        pDialog = (MfLogMessageDialog *)GetWindowLong(hDlg, GWL_USERDATA);\n'
     '        if (uiMsg == WM_COMMAND && (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)) {\n'
     '            EndDialog(hDlg, 0);\n',
     '                   rc.right - rc.left, rc.bottom - rc.top, TRUE);\n'
     '    } else {\n'
     '        pDialog = (MfLogMessageDialog *)GetWindowLongPtr(hDlg, GWLP_USERDATA);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)\n'
     '        if (uiMsg == WM_COMMAND && (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)) {\n'
     '            EndDialog(hDlg, 0);\n', 1),
    ('    if (uiMsg == WM_INITDIALOG) {\n'
     '        pOptions = (MfLoggerOptions *)lParam;\n'
     '        SetWindowLong(hDlg, GWL_USERDATA, lParam);\n'
     '        pOptions->bInitializing = 1;\n'
     '        pOptions->hDlg = hDlg;\n',
     '    if (uiMsg == WM_INITDIALOG) {\n'
     '        pOptions = (MfLoggerOptions *)lParam;\n'
     '        SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)\n'
     '        pOptions->bInitializing = 1;\n'
     '        pOptions->hDlg = hDlg;\n', 1),
    ('        pOptions->bInitializing = 0;\n'
     '    } else {\n'
     '        pOptions = (MfLoggerOptions *)GetWindowLong(hDlg, GWL_USERDATA);\n'
     '        switch (uiMsg) {\n'
     '        case WM_CLOSE:\n',
     '        pOptions->bInitializing = 0;\n'
     '    } else {\n'
     '        pOptions = (MfLoggerOptions *)GetWindowLongPtr(hDlg, GWLP_USERDATA);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)\n'
     '        switch (uiMsg) {\n'
     '        case WM_CLOSE:\n', 1),
]

# mfutil/MfNameDialog.cpp: the window keeps its C++ object in GWLP_USERDATA through Set/GetWindowLongPtr (a LONG truncates the pointer).
EDITS['mfutil/MfNameDialog.cpp'] = [
    ('    if (msg == WM_INITDIALOG) {\n'
     '        MfNameDialog *dlg = (MfNameDialog *)lParam;\n'
     '        SetWindowLongA(hwnd, GWL_USERDATA, lParam);\n'
     '        dlg->m_hDlg = hwnd;\n'
     '        result = dlg->OnInitDialog();\n',
     '    if (msg == WM_INITDIALOG) {\n'
     '        MfNameDialog *dlg = (MfNameDialog *)lParam;\n'
     '        SetWindowLongPtrA(hwnd, GWLP_USERDATA, lParam);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)\n'
     '        dlg->m_hDlg = hwnd;\n'
     '        result = dlg->OnInitDialog();\n', 1),
    ('                   r.right - r.left, r.bottom - r.top, TRUE);\n'
     '    } else {\n'
     '        MfNameDialog *dlg = (MfNameDialog *)GetWindowLongA(hwnd, GWL_USERDATA);\n'
     '        if (msg == WM_COMMAND && (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)) {\n'
     '            char buf[128];\n',
     '                   r.right - r.left, r.bottom - r.top, TRUE);\n'
     '    } else {\n'
     '        MfNameDialog *dlg = (MfNameDialog *)GetWindowLongPtrA(hwnd, GWLP_USERDATA);   // PORT: pointer-sized slot (SetWindowLong truncates the pointer to 32 bits)\n'
     '        if (msg == WM_COMMAND && (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)) {\n'
     '            char buf[128];\n', 1),
]

# mfutil/MfString.cpp: the wide-string interface is 16-bit (unsigned short, as the PBG/PAG exports and every consumer), not the 4-byte wchar_t. FormatInt prints its long with %ld.
EDITS['mfutil/MfString.cpp'] = [
    ('\n'
     '// 0x1000cdc0 ?GetUnicode@MfPointerStorageString@@QAEPAGXZ (name invented; only reached by a tail jump)\n'
     'wchar_t *MfPointerStorageString::GetUnicode()\n'
     '{\n'
     '    delete[] m_unicode;\n'
     '    m_unicode = new wchar_t[m_length + 1];\n'
     '    mbstowcs(m_unicode, m_str, m_length);\n'
     '    m_unicode[m_length] = 0;\n',
     '\n'
     '// 0x1000cdc0 ?GetUnicode@MfPointerStorageString@@QAEPAGXZ (name invented; only reached by a tail jump)\n'
     'unsigned short *MfPointerStorageString::GetUnicode()   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '{\n'
     '    delete[] m_unicode;\n'
     '    m_unicode = new unsigned short[m_length + 1];   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '    mbstowcs(m_unicode, m_str, m_length);\n'
     '    m_unicode[m_length] = 0;\n', 1),
    ('\n'
     '// 0x1000cf10 ??0MfString@@QAE@PBG@Z\n'
     'MfString::MfString(const wchar_t *s)\n'
     '{\n'
     '    if (s) {\n',
     '\n'
     '// 0x1000cf10 ??0MfString@@QAE@PBG@Z\n'
     'MfString::MfString(const unsigned short *s)   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '{\n'
     '    if (s) {\n', 1),
    ('\n'
     '// 0x1000d400 ??4MfString@@QAEAAV0@PBG@Z\n'
     'MfString &MfString::operator=(const wchar_t *s)\n'
     '{\n'
     '    if (mfsr != &mfsrNullInitializer && mfsr->GetCount() <= 1) {\n',
     '\n'
     '// 0x1000d400 ??4MfString@@QAEAAV0@PBG@Z\n'
     'MfString &MfString::operator=(const unsigned short *s)   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '{\n'
     '    if (mfsr != &mfsrNullInitializer && mfsr->GetCount() <= 1) {\n', 1),
    ('\n'
     '// 0x1000d740 ??BMfString@@QBEPAGXZ (= ??BMfString@@QBEPBGXZ)\n'
     'MfString::operator wchar_t *() const\n'
     '{\n'
     '    return CreatePointerStorage()->GetUnicode();\n'
     '}\n'
     '\n'
     'MfString::operator const wchar_t *() const\n'
     '{\n'
     '    return CreatePointerStorage()->GetUnicode();\n',
     '\n'
     '// 0x1000d740 ??BMfString@@QBEPAGXZ (= ??BMfString@@QBEPBGXZ)\n'
     'MfString::operator unsigned short *() const   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '{\n'
     '    return CreatePointerStorage()->GetUnicode();\n'
     '}\n'
     '\n'
     'MfString::operator const unsigned short *() const   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '{\n'
     '    return CreatePointerStorage()->GetUnicode();\n', 1),
    ('\n'
     '// 0x1000d830 ??6MfString@@QAEAAV0@PBG@Z\n'
     'MfString &MfString::operator<<(const wchar_t *s)\n'
     '{\n'
     '    CopyOnWrite(wcslen(s));\n',
     '\n'
     '// 0x1000d830 ??6MfString@@QAEAAV0@PBG@Z\n'
     'MfString &MfString::operator<<(const unsigned short *s)   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '{\n'
     '    CopyOnWrite(wcslen(s));\n', 1),
    ('// 0x1000dc30 ?FormatInt@MfString@@QAEXJH@Z (original ?FormatInt@MfString@@QAEXJH@Z:) n with thousands separators, right\n'
     '// aligned in width characters\n'
     'void MfString::FormatInt(long n, int width)\n'
     '{\n'
     '    char groups[10][32];\n',
     '// 0x1000dc30 ?FormatInt@MfString@@QAEXJH@Z (original ?FormatInt@MfString@@QAEXJH@Z:) n with thousands separators, right\n'
     '// aligned in width characters\n'
     'void MfString::FormatInt(int n, int width)\n'
     '{\n'
     '    char groups[10][32];\n', 1),
    ('        divisor *= 10;\n'
     '\n'
     '    long v = n;\n'
     '    if (v < 0)\n'
     '        v = -v;\n',
     '        divisor *= 10;\n'
     '\n'
     '    int v = n;\n'
     '    if (v < 0)\n'
     '        v = -v;\n', 1),
    ('\n'
     '// 0x1000de50 ??8MfString@@QBEHPBG@Z\n'
     'int MfString::operator==(const wchar_t *s) const\n'
     '{\n'
     '    return mfsr->CompareToInternal(s) == 0;\n',
     '\n'
     '// 0x1000de50 ??8MfString@@QBEHPBG@Z\n'
     'int MfString::operator==(const unsigned short *s) const   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '{\n'
     '    return mfsr->CompareToInternal(s) == 0;\n', 1),
    ('\n'
     '// 0x1000df10 ??9MfString@@QBEHPBG@Z\n'
     'int MfString::operator!=(const wchar_t *s) const\n'
     '{\n'
     '    return mfsr->CompareToInternal(s) != 0;\n',
     '\n'
     '// 0x1000df10 ??9MfString@@QBEHPBG@Z\n'
     'int MfString::operator!=(const unsigned short *s) const   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '{\n'
     '    return mfsr->CompareToInternal(s) != 0;\n', 1),
    ('\n'
     '// 0x1000dfd0 ??MMfString@@QBEHPBG@Z\n'
     'int MfString::operator<(const wchar_t *s) const\n'
     '{\n'
     '    return mfsr->CompareToInternal(s) < 0;\n',
     '\n'
     '// 0x1000dfd0 ??MMfString@@QBEHPBG@Z\n'
     'int MfString::operator<(const unsigned short *s) const   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '{\n'
     '    return mfsr->CompareToInternal(s) < 0;\n', 1),
    ('\n'
     '// 0x1000e070 ??NMfString@@QBEHPBG@Z\n'
     'int MfString::operator<=(const wchar_t *s) const\n'
     '{\n'
     '    return mfsr->CompareToInternal(s) <= 0;\n',
     '\n'
     '// 0x1000e070 ??NMfString@@QBEHPBG@Z\n'
     'int MfString::operator<=(const unsigned short *s) const   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '{\n'
     '    return mfsr->CompareToInternal(s) <= 0;\n', 1),
    ('\n'
     '// 0x1000e110 ??OMfString@@QBEHPBG@Z\n'
     'int MfString::operator>(const wchar_t *s) const\n'
     '{\n'
     '    return mfsr->CompareToInternal(s) > 0;\n',
     '\n'
     '// 0x1000e110 ??OMfString@@QBEHPBG@Z\n'
     'int MfString::operator>(const unsigned short *s) const   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '{\n'
     '    return mfsr->CompareToInternal(s) > 0;\n', 1),
    ('\n'
     '// 0x1000e1b0 ??PMfString@@QBEHPBG@Z\n'
     'int MfString::operator>=(const wchar_t *s) const\n'
     '{\n'
     '    return mfsr->CompareToInternal(s) >= 0;\n',
     '\n'
     '// 0x1000e1b0 ??PMfString@@QBEHPBG@Z\n'
     'int MfString::operator>=(const unsigned short *s) const   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '{\n'
     '    return mfsr->CompareToInternal(s) >= 0;\n', 1),
    ('\n'
     '// 0x1000e1f0 ?Hash@MfString@@QBEKH@Z\n'
     'unsigned long MfString::Hash(int size) const\n'
     '{\n'
     '    unsigned long h = 0;\n'
     '    const char *p = mfsr->m_str;\n'
     '    for (int i = mfsr->m_length; i > 0; i--) {\n',
     '\n'
     '// 0x1000e1f0 ?Hash@MfString@@QBEKH@Z\n'
     'unsigned int MfString::Hash(int size) const\n'
     '{\n'
     '    unsigned int h = 0;\n'
     '    const char *p = mfsr->m_str;\n'
     '    for (int i = mfsr->m_length; i > 0; i--) {\n', 1),
]

# mfutil/MfString.h: the wide-string interface is 16-bit (unsigned short, as the PBG/PAG exports and every consumer), not the 4-byte wchar_t.
# (MfStringRef.h already spells it unsigned short.)
EDITS['mfutil/MfString.h'] = [
    ('    MfPointerStorageString(const MfStringRef &r);\n'
     '    virtual ~MfPointerStorageString();\n'
     '    wchar_t *GetUnicode();\n'
     '    char *GetAscii();\n'
     '\n'
     '    wchar_t *m_unicode; // +0x14\n'
     '    char *m_ascii;      // +0x18\n'
     '};\n',
     '    MfPointerStorageString(const MfStringRef &r);\n'
     '    virtual ~MfPointerStorageString();\n'
     '    unsigned short *GetUnicode();   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '    char *GetAscii();\n'
     '\n'
     '    unsigned short *m_unicode; // +0x14   PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '    char *m_ascii;      // +0x18\n'
     '};\n', 1),
    ('    MfString(const MfString &s);\n'
     '    MfString(const char *s);\n'
     '    MfString(const wchar_t *s);\n'
     '    MfString(const MfNullString &n);\n'
     '    MfString(int n);\n',
     '    MfString(const MfString &s);\n'
     '    MfString(const char *s);\n'
     '    MfString(const unsigned short *s);   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '    MfString(const MfNullString &n);\n'
     '    MfString(int n);\n', 1),
    ('    MfString &operator=(const MfString &s);\n'
     '    MfString &operator=(const char *s);\n'
     '    MfString &operator=(const wchar_t *s);\n'
     '    MfString &operator=(const MfNullString &n);\n'
     '    MfString &operator=(int n);\n',
     '    MfString &operator=(const MfString &s);\n'
     '    MfString &operator=(const char *s);\n'
     '    MfString &operator=(const unsigned short *s);   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '    MfString &operator=(const MfNullString &n);\n'
     '    MfString &operator=(int n);\n', 1),
    ('\n'
     '    void Compact() const;\n'
     '    operator wchar_t *() const;\n'
     '    operator const wchar_t *() const;\n'
     '    operator char *() const;\n'
     '    operator const char *() const;\n'
     '    operator int() const { return atoi(mfsr->m_str); }\n'
     '    operator long() const { return atol(mfsr->m_str); }\n'
     '    operator float() const;\n'
     '    operator double() const;\n',
     '\n'
     '    void Compact() const;\n'
     '    operator unsigned short *() const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '    operator const unsigned short *() const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '    operator char *() const;\n'
     '    operator const char *() const;\n'
     '    operator int() const { return atoi(mfsr->m_str); }\n'
     '    // port: the same as the int declaration above\n'
     '    operator float() const;\n'
     '    operator double() const;\n', 1),
    ('    MfString &operator+=(const wchar_t *s) { return *this << s; }\n',
     '    MfString &operator+=(const unsigned short *s) { return *this << s; }   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n', 1),
    ('    MfString &operator<<(const MfString &s);\n'
     '    MfString &operator<<(const char *s);\n'
     '    MfString &operator<<(const wchar_t *s);\n'
     '    MfString &operator<<(char c);\n'
     '    MfString &operator<<(unsigned short c);\n',
     '    MfString &operator<<(const MfString &s);\n'
     '    MfString &operator<<(const char *s);\n'
     '    MfString &operator<<(const unsigned short *s);   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '    MfString &operator<<(char c);\n'
     '    MfString &operator<<(unsigned short c);\n', 1),
    ('    int FormatTime(char *const fmt, const MfTime &t);\n'
     '    int FormatTime(char *const fmt, const MfSystemTime &t);\n'
     '    void FormatInt(long n, int width);\n'
     '\n'
     '    int operator==(const MfString &s) const;\n'
     '    int operator==(const wchar_t *s) const;\n'
     '    int operator==(const char *s) const;\n'
     '    int operator==(const MfNullString &n) const;\n'
     '    int operator!=(const MfString &s) const;\n'
     '    int operator!=(const wchar_t *s) const;\n'
     '    int operator!=(const char *s) const;\n'
     '    int operator!=(const MfNullString &n) const;\n'
     '    int operator<(const MfString &s) const;\n'
     '    int operator<(const wchar_t *s) const;\n'
     '    int operator<(const char *s) const;\n'
     '    int operator<=(const MfString &s) const;\n'
     '    int operator<=(const wchar_t *s) const;\n'
     '    int operator<=(const char *s) const;\n'
     '    int operator>(const MfString &s) const;\n'
     '    int operator>(const wchar_t *s) const;\n'
     '    int operator>(const char *s) const;\n'
     '    int operator>=(const MfString &s) const;\n'
     '    int operator>=(const wchar_t *s) const;\n'
     '    int operator>=(const char *s) const;\n'
     '    unsigned long Hash(int size) const;\n'
     '    int HaveCharactersInSet(const MfString &set, int start) const;\n'
     '    int ContainsSubString(const MfString &sub, int start) const;\n',
     '    int FormatTime(char *const fmt, const MfTime &t);\n'
     '    int FormatTime(char *const fmt, const MfSystemTime &t);\n'
     '    void FormatInt(int n, int width);\n'
     '\n'
     '    int operator==(const MfString &s) const;\n'
     '    int operator==(const unsigned short *s) const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '    int operator==(const char *s) const;\n'
     '    int operator==(const MfNullString &n) const;\n'
     '    int operator!=(const MfString &s) const;\n'
     '    int operator!=(const unsigned short *s) const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '    int operator!=(const char *s) const;\n'
     '    int operator!=(const MfNullString &n) const;\n'
     '    int operator<(const MfString &s) const;\n'
     '    int operator<(const unsigned short *s) const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '    int operator<(const char *s) const;\n'
     '    int operator<=(const MfString &s) const;\n'
     '    int operator<=(const unsigned short *s) const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '    int operator<=(const char *s) const;\n'
     '    int operator>(const MfString &s) const;\n'
     '    int operator>(const unsigned short *s) const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '    int operator>(const char *s) const;\n'
     '    int operator>=(const MfString &s) const;\n'
     '    int operator>=(const unsigned short *s) const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '    int operator>=(const char *s) const;\n'
     '    unsigned int Hash(int size) const;\n'
     '    int HaveCharactersInSet(const MfString &set, int start) const;\n'
     '    int ContainsSubString(const MfString &sub, int start) const;\n', 1),
    ('    void NoLeadingWhiteSpace() { CopyOnWrite(0); mfsr->NoLeadingWhiteSpace(); }\n'
     '    void NoTrailingWhiteSpace() { CopyOnWrite(0); mfsr->NoTrailingWhiteSpace(); }\n'
     '    wchar_t *DuplicateUnicode() const { return mfsr->DuplicateUnicode(); }\n'
     '    char *DuplicateAscii() const { return mfsr->DuplicateAscii(); }\n'
     '    int ReadFromFile(const MfString &name) { CopyOnWrite(0); return mfsr->ReadFromFile(name); }\n',
     '    void NoLeadingWhiteSpace() { CopyOnWrite(0); mfsr->NoLeadingWhiteSpace(); }\n'
     '    void NoTrailingWhiteSpace() { CopyOnWrite(0); mfsr->NoTrailingWhiteSpace(); }\n'
     '    unsigned short *DuplicateUnicode() const { return mfsr->DuplicateUnicode(); }   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)\n'
     '    char *DuplicateAscii() const { return mfsr->DuplicateAscii(); }\n'
     '    int ReadFromFile(const MfString &name) { CopyOnWrite(0); return mfsr->ReadFromFile(name); }\n', 1),
]

# mfutil/MfVersionResource.cpp: the VS_FIXEDFILEINFO signature scan steps in 32-bit DWORDs (unsigned long is 8 bytes here).
EDITS['mfutil/MfVersionResource.cpp'] = [
    ('// MfVersionResource (mfutil.dll 0x10011470-0x100116e0).\n'
     '#include <windows.h>\n'
     '#include <string.h>\n',
     '// MfVersionResource (mfutil.dll 0x10011470-0x100116e0).\n'
     '#include <stdint.h>\n'
     '#include <windows.h>\n'
     '#include <string.h>\n'
     '#line 4\n', 1),
    ('\n'
     '    // the children follow the VS_FIXEDFILEINFO value\n'
     '    unsigned long *pFixed = (unsigned long *)pData;\n'
     '    while (*pFixed != VS_FFI_SIGNATURE)\n'
     '        pFixed++;\n'
     '    unsigned char *pChild = (unsigned char *)pFixed + BLOCK_VALUELENGTH(pData);\n'
     '    while ((unsigned long)pChild & 3)\n'
     '        pChild++;\n'
     '\n',
     '\n'
     '    // the children follow the VS_FIXEDFILEINFO value\n'
     '    DWORD *pFixed = (DWORD *)pData;   // PORT: 32-bit words, as in the resource\n'
     '    while (*pFixed != VS_FFI_SIGNATURE)\n'
     '        pFixed++;\n'
     '    unsigned char *pChild = (unsigned char *)pFixed + BLOCK_VALUELENGTH(pData);\n'
     '    while ((uintptr_t)pChild & 3)\n'
     '        pChild++;\n'
     '\n', 1),
    ('            continue;\n'
     '        unsigned char *pTable = (unsigned char *)BLOCK_KEY(pChild) + (wcslen(BLOCK_KEY(pChild)) + 1) * 2;\n'
     '        while ((unsigned long)pTable & 3)\n'
     '            pTable++;\n'
     '        for (; pTable < BLOCK_END(pChild); pTable = BLOCK_END(pTable)) {\n',
     '            continue;\n'
     '        unsigned char *pTable = (unsigned char *)BLOCK_KEY(pChild) + (wcslen(BLOCK_KEY(pChild)) + 1) * 2;\n'
     '        while ((uintptr_t)pTable & 3)\n'
     '            pTable++;\n'
     '        for (; pTable < BLOCK_END(pChild); pTable = BLOCK_END(pTable)) {\n', 1),
    ('                _wcsicmp(L"040904e4", BLOCK_KEY(pTable)) == 0) {\n'
     '                unsigned char *pString = (unsigned char *)BLOCK_KEY(pTable) + (wcslen(BLOCK_KEY(pTable)) + 1) * 2;\n'
     '                while ((unsigned long)pString & 3)\n'
     '                    pString++;\n'
     '                pFirst = pString;\n',
     '                _wcsicmp(L"040904e4", BLOCK_KEY(pTable)) == 0) {\n'
     '                unsigned char *pString = (unsigned char *)BLOCK_KEY(pTable) + (wcslen(BLOCK_KEY(pTable)) + 1) * 2;\n'
     '                while ((uintptr_t)pString & 3)\n'
     '                    pString++;\n'
     '                pFirst = pString;\n', 1),
    ('\n'
     '// EQUIV 0x10011620 ?GetString@MfVersionResource@@QAEPAGPAGPAK@Z: reviewed: the value length is read into si instead of cx\n'
     'unsigned short *MfVersionResource::GetString(unsigned short *name, unsigned long *pLength)\n'
     '{\n'
     '    *pLength = 0;\n',
     '\n'
     '// EQUIV 0x10011620 ?GetString@MfVersionResource@@QAEPAGPAGPAK@Z: reviewed: the value length is read into si instead of cx\n'
     'unsigned short *MfVersionResource::GetString(unsigned short *name, unsigned int *pLength)\n'
     '{\n'
     '    *pLength = 0;\n', 1),
    ('                    return 0;\n'
     '                unsigned char *pValue = (unsigned char *)BLOCK_KEY(pString) + (wcslen(BLOCK_KEY(pString)) + 1) * 2;\n'
     '                while ((unsigned long)pValue & 3)\n'
     '                    pValue++;\n'
     '                return (unsigned short *)pValue;\n'
     '            }\n'
     '            unsigned char *pNext = (unsigned char *)BLOCK_KEY(pString) + (wcslen(BLOCK_KEY(pString)) + 1) * 2;\n'
     '            while ((unsigned long)pNext & 3)\n'
     '                pNext++;\n'
     '            if (BLOCK_VALUELENGTH(pString) > 0) {\n'
     '                pNext += BLOCK_VALUELENGTH(pString) * 2;\n'
     '                while ((unsigned long)pNext & 3)\n'
     '                    pNext++;\n'
     '            }\n',
     '                    return 0;\n'
     '                unsigned char *pValue = (unsigned char *)BLOCK_KEY(pString) + (wcslen(BLOCK_KEY(pString)) + 1) * 2;\n'
     '                while ((uintptr_t)pValue & 3)\n'
     '                    pValue++;\n'
     '                return (unsigned short *)pValue;\n'
     '            }\n'
     '            unsigned char *pNext = (unsigned char *)BLOCK_KEY(pString) + (wcslen(BLOCK_KEY(pString)) + 1) * 2;\n'
     '            while ((uintptr_t)pNext & 3)\n'
     '                pNext++;\n'
     '            if (BLOCK_VALUELENGTH(pString) > 0) {\n'
     '                pNext += BLOCK_VALUELENGTH(pString) * 2;\n'
     '                while ((uintptr_t)pNext & 3)\n'
     '                    pNext++;\n'
     '            }\n', 1),
]
