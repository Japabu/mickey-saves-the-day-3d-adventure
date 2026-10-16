// PORT: stack words are uintptr_t (pointer-sized): the 32-bit source used unsigned long, which was the size of a pointer there.
// PiccoloRT.dll 0x100089a0-0x100094df: MfScConstUsageInfo and MfScDebugInfo.
#include "MfScDebugInfo.h"
#include "MfScConstUsageInfo.h"
#include "MfScLineInfo.h"
#include "MfScCodeLevelInfo.h"
#include "MfScVariableDebugInfo.h"
#include "MfScDataHandle.h"

// 0x100089a0 ??0MfScConstUsageInfo@@QAE@XZ
MfScConstUsageInfo::MfScConstUsageInfo()
{
}

// 0x100089f0 ??0MfScConstUsageInfo@@QAE@ABVMfString@@ABVMfVariant@@@Z (inline, header)

// 0x10008a50 ??0MfScConstUsageInfo@@QAE@ABV0@@Z
MfScConstUsageInfo::MfScConstUsageInfo(MfScConstUsageInfo const &other)
    : mName(other.mName), mValue(other.mValue)
{
}

// 0x10008ab0 ??1MfScConstUsageInfo@@UAE@XZ (inline, header)

// 0x10008b00 ??4MfScConstUsageInfo@@QAEABV0@ABV0@@Z (inline, header)
// 0x10008b30 ?Count@MfScLinkData@@QAEAAHXZ (inline, header; folded)
// 0x10008b40 ?ModuleType@MfScFunctionRegistryModule@@QBEABVMfString@@XZ (inline, header; folded)
// 0x10008b50 ??_EMfScConstUsageInfo@@UAEPAXI@Z (compiler-generated)

// 0x10008ba0 ??0MfScDebugInfo@@QAE@XZ
MfScDebugInfo::MfScDebugInfo()
    : mCodeSize(0), mLineInfo(0), mNumLineInfo(0), mVarDebugInfo(0), mNumVarDebugInfo(0),
      mCodeLevelInfo(0), mNumCodeLevelInfo(0), mConstUsageInfo(0), mNumConstUsageInfo(0)
{
}

// 0x10008bd0 ??1MfScDebugInfo@@UAE@XZ
MfScDebugInfo::~MfScDebugInfo()
{
    delete[] mLineInfo;
    delete[] mVarDebugInfo;
    delete[] mCodeLevelInfo;
    delete[] mConstUsageInfo;
}

// 0x10008c60 ?InitializeLineInfo@MfScDebugInfo@@QAEHH@Z
int MfScDebugInfo::InitializeLineInfo(int count)
{
    mNumLineInfo = count;
    mLineInfo = new MfScLineInfo[count];
    return 1;
}

// 0x10008ce0 ?DeleteLineInfo@MfScDebugInfo@@QAEXXZ
void MfScDebugInfo::DeleteLineInfo()
{
    delete[] mLineInfo;
    mLineInfo = 0;
}

// 0x10008d20 ?GetLineInfo@MfScDebugInfo@@QBEABVMfScLineInfo@@H@Z
MfScLineInfo const & MfScDebugInfo::GetLineInfo(int i) const
{
    return mLineInfo[i];
}

// 0x10008d30 ?SetLineInfo@MfScDebugInfo@@QAEXABVMfScLineInfo@@H@Z
void MfScDebugInfo::SetLineInfo(MfScLineInfo const &info, int i)
{
    mLineInfo[i] = info;
}

// 0x10008d50 ?FindLineIndex@MfScDebugInfo@@AAEHH@Z
int MfScDebugInfo::FindLineIndex(int offset)
{
    if (!mNumLineInfo || !mLineInfo)
        return -1;
    int start = mLineInfo[0].mOffset;
    for (int i = 1; i < mNumLineInfo; i++) {
        if (offset >= start && offset < mLineInfo[i].mOffset)
            return i - 1;
        start = mLineInfo[i].mOffset;
    }
    return mNumLineInfo;
}

// 0x10008da0 ?FindLine@MfScDebugInfo@@QAEHH@Z
int MfScDebugInfo::FindLine(int offset)
{
    int i = FindLineIndex(offset);
    if (i < 0)
        return -1;
    if (i >= mNumLineInfo)
        return mLineInfo[mNumLineInfo - 1].mLine;
    return mLineInfo[i].mLine;
}

// 0x10008de0 ?FindOffset@MfScDebugInfo@@QAEHH@Z
int MfScDebugInfo::FindOffset(int line)
{
    if (!mNumLineInfo || !mLineInfo)
        return -1;
    int i;
    for (i = 0; i < mNumLineInfo; i++) {
        if (line <= mLineInfo[i].mLine)
            return mLineInfo[i].mOffset;
    }
    return mLineInfo[i - 1].mOffset;
}

// 0x10008e30 ?FindOffsetOfFirstLine@MfScDebugInfo@@QAEHXZ
int MfScDebugInfo::FindOffsetOfFirstLine()
{
    if (mNumLineInfo && mLineInfo)
        return mLineInfo[0].mOffset;
    return -1;
}

// 0x10008e50 ?FindOffsetOfNextLine@MfScDebugInfo@@QAEHHPAH@Z
int MfScDebugInfo::FindOffsetOfNextLine(int line, int *nextLine)
{
    *nextLine = line;
    if (!mNumLineInfo || !mLineInfo)
        return -1;
    for (int i = 0; i < mNumLineInfo; i++) {
        if (line == mLineInfo[i].mLine) {
            if (i + 1 < mNumLineInfo) {
                *nextLine = mLineInfo[i + 1].mLine;
                return mLineInfo[i + 1].mOffset;
            }
            *nextLine = -1;
            return -1;
        }
    }
    return -1;
}

// 0x10008ec0 ?FindOffsetOfNextLine@MfScDebugInfo@@QAEHH@Z
int MfScDebugInfo::FindOffsetOfNextLine(int offset)
{
    int i = FindLineIndex(offset);
    if (i >= 0 && i < mNumLineInfo - 1)
        return mLineInfo[i + 1].mOffset;
    return -1;
}

// 0x10008ef0 ?ComputeOffsetRange@MfScDebugInfo@@QAEHHPAH0@Z
int MfScDebugInfo::ComputeOffsetRange(int offset, int *start, int *end)
{
    int i = FindLineIndex(offset);
    if (i < 0)
        return 0;
    if (i >= mNumLineInfo - 1) {
        *start = mLineInfo[mNumLineInfo].mOffset;
        *end = mCodeSize - 1;
        return 1;
    }
    *start = mLineInfo[i].mOffset;
    *end = mLineInfo[i + 1].mOffset;
    return 1;
}

// 0x10008f60 ?GetRange@MfScDebugInfo@@QAEXAAH0@Z
void MfScDebugInfo::GetRange(int &first, int &last)
{
    first = -1;
    last = -1;
    if (mNumLineInfo && mLineInfo) {
        first = mLineInfo[0].mLine;
        last = mLineInfo[mNumLineInfo - 1].mLine;
    }
}

// 0x10008fa0 ?InitializeVarDebugInfo@MfScDebugInfo@@QAEHH@Z
int MfScDebugInfo::InitializeVarDebugInfo(int count)
{
    mNumVarDebugInfo = count;
    mVarDebugInfo = new MfScVariableDebugInfo[count];
    return 1;
}

// 0x10009020 ??0MfScVariableDebugInfo@@QAE@XZ (inline, header)

// 0x10009040 ?DeleteVarDebugInfo@MfScDebugInfo@@QAEXXZ
void MfScDebugInfo::DeleteVarDebugInfo()
{
    delete[] mVarDebugInfo;
    mVarDebugInfo = 0;
}

// 0x10009080 ?GetVarDebugInfo@MfScDebugInfo@@QBEABVMfScVariableDebugInfo@@H@Z
MfScVariableDebugInfo const & MfScDebugInfo::GetVarDebugInfo(int i) const
{
    return mVarDebugInfo[i];
}

// 0x10009090 ?SetVarDebugInfo@MfScDebugInfo@@QAEXABVMfScVariableDebugInfo@@H@Z
void MfScDebugInfo::SetVarDebugInfo(MfScVariableDebugInfo const &info, int i)
{
    mVarDebugInfo[i] = info;
}

// 0x100090c0 ?InitializeCodeLevelInfo@MfScDebugInfo@@QAEHH@Z
int MfScDebugInfo::InitializeCodeLevelInfo(int count)
{
    mNumCodeLevelInfo = count;
    mCodeLevelInfo = new MfScCodeLevelInfo[count];
    return 1;
}

// 0x10009140 ??0MfScCodeLevelInfo@@QAE@XZ

// 0x10009150 ?DeleteCodeLevelInfo@MfScDebugInfo@@QAEXXZ
void MfScDebugInfo::DeleteCodeLevelInfo()
{
    delete[] mCodeLevelInfo;
    mCodeLevelInfo = 0;
}

// 0x10009190 ?GetCodeLevelInfo@MfScDebugInfo@@QBEABVMfScCodeLevelInfo@@H@Z
MfScCodeLevelInfo const & MfScDebugInfo::GetCodeLevelInfo(int i) const
{
    return mCodeLevelInfo[i];
}

// 0x100091a0 ?SetCodeLevelInfo@MfScDebugInfo@@QAEXABVMfScCodeLevelInfo@@H@Z
void MfScDebugInfo::SetCodeLevelInfo(MfScCodeLevelInfo const &info, int i)
{
    mCodeLevelInfo[i] = info;
}

// 0x100091d0 ??4MfScCodeLevelInfo@@QAE?AV0@ABV0@@Z (inline, header)

// 0x10009200 ?FindLevel@MfScDebugInfo@@QAEHH@Z
int MfScDebugInfo::FindLevel(int offset)
{
    if (!mNumCodeLevelInfo || !mCodeLevelInfo)
        return 0;
    int start = 0;
    int i;
    for (i = 0; i < mNumCodeLevelInfo; i++) {
        if (start <= offset && offset < mCodeLevelInfo[i].mOffset)
            break;
        start = mCodeLevelInfo[i].mOffset;
    }
    return mCodeLevelInfo[i - 1].mLevel;
}

// 0x10009250 ?GetVariable@MfScDebugInfo@@QAEHABVMfString@@PAKHPAPAVMfScDataHandle@@@Z
int MfScDebugInfo::GetVariable(MfString const &name, uintptr_t *frame, /* PORT: uintptr_t */ int level, MfScDataHandle **value)
{
    int found = 0;
    *value = 0;
    if (mNumVarDebugInfo > 0 && mVarDebugInfo) {
        for (int i = 0; i < mNumVarDebugInfo; i++) {
            if (mVarDebugInfo[i].mName == name && level >= mVarDebugInfo[i].mLevel) {
                *value = (MfScDataHandle *)frame[mVarDebugInfo[i].mSlot];
                if (*value)
                    (*value)->AddRef();
                found = 1;
                break;
            }
        }
    }
    if (!found && mNumConstUsageInfo && mConstUsageInfo) {
        for (int i = 0; i < mNumConstUsageInfo; i++) {
            if (mConstUsageInfo[i].mName == name) {
                *value = MfScDataHandle::CreateData(mConstUsageInfo[i].mValue, 0);
                found = 1;
                break;
            }
        }
    }
    return found;
}

// EQUIV 0x10009350 ?MemoryUsage@MfScDebugInfo@@QAEIXZ: reviewed: the compiled function is 10009350 plus the 85-byte block 100093ab (switch cases + jump table) that the tool splits off as fn_100093ab; the same instructions and table
unsigned int MfScDebugInfo::MemoryUsage()
{
    unsigned int size = mNumLineInfo * sizeof(MfScLineInfo);
    int i;
    for (i = 0; i < mNumVarDebugInfo; i++)
        size += mVarDebugInfo[i].mName.Length() + 1 + 2 * sizeof(int);
    size += mNumCodeLevelInfo * sizeof(MfScCodeLevelInfo);
    for (i = 0; i < mNumConstUsageInfo; i++) {
        size += mConstUsageInfo[i].mName.Length() + 1;
        switch (mConstUsageInfo[i].mValue.Type()) {
        case MfVariant::kInt:
        case MfVariant::kLong:
            size += 12;
            break;
        case MfVariant::kFloat:
        case MfVariant::kDouble:
            size += 12;
            break;
        case MfVariant::kString: {
            MfString s = mConstUsageInfo[i].mValue.ToString();
            size += s.Length() + 1 + 2 * sizeof(int);
            break;
        }
        }
    }
    return size;
}

// 0x10009400 ?InitializeConstUsageInfo@MfScDebugInfo@@QAEHH@Z
int MfScDebugInfo::InitializeConstUsageInfo(int count)
{
    mNumConstUsageInfo = count;
    mConstUsageInfo = new MfScConstUsageInfo[count];
    return 1;
}

// 0x10009480 ?DeleteConstUsageInfo@MfScDebugInfo@@QAEXXZ
void MfScDebugInfo::DeleteConstUsageInfo()
{
    delete[] mConstUsageInfo;
    mConstUsageInfo = 0;
}

// 0x100094a0 ?GetConstUsageInfo@MfScDebugInfo@@QBEABVMfScConstUsageInfo@@H@Z
MfScConstUsageInfo const & MfScDebugInfo::GetConstUsageInfo(int i) const
{
    return mConstUsageInfo[i];
}

// EQUIV 0x100094b0 ?SetConstUsageInfo@MfScDebugInfo@@QAEXABVMfScConstUsageInfo@@H@Z: reviewed: the index is loaded before mConstUsageInfo in the original; register choice otherwise
void MfScDebugInfo::SetConstUsageInfo(MfScConstUsageInfo const &info, int i)
{
    mConstUsageInfo[i] = info;
}


// Matched by this code:
// 0x10008630 ?GetCodeSize@MfScDebugInfo@@QBEHXZ