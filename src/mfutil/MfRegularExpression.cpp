// MfRegularExpression (mfutil.dll 0x1000b100-0x1000b6b0)
// The first functions are inline/implicit members of the exported class that this
// object emitted (no source of their own here; they come from MfRegularExpression.h):
#include "MfRegularExpression_unit.h"

// 0x1000b100 ?GetState@MfThread@@QBEIXZ             (inline)
// 0x1000b110 ?CompileErrorString@MfRegularExpression@@QBEABVMfString@@XZ (inline)
// 0x1000b120 ?HaveMatch@MfRegularExpression@@QBEHXZ         (inline)
// 0x1000b130 ?MatchedStart@MfRegularExpression@@QBEHXZ      (inline)
// 0x1000b140 ?MatchedEnd@MfRegularExpression@@QBEHXZ        (inline)
// 0x1000b150 ?RegisterStart@MfRegularExpression@@QBEHH@Z    (inline)
// 0x1000b160 ?RegisterEnd@MfRegularExpression@@QBEHH@Z      (inline)
// 0x1000b170 ?GetCount@MfStringRef@@QAEHXZ     (inline)
// 0x1000b180 ??0MfRegularExpression@@QAE@ABV0@@Z            (implicit)
// 0x1000b220 ??4MfRegularExpression@@QAEAAV0@ABV0@@Z        (implicit)
// 0x1000b290 ??_EMfRegularExpression@@UAEPAXI@Z             (vector deleting dtor)

// 0x1000b2f0 ??0MfRegularExpression@@QAE@ABVMfString@@@Z
MfRegularExpression::MfRegularExpression(const MfString &pattern)
    : m_pattern(pattern), m_registerCount(0), m_valid(0), m_fastmap(0)
{
    Initialize();
}

// 0x1000b360 ??1MfRegularExpression@@UAE@XZ
MfRegularExpression::~MfRegularExpression()
{
    if (m_fastmap)
        free(m_fastmap);
    if (m_buf.buffer)
        free(m_buf.buffer);
}

// 0x1000b3e0 ?Initialize@MfRegularExpression@@AAEXXZ
void MfRegularExpression::Initialize()
{
    memset(&m_buf, 0, sizeof(m_buf));
    MfString open("(");
    int i = m_pattern.HaveCharactersInSet(open, 0);
    while (i != -1) {
        m_registerCount++;
        i = m_pattern.HaveCharactersInSet(open, i + 1);
    }
    re_set_syntax(RE_NO_BK_PARENS | RE_NO_BK_VBAR | RE_NEWLINE_OR | RE_CONTEXT_INDEP_OPS |
                  RE_ANSI_HEX | RE_NO_GNU_EXTENSIONS | RE_IGNORE_CASE);
    if ((m_error = re_compile_pattern((char *)m_pattern, m_pattern.Length(), &m_buf)) == NULL_STRING)
        m_valid = 1;
    else
        m_registerCount = 0;
}

// 0x1000b4c0 ?Optimize@MfRegularExpression@@QAEXXZ
void MfRegularExpression::Optimize()
{
    if (m_buf.fastmap == 0) {
        m_buf.fastmap = m_fastmap = (char *)malloc(256);
        re_compile_fastmap(&m_buf);
    }
}

// 0x1000b4f0 ?Matches@MfRegularExpression@@QAEHABVMfString@@@Z
int MfRegularExpression::Matches(const MfString &s)
{
    m_match = re_search(&m_buf, (char *)s, s.Length(), 0, s.Length(), &m_regs);
    return m_match >= 0;
}

// 0x1000b530 ?DosToRegEx@MfRegularExpression@@SA?AVMfString@@ABV2@@Z
MfString MfRegularExpression::DosToRegEx(const MfString &dos)
{
    MfString special("*?.");
    MfString result(MfStringSize(dos.Length() * 2));
    result << '^';
    int start = 0;
    int i;
    while ((i = dos.HaveCharactersInSet(special, start)) >= 0) {
        result << dos.MidString(start, i - start);
        char c = dos.GetCharacter(i);
        if (c == '*')
            result << ".*";
        else if (c == '.')
            result << "\\.";
        else if (c == '?')
            result << ".";
        start = i + 1;
    }
    result << dos.RightString(dos.Length() - start) << "$";
    return result;
}
