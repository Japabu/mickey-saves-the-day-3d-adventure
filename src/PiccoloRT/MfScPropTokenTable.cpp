// PiccoloRT.dll 0x100074c0-0x1000777f. The object starts with the inline
// members of MfScPropToken (exported class, so they are emitted here):
// 0x100074c0 ??0MfScPropToken@@QAE@ABVMfString@@H@Z (inline, header)
// 0x100074e0 ??1MfScPropToken@@QAE@XZ (inline, header)
// 0x100074f0 ??0MfScPropToken@@QAE@ABV0@@Z (compiler-generated)
// 0x10007510 ??4MfScPropToken@@QAEAAV0@ABV0@@Z (compiler-generated)
#include "MfScPropTokenTable.h"
#include "MfScPropToken.h"
#include "MfString.h"

// 0x10007530 ??0MfScPropTokenTable@@QAE@XZ (inline, header)

// 0x10007560 ??1MfScPropTokenTable@@UAE@XZ
MfScPropTokenTable::~MfScPropTokenTable()
{
    MfDictionary::Clear(this);
}

// 0x100075e0 ?Add@MfScPropTokenTable@@QAEXPAVMfScPropToken@@@Z
void MfScPropTokenTable::Add(MfScPropToken *token)
{
    MfDictionary::Add(token->Property(), token);
}

// 0x10007600 ?Add@MfScPropTokenTable@@QAEXABVMfString@@H@Z (inline, header)

// 0x10007680 ?Find@MfScPropTokenTable@@QAEPAVMfScPropToken@@ABVMfString@@@Z (inline, header)

// 0x10007690 ?Delete@MfScPropTokenTable@@QAEXABVMfString@@@Z
void MfScPropTokenTable::Delete(MfString const &property)
{
    MfScPropToken *token = (MfScPropToken *)MfDictionary::Delete(property);
    if (token)
        delete token;
}

// 0x100076c0 ?Clear@MfScPropTokenTable@@QAEXXZ
void MfScPropTokenTable::Clear()
{
    MfDictionary::Clear(this);
}

// 0x100076e0 ??0MfScPropTokenTable@@QAE@ABV0@@Z (compiler-generated)
// 0x10007730 ??_EMfScPropTokenTable@@UAEPAXI@Z (compiler-generated)


// Matched by this code:
// 0x10008500 ??_EMfScPropTokenTable@@WBE@AEPAXI@Z