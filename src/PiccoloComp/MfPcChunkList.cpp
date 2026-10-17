// PiccoloComp.dll MfPcChunkList.cpp (0x10003680-0x10004200): the chunk list
// of a function: listing, layout of the code and its debug records.
#include "MfPcChunkList.h"
#include "MfPcCodeChunk.h"
#include "MfScBytecodeRoutine.h"
#include "MfScCodeLevelInfo.h"
#include "MfScConstUsageInfo.h"
#include "../PiccoloRT/MfScDebugInfo.h"
#include "MfScDiagnostics.h"
#include "../PiccoloRT/MfScLineInfo.h"
#include "MfScVariableDebugInfo.h"
#include "MfDictionary.h"
#include "MfPcIdentifier.h"

// 0x10003680 ?Dump@MfPcChunkList@@QAEXXZ
void MfPcChunkList::Dump()
{

    if (MfScDiagnostics::Active(4)) {
        unsigned char code[12];
        unsigned char *p = code;
        MfString hex;
        MfString line;
        MfScDiagnostics::PrintNl(4, MfString("+++++++++++++ Code Chunk List"));
        MfDblListItem *pli = 0;
        for (MfPcCodeChunk *chunk = (MfPcCodeChunk *)First(pli); chunk;
             chunk = (MfPcCodeChunk *)Next(pli)) {
            p = code;
            chunk->CopyCode(p);
            hex.Format("%08X  ", chunk->mOffset);
            line = hex;
            if (chunk->mType != PC_CHUNK_LABEL) {
                int size = chunk->mSize;
                for (int i = 0; i < size; i++) {
                    hex.Format("%02X", code[i]);
                    line << hex;
                }
                if (size < 3)
                    line << '\t';
            } else {
                line << "\t";
            }
            line << *chunk->mText;
            MfScDiagnostics::PrintNl(4, line);
        }
        MfScDiagnostics::PrintNl(4, MfString("------------- Code Chunk List"));
    }
}

// EQUIV 0x10003870 ?CodeSize@MfPcChunkList@@QAEHXZ: reviewed: the two summed members are loaded in the other order
int MfPcChunkList::CodeSize()
{
    MfDblListItem *pli;
    MfPcCodeChunk *first = (MfPcCodeChunk *)First(pli);
    MfPcCodeChunk *last = (MfPcCodeChunk *)Last(pli);
    return last->mSize + last->mOffset - first->mOffset;
}

// 0x100038b0 ?CopyCode@MfPcChunkList@@QAEXPAE@Z
void MfPcChunkList::CopyCode(unsigned char *p)
{
    MfDblListItem *pli;
    for (MfPcCodeChunk *chunk = (MfPcCodeChunk *)First(pli); chunk;
         chunk = (MfPcCodeChunk *)Next(pli))
        chunk->CopyCode(p);
}

// 0x100038f0 ?AssignOffsets@MfPcChunkList@@QAEXXZ
void MfPcChunkList::AssignOffsets()
{
    int offset = 0;
    MfDblListItem *pli;
    for (MfPcCodeChunk *chunk = (MfPcCodeChunk *)First(pli); chunk;
         chunk = (MfPcCodeChunk *)Next(pli)) {
        chunk->mOffset = offset;
        offset += chunk->mSize;
    }
}

// 0x10003930 ?SetJumpSizes@MfPcChunkList@@QAEXHH@Z
void MfPcChunkList::SetJumpSizes(int size, int all)
{
    if (size == 1 || size == 2 || size == 4) {
        MfDblListItem *pli;
        for (MfPcCodeChunk *chunk = (MfPcCodeChunk *)First(pli); chunk;
             chunk = (MfPcCodeChunk *)Next(pli)) {
            if (chunk->mType == PC_CHUNK_JUMP && (all == 1 || chunk->mSize == 0))
                chunk->mSize = size + 1;
        }
    }
}

// 0x10003990 ?ComputeDisplacements@MfPcChunkList@@QAEXXZ
void MfPcChunkList::ComputeDisplacements()
{
    MfDblListItem *pli;
    for (MfPcCodeChunk *chunk = (MfPcCodeChunk *)First(pli); chunk;
         chunk = (MfPcCodeChunk *)Next(pli)) {
        if (chunk->mType == PC_CHUNK_JUMP) {
            MfPcChunkJump *jump = (MfPcChunkJump *)chunk;
            jump->mDisplacement = jump->mTarget->mOffset - jump->mOffset;
        }
    }
}

// 0x100039e0 ?AssembleJumps@MfPcChunkList@@QAEXXZ
void MfPcChunkList::AssembleJumps()
{
    MfDblListItem *pli;
    for (MfPcCodeChunk *chunk = (MfPcCodeChunk *)First(pli); chunk;
         chunk = (MfPcCodeChunk *)Next(pli)) {
        if (chunk->mType == PC_CHUNK_JUMP)
            ((MfPcChunkJump *)chunk)->Assemble();
    }
}

// 0x10003a20 ?ShrinkJumps@MfPcChunkList@@QAEHH@Z
int MfPcChunkList::ShrinkJumps(int)
{
    int changed = 0;
    MfDblListItem *pli;
    for (MfPcCodeChunk *chunk = (MfPcCodeChunk *)First(pli); chunk;
         chunk = (MfPcCodeChunk *)Next(pli)) {
        if (chunk->mType == PC_CHUNK_JUMP) {
            int size = ((MfPcChunkJump *)chunk)->DisplacementSize() + 1;
            if (size < chunk->mSize) {
                chunk->mSize = size;
                changed = 1;
            }
        }
    }
    return changed;
}

// TODO 0x10003a80 ?Walk@MfPcChunkList@@QAEXXZ: (same results, but the branch layout differs) one branch goes straight to the shared target instead of through a second test of the same flag
void MfPcChunkList::Walk()
{
    MfDblListItem *pli;
    for (MfPcCodeChunk *chunk = (MfPcCodeChunk *)First(pli); chunk;
         chunk = (MfPcCodeChunk *)Next(pli))
        ;
}

// EQUIV 0x10003ab0 ?BuildLineInfo@MfPcChunkList@@QAEXPAVMfScBytecodeRoutine@@@Z: reviewed: (93% similar) the counting loop calls the imported GetNext through its IAT slot in the original; ours loads the address into esi once before the loop; same calls, arguments and flow

void MfPcChunkList::BuildLineInfo(MfScBytecodeRoutine *routine)
{
    int base = 0;
    MfDblListItem *pli;
    if (GetCount() > 0)
        base = ((MfPcCodeChunk *)MfDblList::GetFirst(pli))->mOffset;
    int count = 0;
    MfPcCodeChunk *chunk;
    for (chunk = (MfPcCodeChunk *)First(pli); chunk;
         chunk = (MfPcCodeChunk *)MfDblList::GetNext(pli)) {
        if (chunk->mType == PC_CHUNK_LINEINFO)
            count++;
    }
    if (count) {
        MfScDebugInfo *info = routine->DebugInfo();
        if (!info) {
            info = new MfScDebugInfo;
            routine->Attach(info);
        }
        info->InitializeLineInfo(count);
        int i = 0;
        for (chunk = (MfPcCodeChunk *)First(pli); chunk;
             chunk = (MfPcCodeChunk *)Next(pli)) {
            if (chunk->mType == PC_CHUNK_LINEINFO) {
                MfScLineInfo line(chunk->mOffset - base, ((MfPcChunkLineInfo *)chunk)->mLine);
                info->SetLineInfo(line, i);
                i++;
            }
        }
    }
}

// 0x10003c00 ??_GMfScDebugInfo@@UAEPAXI@Z (new MfScDebugInfo)

// 0x10003c20 ?BuildVariableInfo@MfPcChunkList@@QAEXPAVMfScBytecodeRoutine@@@Z
void MfPcChunkList::BuildVariableInfo(MfScBytecodeRoutine *routine)
{
    int count = 0;
    MfDblListItem *pli;
    MfPcCodeChunk *chunk;
    for (chunk = (MfPcCodeChunk *)First(pli); chunk; chunk = (MfPcCodeChunk *)Next(pli)) {
        if (chunk->mType == PC_CHUNK_VARIABLEINFO)
            count++;
    }
    if (count) {
        MfScDebugInfo *info = routine->DebugInfo();
        if (!info) {
            info = new MfScDebugInfo;
            routine->Attach(info);
        }
        info->InitializeVarDebugInfo(count);
        int i = 0;
        for (chunk = (MfPcCodeChunk *)First(pli); chunk; chunk = (MfPcCodeChunk *)Next(pli)) {
            if (chunk->mType == PC_CHUNK_VARIABLEINFO) {
                MfPcChunkVariableInfo *var = (MfPcChunkVariableInfo *)chunk;
                MfScVariableDebugInfo vdi(var->mName, var->mSlot, var->mLevel);
                info->SetVarDebugInfo(vdi, i);
                i++;
            }
        }
    }
}

// 0x10003d60 ?BuildCodeLevelInfo@MfPcChunkList@@QAEXPAVMfScBytecodeRoutine@@@Z
void MfPcChunkList::BuildCodeLevelInfo(MfScBytecodeRoutine *routine)
{
    MfDblListItem *pli;
    int base = ((MfPcCodeChunk *)First(pli))->mOffset;
    int count = 0;
    MfPcCodeChunk *chunk;
    for (chunk = (MfPcCodeChunk *)First(pli); chunk; chunk = (MfPcCodeChunk *)Next(pli)) {
        if (chunk->mType == PC_CHUNK_CODELEVEL)
            count++;
    }
    if (count) {
        MfScDebugInfo *info = routine->DebugInfo();
        if (!info) {
            info = new MfScDebugInfo;
            routine->Attach(info);
        }
        info->InitializeCodeLevelInfo(count);
        int i = 0;
        for (chunk = (MfPcCodeChunk *)First(pli); chunk; chunk = (MfPcCodeChunk *)Next(pli)) {
            if (chunk->mType == PC_CHUNK_CODELEVEL) {
                MfScCodeLevelInfo level(chunk->mOffset - base, ((MfPcChunkCodeLevel *)chunk)->mLevel);
                info->SetCodeLevelInfo(level, i);
                i++;
            }
        }
    }
}

// EQUIV 0x10003eb0 ?BuildConstUsageInfo@MfPcChunkList@@QAEXPAVMfScBytecodeRoutine@@@Z: reviewed: (85% similar) register choice only: the original keeps 0 and then the constant 7 in edi and calls GetNext through its IAT slot, ours compares with an immediate 7 and keeps the GetNext address in ebp (and Add's in edi)
void MfPcChunkList::BuildConstUsageInfo(MfScBytecodeRoutine *routine)
{
    MfDictionary seen(0x35);
    MfDblList constants;
    MfDblList &list = constants;
    MfDblListItem *pli;
    MfPcCodeChunk *chunk;
    for (chunk = (MfPcCodeChunk *)MfDblList::GetFirst(pli); chunk;
         chunk = (MfPcCodeChunk *)MfDblList::GetNext(pli)) {
        if (chunk->mType == PC_CHUNK_CONSTUSAGE) {
            MfPcChunkConstUsage *usage = (MfPcChunkConstUsage *)chunk;
            if (!seen.Add(usage->mConstant->mName, usage->mConstant))
                list.Append(usage->mConstant);
        }
    }
    if (constants.GetCount()) {
        MfScDebugInfo *info = routine->DebugInfo();
        if (!info) {
            info = new MfScDebugInfo;
            routine->Attach(info);
        }
        info->InitializeConstUsageInfo(constants.GetCount());
        int i = 0;
        for (MfPcIdentifier *constant = (MfPcIdentifier *)constants.First(pli); constant;
             constant = (MfPcIdentifier *)constants.Next(pli)) {
            MfScConstUsageInfo usage(constant->mName, *constant->mValue);
            info->SetConstUsageInfo(usage, i);
            i++;
        }
    }
}

// 0x10004090 ?ListParameters@MfPcChunkList@@QAEXPAVMfScBytecodeRoutine@@@Z
void MfPcChunkList::ListParameters(MfScBytecodeRoutine *routine)
{
    MfDblListItem *pli;
    MfPcCodeChunk *chunk = (MfPcCodeChunk *)First(pli);
    for (int n = 0; chunk; chunk = (MfPcCodeChunk *)Next(pli)) {
        if (chunk->mType == PC_CHUNK_VARIABLEINFO && ((MfPcChunkVariableInfo *)chunk)->mLevel == 0) {
            if (n > 0)
                routine->FunctionParams() << ",";
            n++;
            routine->FunctionParams() << ((MfPcChunkVariableInfo *)chunk)->mName;
        }
    }
}

// 0x10004100 ?AddLineInfo@MfPcChunkList@@QAEXXZ
void MfPcChunkList::AddLineInfo()
{
    MfDblListItem *pli;
    MfPcCodeChunk *chunk;
    for (chunk = (MfPcCodeChunk *)First(pli); chunk; chunk = (MfPcCodeChunk *)Next(pli)) {
        if (chunk->mType == PC_CHUNK_BLOCKLINE)
            break;
    }
    Prepend(new MfPcChunkLineInfo(((MfPcChunkBlockLine *)chunk)->mLine));
    for (chunk = (MfPcCodeChunk *)Last(pli); chunk; chunk = (MfPcCodeChunk *)Prev(pli)) {
        if (chunk->mType == PC_CHUNK_BLOCKLINE)
            break;
    }
    Insert(new MfPcChunkLineInfo(((MfPcChunkBlockLine *)chunk)->mLine), LP_BEFORE, pli);
}
