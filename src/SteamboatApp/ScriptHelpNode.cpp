// ScriptHelpNode as built into SteamboatApp.dll: the same source as MBEngine's, compiled with
// SteamboatApp's options (/GX).
// The engine source includes "ScriptHelpNode.h", which VC6 finds in this directory first: the
// declarations of the MBEngine unit are included here and the game header is skipped.
#include "../MBEngine/ScriptHelpNode.h"
#define SCRIPTHELPNODE_H
#include "../MBEngine/ScriptHelpNode.cpp"


// Matched by this code:
// 0x10118410 ??0ScriptHelpNode@@QAE@PAVIMBGameEngine@@@Z
// 0x10118470 ??1ScriptHelpNode@@UAE@XZ
// 0x10118610 ?OnStart@ScriptHelpNode@@UAEXXZ
// 0x10118630 ?OnPause@ScriptHelpNode@@UAEXXZ
// 0x10118650 ?OnResume@ScriptHelpNode@@UAEXXZ
// 0x10118670 ?OnEnd@ScriptHelpNode@@UAEXXZ
// 0x10118690 ?Idle@ScriptHelpNode@@UAEXXZ
// 0x10118440 ??_GScriptHelpNode@@UAEPAXI@Z
// 0x10118490 ?StartListening@ScriptHelpNode@@MAEXXZ
// 0x10118550 ?StopListening@ScriptHelpNode@@MAEXXZ
// 0x101186a0 ?ListenToEvent@ScriptHelpNode@@UAEXJJPAX@Z
// 0x101187b0 ??0ScriptManager@@QAE@XZ
// 0x10118890 ?QueryInterface@ScriptManager@@UAEJABUMBGUID@@PAPAX@Z
// 0x10118960 ?AddRef@ScriptManager@@UAEKXZ
// 0x10118970 ?Release@ScriptManager@@UAEKXZ
// 0x10118ac0 ?ListenToEvent@ScriptManager@@UAEXJJPAX@Z
// 0x10118c10 ??1?$MBMultimap@KPAVIMBScript@@U?$less@K@std@@@@QAE@XZ
// 0x10118c90 ?StartScript@ScriptHelpNode@@UAE_NPBD@Z
// 0x10118cc0 ?StartScript@ScriptHelpNode@@UAE_NK@Z
// 0x10118d80 ?StopScript@ScriptHelpNode@@UAEXPBD@Z
// 0x10118db0 ?StopScript@ScriptHelpNode@@UAEXK@Z
// 0x10118e40 ?erase@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AViterator@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@2@V342@@Z
// 0x10118e60 ??Eiterator@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AV012@H@Z
// 0x10118e80 ?insert@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@_N@2@ABU?$pair@$$CBKPAVIMBScript@@@2@@Z
// 0x10118fb0 ?erase@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x101194b0 ?erase@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x101195f0 ?find@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AViterator@12@ABK@Z
// 0x10119640 ?lower_bound@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AViterator@12@ABK@Z
// 0x10119660 ?upper_bound@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AViterator@12@ABK@Z
// 0x10119680 ?_Erase@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10119710 ?_Init@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@IAEXXZ
// 0x10119790 ?_Insert@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBKPAVIMBScript@@@2@@Z
// 0x10119a90 ?_Lbound@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@IBEPAU_Node@12@ABK@Z
// 0x10119ae0 ?_Ubound@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@IBEPAU_Node@12@ABK@Z
// 0x10119b30 ?_Inc@const_iterator@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAEXXZ
// 0x10119be0 ?_Dec@const_iterator@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAEXXZ
// 0x10119ca0 ?_Construct@std@@YAXPAU?$pair@$$CBKPAVIMBScript@@@1@ABU21@@Z
// static initializers (colour constants and ctype ids of the headers; unnamed in the PDB):
// 0x10117770 _$E2
// 0x10117780 _$E1
// 0x10117850 _$E5
// 0x10117860 _$E4
// 0x10117930 _$E8
// 0x10117940 _$E7
// 0x10117a10 _$E11
// 0x10117a20 _$E10
// 0x10117ae0 _$E14
// 0x10117af0 _$E13
// 0x10117bb0 _$E17
// 0x10117bc0 _$E16
// 0x10117c70 _$E20
// 0x10117c80 _$E19
// 0x10117d30 _$E23
// 0x10117d40 _$E22
// 0x10117df0 _$E26
// 0x10117e00 _$E25
// 0x10117ec0 _$E29
// 0x10117ed0 _$E28
// 0x10117f90 _$E32
// 0x10117fa0 _$E31
// 0x10118050 _$E35
// 0x10118060 _$E34
// 0x10118110 _$E38
// 0x10118120 _$E37
// 0x101181d0 _$E41
// 0x101181e0 _$E40
// 0x10118290 _$E44
// 0x101182a0 _$E43
// 0x10118350 _$E47
// 0x10118360 _$E46
// 0x101187a0 _$E68
// 0x10118bf0 _$E70
// 0x10118c00 _$E69

// Not claimed (static initializers and header instances of this object):
// TODO 0x101189a0 ??1ScriptManager@@QAE@XZ: MBEngine source compiled with /GX, 83% similar (CreateObject inlined, the original calls it)
// TODO 0x10119cc0 (unnamed): the ctype guard initializer followed by its atexit stub; our _$E pair has the same code but the function list merges the two (tool)
