// PscDebugger.dll 0x10006ed0-0x100074fa: the debugger's function registry.
// While the debugger runs it replaces the runtime's registry; its entries know
// the breakpoints planted in their bytecode.
#include "PscDebugger.h"

// 0x10006ed0 ??0MfPdbFunctionRegistry@@QAE@XZ
MfPdbFunctionRegistry::MfPdbFunctionRegistry()
{
}

// 0x10006ef0 ??_GMfPdbFunctionRegistry@@UAEPAXI@Z

// 0x10006f10 ??1MfPdbFunctionRegistry@@UAE@XZ
MfPdbFunctionRegistry::~MfPdbFunctionRegistry()
{
}

// 0x10006f20 ?Add@MfPdbFunctionRegistry@@UAEHPAVMfScDataHandle@@@Z
int MfPdbFunctionRegistry::Add(MfScDataHandle *function)
{
    MfScBytecodeRoutine *routine =
        function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;
    Lock();
    MfScFunctionRegistryModule *module = FindModule(routine->ModuleName(), 0);
    if (!module) {
        module = new MfPdbFunctionRegistryModule;
        mModules->Add(module);
    }
    module->AddFunction(function);
    Unlock();
    return 1;
}

// 0x10007000 ??_GMfPdbFunctionRegistryModule@@UAEPAXI@Z

// 0x10007020 ??0MfPdbFunctionRegistryEntry@@QAE@PAVMfScDataHandle@@@Z
MfPdbFunctionRegistryEntry::MfPdbFunctionRegistryEntry(MfScDataHandle *function)
    : MfScFunctionRegistryEntry(function), mBreakpoints(0)
{
    mBreakpoints = new MfDblList;
}

// 0x100070a0 ??_GMfPdbFunctionRegistryEntry@@UAEPAXI@Z

// 0x100070c0 ??1MfPdbFunctionRegistryEntry@@UAE@XZ
MfPdbFunctionRegistryEntry::~MfPdbFunctionRegistryEntry()
{
    if (mBreakpoints) {
        MfDblListItem *pos;
        for (MfPdbBreakpoint *bp = (MfPdbBreakpoint *)mBreakpoints->GetFirst(pos); bp;
             bp = (MfPdbBreakpoint *)mBreakpoints->GetNext(pos)) {
            bp->Restore();
            bp->ReleaseEntry(1);
        }
        mBreakpoints->Clear();
        delete mBreakpoints;
    }
}

// 0x10007170 ?Plant@MfPdbFunctionRegistryEntry@@QAEPAVMfPdbBreakpoint@@PAVMfPdbSetBreakpoint@@@Z
MfPdbBreakpoint *MfPdbFunctionRegistryEntry::Plant(MfPdbSetBreakpoint *set)
{
    MfPdbBreakpoint *bp = 0;
    if (set->mEnabled && mData) {
        MfScBytecodeRoutine *routine = dynamic_cast<MfScBytecodeRoutine *>(mData->GetData());
        MfScDebugInfo *info = routine->DebugInfo();
        if (info) {
            int offset = info->FindOffset(set->mLine);
            unsigned char *address = routine->mBytecodes + offset;
            if (*address != BC_BREAKPOINT) {
                bp = new MfPdbBreakpoint(address, kStepPlanted);
                bp->SetEntry(this);
                *address = BC_BREAKPOINT;
                int line = info->FindLine(offset);
                if (line != set->mLine)
                    pdbDebugger->mBreakpoints->MoveSetBreakpoint(set, line);
            }
        }
    }
    return bp;
}

// 0x10007280 ?PlantLine@MfPdbFunctionRegistryEntry@@QAEPAVMfPdbBreakpoint@@H@Z
MfPdbBreakpoint *MfPdbFunctionRegistryEntry::PlantLine(int line)
{
    MfPdbBreakpoint *bp = 0;
    if (mData) {
        MfScBytecodeRoutine *routine = dynamic_cast<MfScBytecodeRoutine *>(mData->GetData());
        MfScDebugInfo *info = routine->DebugInfo();
        if (info) {
            int offset = info->FindOffset(line);
            unsigned char *address = routine->mBytecodes + offset;
            if (*address != BC_BREAKPOINT) {
                bp = new MfPdbBreakpoint(address, kStepTemporary);
                bp->SetEntry(this);
                *address = BC_BREAKPOINT;
            }
        }
    }
    return bp;
}

// 0x10007340 ?FindPlanted@MfPdbFunctionRegistryEntry@@QAEPAEPAVMfPdbSetBreakpoint@@@Z
unsigned char *MfPdbFunctionRegistryEntry::FindPlanted(MfPdbSetBreakpoint *set)
{
    unsigned char *address = 0;
    if (mData) {
        MfScBytecodeRoutine *routine = dynamic_cast<MfScBytecodeRoutine *>(mData->GetData());
        MfScDebugInfo *info = routine->DebugInfo();
        if (info) {
            int offset = info->FindOffset(set->mLine);
            address = routine->mBytecodes + offset;
            if (*address != BC_BREAKPOINT)
                address = 0;
        }
    }
    return address;
}

// 0x100073a0 ?AddBreakpoint@MfPdbFunctionRegistryEntry@@QAEXPAVMfPdbBreakpoint@@@Z
void MfPdbFunctionRegistryEntry::AddBreakpoint(MfPdbBreakpoint *bp)
{
    if (!mBreakpoints->Find(bp))
        mBreakpoints->Add(bp);
}

// 0x100073d0 ?RemoveBreakpoint@MfPdbFunctionRegistryEntry@@QAEXPAVMfPdbBreakpoint@@@Z
void MfPdbFunctionRegistryEntry::RemoveBreakpoint(MfPdbBreakpoint *bp)
{
    MfDblListItem *pos = mBreakpoints->Find(bp);
    mBreakpoints->Delete(pos);
}

// 0x10007400 ??1MfPdbFunctionRegistryModule@@UAE@XZ
MfPdbFunctionRegistryModule::~MfPdbFunctionRegistryModule()
{
}

// 0x10007410 ?AddFunction@MfPdbFunctionRegistryModule@@UAEXPAVMfScDataHandle@@@Z
void MfPdbFunctionRegistryModule::AddFunction(MfScDataHandle *function)
{
    MfScBytecodeRoutine *routine =
        function ? dynamic_cast<MfScBytecodeRoutine *>(function->GetData()) : 0;
    MfDblListItem *pos;
    MfScFunctionRegistryEntry *entry = FindFunction(routine->Name(), &pos);
    if (entry) {
        mFunctions->Delete(pos);
        delete entry;
    }
    if (mFunctions->GetCount() == 0) {
        mModule = routine->ModuleName();
        mModuleType = *routine->mModuleType;
    }
    mFunctions->Add(new MfPdbFunctionRegistryEntry(function));
}
