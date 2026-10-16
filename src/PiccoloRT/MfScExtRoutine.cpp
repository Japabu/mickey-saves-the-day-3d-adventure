// PiccoloRT.dll 0x10001000-0x100016ba (vc5 /O2 /GX). No PDB; names from exports.
// Most functions here are inline members / compiler-generated members of the
// exported classes in MfScExtRoutine.h, emitted by this object.
#include "MfScExtRoutine_unit.h"

MfScBreakpointManager *MfScBreakpointManager::pbrkptmgrMain;   // 0x10034f50

// 0x10001000 ?Manager@MfScBreakpointManager@@SAPAV1@XZ (inline, header)
// 0x10001010 ??0MfScBreakpointManager@@QAE@ABV0@@Z (compiler-generated)
// 0x10001020 ??_EMfScBreakpointManager@@EAEPAXI@Z (compiler-generated)

// 0x10001070 ??0MfScBreakpointManager@@AAE@XZ
MfScBreakpointManager::MfScBreakpointManager()
{
}

// 0x10001080 ??1MfScBreakpointManager@@EAE@XZ
MfScBreakpointManager::~MfScBreakpointManager()
{
}

// 0x10001090 ?Startup@MfScBreakpointManager@@SAXXZ
void MfScBreakpointManager::Startup()
{
    pbrkptmgrMain = new MfScBreakpointManager;
}

// 0x10001100 ?Shutdown@MfScBreakpointManager@@SAXXZ
void MfScBreakpointManager::Shutdown()
{
    if (pbrkptmgrMain)
        delete pbrkptmgrMain;
    pbrkptmgrMain = 0;
}

// 0x10001120 ?Name@MfScData@@QAEAAVMfString@@XZ (inline, header)
// 0x10001130 ?Lock@MfScData@@QAEXXZ (inline, header)
// 0x10001140 ?Unlock@MfScData@@QAEXXZ (inline, header)
// 0x10001150 ??0MfScData@@QAE@ABV0@@Z (compiler-generated)
// 0x10001170 ??4MfScData@@QAEAAV0@ABV0@@Z (compiler-generated)
// 0x10001190 ??_EMfScData@@UAEPAXI@Z (compiler-generated)
// 0x100011e0 ?GetConstant@MfScDataMethod@@QAEPAVMfScDataHandle@@H@Z (inline, header)
// 0x100011f0 ?ConstantPoolSize@MfScDataMethod@@QBEHXZ (inline, header)
// 0x10001200 ??0MfScDataMethod@@QAE@ABV0@@Z (compiler-generated)
// 0x10001240 ??4MfScDataMethod@@QAEAAV0@ABV0@@Z (compiler-generated)
// 0x10001270 ??_EMfScDataMethod@@UAEPAXI@Z (compiler-generated)
// 0x100012c0 ??0MfScFunctionRoutine@@QAE@P6APAVMfScDataHandle@@PAV1@HPAPAV1@@ZH@Z (inline, header)
// 0x100012f0 ??0MfScFunctionRoutine@@QAE@ABV0@@Z (compiler-generated)
// 0x10001340 ??4MfScFunctionRoutine@@QAEAAV0@ABV0@@Z (compiler-generated)
// 0x10001380 ??_FMfScFunctionRoutine@@QAEXXZ (compiler-generated)
// 0x10001390 ??_EMfScFunctionRoutine@@UAEPAXI@Z (compiler-generated)
// 0x100013e0 ??0MfScMethodRoutine@@QAE@P8MfScData@@AEPAVMfScDataHandle@@PAV2@HPAPAV2@@ZH@Z (inline, header)
// 0x10001410 ??0MfScMethodRoutine@@QAE@HP8MfScData@@AEPAVMfScDataHandle@@PAV2@HPAPAV2@@ZH@Z (inline, header)
// 0x10001440 ?SetFunction@MfScFunctionRoutine@@QAEXP6APAVMfScDataHandle@@PAV2@HPAPAV2@@Z@Z (original ?SetFunction@MfScFunctionRoutine@@...) = ?SetMethod@MfScMethodRoutine@@... (inline, header)
// 0x10001450 ?SetOrdinal@MfScMethodRoutine@@QAEXH@Z (inline, header)
// 0x10001460 ??0MfScMethodRoutine@@QAE@ABV0@@Z (compiler-generated)
// 0x100014b0 ??_FMfScMethodRoutine@@QAEXXZ (compiler-generated)
// 0x100014c0 ??_EMfScMethodRoutine@@UAEPAXI@Z (compiler-generated)

// 0x10001510 ??0MfScExtRoutine@@QAE@PAVMfScData@@P81@AEPAVMfScDataHandle@@PAV2@HPAPAV2@@ZH@Z
MfScExtRoutine::MfScExtRoutine(MfScData *object, MfScMethod method, int numArgs)
    : MfScDataMethod(kExtMethod, numArgs), m_method(method), m_object(object), m_ordinal(0)
{
}

// 0x10001540 ??0MfScExtRoutine@@QAE@PAVMfScData@@HP81@AEPAVMfScDataHandle@@PAV2@HPAPAV2@@ZH@Z
MfScExtRoutine::MfScExtRoutine(MfScData *object, int ordinal, MfScMethod method, int numArgs)
    : MfScDataMethod(kMethod, numArgs), m_ordinal(ordinal), m_method(method), m_object(object)
{
}

// 0x10001570 ?Bind@MfScExtRoutine@@QAEXPAVMfScData@@P82@AEPAVMfScDataHandle@@PAV3@HPAPAV3@@Z@Z
void MfScExtRoutine::Bind(MfScData *object, MfScMethod method)
{
    m_object = object;
    m_method = method;
}

// 0x10001590 ?SetOrdinal@MfScExtRoutine@@QAEXH@Z
void MfScExtRoutine::SetOrdinal(int ordinal)
{
    m_ordinal = ordinal;
}

// 0x100015a0 ??0MfScExtRoutine@@QAE@ABV0@@Z (compiler-generated)
// 0x10001600 ??4MfScDebugInfo@@QAEAAV0@ABV0@@Z (compiler-generated)
// 0x10001640 ??1MfScExtRoutine@@UAE@XZ (implicit; = ??1MfScFunctionRoutine, ??1MfScMethodRoutine)
// 0x10001650 ??_EMfScExtRoutine@@UAEPAXI@Z (compiler-generated)
// 0x100016a0 ?GetRefCount@MfScDataHandle@@QBEGXZ (inline, header)
// 0x100016b0 ?GetType@MfScDataHandle@@QBEIXZ (inline, header)
