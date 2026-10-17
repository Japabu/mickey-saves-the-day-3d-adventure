// MfActorLib.dll 0x10010910-0x10010bf0: MfProtoProxDef.
#include "MfProtoProxDef.h"
#include "MfDblList.h"
#include "MfStream.h"
#include "MfStreamConstructorEntry.h"

extern __declspec(dllimport) MfNullString NULL_STRING;

// 0x10010910 ??0MfProtoProxDef@@QAE@XZ
MfProtoProxDef::MfProtoProxDef()
    : fRadius(1.0f), fRadiusSquared(1.0f), fBCWidth(1.0f), fBCLength(1.0f), enabled(0)
{
}

// 0x10010950 ??_EMfProtoProxDef@@UAEPAXI@Z (compiler generated)

// 0x100109a0 ??0MfProtoProxDef@@QAE@ABV0@@Z
MfProtoProxDef::MfProtoProxDef(const MfProtoProxDef &d)
    : protoName(d.protoName), fRadius(d.fRadius), fRadiusSquared(d.fRadiusSquared),
      fBCWidth(d.fBCWidth), fBCLength(d.fBCLength), enabled(d.enabled)
{
}

// 0x100109f0 ??1MfProtoProxDef@@UAE@XZ
MfProtoProxDef::~MfProtoProxDef()
{
}

// 0x10010a00 ?SetToMaximum@MfProtoProxDef@@QAEXPAVMfDblList@@@Z
void MfProtoProxDef::SetToMaximum(MfDblList *list)
{
    fBCLength = 0;
    fBCWidth = 0;
    fRadius = 0;
    protoName = NULL_STRING;
    MfDblListItem *pli;
    for (MfProtoProxDef *d = (MfProtoProxDef *)list->GetFirst(pli); d;
         d = (MfProtoProxDef *)list->GetNext(pli)) {
        if (d->fRadius > fRadius)
            fRadius = d->fRadius;
        if (d->fBCWidth > fBCWidth)
            fBCWidth = d->fBCWidth;
        if (d->fBCLength > fBCLength)
            fBCLength = d->fBCLength;
    }
    fRadiusSquared = fRadius * fRadius;
}

const long MfProtoProxDef::STREAM_TYPE = 'PPrD';

// 0x10010aa0 ?fn_10010aa0@@YAPAVMfStreamObject@@XZ (stream constructor for 'PPrD')
static MfStreamObject *fn_10010aa0()
{
    return new MfProtoProxDef;
}

// 0x10010b00 _$E6 (jumps to _$E5)
// 0x10010b10 _$E5 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_protoProxDefEntry(MfProtoProxDef::STREAM_TYPE, fn_10010aa0);

// 0x10010b30 ?StreamType@MfProtoProxDef@@UBEJXZ
long MfProtoProxDef::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10010b40 ?ReadFromStream@MfProtoProxDef@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoProxDef::ReadFromStream(MfStream &s)
{
    s >> protoName >> fRadius >> fBCWidth >> fBCLength >> enabled;
    fRadiusSquared = fRadius * fRadius;
    return s.Status();
}

// 0x10010ba0 ?WriteToStream@MfProtoProxDef@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoProxDef::WriteToStream(MfStream &s) const
{
    s << protoName << fRadius << fBCWidth << fBCLength << enabled;
    return s.Status();
}
