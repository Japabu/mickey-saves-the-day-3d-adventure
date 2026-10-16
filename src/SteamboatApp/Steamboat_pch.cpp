// Steamboat_pch.cpp: the unit that builds SteamboatApp's precompiled header. Its object holds only
// the static initializers of the headers: the angle constants (names from the PDB), the colour
// constants and colour masks of the engine's colour header, and the ctype/collate ids of <string>.
// The $E numbers of the original need its exact header set (69 numbers before the first constant),
// so none of them is claimed.
static const double PI = 3.141592653589793;
static const double PIOver2 = PI * 0.5;
static const double PIOver4 = PI * 0.25;
static const double InversePI = 1.0 / PI;
static const double DegreesToRadians = PI * (1.0 / 180.0);
static const double RadiansToDegrees = 180.0 / PI;

#include "../MBEngine/SimpleNode.h"

class MfScDataHandle;

static MfScDataHandle *NULL_VARIABLE = 0;

// Not claimed (static initializers and header instances of this object):
// TODO 0x100a0020 $E297: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E290); its $E number depends on the original header set
// 0x100a0060 _$E85
// TODO 0x100a0080 $E293: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
// 0x100a00a0 _$E11
// 0x100a00c0 _$E1
// 0x100a00f0 _$E14
// 0x100a0110 _$E4
// 0x100a0140 _$E17
// 0x100a0160 _$E7
// 0x100a0190 _$E2
// 0x100a01b0 _$E10
// 0x100a01e0 _$E20
// 0x100a0200 _$E13
// 0x100a0230 _$E23
// 0x100a0250 _$E16
// 0x100a0360 _$E26
// 0x100a0380 _$E19
// 0x100a0490 _$E29
// 0x100a04b0 _$E22
// 0x100a05c0 _$E32
// 0x100a05e0 _$E25
// 0x100a06d0 _$E35
// 0x100a06f0 _$E28
// 0x100a07e0 _$E38
// 0x100a0800 _$E31
// 0x100a08e0 _$E41
// 0x100a0900 _$E34
// 0x100a09e0 _$E44
// 0x100a0a00 _$E37
// 0x100a0ae0 _$E47
// 0x100a0b00 _$E40
// 0x100a0bf0 _$E5
// 0x100a0c10 _$E43
// 0x100a0d00 _$E50
// 0x100a0d20 _$E46
// 0x100a0e00 _$E53
// 0x100a0e20 _$E49
// 0x100a0f00 _$E56
// 0x100a0f20 _$E52
// 0x100a1000 _$E59
// 0x100a1020 _$E55
// 0x100a1100 _$E62
// 0x100a1120 _$E58
// 0x100a1200 _$E8
// 0x100a1220 _$E61
