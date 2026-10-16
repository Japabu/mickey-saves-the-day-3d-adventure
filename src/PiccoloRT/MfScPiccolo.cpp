#include "MfScPiccolo.h"
#include "MfAiSourceResolver.h"
#include "MfScDataHandle.h"
#include "MfScDataObject.h"
#include "MfScSourceResolver.h"
#include "MfString.h"

// 0x10004fd0 ?IsRunning@MfScPiccolo@@SAHXZ
int __cdecl MfScPiccolo::IsRunning()
{
    return bIsRunning;
}

// 0x10004fe0 ?StartRunning@MfScPiccolo@@SAXXZ
void __cdecl MfScPiccolo::StartRunning()
{
    bIsRunning = 1;
}

// 0x10004ff0 ?StopRunning@MfScPiccolo@@SAXXZ
void __cdecl MfScPiccolo::StopRunning()
{
    bIsRunning = 0;
}

// 0x10005020 ?GetDataPath@MfScPiccolo@@SAABVMfString@@XZ
MfString const & __cdecl MfScPiccolo::GetDataPath()
{
    return fsDataPath;
}

// 0x10005000 ?SetDataPath@MfScPiccolo@@SAXABVMfString@@@Z
void __cdecl MfScPiccolo::SetDataPath(MfString const &path)
{
    fsDataPath = path;
}

// 0x10005030 ??0MfScPiccolo@@AAE@XZ (inline, header)

// 0x10005050 ??1MfScPiccolo@@EAE@XZ
MfScPiccolo::~MfScPiccolo()
{
}

// 0x10005060 ??0MfScPiccolo@@QAE@ABV0@@Z (compiler-generated)
// 0x100050a0 ??_EMfScPiccolo@@EAEPAXI@Z (compiler-generated)

// 0x10012780 ?PMloadFunctions@MfScPiccolo@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle * MfScPiccolo::PMloadFunctions(MfScDataHandle *, int, MfScDataHandle * *)
{
    LoadPiccoloDataPathFunctions(1);
    return 0;
}

// 0x10012790 ?PMflushFunctions@MfScPiccolo@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle * MfScPiccolo::PMflushFunctions(MfScDataHandle *, int, MfScDataHandle * *)
{
    FlushPiccoloDataPathFunctions();
    return 0;
}

// 0x100132a0 ?GetSourceResolver@MfScPiccolo@@SAPAVMfAiSourceResolver@@XZ
MfAiSourceResolver * __cdecl MfScPiccolo::GetSourceResolver()
{
    return psrResolver;
}
