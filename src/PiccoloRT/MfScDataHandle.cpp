// PiccoloRT.dll 0x100016c0-0x10001aed: no out-of-line code of its own in this
// range; the object emits the inline members of the exported classes declared
// in MfScDataHandle.h, in this order:
// 0x100016c0 ?IsType@MfScDataHandle@@QBEHK@Z
// 0x100016d0 ?SetType@MfScDataHandle@@QAEXK@Z
// 0x100016e0 ?IsReadOnly@MfScDataHandle@@QBEHXZ
// 0x100016f0 ?SetReadOnly@MfScDataHandle@@QAEXXZ
// 0x10001700 ?SetReadWrite@MfScDataHandle@@QAEXXZ
// 0x10001710 ?IsVisiting@MfScDataHandle@@QBEHXZ
// 0x10001720 ?SetVisiting@MfScDataHandle@@QAEXXZ
// 0x10001730 ?ResetVisiting@MfScDataHandle@@QAEXXZ
// 0x10001740 ?GetFloat@MfScDataHandle@@QBEMXZ
// 0x10001750 ?GetNumber@MfScDataHandle@@QBEMXZ
// 0x10001760 ?GetData@MfScDataHandle@@QBEPAVMfScData@@XZ
// 0x10001770 ?AddGCHint@MfScDataHandle@@SAXK@Z
// 0x10001790 ?RemoveGCHint@MfScDataHandle@@SAXK@Z
// 0x100017b0 ?GetGCHints@MfScDataHandle@@SAKXZ
// 0x100017c0 ?DisableGCForVM@MfScDataHandle@@SAXXZ
// 0x100017d0 ?EnableGCForVM@MfScDataHandle@@SAXXZ
// 0x100017e0 ?IsGCRunning@MfScDataHandle@@SAHXZ
// 0x100017f0 ?SignalGCIsRunning@MfScDataHandle@@CAXXZ
// 0x10001800 ?SignalGCIsQuitting@MfScDataHandle@@CAXXZ
// 0x10001810 ?LockGC@MfScDataHandle@@SAXXZ
// 0x10001820 ?UnlockGC@MfScDataHandle@@SAXXZ
// 0x10001830 ??0MfScDataHandle@@AAE@ABV0@@Z (folded with several empty operator=)
// 0x10001840 ?AddRef@MfScDataHandle@@QAEGXZ
// 0x10001850 ?Release@MfScDataHandle@@QAEGXZ
// 0x10001860 ??0MfScVariableStack@@QAE@ABV0@@Z (implicit)
// 0x10001890 ??4MfScFunctionRegistry@@QAEAAV0@ABV0@@Z (implicit)
// 0x100018b0 ??_EMfScVariableStack@@UAEPAXI@Z (compiler-generated, unnamed in the binary)
// 0x10001900 ?PushVarPool@MfScVariableStack@@QAE?AW4MfScVariableErrorCode@1@PAVMfScHandleDict@@@Z (original ?PushVarPool@MfScVariableStack@@...)
// 0x10001930 ?PopVarPool@MfScVariableStack@@QAE?AW4MfScVariableErrorCode@1@XZ (original ?PopVarPool@MfScVariableStack@@...)
// 0x10001950 ?SetVarPoolAtIndex@MfScVariableStack@@QAE?AW4MfScVariableErrorCode@1@HPAVMfScHandleDict@@@Z (original ?SetVarPoolAtIndex@MfScVariableStack@@...)
// 0x10001980 ?GetVarPoolAtIndex@MfScVariableStack@@QAE?AW4MfScVariableErrorCode@1@HPAPAVMfScHandleDict@@@Z (original ?GetVarPoolAtIndex@MfScVariableStack@@...)
// 0x100019b0 ??4MfScVmFrame@@QAEAAV0@ABV0@@Z (implicit)
// 0x100019d0 ?GetSPHandle@MfScVmEnv@@QBEPAVMfScDataHandle@@H@Z
// 0x100019e0 ?SetSPHandle@MfScVmEnv@@QAEXHPAVMfScDataHandle@@@Z
// 0x10001a00 ?ClearScratchREG0@MfScVmEnv@@QAEXXZ
// 0x10001a20 ?ClearScratchREG1@MfScVmEnv@@QAEXXZ
// 0x10001a40 ?ClearScratchREG2@MfScVmEnv@@QAEXXZ
// 0x10001a60 ?ClearScratchREG3@MfScVmEnv@@QAEXXZ
// 0x10001a80 ?ClearScratchRegisters@MfScVmEnv@@QAEXXZ
// 0x10001ae0 ?SetVariableStack@MfScVmEnv@@QAEPAVMfScVariableStack@@PAV2@@Z
#include "MfScDataHandle_unit.h"
