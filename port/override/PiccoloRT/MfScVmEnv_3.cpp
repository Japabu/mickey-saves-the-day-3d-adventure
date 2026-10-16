// PORT: stack words are uintptr_t (pointer-sized), and the initial stack pointer is counted in words of that size (the source divided by 4).
// PiccoloRT.dll 0x1001bac0-0x1001c1ef: MfScVmEnv call frames, variables and
// exceptions.
#include "MfString.h"
#include "MfScDataHandle.h"
#include "MfScVmEnv.h"
#include "MfScVmFrame.h"
#include "MfScHandleDict.h"
#include <windows.h>
#include "MfHPTimer.h"
#include "MfScVmStack.h"
#include "MfScVmManager.h"
#include "MfScVmCallStackList.h"
#include "MfScBytecodeRoutine.h"
#include "MfScDebugInfo.h"

_RTL_CRITICAL_SECTION MfScVmEnv::csGuard;         // 0x10035280
MfScHandleDict *MfScVmEnv::varpool;
int MfScVmEnv::iIdPool;

// the global variable pool is made on first use
#define INIT_VARPOOL()                            \
    if (!varpool) {                               \
        InitializeCriticalSection(&csGuard);      \
        varpool = new MfScHandleDict;             \
    }

// 0x1001b900 ??0MfScVmEnv@@QAE@XZ
MfScVmEnv::MfScVmEnv()
    : m_ip(0), m_sp(0), m_frame(0), m_constants(0), m_object(0), m_function(0), m_reg0(0), m_reg1(0),
      m_reg2(0), m_reg3(0), m_varStack(0), m_stackBase(0), m_stackSize(0), m_stack(0),
      m_exception(0), m_artnSet(0), m_artnValue(0), m_messageThread(0), m_interrupted(0),
      m_executing(0), m_id(0), m_accumTime(0)
{
    unsigned int committed;
    m_stack = new MfScVmStack(MfScVmStack::PageSize() << 5, 1, MfScVmStack::PageSize() << 4,
                              (void **)&m_stackBase, &committed);
    m_sp = (MfScDataHandle **)&m_stackBase[committed / sizeof(*m_stackBase) - 1]; // PORT: was committed / 4
    m_stackSize += committed;
    INIT_VARPOOL();
    m_id = iIdPool++;
    gVmManager->AddEnv(this);
}

// 0x1001ba60 ??1MfScVmEnv@@UAE@XZ
MfScVmEnv::~MfScVmEnv()
{
    delete m_stack;
    gVmManager->RemoveEnv(this);
}

// 0x1001bac0 ?PushFunctionFrame@MfScVmEnv@@QAEHXZ
int MfScVmEnv::PushFunctionFrame()
{
    PushWord((uintptr_t)m_ip); // PORT: uintptr_t stack words (here and below)
    PushWord((uintptr_t)m_object);
    PushWord((uintptr_t)m_function);
    PushWord((uintptr_t)m_constants);
    PushWord((uintptr_t)m_frame);
    m_frame = (uintptr_t *)m_sp;
    return 1;
}

// 0x1001bb20 ?PopFunctionFrame@MfScVmEnv@@QAEXXZ
void MfScVmEnv::PopFunctionFrame()
{
    ReleaseStack(m_frame);
    m_frame = (uintptr_t *)PopWord();
    m_constants = (MfScDataHandle **)PopWord();
    if (m_function)
        m_function->Release();
    m_function = (MfScDataHandle *)PopWord();
    if (m_object)
        m_object->Release();
    m_object = (MfScDataHandle *)PopWord();
    m_ip = (unsigned char *)PopWord();
}

// 0x1001bba0 ?ReleaseStack@MfScVmEnv@@QAEXPAK@Z
void MfScVmEnv::ReleaseStack(uintptr_t *to)
{
    while ((uintptr_t *)m_sp < to)
        PopAndReleaseData();
}

// 0x1001bbd0 ?FirstFrame@MfScVmEnv@@QBEHPAVMfScVmFrame@@@Z
int MfScVmEnv::FirstFrame(MfScVmFrame *frame) const
{
    frame->ReleaseHandles();
    frame->mIP = (uintptr_t)m_ip;
    frame->mObject = m_object;
    if (frame->mObject)
        frame->mObject->AddRef();
    frame->mFunction = m_function;
    if (frame->mFunction)
        frame->mFunction->AddRef();
    frame->mConstants = (uintptr_t)m_constants;
    frame->mFrame = m_frame;
    return frame->mFrame != 0;
}

// 0x1001bc20 ?NextFrame@MfScVmEnv@@QBEHPAVMfScVmFrame@@@Z
int MfScVmEnv::NextFrame(MfScVmFrame *frame) const
{
    frame->mIP = frame->mFrame[5];
    if (frame->mObject)
        frame->mObject->Release();
    frame->mObject = (MfScDataHandle *)frame->mFrame[4];
    if (frame->mObject)
        frame->mObject->AddRef();
    if (frame->mFunction)
        frame->mFunction->Release();
    frame->mFunction = (MfScDataHandle *)frame->mFrame[3];
    if (frame->mFunction)
        frame->mFunction->AddRef();
    frame->mConstants = frame->mFrame[2];
    frame->mFrame = (uintptr_t *)frame->mFrame[1];
    return frame->mFrame != 0;
}

// 0x1001bca0 ?RestoreFrame@MfScVmEnv@@QAEXPAVMfScVmFrame@@@Z
void MfScVmEnv::RestoreFrame(MfScVmFrame *frame)
{
    MfScDataHandle *object = m_object;
    m_ip = (unsigned char *)frame->mIP;
    m_constants = (MfScDataHandle **)frame->mConstants;
    m_frame = frame->mFrame;
    m_object = frame->mObject;
    if (frame->mObject)
        frame->mObject->AddRef();
    if (object)
        object->Release();
    MfScDataHandle *function = m_function;
    m_function = frame->mFunction;
    if (frame->mFunction)
        frame->mFunction->AddRef();
    if (function)
        function->Release();
}

// 0x1001bd00 ?GetVariable@MfScVmEnv@@QBE?AW4MfScVariableErrorCode@MfScVariableStack@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScVariableStack::MfScVariableErrorCode MfScVmEnv::GetVariable(const MfString &name, MfScDataHandle **h) const
{
    if (m_varStack) {
        if (m_varStack->GetVariable(name, h) != MfScVariableStack::kVarOk)
            return GetGlobalVariable(name, h);
        return MfScVariableStack::kVarOk;
    }
    return GetGlobalVariable(name, h);
}

// 0x1001bd50 ?SetVariable@MfScVmEnv@@QAE?AW4MfScVariableErrorCode@MfScVariableStack@@ABVMfString@@PAVMfScDataHandle@@HH@Z
MfScVariableStack::MfScVariableErrorCode MfScVmEnv::SetVariable(const MfString &name, MfScDataHandle *h, int passRef, int global)
{
    if (m_varStack) {
        if (m_varStack->SetVariable(name, h, passRef) != MfScVariableStack::kVarOk)
            return SetGlobalVariable(name, h, passRef, global);
        if (h && passRef)
            h->Release();
        return MfScVariableStack::kVarOk;
    }
    return SetGlobalVariable(name, h, passRef, 0);
}

// 0x1001bdd0 ?GetGlobalVariable@MfScVmEnv@@SA?AW4MfScVariableErrorCode@MfScVariableStack@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScVariableStack::MfScVariableErrorCode MfScVmEnv::GetGlobalVariable(const MfString &name, MfScDataHandle **h)
{
    *h = 0;
    INIT_VARPOOL();
    EnterCriticalSection(&csGuard);
    MfScDataHandle *found = varpool->Find(name);
    LeaveCriticalSection(&csGuard);
    if (found == pdhUndefined) {
        *h = 0;
        return MfScVariableStack::kVarOk;
    }
    if (found) {
        found->AddRef();
        *h = found;
        return MfScVariableStack::kVarOk;
    }
    return MfScVariableStack::kVarNotFound;
}

// 0x1001bec0 ?SetGlobalVariable@MfScVmEnv@@SA?AW4MfScVariableErrorCode@MfScVariableStack@@ABVMfString@@PAVMfScDataHandle@@HH@Z
MfScVariableStack::MfScVariableErrorCode MfScVmEnv::SetGlobalVariable(const MfString &name, MfScDataHandle *h, int passRef, int mustExist)
{
    INIT_VARPOOL();
    MfScVariableStack::MfScVariableErrorCode code = MfScVariableStack::kVarOk;
    EnterCriticalSection(&csGuard);
    MfScDataHandle *old = varpool->Find(name);
    if (old && old != pdhUndefined && old->IsReadOnly()) {
        code = MfScVariableStack::kVarReadOnly;
        if (h && passRef)
            h->Release();
    } else if (!old && mustExist) {
        code = MfScVariableStack::kVarNotFound;
        if (h && passRef)
            h->Release();
    } else if (!h)
        varpool->Replace(name, pdhUndefined, MfScHandleDict::kAddRef);
    else
        varpool->Replace(name, h, (MfScHandleDict::HandleDictPassRef)(passRef != 0));
    LeaveCriticalSection(&csGuard);
    return code;
}

// 0x1001bfe0 ?ClearGlobalVariable@MfScVmEnv@@SA?AW4MfScVariableErrorCode@MfScVariableStack@@ABVMfString@@H@Z
MfScVariableStack::MfScVariableErrorCode MfScVmEnv::ClearGlobalVariable(const MfString &name, int force)
{
    INIT_VARPOOL();
    MfScVariableStack::MfScVariableErrorCode code = MfScVariableStack::kVarOk;
    EnterCriticalSection(&csGuard);
    MfScDataHandle *old = varpool->Find(name);
    if (old && old != pdhUndefined && old->IsReadOnly() && !force)
        code = MfScVariableStack::kVarReadOnly;
    else
        varpool->Replace(name, 0, MfScHandleDict::kAddRef);
    LeaveCriticalSection(&csGuard);
    return code;
}

// 0x1001c0c0 ?EnumGlobalHandles@MfScVmEnv@@SAXAAVMfScHandleDictIterator@@@Z
void MfScVmEnv::EnumGlobalHandles(MfScHandleDictIterator &iterator)
{
    if (varpool) {
        EnterCriticalSection(&csGuard);
        varpool->EnumHandles(iterator);
        LeaveCriticalSection(&csGuard);
    }
}

// 0x1001c0f0 ?Shutdown@MfScVmEnv@@SAXXZ
void MfScVmEnv::Shutdown()
{
    delete varpool;
    varpool = 0;
    DeleteCriticalSection(&csGuard);
}

// 0x1001c120 ?ThrowException@MfScVmEnv@@QAEXHABVMfString@@@Z
void MfScVmEnv::ThrowException(int code, const MfString &message)
{
    delete m_exception;
    m_exception = new MfScVmException(code, message);
}

// 0x1001c1c0 ??1MfScVmException@@UAE@XZ

// 0x1001c1d0 ?ClearException@MfScVmEnv@@QAEXXZ
void MfScVmEnv::ClearException()
{
    delete m_exception;
    m_exception = 0;
}

// 0x1001c200 ?GetCurrentCallStack@MfScVmEnv@@QAEXPAVMfScVmCallStackList@@@Z
void MfScVmEnv::GetCurrentCallStack(MfScVmCallStackList *list)
{
    list->Clear();
    MfScVmFrame frame;
    FirstFrame(&frame);
    while (frame.mFrame) {
        MfScData *data = frame.mFunction ? dynamic_cast<MfScData *>(frame.mFunction->GetData()) : 0;
        MfScBytecodeRoutine *routine = frame.mFunction
            ? dynamic_cast<MfScBytecodeRoutine *>(frame.mFunction->GetData()) : 0;
        MfScVmCallStackEntry *entry = new MfScVmCallStackEntry;
        entry->mFunction = data->Name();
        if (routine) {
            entry->mModule << routine->ModuleName();
            entry->mOffset = frame.mIP - (uintptr_t)routine->mBytecodes;
            if (routine->mDebugInfo)
                entry->mLine = routine->mDebugInfo->FindLine(entry->mOffset);
        }
        list->Add(entry);
        NextFrame(&frame);
    }
}

// 0x1001c380 ?Clear@MfScVmCallStackList@@UAEXXZ
void MfScVmCallStackList::Clear()
{
    MfDblList::Clear(*(MfListItemInterface *)this);
}

// 0x1001c3a0 ?Destroy@MfScVmCallStackList@@UAEXPAX@Z
void MfScVmCallStackList::Destroy(void *item)
{
    delete (MfScVmCallStackEntry *)item;
}

// 0x1001c3c0 ??1MfScVmCallStackEntry@@QAE@XZ
MfScVmCallStackEntry::~MfScVmCallStackEntry()
{
}
