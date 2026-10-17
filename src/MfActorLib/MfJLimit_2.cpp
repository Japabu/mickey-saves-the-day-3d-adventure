// MfActorLib.dll 0x100080c0-0x100085f0: the MfJLimit object. It starts with the
// inline members of all joint-limit classes (MfJLimit.h, exported classes):
// 0x100080c0 ??0MfJLimit@@QAE@XZ
// 0x100080d0 ??0MfJLimit@@QAE@ABV0@@Z
// 0x100080e0 ??1MfJLEuler@@UAE@XZ (original ??1MfJLimit@@UAE@XZ)
// 0x100080f0 ?Duplicate@MfJLimit@@UAEPAV1@XZ
// 0x10008110 ??_EMfJLimit@@UAEPAXI@Z (compiler-generated)
// 0x10002d70 ?PivotLimit@MfJLimit@@UAEXAAVMfRotation3@@@Z (inline; the linker folded the empty functions of several classes into this copy)
// 0x10008160 ?Duplicate@MfJLHinge@@UAEPAVMfJLimit@@XZ
// 0x100081c0 ?GetType@MfJLHinge@@UAEHXZ
// 0x100081d0 ?GetValue@MfJLHinge@@UBEXAAVMfRotation3@@0AAH@Z
// 0x10008220 ??4MfJLHinge@@QAEAAV0@ABV0@@Z (compiler-generated)
// 0x10008270 ??_EMfJLHinge@@UAEPAXI@Z (compiler-generated)
// 0x100082c0 ?Duplicate@MfJLEuler@@UAEPAVMfJLimit@@XZ
// 0x10008320 ?GetType@MfJLEuler@@UAEHXZ
// 0x10008330 ?SetValue@MfJLEuler@@UAEXHMM@Z
// 0x10008350 ?GetValue@MfJLEuler@@UBEXHAAM0@Z
// 0x10008370 ??4MfJLEuler@@QAEAAV0@ABV0@@Z (compiler-generated)
// 0x100083b0 ??_EMfJLEuler@@UAEPAXI@Z (compiler-generated; = ??_EMfJLTranslate)
// 0x10008400 ?Duplicate@MfJLLinear@@UAEPAVMfJLimit@@XZ
// 0x10008460 ?GetType@MfJLLinear@@UAEHXZ
// 0x10008470 ??4MfJLLinear@@QAEAAV0@ABV0@@Z (compiler-generated)
// 0x100084c0 ??_EMfJLLinear@@UAEPAXI@Z (compiler-generated)
// 0x10008510 ?Duplicate@MfJLTranslate@@UAEPAVMfJLimit@@XZ
// 0x10008570 ?GetType@MfJLTranslate@@UAEHXZ
#include "MfJLimit.h"
#include "MfJLHinge.h"
#include "MfJLEuler.h"
#include "MfJLLinear.h"
#include "MfJLTranslate.h"
#include "MfStream.h"
#include "MfStreamConstructorEntry.h"

// 0x10008580 ?fn_10008580@@YAPAVMfStreamObject@@XZ (original fn_10008580:) stream constructor for 'miLJ'
static MfStreamObject *fn_10008580()
{
    return new MfJLimit;
}

// 0x100085a0 _$E10 (jumps to _$E9)
// 0x100085b0 _$E9 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_jlimitEntry('miLJ', fn_10008580);

const long MfJLimit::STREAM_TYPE = 'miLJ';

// 0x100085d0 ?StreamType@MfJLimit@@UBEJXZ
long MfJLimit::StreamType() const
{
    return STREAM_TYPE;
}

// 0x100085e0 ?ReadFromStream@MfJLimit@@UAE?AW4MfStreamError@@AAVMfStream@@@Z (= WriteToStream)
MfStreamError MfJLimit::ReadFromStream(MfStream &s)
{
    return s.Status();
}

MfStreamError MfJLimit::WriteToStream(MfStream &s) const
{
    return s.Status();
}
