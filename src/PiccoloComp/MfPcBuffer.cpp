// PiccoloComp.dll MfPcBuffer.cpp (0x10002cf0-0x10002e90): the source text of a
// compilation, split into lines.
#include <string.h>
#include "MfPcBuffer.h"
#include "MfString.h"

// 0x10002cf0 ??0MfPcBuffer@@QAE@XZ
MfPcBuffer::MfPcBuffer()
    : mLineCount(0), mLength(0)
{
    mLines = new char *[0x400];
    mText = new char[0x8000];
}

// 0x10002d30 ??_GMfPcBuffer@@UAEPAXI@Z (compiler-generated)

// 0x10002d50 ??1MfPcBuffer@@UAE@XZ
MfPcBuffer::~MfPcBuffer()
{
    delete mLines;
    delete mText;
}

// 0x10002d80 ?SetText@MfPcBuffer@@QAEHAAVMfString@@@Z
int MfPcBuffer::SetText(MfString &text)
{
    mLength = text.Length();
    if (mLength > 0x7fff)
        return 0;
    strcpy(mText, text);
    if (mLength > 0 && mText[mLength - 1] != '\n') {
        mText[++mLength - 1] = '\n';
        mText[mLength] = 0;
    }
    mLineCount = 1;
    mLines[0] = mText;
    return 1;
}

// 0x10002e00 ?GetLine@MfPcBuffer@@QAEHHAAVMfString@@@Z
int MfPcBuffer::GetLine(int line, MfString &out)
{
    if (mLineCount && (line <= 0 || line > mLineCount))
        return 0;
    char *start = mLines[line - 1];
    char *p = start;
    while (*p != '\n' && *p)
        p++;
    int c = *p;
    *p = 0;
    out = start;
    *p = c;
    return 1;
}

// 0x10002e60 ?AddLine@MfPcBuffer@@QAEHH@Z
int MfPcBuffer::AddLine(int offset)
{
    if (++mLineCount > 0x400)
        return 0;
    mLines[mLineCount - 1] = mText + offset;
    return 1;
}
