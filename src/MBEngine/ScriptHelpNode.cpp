#include "ScriptHelpNode.h"

// Keeps a reference to every running script (by ID) so that StopScript can find it and the
// scripts are stopped when the game quits.
#include "../MBEngine/ScriptManager.h"

// 0x10001000 ??0ScriptHelpNode@@QAE@PAVIMBGameEngine@@@Z
ScriptHelpNode::ScriptHelpNode(IMBGameEngine *engine) : SimpleNode(engine, true)
{
}

// 0x10001060 ??1ScriptHelpNode@@UAE@XZ
ScriptHelpNode::~ScriptHelpNode()
{
}

// 0x10001080 ?StartListening@ScriptHelpNode@@MAEXXZ
void ScriptHelpNode::StartListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster) {
        broadcaster->AddListener(-6, this);
        broadcaster->AddListener(-7, this);
    }
    SimpleNode::StartListening();
}

// 0x10001110 ?StopListening@ScriptHelpNode@@MAEXXZ
void ScriptHelpNode::StopListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster) {
        broadcaster->RemoveListener(-6, this);
        broadcaster->RemoveListener(-7, this);
    }
    SimpleNode::StopListening();
}

// 0x100011a0 ?OnStart@ScriptHelpNode@@UAEXXZ
void ScriptHelpNode::OnStart()
{
    SimpleNode::OnStart();
    StartListening();
}

// 0x100011c0 ?OnPause@ScriptHelpNode@@UAEXXZ
void ScriptHelpNode::OnPause()
{
    SimpleNode::OnPause();
    StopListening();
}

// 0x100011e0 ?OnResume@ScriptHelpNode@@UAEXXZ
void ScriptHelpNode::OnResume()
{
    SimpleNode::OnResume();
    StartListening();
}

// 0x10001200 ?OnEnd@ScriptHelpNode@@UAEXXZ
void ScriptHelpNode::OnEnd()
{
    SimpleNode::OnEnd();
    StopListening();
}

// 0x10001220 ?Idle@ScriptHelpNode@@UAEXXZ
void ScriptHelpNode::Idle()
{
    SimpleNode::Idle();
}

// 0x10001230 ?ListenToEvent@ScriptHelpNode@@UAEXJJPAX@Z
void ScriptHelpNode::ListenToEvent(long channel, long event, void *data)
{
    SimpleNode::ListenToEvent(channel, event, data);
    if (channel == -6) {
    } else if (channel == -7) {
        if (event == 5) {
            MBScriptEvent *scriptEvent = (MBScriptEvent *)data;
            MBComPtr<IMBAnim> anim = CreateAnim(scriptEvent->param.startAnim.id);
            if (anim) {
                MBPoint3 position = anim->GetPosition();
                position.x = scriptEvent->param.startAnim.loc.x;
                position.y = scriptEvent->param.startAnim.loc.y;
                anim->Place(position);
                anim->SetCurrentSequence(scriptEvent->param.startAnim.seq);
                anim->Play();
            }
        }
    }
}

static ScriptManager sScriptManager;

// 0x100013e0 ?QueryInterface@ScriptManager@@UAEJABUMBGUID@@PAPAX@Z
long ScriptManager::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (iid == MBIID_IMBEventListener)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x100014b0 ?AddRef@ScriptManager@@UAEKXZ
unsigned long ScriptManager::AddRef()
{
    return ++_mRefCnt;
}

// 0x100014c0 ?Release@ScriptManager@@UAEKXZ
unsigned long ScriptManager::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x100015e0 ?ListenToEvent@ScriptManager@@UAEXJJPAX@Z
void ScriptManager::ListenToEvent(long channel, long event, void *data)
{
    if (channel == -6) {
        switch (event) {
        case 1:
            AddScript((MBScriptEvent *)data);
            break;
        case 6:
            RemoveScript((MBScriptEvent *)data);
            break;
        }
    } else if (channel == -3 && event == 5) {
        MBMultimap<unsigned long, IMBScript *, std::less<unsigned long> >::iterator it = scripts.begin();
        while (it != scripts.end())
            (*it++).second->Stop();
    }
}

// 0x10001900 ?StartScript@ScriptHelpNode@@UAE_NPBD@Z
bool ScriptHelpNode::StartScript(const char *name)
{
    return StartScript(MBHashString(name));
}

// 0x10001930 ?StartScript@ScriptHelpNode@@UAE_NK@Z
bool ScriptHelpNode::StartScript(unsigned long id)
{
    IMBScript *script;
    MBComCreateInstance(0, MBIID_CMBScript, (IMBUnknown **)&script);
    bool loaded = script->Load(id, GetStream(id, 'mbsc'));
    if (loaded) {
        script->Start();
        script->Release();
        return true;
    }
    script->Release();
    return false;
}

// 0x100019b0 ?StopScript@ScriptHelpNode@@UAEXPBD@Z
void ScriptHelpNode::StopScript(const char *name)
{
    StopScript(MBHashString(name));
}

// 0x100019e0 ?StopScript@ScriptHelpNode@@UAEXK@Z
void ScriptHelpNode::StopScript(unsigned long id)
{
    IMBScript *script = sScriptManager.FindScript(id);
    while (script) {
        script->Stop();
        script = sScriptManager.FindScript(id);
    }
}


// Matched by this code:
// 0x100012f0 ??2@YAPAXI@Z
// 0x10001300 ??3@YAXPAX@Z
// 0x10003970 ?AddRef@SimpleNode@@UAEKXZ
// 0x10003980 ?Release@SimpleNode@@UAEKXZ
// 0x10003a80 ?QueryInterface@SimpleNode@@W3AEJABUMBGUID@@PAPAX@Z
// 0x10003a90 ?AddRef@SimpleNode@@W3AEKXZ
// 0x10003aa0 ?Release@SimpleNode@@W3AEKXZ
// 0x10003ab0 ?QueryInterface@SimpleNode@@W7AEJABUMBGUID@@PAPAX@Z
// 0x10003ac0 ?AddRef@SimpleNode@@W7AEKXZ
// 0x10003ad0 ?Release@SimpleNode@@W7AEKXZ
// 0x10001030 ??_GScriptHelpNode@@UAEPAXI@Z
// 0x10001320 _$E71
// 0x10001760 _$E70
// 0x10001770 _$E69
// 0x10001a70 _$E2
// 0x10001a80 _$E1
// 0x10001b50 _$E5
// 0x10001b60 _$E4
// 0x10001c30 _$E8
// 0x10001c40 _$E7
// 0x10001d10 _$E11
// 0x10001d20 _$E10
// 0x10001de0 _$E14
// 0x10001df0 _$E13
// 0x10001eb0 _$E17
// 0x10001ec0 _$E16
// 0x10001f70 _$E20
// 0x10001f80 _$E19
// 0x10002030 _$E23
// 0x10002040 _$E22
// 0x100020f0 _$E26
// 0x10002100 _$E25
// 0x100021c0 _$E29
// 0x100021d0 _$E28
// 0x10002290 _$E32
// 0x100022a0 _$E31
// 0x10002350 _$E35
// 0x10002360 _$E34
// 0x10002410 _$E38
// 0x10002420 _$E37
// 0x100024d0 _$E41
// 0x100024e0 _$E40
// 0x10002590 _$E44
// 0x100025a0 _$E43
// 0x10002650 _$E47
// 0x10002660 _$E46
// 0x10002710 ?erase@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AViterator@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@2@V342@@Z
// 0x10002730 ??Eiterator@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AV012@H@Z
// 0x100027c0 ?CreateObject@?$MBComPtr@VIMBEventManager@@@@QAEJABUMBGUID@@@Z
// 0x10002900 ?begin@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AViterator@12@XZ
// 0x10002910 ?insert@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@_N@2@ABU?$pair@$$CBKPAVIMBScript@@@2@@Z
// 0x10002bb0 ?erase@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x100030a0 ?erase@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10003190 ?find@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AViterator@12@ABK@Z
// 0x100031e0 ?lower_bound@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AViterator@12@ABK@Z
// 0x10003230 ?upper_bound@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE?AViterator@12@ABK@Z
// 0x10003280 ?_Erase@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@IAEXPAU_Node@12@@Z
// 0x100032e0 ?_Insert@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBKPAVIMBScript@@@2@@Z
// 0x100035c0 ?_Lbound@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@IBEPAU_Node@12@ABK@Z
// 0x10003610 ?_Lrotate@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10003670 ?_Rrotate@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@IAEXPAU_Node@12@@Z
// 0x100036d0 ?_Inc@const_iterator@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAEXXZ
// 0x10003760 ?_Buynode@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@IAEPAU_Node@12@PAU312@W4_Redbl@12@@Z
// 0x10003780 ?_Dec@const_iterator@?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAEXXZ
// 0x10003820 ?QueryInterface@SimpleNode@@UAEJABUMBGUID@@PAPAX@Z
// 0x100039a0 ?_QueryInterface@?$MBComPtr@VIMBEventManager@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10003a20 ?_Construct@std@@YAXPAU?$pair@$$CBKPAVIMBScript@@@1@ABU21@@Z
// 0x10003a40 _$E78
// 0x10003a60 _$E77
// 0x10003a70 ?id@?$ctype@G@std@@$E
// TODO 0x10001330 _$E68: sScriptManager construction: the original calls the multimap's _Tree constructor out of line (inline budget); ours inlines it
// TODO 0x10002860 ??0?$_Tree@KU?$pair@$$CBKPAVIMBScript@@@std@@U_Kfn@?$multimap@KPAVIMBScript@@U?$less@K@std@@V?$MBAllocator@PAVIMBScript@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBScript@@@@@std@@QAE@ABU?$less@K@1@_NABV?$MBAllocator@PAVIMBScript@@@@@Z: the multimap _Tree constructor $E68 calls out of line; our compile inlines it into $E68 (inline budget), so it is not emitted
