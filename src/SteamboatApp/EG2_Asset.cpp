#include "EG2_Asset_decl.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x10112c00 ??0Asset@EG2@@IAE@XZ
EG2::Asset::Asset()
    : mInstanceCount(0)
{
}

// 0x10112c30 ??1Asset@EG2@@MAE@XZ
EG2::Asset::~Asset()
{
}


// Matched by this code:
// 0x10112c10 ??_GAsset@EG2@@MAEPAXI@Z

// 0x10112c40 _$E24
// 0x10112c70 _$E20
// 0x10112c80 _$E23
