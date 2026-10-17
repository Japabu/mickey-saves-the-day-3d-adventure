// PscDebugger.dll 0x10001000-0x10002e40: breakpoints. The user sets
// breakpoints on lines (MfPdbSetBreakpoint); while the debugger is active they
// are planted into the bytecode of the functions (MfPdbBreakpoint), indexed
// by address.
#include "PscDebugger.h"
#include "PdbOptions.h"

// 0x10001000 ??0MfPdbBreakpoint@@QAE@PAEK@Z
MfPdbBreakpoint::MfPdbBreakpoint(unsigned char *address, unsigned long data)
    : mAddress(address), mBytecode(*address), mData(data), mEntry(0)
{
}

// 0x10001030 ?Copy@MfListItemInterface@@UAEPAXPAX@Z
// 0x10001040 ?Iterate@MfListItemInterface@@UAEHPAX@Z
// 0x10001050 ?Matches@MfListItemInterface@@UAEHPAX0@Z
// 0x10001060 ??_GMfPdbBreakpoint@@UAEPAXI@Z

// 0x10001080 ??1MfPdbBreakpoint@@UAE@XZ
MfPdbBreakpoint::~MfPdbBreakpoint()
{
}

// 0x10001090 ?Destroy@MfPdbBreakpoint@@UAEXPAX@Z

// 0x100010b0 ?SetEntry@MfPdbBreakpoint@@QAEXPAVMfPdbFunctionRegistryEntry@@@Z
void MfPdbBreakpoint::SetEntry(MfPdbFunctionRegistryEntry *entry)
{
    if (mEntry)
        mEntry->RemoveBreakpoint(this);
    mEntry = entry;
    entry->AddBreakpoint(this);
}

// 0x100010e0 ?ReleaseEntry@MfPdbBreakpoint@@QAEXH@Z
void MfPdbBreakpoint::ReleaseEntry(int entryGone)
{
    if (mEntry) {
        if (!entryGone)
            mEntry->RemoveBreakpoint(this);
        mEntry = 0;
    }
}

// 0x10001110 ??0MfPdbBreakpointManager@@QAE@XZ
MfPdbBreakpointManager::MfPdbBreakpointManager()
    : mRefs(1)
{
    mSetBreakpoints = new MfDblList;
    mBreakpoints = new MfDblList;
    mPlanted = new MfIndex(0x35);
}

// 0x100011d0 ??_GMfDblList@@UAEPAXI@Z
// 0x100011f0 ??_GMfPdbBreakpointManager@@UAEPAXI@Z
// 0x10001210 ??_GMfIndex@@UAEPAXI@Z

// 0x10001230 ??1MfPdbBreakpointManager@@UAE@XZ
MfPdbBreakpointManager::~MfPdbBreakpointManager()
{
    if (mSetBreakpoints) {
        LiiExemplar exemplar;
        MfPdbSetBreakpoint destroyer(exemplar);
        mSetBreakpoints->Clear(destroyer);
    }
    delete mSetBreakpoints;
    delete mBreakpoints;
    delete mPlanted;
}

// 0x100012f0 ??1MfPdbSetBreakpoint@@UAE@XZ
// 0x10001300 ??_GMfPdbSetBreakpoint@@UAEPAXI@Z

// 0x10001320 ?GetInterfaceName@MfPdbBreakpointManager@@UAEXPADHAAH1@Z
void MfPdbBreakpointManager::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "BreakpointManager", size);
    major = 1;
    minor = 0;
}

// 0x10001350 ?AddRef@MfPdbBreakpointManager@@UAEHXZ
// 0x10001360 ?Release@MfPdbBreakpointManager@@UAEHXZ

// 0x10001380 ?AddBreakpoint@MfPdbBreakpointManager@@UAEHABVMfString@@H@Z
int MfPdbBreakpointManager::AddBreakpoint(const MfString &module, int line)
{
    int added = 0;
    if (!FindSetBreakpoint(module, line, 0)) {
        MfPdbSetBreakpoint *set = new MfPdbSetBreakpoint(module, line, 1);
        added = 1;
        mSetBreakpoints->Add(set);
        set->ShowGlyph(0);
        if (pdbDebugger->IsActive())
            Plant(set);
    }
    return added;
}

// 0x10001440 ?RemoveBreakpoint@MfPdbBreakpointManager@@UAEHABVMfString@@H@Z
int MfPdbBreakpointManager::RemoveBreakpoint(const MfString &module, int line)
{
    int removed = 0;
    MfDblListItem *where;
    MfPdbSetBreakpoint *set = FindSetBreakpoint(module, line, &where);
    if (set) {
        DeleteSetBreakpoint(set);
        removed = 1;
    }
    return removed;
}

// 0x10001480 ?DeleteSetBreakpoint@MfPdbBreakpointManager@@QAEHPAVMfPdbSetBreakpoint@@@Z
int MfPdbBreakpointManager::DeleteSetBreakpoint(MfPdbSetBreakpoint *set)
{
    int deleted = 0;
    MfDblListItem *where = mSetBreakpoints->Find(set);
    if (where) {
        set->ShowGlyph(1);
        if (pdbDebugger->IsActive())
            Unplant(set);
        mSetBreakpoints->Delete(where);
        delete set;
        deleted = 1;
    }
    return deleted;
}

// 0x100014f0 ?ToggleBreakpoint@MfPdbBreakpointManager@@UAEHABVMfString@@H@Z
int MfPdbBreakpointManager::ToggleBreakpoint(const MfString &module, int line)
{
    int set;
    MfDblListItem *where;
    MfPdbSetBreakpoint *bp = FindSetBreakpoint(module, line, &where);
    if (bp) {
        bp->ShowGlyph(1);
        if (pdbDebugger->IsActive())
            Unplant(bp);
        mSetBreakpoints->Delete(where);
        delete bp;
        set = 0;
    } else {
        bp = new MfPdbSetBreakpoint(module, line, 1);
        mSetBreakpoints->Add(bp);
        set = 1;
        bp->ShowGlyph(0);
        if (pdbDebugger->IsActive())
            Plant(bp);
    }
    return set;
}

// 0x10001600 ?EnableBreakpoint@MfPdbBreakpointManager@@UAEHABVMfString@@H@Z
int MfPdbBreakpointManager::EnableBreakpoint(const MfString &module, int line)
{
    int found = 0;
    MfPdbSetBreakpoint *set = FindSetBreakpoint(module, line, 0);
    if (set) {
        found = 1;
        set->ShowGlyph(1);
        set->mEnabled = 1;
        if (pdbDebugger->IsActive())
            Plant(set);
        set->ShowGlyph(0);
    }
    return found;
}

// 0x10001660 ?DisableBreakpoint@MfPdbBreakpointManager@@UAEHABVMfString@@H@Z
int MfPdbBreakpointManager::DisableBreakpoint(const MfString &module, int line)
{
    int found = 0;
    MfPdbSetBreakpoint *set = FindSetBreakpoint(module, line, 0);
    if (set) {
        set->ShowGlyph(1);
        set->mEnabled = 0;
        if (pdbDebugger->IsActive())
            Unplant(set);
        set->ShowGlyph(0);
        found = 1;
    }
    return found;
}

// 0x100016c0 ?ToggleEnabled@MfPdbBreakpointManager@@UAEHABVMfString@@H@Z
int MfPdbBreakpointManager::ToggleEnabled(const MfString &module, int line)
{
    int found = 0;
    MfPdbSetBreakpoint *set = FindSetBreakpoint(module, line, 0);
    if (set)
        found = 1;
    ToggleEnabled(set);     // checks for 0 itself
    return found;
}

// 0x100016f0 ?ToggleEnabled@MfPdbBreakpointManager@@QAEXPAVMfPdbSetBreakpoint@@@Z
void MfPdbBreakpointManager::ToggleEnabled(MfPdbSetBreakpoint *set)
{
    if (set) {
        set->ShowGlyph(1);
        if (set->mEnabled) {
            set->mEnabled = 0;
            if (pdbDebugger->IsActive())
                Unplant(set);
        } else {
            set->mEnabled = 1;
            if (pdbDebugger->IsActive())
                Plant(set);
        }
        set->ShowGlyph(0);
    }
}

// 0x10001770 ?RemoveAll@MfPdbBreakpointManager@@UAEXXZ
void MfPdbBreakpointManager::RemoveAll()
{
    if (mSetBreakpoints) {
        if (pdbDebugger->IsActive())
            UnplantAll();
        LiiExemplar exemplar;
        MfPdbSetBreakpoint destroyer(exemplar);
        mSetBreakpoints->Clear(destroyer);
        CPiccoloWnd::ClearDisplayedGlyphs(kGlyphBreakpoint);
        MfPeCodeEditor::ClearGlobalGlyphs(kGlyphBreakpoint);
        CPiccoloWnd::ClearDisplayedGlyphs(kGlyphDisabledBreakpoint);
        MfPeCodeEditor::ClearGlobalGlyphs(kGlyphDisabledBreakpoint);
    }
}

// 0x10001850 ?RemoveModule@MfPdbBreakpointManager@@UAEXABVMfString@@@Z
void MfPdbBreakpointManager::RemoveModule(const MfString &module)
{
    if (mSetBreakpoints) {
        MfDblListItem *pli;
        MfPdbSetBreakpoint *set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetFirst(pli);
        while (set) {
            if (module == set->mModule) {
                Unplant(set);
                set->ShowGlyph(1);
                mSetBreakpoints->Delete(pli);
                delete set;
                set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetCurrent(pli);
            } else
                set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetNext(pli);
        }
    }
}

// 0x100018e0 ?DisableAll@MfPdbBreakpointManager@@UAEXXZ
void MfPdbBreakpointManager::DisableAll()
{
    if (mSetBreakpoints) {
        if (pdbDebugger->IsActive())
            UnplantAll();
        MfDblListItem *pli;
        for (MfPdbSetBreakpoint *set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetFirst(pli); set;
             set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetNext(pli)) {
            set->ShowGlyph(1);
            set->mEnabled = 0;
            set->ShowGlyph(0);
        }
    }
}

// 0x10001950 ?DisableModule@MfPdbBreakpointManager@@UAEXABVMfString@@@Z
void MfPdbBreakpointManager::DisableModule(const MfString &module)
{
    if (mSetBreakpoints) {
        MfDblListItem *pli;
        for (MfPdbSetBreakpoint *set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetFirst(pli); set;
             set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetNext(pli)) {
            if (module == set->mModule) {
                if (pdbDebugger->IsActive())
                    Unplant(set);
                set->ShowGlyph(1);
                set->mEnabled = 0;
                set->ShowGlyph(0);
            }
        }
    }
}

// 0x100019d0 ?PlantAll@MfPdbBreakpointManager@@UAEHXZ
int MfPdbBreakpointManager::PlantAll()
{
    MfDblListItem *pli;
    for (MfPdbSetBreakpoint *set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetFirst(pli); set;
         set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetNext(pli))
        Plant(set);
    return 1;
}

// 0x10001a10 ?Plant@MfPdbBreakpointManager@@QAEXPAVMfPdbSetBreakpoint@@@Z
void MfPdbBreakpointManager::Plant(MfPdbSetBreakpoint *set)
{
    if (set->mEnabled) {
        MfPdbFunctionRegistryEntry *entry =
            dynamic_cast<MfPdbFunctionRegistryEntry *>(pdbDebugger->mRegistry->Find(set->mModule, set->mLine));
        if (entry) {
            MfPdbBreakpoint *bp = entry->Plant(set);
            if (bp) {
                mBreakpoints->Add(bp);
                mPlanted->Add((unsigned long)bp->mAddress, bp);
            }
        }
    }
}

// 0x10001a80 ?Unplant@MfPdbBreakpointManager@@QAEXPAVMfPdbSetBreakpoint@@@Z
void MfPdbBreakpointManager::Unplant(MfPdbSetBreakpoint *set)
{
    MfPdbFunctionRegistryEntry *entry =
        dynamic_cast<MfPdbFunctionRegistryEntry *>(pdbDebugger->mRegistry->Find(set->mModule, set->mLine));
    if (entry) {
        unsigned char *address = entry->FindPlanted(set);
        if (address)
            DeleteBreakpoint((unsigned long)address);
    }
}

// 0x10001ae0 ?UnplantAll@MfPdbBreakpointManager@@UAEHXZ
int MfPdbBreakpointManager::UnplantAll()
{
    MfDblListItem *pli;
    for (MfPdbBreakpoint *bp = (MfPdbBreakpoint *)mBreakpoints->GetFirst(pli); bp;
         bp = (MfPdbBreakpoint *)mBreakpoints->GetNext(pli)) {
        bp->Restore();
        bp->ReleaseEntry(0);
        delete bp;
    }
    mBreakpoints->Clear();
    mPlanted->Clear();
    return 1;
}

// EQUIV 0x10001b60 ?BreakpointHit@MfPdbBreakpointManager@@UAEHKHPAVMfScVmEnv@@@Z: reviewed: register choice and stack slots only (the original keeps replant in the unused parameter's slot, the new block in env's slot and reloads address, using esi for 0; here replant is in ebx and address stays in esi)
int MfPdbBreakpointManager::BreakpointHit(unsigned long address, int, MfScVmEnv *env)
{
    MfPdbBreakpoint *hit = FindBreakpoint(address);
    if (!hit)
        return 0;
    unsigned char *replant = (unsigned char *)hit->mData;
    DeleteBreakpoint(address);
    MfScDataHandle *h = env->m_function;
    MfScBytecodeRoutine *routine = h ? dynamic_cast<MfScBytecodeRoutine *>(h->GetData()) : 0;
    MfPdbFunctionRegistryEntry *entry = dynamic_cast<MfPdbFunctionRegistryEntry *>(
        pdbDebugger->mRegistry->Find(routine->ModuleName(), routine->Name()));
    if (entry && *replant != BC_BREAKPOINT) {
        MfPdbBreakpoint *bp = new MfPdbBreakpoint(replant, address);
        bp->SetEntry(entry);
        *replant = BC_BREAKPOINT;
        mBreakpoints->Add(bp);
        mPlanted->Add((unsigned long)bp->mAddress, bp);
    }
    return 1;
}

// 0x10001c90 ?GetBreakpointInfo@MfPdbBreakpointManager@@UAEHKHPAUMfPdbBreakpointInfo@@@Z
int MfPdbBreakpointManager::GetBreakpointInfo(unsigned long address, int, MfPdbBreakpointInfo *info)
{
    MfPdbBreakpoint *bp = FindBreakpoint(address);
    if (!bp)
        return 0;
    info->data = bp->mData;
    info->bytecode = bp->mBytecode;
    info->entry = bp->mEntry;
    return 1;
}

// 0x10001cc0 ?PlantModule@MfPdbBreakpointManager@@UAEHABVMfString@@@Z
int MfPdbBreakpointManager::PlantModule(const MfString &module)
{
    MfDblListItem *pli;
    for (MfPdbSetBreakpoint *set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetFirst(pli); set;
         set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetNext(pli))
        if (set->mModule == module)
            Plant(set);
    return 1;
}

// 0x10001d20 ?Save@MfPdbBreakpointManager@@UAEHXZ

// 0x10001d30 ?FindSetBreakpoint@MfPdbBreakpointManager@@QAEPAVMfPdbSetBreakpoint@@ABVMfString@@HPAPAVMfDblListItem@@@Z
MfPdbSetBreakpoint *MfPdbBreakpointManager::FindSetBreakpoint(const MfString &module, int line, MfDblListItem **where)
{
    MfDblListItem *pli;
    for (MfPdbSetBreakpoint *set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetFirst(pli); set;
         set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetNext(pli)) {
        if (set->mModule == module && set->mLine == line) {
            if (where)
                *where = pli;
            return set;
        }
    }
    return 0;
}

// 0x10001db0 ?FindBreakpoint@MfPdbBreakpointManager@@QAEPAVMfPdbBreakpoint@@K@Z
MfPdbBreakpoint *MfPdbBreakpointManager::FindBreakpoint(unsigned long address)
{
    return (MfPdbBreakpoint *)mPlanted->Find(address);
}

// 0x10001dd0 ?DeleteBreakpoint@MfPdbBreakpointManager@@QAEXK@Z
void MfPdbBreakpointManager::DeleteBreakpoint(unsigned long address)
{
    MfPdbBreakpoint *bp = (MfPdbBreakpoint *)mPlanted->Find(address);
    if (bp) {
        MfDblListItem *pli = mBreakpoints->Find(bp);
        mBreakpoints->Delete(pli);
        mPlanted->Delete(address);
        bp->Restore();
        bp->ReleaseEntry(0);
        delete bp;
    }
}

// 0x10001e40 ?MoveSetBreakpoint@MfPdbBreakpointManager@@QAEXPAVMfPdbSetBreakpoint@@H@Z
void MfPdbBreakpointManager::MoveSetBreakpoint(MfPdbSetBreakpoint *set, int line)
{
    set->ShowGlyph(1);
    set->mLine = line;
    set->ShowGlyph(0);
}

// EQUIV 0x10001e70 ?PlantStep@MfPdbBreakpointManager@@QAEPAVMfPdbBreakpoint@@PAUMfPdbStepContext@@KH@Z: reviewed: register choice and stack slots (this/stop/end/bytecode in other registers and slots), the two loads for the end address in the other order, and the breakpoint's saved bytecode zero-extended through a stack temporary instead of xor/mov; same calls, arguments and branches
MfPdbBreakpoint *MfPdbBreakpointManager::PlantStep(MfPdbStepContext *ctx, unsigned long flags, int atNext)
{
    MfPdbBreakpoint *result = 0;
    int offset = ctx->env->m_ip - ctx->routine->mBytecodes;
    int line = ctx->routine->mDebugInfo ? ctx->routine->mDebugInfo->FindLine(offset) : -1;
    unsigned char *ip = ctx->env->m_ip - 1;
    offset = ip - ctx->routine->mBytecodes;
    if (!atNext)
        ip += MfScBytecodes::Size(*ip);
    int next = -1;
    if (ctx->routine->mDebugInfo)
        next = ctx->routine->mDebugInfo->FindOffsetOfNextLine(offset);
    unsigned char *end;
    if (next == -1)
        end = ctx->routine->mBytecodes + ctx->routine->mCodeSize - 1;
    else
        end = ctx->routine->mBytecodes + next;
    if (ip == ctx->env->m_ip - 1) {
        int planted = 0;
        unsigned char bytecode = *ip;
        if (bytecode == BC_BREAKPOINT) {
            bytecode = FindBreakpoint((unsigned long)ip)->mBytecode;
            planted = 1;
        }
        if (MfScBytecodes::IsJump(bytecode)) {
            if (planted)
                *ip = bytecode;
            result = PlantJump(ctx, flags);
            if (planted)
                *ip = BC_BREAKPOINT;
            return result;
        }
    }
    unsigned char *stop = 0;
    while (ip < end) {
        if (MfScBytecodes::IsJump(*ip)) {
            stop = ip;
            break;
        }
        if (flags == kStepOver && (*ip == BC_INVOKEMETH || *ip == BC_INVOKEFUNC)) {
            stop = ip;
            break;
        }
        if (*ip == BC_BREAKPOINT)
            ip += MfScBytecodes::Size(FindBreakpoint((unsigned long)ip)->mBytecode);
        else
            ip += MfScBytecodes::Size(*ip);
    }
    if (stop) {
        MfPdbFunctionRegistryEntry *entry = dynamic_cast<MfPdbFunctionRegistryEntry *>(
            pdbDebugger->mRegistry->Find(ctx->routine->ModuleName(), ctx->routine->Name()));
        if (entry) {
            unsigned long stopFlags;
            if (*stop == BC_INVOKEMETH || *stop == BC_INVOKEFUNC)
                stopFlags = flags | kStepInvoke;
            else
                stopFlags = flags | kStepJump;
            MfPdbBreakpoint *bp = new MfPdbBreakpoint(stop, stopFlags);
            result = bp;
            bp->SetEntry(entry);
            *stop = BC_BREAKPOINT;
            mBreakpoints->Add(bp);
            mPlanted->Add((unsigned long)bp->mAddress, bp);
        }
    } else
        result = PlantNext(line, next, ctx);
    return result;
}

// 0x100020f0 ?PlantStepInto@MfPdbBreakpointManager@@QAEPAVMfPdbBreakpoint@@PAUMfPdbStepContext@@@Z
MfPdbBreakpoint *MfPdbBreakpointManager::PlantStepInto(MfPdbStepContext *ctx)
{
    MfPdbBreakpoint *result = 0;
    int offset = ctx->env->m_ip - ctx->routine->mBytecodes;
    int line = ctx->routine->mDebugInfo ? ctx->routine->mDebugInfo->FindLine(offset) : -1;
    unsigned char *ip = ctx->env->m_ip - 1;
    MfScDataHandle *h = ctx->env->m_sp[1];
    h->AddRef();
    if (h->IsType(0x800)) {
        MfScBytecodeRoutine *callee = (MfScBytecodeRoutine *)h->GetData();
        if (callee->m_type == 0 && callee->mDebugInfo) {
            unsigned char *first = callee->mBytecodes;
            MfScDebugInfo *info = callee->mDebugInfo;
            if (info)
                first = info->FindOffsetOfFirstLine() + callee->mBytecodes;
            MfPdbFunctionRegistryEntry *entry = dynamic_cast<MfPdbFunctionRegistryEntry *>(
                pdbDebugger->mRegistry->Find(ctx->routine->ModuleName(), ctx->routine->Name()));
            if (entry && *first != BC_BREAKPOINT) {
                MfPdbBreakpoint *bp = new MfPdbBreakpoint(first, kStepTemporary);
                result = bp;
                bp->SetEntry(entry);
                *first = BC_BREAKPOINT;
                mBreakpoints->Add(bp);
                mPlanted->Add((unsigned long)bp->mAddress, bp);
            }
        } else {
            offset = ip - ctx->routine->mBytecodes;
            int next = MfScBytecodes::Size(*ip) + offset;
            if (ctx->routine->mDebugInfo)
                next = ctx->routine->mDebugInfo->FindOffsetOfNextLine(offset);
            result = PlantNext(line, next, ctx);
        }
    }
    h->Release();
    return result;
}

// EQUIV 0x100022a0 ?PlantJump@MfPdbBreakpointManager@@QAEPAVMfPdbBreakpoint@@PAUMfPdbStepContext@@K@Z: reviewed: register choice only (ctx in edi and taken/target in ebx in the original, swapped here; the target sum through lea; this reloaded before instead of after the 0xfa store); same calls, arguments and branches
MfPdbBreakpoint *MfPdbBreakpointManager::PlantJump(MfPdbStepContext *ctx, unsigned long flags)
{
    int offset = ctx->env->m_ip - ctx->routine->mBytecodes;
    int line = ctx->routine->mDebugInfo ? ctx->routine->mDebugInfo->FindLine(offset) : -1;
    int taken = 0;
    unsigned char *ip = ctx->env->m_ip - 1;
    if (MfScBytecodes::IsUnconditionalJump(*ip))
        taken = 1;
    else if (MfScBytecodes::IsJumpZero(*ip)) {
        MfScDataHandle *h = ctx->env->m_sp[1];
        h->AddRef();
        if (h->IsType(0x10) && h->GetInt() == 0)
            taken = 1;
        h->Release();
    } else if (MfScBytecodes::IsJumpNotZero(*ip)) {
        MfScDataHandle *h = ctx->env->m_sp[1];
        h->AddRef();
        if (h->IsType(0x10) && h->GetInt() != 0)
            taken = 1;
        h->Release();
    }
    if (taken) {
        int first, last;
        if (ctx->routine->mDebugInfo)
            ctx->routine->mDebugInfo->ComputeOffsetRange(ip - ctx->routine->mBytecodes, &first, &last);
        unsigned char *target = ip + MfScBytecodes::JumpOffset(ip);
        offset = target - ctx->routine->mBytecodes;
        int targetLine = 0;
        if (ctx->routine->mDebugInfo)
            targetLine = ctx->routine->mDebugInfo->FindLine(offset);
        unsigned long targetFlags = kStepTemporary;
        if (targetLine == line || (offset >= first && offset < last)) {
            targetFlags = flags & ~kStepJump;
            if (MfScBytecodes::IsJump(*target))
                targetFlags |= kStepJump;
            else
                targetFlags |= kStepTarget;
        }
        MfPdbFunctionRegistryEntry *entry = dynamic_cast<MfPdbFunctionRegistryEntry *>(
            pdbDebugger->mRegistry->Find(ctx->routine->ModuleName(), ctx->routine->Name()));
        if (entry && *target != BC_BREAKPOINT) {
            MfPdbBreakpoint *bp = new MfPdbBreakpoint(target, targetFlags);
            bp->SetEntry(entry);
            *target = BC_BREAKPOINT;
            mBreakpoints->Add(bp);
            mPlanted->Add((unsigned long)bp->mAddress, bp);
            return bp;
        }
        return 0;
    }
    return PlantStep(ctx, flags & kStepModes, 0);
}

// 0x100024d0 ?PlantLine@MfPdbBreakpointManager@@QAEPAVMfPdbBreakpoint@@PAUMfPdbStepContext@@ABVMfString@@H@Z
MfPdbBreakpoint *MfPdbBreakpointManager::PlantLine(MfPdbStepContext *, const MfString &module, int line)
{
    MfPdbBreakpoint *bp = 0;
    MfPdbFunctionRegistryEntry *entry =
        dynamic_cast<MfPdbFunctionRegistryEntry *>(pdbDebugger->mRegistry->Find(module, line));
    if (entry) {
        bp = entry->PlantLine(line);
        if (bp) {
            mBreakpoints->Add(bp);
            mPlanted->Add((unsigned long)bp->mAddress, bp);
        }
    }
    return bp;
}

// 0x10002540 ?PlantReturn@MfPdbBreakpointManager@@QAEPAVMfPdbBreakpoint@@PAUMfPdbStepContext@@@Z
MfPdbBreakpoint *MfPdbBreakpointManager::PlantReturn(MfPdbStepContext *ctx)
{
    MfPdbBreakpoint *bp = 0;
    unsigned char *ip = 0;
    MfScVmFrame frame;
    if (ctx->env->FirstFrame(&frame)) {
        ctx->env->NextFrame(&frame);
        ip = (unsigned char *)frame.mIP;
    }
    if (ip && *ip != BC_4C && *ip != BC_BREAKPOINT) {
        bp = new MfPdbBreakpoint(ip, kStepTemporary);
        *ip = BC_BREAKPOINT;
        MfPdbFunctionRegistryEntry *entry = dynamic_cast<MfPdbFunctionRegistryEntry *>(
            pdbDebugger->mRegistry->Find(ctx->routine->ModuleName(), ctx->routine->Name()));
        if (entry) {
            bp->SetEntry(entry);
            mBreakpoints->Add(bp);
            mPlanted->Add((unsigned long)bp->mAddress, bp);
        }
    }
    return bp;
}

// 0x10002670 ?PlantNext@MfPdbBreakpointManager@@QAEPAVMfPdbBreakpoint@@HHPAUMfPdbStepContext@@@Z
MfPdbBreakpoint *MfPdbBreakpointManager::PlantNext(int, int next, MfPdbStepContext *ctx)
{
    MfPdbBreakpoint *bp = 0;
    unsigned char *ip = 0;
    if (next == -1) {
        MfScVmFrame frame;
        if (ctx->env->FirstFrame(&frame)) {
            ctx->env->NextFrame(&frame);
            ip = (unsigned char *)frame.mIP;
        }
    } else
        ip = ctx->routine->mBytecodes + next;
    if (ip && *ip != BC_4C && *ip != BC_BREAKPOINT) {
        bp = new MfPdbBreakpoint(ip, kStepTemporary);
        *ip = BC_BREAKPOINT;
        MfPdbFunctionRegistryEntry *entry = dynamic_cast<MfPdbFunctionRegistryEntry *>(
            pdbDebugger->mRegistry->Find(ctx->routine->ModuleName(), ctx->routine->Name()));
        if (entry) {
            bp->SetEntry(entry);
            mBreakpoints->Add(bp);
            mPlanted->Add((unsigned long)bp->mAddress, bp);
        }
    }
    return bp;
}

// 0x100027b0 ?FillListBox@MfPdbBreakpointManager@@QAEXPAVCListBox@@@Z
void MfPdbBreakpointManager::FillListBox(CListBox *list)
{
    list->ResetContent();
    if (mSetBreakpoints) {
        MfDblListItem *pli;
        for (MfPdbSetBreakpoint *set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetFirst(pli); set;
             set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetNext(pli)) {
            int index = list->AddString(set->mModule);
            if (index != LB_ERR)
                list->SetItemDataPtr(index, set);
        }
    }
}

// 0x10002840 ?SaveBreakpoints@MfPdbBreakpointManager@@QAEXXZ
void MfPdbBreakpointManager::SaveBreakpoints()
{
    UtilityApp app;
    int count = 0;
    if (mSetBreakpoints) {
        MfString value;
        MfString entry;
        int i = 1;
        MfDblListItem *pli;
        for (MfPdbSetBreakpoint *set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetFirst(pli); set;
             set = (MfPdbSetBreakpoint *)mSetBreakpoints->GetNext(pli)) {
            value = NULL_STRING;
            value << set->mLine << "," << (set->mEnabled ? "TRUE" : "FALSE") << "," << set->mModule;
            entry = MfString(pszBreakpoint) + MfString(i);
            app.WriteProfileStringA(pszDebuggerOptions, entry, value);
            i++;
        }
        count = i - 1;
    }
    app.WriteProfileInt(pszDebuggerOptions, pszBreakpointCount, count);
}

// EQUIV 0x10002a50 ?LoadBreakpoints@MfPdbBreakpointManager@@QAEXXZ: reviewed: function body byte-identical; only the unwind funclet's scratch temporary sits in another stack slot (ebp-0xb4 in the original, ebp-0x90 here)
void MfPdbBreakpointManager::LoadBreakpoints()
{
    UtilityApp app;
    int count = app.GetProfileIntA(pszDebuggerOptions, pszBreakpointCount, 0);
    MfString entry;
    MfString value;
    MfStringList fields;
    for (int i = 1; i <= count; i++) {
        fields.Clear();
        entry = MfString(pszBreakpoint) + MfString(i);
        value = app.GetProfileStringA(pszDebuggerOptions, entry, NULL_STRING);
        if (value != NULL_STRING) {
            value.Tokenize(",", fields);
            if (fields.GetCount() == 3) {
                MfDblListItem *pos;
                MfString *lineText = fields.First(pos);
                MfString *enabledText = fields.Next(pos);
                MfString *module = fields.Next(pos);
                if (lineText && enabledText && module) {
                    int enabled = !enabledText->CaseCompare("TRUE");
                    int line = *lineText;
                    MfPdbSetBreakpoint *set = new MfPdbSetBreakpoint(*module, line, enabled);
                    MfDblListItem *where;
                    MfPdbSetBreakpoint *old = FindSetBreakpoint(*module, line, &where);
                    if (old) {
                        mSetBreakpoints->Delete(where);
                        delete old;
                    }
                    mSetBreakpoints->Add(set);
                    set->ShowGlyph(0);
                    if (pdbDebugger->IsActive())
                        Plant(set);
                }
            }
        }
    }
}
