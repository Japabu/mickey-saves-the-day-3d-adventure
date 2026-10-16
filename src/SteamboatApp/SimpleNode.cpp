// SimpleNode as built into SteamboatApp.dll: the same source as MBEngine's, compiled with
// SteamboatApp's options (/GX).
// The engine source includes "SimpleNode.h", which VC6 finds in this directory first: the
// declarations of the MBEngine unit are included here and the game header is skipped.
#include "../MBEngine/SimpleNode.h"
#define SIMPLENODE_H
#include "../MBEngine/SimpleNode.cpp"


// Matched by this code:
// (0x1003b2e0 ?AddRef@SimpleNode@@UAEKXZ is the inline copy in SBActivityNode.obj, claimed there)
// 0x10114050 ?OnPause@SimpleNode@@UAEXXZ
// 0x10114070 ?OnResume@SimpleNode@@UAEXXZ
// 0x10114090 ?OnEnd@SimpleNode@@UAEXXZ
// 0x101140a0 ?OnQuit@SimpleNode@@UAEXXZ
// 0x101140b0 ?OnPreRender@SimpleNode@@UAEXV?$MBComPtr@VIMBScreen@@@@@Z
// 0x10114100 ?OnPostRender@SimpleNode@@UAEXV?$MBComPtr@VIMBScreen@@@@@Z
// 0x10114110 ?OnPostFlip@SimpleNode@@UAEXV?$MBComPtr@VIMBScreen@@@@@Z
// 0x10114120 ?OnKeyDown@SimpleNode@@UAEXAAUMBKeyboardEvent@@@Z
// 0x10114130 ?OnKeyUp@SimpleNode@@UAEXAAUMBKeyboardEvent@@@Z
// 0x10114140 ?OnLMouseDown@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
// 0x10114150 ?OnLMouseUp@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
// 0x10114160 ?OnLMouseDblClk@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
// 0x10114170 ?OnMMouseDown@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
// 0x10114180 ?OnMMouseUp@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
// 0x10114190 ?OnMMouseDblClk@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
// 0x101141a0 ?OnRMouseDown@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
// 0x101141b0 ?OnRMouseUp@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
// 0x101141c0 ?OnRMouseDblClk@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
// 0x101141d0 ?OnMouseMove@SimpleNode@@UAEXAAUMBMouseEvent@@@Z
// 0x101141e0 ?OnAnimSeqStarted@SimpleNode@@UAEXKK@Z
// 0x101141f0 ?OnAnimSeqEnded@SimpleNode@@UAEXKK@Z
// 0x10114200 ?OnAnimNotify@SimpleNode@@UAEXKKK@Z
// 0x10114480 ?Idle@SimpleNode@@UAEXXZ
// 0x101144b0 ?ShowCursor@SimpleNode@@UAEXXZ
// 0x10114660 ?SetCursorLocation@SimpleNode@@UAEXVMBPoint@@@Z
// 0x10114cd0 ?CreateFont@SimpleNode@@QAE?AV?$MBComPtr@VIMBFont@@@@PBD@Z
// 0x10114ec0 ?CreateUnicodeFont@SimpleNode@@QAE?AV?$MBComPtr@VIMBFont@@@@PBD@Z
// 0x10115230 ?OnSoundStarted@SimpleNode@@UAEXJK@Z
// 0x10115240 ?OnSoundEnded@SimpleNode@@UAEXJK@Z
// 0x10115580 _wmemcpy
// 0x10116160 ?Startup@SimpleNode@@SAXPAVIMBGameEngine@@@Z
// 0x10116170 ?Shutdown@SimpleNode@@SAXPAVIMBGameEngine@@@Z
// 0x10113950 ??0MBLocale@@QAE@XZ
// 0x10113c30 ??_GSimpleNode@@UAEPAXI@Z
// 0x10113c60 ??1SimpleNode@@UAE@XZ
// 0x10113dc0 ?StartListening@SimpleNode@@MAEXXZ
// 0x10113eb0 ?StopListening@SimpleNode@@MAEXXZ
// 0x10114490 ?HideCursor@SimpleNode@@UAEXXZ
// 0x101144e0 ?PlaceCursor@SimpleNode@@IAEXXZ
// 0x10114590 ?SetCursor@SimpleNode@@UAEXV?$MBComPtr@VIMBAnim@@@@FF@Z
// 0x101146b0 ??1DatabaseMgr@@QAE@XZ
// 0x101147d0 ??0DatabaseMgr@@QAE@XZ
// 0x10114880 ??1?$MBMultimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@@@QAE@XZ
// 0x10114900 ?SetResourceDirectories@SimpleNode@@QAEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@0@Z
// 0x10114920 ?AddDatabase@SimpleNode@@QAEXPBD@Z
// 0x10114940 ?RemoveDatabase@SimpleNode@@QAEXPBD@Z
// 0x10114960 ?GetStream@SimpleNode@@QAE?AV?$MBComPtr@VIMBStream@@@@KK@Z
// 0x10114a50 ?CreateAnim@SimpleNode@@QAE?AV?$MBComPtr@VIMBAnim@@@@PBD@Z
// 0x10114aa0 ?CreateAnim@SimpleNode@@QAE?AV?$MBComPtr@VIMBAnim@@@@J@Z
// 0x10114d00 ?CreateFont@SimpleNode@@QAE?AV?$MBComPtr@VIMBFont@@@@J@Z
// 0x10114ef0 ?CreateUnicodeFont@SimpleNode@@QAE?AV?$MBComPtr@VIMBFont@@@@J@Z
// 0x10115210 ?StartSound@SimpleNode@@QAEXPBD@Z
// 0x10115250 ?GetString@SimpleNode@@QAEXKAAV?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@@Z
// 0x10115270 ?GetNamedString@SimpleNode@@QAEXPBDAAV?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@@Z
// 0x10115360 ?SetLocale@SimpleNode@@QAEXAAVMBLocale@@@Z
// 0x101155b0 ?LoadStringTable@SimpleNode@@UAEXPBD@Z
// 0x10115750 ?ReleaseDBRef@Database@DatabaseMgr@@QAEKXZ
// 0x10115a30 ?AddZipDatabase@DatabaseMgr@@IAE_NPAVIMBFileSpec@@PBD@Z
// 0x10115c70 ?AddDirDatabase@DatabaseMgr@@IAE_NPAVIMBFileSpec@@PBD@Z
// 0x10116070 ?RemoveDatabase@DatabaseMgr@@QAE_NPBD@Z
// 0x10116140 ??1Database@DatabaseMgr@@QAE@XZ
// 0x10116190 ??1?$MBComPtr@VIMBWorld@@@@QAE@XZ
// 0x101161b0 ??1?$MBComPtr@VIMBView@@@@QAE@XZ
// 0x101161d0 ??1?$MBComPtr@VIMBResourceFile@@@@QAE@XZ
// 0x101161f0 ??4?$MBComPtr@VIMBFileSpec@@@@QAEAAV0@PAVIMBFileSpec@@@Z
// 0x10116220 ??1?$MBComPtr@VIMBBitmapFont@@@@QAE@XZ
// 0x10116240 ??1?$MBComPtr@VIMBSystemFont@@@@QAE@XZ
// 0x10116280 ?CreateObject@?$MBComPtr@VIMBMouseMonitor@@@@QAEJABUMBGUID@@@Z
// 0x10116320 ?CreateObject@?$MBComPtr@VIMBResourceFile@@@@QAEJABUMBGUID@@@Z
// 0x101163c0 ?insert@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@_N@2@ABU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@2@@Z
// 0x101164f0 ?erase@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x101169f0 ?erase@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10116b30 ?find@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@QAE?AViterator@12@ABK@Z
// 0x10116b90 ?erase@?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@QAEAAV12@II@Z
// 0x10116ce0 ?_Eos@?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@AAEXI@Z
// 0x10116e30 ?_Erase@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10116ec0 ?_Insert@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@2@@Z
// 0x101171c0 ?_Ubound@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@IBEPAU_Node@12@ABK@Z
// 0x10117210 ?_Inc@const_iterator@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@QAEXXZ
// 0x101172c0 ?_Copy@?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@AAEXI@Z
// 0x10117370 ?_Lbound@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@IBEPAU_Node@12@ABK@Z
// 0x101173c0 ?_Buynode@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@IAEPAU_Node@12@PAU312@W4_Redbl@12@@Z
// 0x101173e0 ?_Dec@const_iterator@?$_Tree@KU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@std@@U_Kfn@?$multimap@KPAVDatabase@DatabaseMgr@@U?$less@K@std@@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@2@U?$less@K@2@V?$MBAllocator@PAVDatabase@DatabaseMgr@@@@@std@@QAEXXZ
// 0x101174a0 ?_QueryInterface@?$MBComPtr@VIMBView@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10117520 ?_QueryInterface@?$MBComPtr@VIMBFont@@@@AAEJABQAVIMBUnknown@@@Z
// 0x101175a0 ?_Construct@std@@YAXPAU?$pair@$$CBKPAVDatabase@DatabaseMgr@@@1@ABU21@@Z
// 0x101175c0 ?_QueryInterface@?$MBComPtr@VIMBBitmapFont@@@@AAEJABV?$MBComPtr@VIMBFont@@@@@Z
// 0x101176c0 ?_QueryInterface@?$MBComPtr@VIMBFile@@@@AAEJABV?$MBComPtr@VIMBResourceFile@@@@@Z

// Template and inline instances this unit also emits:
// 0x10014570 ??1?$MBComPtr@VIMBScreen@@@@QAE@XZ
// 0x100145d0 ?Release@?$MBComPtr@VIMBScreen@@@@QAEXXZ
// 0x10018810 ??1?$MBComPtr@VIMBMouseMonitor@@@@QAE@XZ
// 0x100253d0 ??1?$MBComPtr@VIMBFileSpec@@@@QAE@XZ
// 0x10039920 ?append@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAEAAV12@PBDI@Z
// 0x100b2f40 ??1?$MBComPtr@VIMBFont@@@@QAE@XZ
// 0x100b2ff0 ??1?$MBComPtr@VIMBViewManager@@@@QAE@XZ
// 0x100b3050 ??1?$MBComPtr@VIMBFile@@@@QAE@XZ
// 0x100b3080 ?_Tidy@?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@AAEX_N@Z
// 0x100b36c0 ?Release@?$MBComPtr@VIMBViewManager@@@@QAEXXZ
// 0x10113fa0 ?OnStart@SimpleNode@@UAEXXZ
// 0x101150b0 ?CreateSystemFont@SimpleNode@@QAE?AV?$MBComPtr@VIMBFont@@@@PBDHABVMBColor@@H@Z
// 0x10115e00 ?AddDatabase@DatabaseMgr@@QAE_NPBD@Z
// 0x10116260 ?Release@?$MBComPtr@VIMBView@@@@QAEXXZ
// 0x10117640 ?_QueryInterface@?$MBComPtr@VIMBSystemFont@@@@AAEJABV?$MBComPtr@VIMBFont@@@@@Z
// static initializers (colour constants, GUID-less statics and ctype ids of the headers; unnamed in the PDB):
// 0x10112c90 _$E2
// 0x10112ca0 _$E1
// 0x10112d70 _$E5
// 0x10112d80 _$E4
// 0x10112e50 _$E8
// 0x10112e60 _$E7
// 0x10112f30 _$E11
// 0x10112f40 _$E10
// 0x10113000 _$E14
// 0x10113010 _$E13
// 0x101130d0 _$E17
// 0x101130e0 _$E16
// 0x10113190 _$E20
// 0x101131a0 _$E19
// 0x10113250 _$E23
// 0x10113260 _$E22
// 0x10113310 _$E26
// 0x10113320 _$E25
// 0x101133e0 _$E29
// 0x101133f0 _$E28
// 0x101134b0 _$E32
// 0x101134c0 _$E31
// 0x10113570 _$E35
// 0x10113580 _$E34
// 0x10113630 _$E38
// 0x10113640 _$E37
// 0x101136f0 _$E41
// 0x10113700 _$E40
// 0x101137b0 _$E44
// 0x101137c0 _$E43
// 0x10113870 _$E47
// 0x10113880 _$E46
// 0x10113940 _$E66
// 0x10113980 _$E69
// 0x10113990 _$E67
// 0x10114680 _$E74
// 0x10114690 _$E76
// 0x101146a0 _$E75

// Not claimed (static initializers and header instances of this object):
// TODO 0x101139b0 ??0SimpleNode@@QAE@PAVIMBGameEngine@@_N@Z: MBEngine source compiled with /GX, 82% similar (VC6 inlines differently: the inline budget)
// TODO 0x10114210 ?ListenToEvent@SimpleNode@@UAEXJJPAX@Z: MBEngine source compiled with /GX, 80% similar (VC6 inlines differently: the inline budget)
// TODO 0x10114c10 ?CloneAnim@SimpleNode@@QAE?AV?$MBComPtr@VIMBAnim@@@@ABV2@@Z: MBEngine source compiled with /GX, 92% similar (VC6 inlines differently: the inline budget)
// TODO 0x10115670 ??0Database@DatabaseMgr@@QAE@PAVIMBResourceFile@@@Z: MBEngine source compiled with /GX, 92% similar (VC6 inlines differently: the inline budget)
// TODO 0x10115810 ?Initialize@DatabaseMgr@@QAE_NABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@0@Z: MBEngine source compiled with /GX, 63% similar (VC6 inlines differently: the inline budget)
// TODO 0x10116b80 ?AddRef@?$MBComPtr@VIMBFileSpec@@@@QAEXXZ: header/template instance that our object does not emit (VC6 inlines it or another unit provides it)
// TODO 0x10116d00 ?_Grow@?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@AAE_NI_N@Z: header/template instance, 53% similar
// TODO 0x10117740 (unnamed): the ctype guard initializer followed by its atexit stub; our _$E pair has the same code but the function list merges the two (tool)
