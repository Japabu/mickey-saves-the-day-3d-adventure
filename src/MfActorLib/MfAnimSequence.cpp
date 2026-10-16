// MfActorLib.dll 0x10002230-0x100023c0: MfAnimSequence (inline members first)
#include "MfAnimSequence_unit.h"

// Inline members defined in MfAnimSequence.h, emitted here:
// 0x10002230 ?GetPath@MfAnimSequence@@QAEABVMfString@@XZ
// 0x10002240 ?SetTimeStep@MfAnimSequence@@QAEXK@Z
// 0x10002250 ?NextFrame@MfAnimSequence@@QAEHXZ
// 0x10002260 ?GetNumFrames@MfAnimSequence@@QBEHXZ
// 0x10002270 ?GetTimeStep@MfAnimSequence@@QBEKXZ
// 0x10002280 ?ColSpheres@MfProtoNode@@QAEAAVMfDblList@@XZ (shared with MfProtoNode::ColSpheres)
// 0x10002290 ?GetAddActorEvents@MfAnimSequence@@QBEABVMfAnimEventList@@XZ
// 0x100022a0 ?GetRemoveActorEvents@MfAnimSequence@@QBEABVMfAnimEventList@@XZ
// 0x100022b0 ?GetSoundEvents@MfAnimSequence@@QBEABVMfAnimEventList@@XZ
// 0x100022c0 ?GetMaterialEvents@MfAnimSequence@@QBEABVMfAnimEventList@@XZ

// 0x100022d0 ?ClassTypeInFile@MfAnimSequence@@CA?AVMfFileType@@XZ
MfFileType MfAnimSequence::ClassTypeInFile()
{
    return MfFileType(MfString("Sequ"));
}

// 0x10002300 ??0MfTypedValue@@QAE@XZ (inline)
// 0x10002320 ??1MfColor@@UAE@XZ (inline; shared with MfColor, MfFloatValue, MfIntValue, ...)
// 0x10002330 ?Compare@MfTypedValue@@UAEHABV1@@Z (inline)
// 0x10002340 ??0MfTypedValue@@QAE@ABV0@@Z (compiler-generated)
// 0x10002370 ??_EMfTypedValue@@UAEPAXI@Z (compiler-generated)
