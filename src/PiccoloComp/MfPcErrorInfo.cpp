// PiccoloComp.dll MfPcErrorInfo.cpp (0x1000c210-0x1000c670)
#include "MfPcErrorInfo.h"
#include "MfPcTokenInfo.h"
#include "MfStringList.h"

static const char *const kIndent = "    ";
static const char *const kRule = "----";

// 0x10006d00 ?GetModule@MfPcErrorInfo@@UBEABVMfString@@XZ (inline)
// 0x10006d10 ?GetMessageA@MfPcErrorInfo@@UBEABVMfString@@XZ (inline)

// 0x1000c210 ?FormatLine@MfPcErrorInfo@@UBEXAAVMfStringList@@@Z
void MfPcErrorInfo::FormatLine(MfStringList &lines) const
{
    MfString text(MfStringSize(0x200));
    lines.Clear();
    MfString line(mLine);
    int start = line.CountCharacters(mToken->mStartColumn, 4);
    int end = line.CountCharacters(mToken->mEndColumn, 4);
    line.ExpandTabs(4);
    int width = end - start + 1;
    if (width <= 0)
        width = 1;
    if (line.Length() > 0) {
        text = MfString(kIndent) + line;
        lines.Add(text);
        text = kIndent;
        int i;
        for (i = 0; i < end - width; i++)
            text << ' ';
        for (i = 0; i < width; i++)
            text << '^';
        lines.Add(text);
        text = kIndent;
        int middle = end - width / 2 - 1;
        for (i = 0; i < middle; i++)
            text << ' ';
        text << '|';
        lines.Add(text);
        text = kRule;
        for (i = 0; i < middle; i++)
            text << '-';
        text << '+';
        lines.Add(text);
    }
}

// 0x1000c430 ?ParseModuleName@MfPcErrorInfo@@SA?AVMfString@@ABV2@@Z
MfString MfPcErrorInfo::ParseModuleName(MfString const &info)
{
    MfString module;
    int found = info.ContainsSubString("****", 0);
    if (found == 0) {
        int end = info.HaveCharactersInSet("(", 0);
        if (end > 5)
            module = info.MidString(5, end - 5);
    }
    return module;
}

// 0x1000c540 ?ParseLineNumber@MfPcErrorInfo@@SAHABVMfString@@@Z
int MfPcErrorInfo::ParseLineNumber(MfString const &info)
{
    int line = -1;
    int found = info.ContainsSubString("****", 0);
    if (found == 0) {
        int start = info.ContainsSubString("Line", 0);
        if (start > 0) {
            int end = info.HaveCharactersInSet(")", start);
            if (end > 0) {
                start += 4;
                MfString number = info.MidString(start, end - start);
                line = number;
            }
        }
    }
    return line;
}
