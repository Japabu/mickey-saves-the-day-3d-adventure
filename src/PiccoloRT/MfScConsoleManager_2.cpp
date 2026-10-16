// PiccoloRT.dll 0x10004500-0x10004f07 (vc5 /O2 /GX)
#include "MfScConsoleManager_2_unit.h"

// 0x10004500 ?Console_Print@MfScConsoleManager@@CAPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScConsoleManager::Console_Print(MfScDataHandle *self, int argc, MfScDataHandle **argv)
{
    MfString out((MfStringSize(256)));
    MfString item;
    for (int i = 0; i < argc; i++) {
        if (argv[i])
            argv[i]->InfoShort(&item);
        else
            item = "NIL";
        out << item;
    }
    if (argc > 0)
        Manager()->PrintString(out);
    return 0;
}

// 0x100045e0 ?Console_PrintNl@MfScConsoleManager@@CAPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScConsoleManager::Console_PrintNl(MfScDataHandle *self, int argc, MfScDataHandle **argv)
{
    MfString out((MfStringSize(256)));
    MfString item;
    for (int i = 0; i < argc; i++) {
        if (argv[i])
            argv[i]->InfoShort(&item);
        else
            item = "NIL";
        out << item;
    }
    if (argc > 0)
        Manager()->PrintStringNl(out);
    return 0;
}

// 0x100046c0 ?Console_Clear@MfScConsoleManager@@CAPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScConsoleManager::Console_Clear(MfScDataHandle *self, int argc, MfScDataHandle **argv)
{
    Manager()->ClearConsole();
    return 0;
}

// 0x100046d0 ??0MfScLinkData@@QAE@XZ (inline)
// 0x100046e0 ??1MfScLinkData@@UAE@XZ (inline)
// 0x100046f0 ??0MfScLinkData@@QAE@ABV0@@Z (implicit)
// 0x10004710 ??_EMfScLinkData@@UAEPAXI@Z (implicit)

// 0x10004760 ??0MfScData@@QAE@XZ
MfScData::MfScData()
{
    m_name = new MfString;
}

// 0x100047e0 ??1MfScData@@UAE@XZ
MfScData::~MfScData()
{
    delete m_name;
}

// 0x10004840 ?GetClassNameA@MfScDataArray@@MAE?AVMfString@@XZ
MfString MfScDataArray::GetClassName()
{
    return MfString("Array");
}

// 0x10004870 ??0MfScDataArray@@QAE@ABV0@@Z (implicit)
// 0x100048b0 ??4MfScDataArray@@QAEAAV0@ABV0@@Z (implicit)
// 0x100048e0 ??_EMfScDataArray@@UAEPAXI@Z (implicit)

// 0x10004930 ??0MfScDataArray@@IAE@H@Z
MfScDataArray::MfScDataArray(int size) : MfScDataObject(1), m_size(size)
{
}

// 0x10004950 ??1MfScDataArray@@UAE@XZ
MfScDataArray::~MfScDataArray()
{
}

// 0x10004960 ?GetProperty@MfScDataArray@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfScDataArray::GetProperty(const MfString &name, MfScDataHandle **value)
{
    *value = 0;
    if (name == "size") {
        *value = MfScDataHandle::CreateData(m_size);
        return kPropOk;
    }
    return kPropNotFound;
}

// 0x100049a0 ?SetProperty@MfScDataArray@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfScDataArray::SetProperty(MfScDataHandle *self, const MfString &name,
                                                               MfScDataHandle *value, int create)
{
    if (create && value)
        value->Release();
    return kPropReadOnly;
}

// 0x100049d0 ??0MfScDataArrayImp@@QAE@H@Z
MfScDataArrayImp::MfScDataArrayImp(int size) : MfScDataArray(size), m_items(0)
{
    m_items = new MfScDataHandle *[size];
    for (int i = 0; i < m_size; i++)
        m_items[i] = 0;
}

// 0x10004a60 ?IsValid@MfScDataArrayImp@@UAEHXZ
int MfScDataArrayImp::IsValid()
{
    return 1;
}

// 0x10004a70 ??_GMfScDataArrayImp@@UAEPAXI@Z (implicit)

// 0x10004a90 ??1MfScDataArrayImp@@UAE@XZ
MfScDataArrayImp::~MfScDataArrayImp()
{
    if (m_items) {
        for (int i = 0; i < m_size; i++) {
            if (m_items[i])
                m_items[i]->Release();
        }
        delete[] m_items;
    }
}

// 0x10004b20 ?SetAt@MfScDataArrayImp@@UAEHHPAVMfScDataHandle@@H@Z
int MfScDataArrayImp::SetAt(int index, MfScDataHandle *value, int noAddRef)
{
    if (index < 0 || index >= m_size)
        return 0;
    if (m_items[index])
        m_items[index]->Release();
    if (!noAddRef && value)
        value->AddRef();
    m_items[index] = value;
    return 1;
}

// 0x10004b80 ?GetAt@MfScDataArrayImp@@UAEHHPAPAVMfScDataHandle@@H@Z
int MfScDataArrayImp::GetAt(int index, MfScDataHandle **value, int addRef)
{
    if (index < 0 || index >= m_size)
        return 0;
    *value = m_items[index];
    if (addRef == 1 && *value)
        (*value)->AddRef();
    return 1;
}

// 0x10004bc0 ?CountCircLinks@MfScDataArrayImp@@UAEPAVMfScLinkData@@PAVMfScDataHandle@@@Z
MfScLinkData *MfScDataArrayImp::CountCircLinks(MfScDataHandle *h)
{
    MfScListLinkData<int> *links = new MfScListLinkData<int>;
    int count = 0;
    for (int i = 0; i < m_size; i++) {
        if (m_items[i] == h) {
            count++;
            links->Add(i);
        } else if (m_items[i] && m_items[i]->IsType(0x7c0)) {
            MfScData *data = m_items[i]->GetData();
            m_items[i]->SetVisiting();
            int n = data->GetThroughCount(h);
            if (n) {
                count += n;
                links->Add(i);
            }
            m_items[i]->ResetVisiting();
        }
    }
    links->m_count = count;
    return links;
}

// 0x10004cb0 ?ReleaseCircLinks@MfScDataArrayImp@@UAEXPAVMfScLinkData@@@Z
void MfScDataArrayImp::ReleaseCircLinks(MfScLinkData *l)
{
    for (MfScListLinkData<int>::Item *item = ((MfScListLinkData<int> *)l)->m_head; item; item = item->m_next) {
        if (m_items[item->m_value]) {
            m_items[item->m_value]->Release();
            m_items[item->m_value] = 0;
        }
    }
}

// 0x10004cf0 ?GetThroughCount@MfScDataArrayImp@@UAEHPAVMfScDataHandle@@@Z
int MfScDataArrayImp::GetThroughCount(MfScDataHandle *h)
{
    int count = 0;
    for (int i = 0; i < m_size; i++) {
        if (m_items[i] == h) {
            count++;
        } else if (m_items[i] && m_items[i]->IsType(0x7c0) && !m_items[i]->IsVisiting()) {
            MfScData *data = m_items[i]->GetData();
            m_items[i]->SetVisiting();
            count += data->GetThroughCount(h);
            m_items[i]->ResetVisiting();
        }
    }
    return count;
}

// 0x10004d60 ??_G?$MfScListLinkData@H@@UAEPAXI@Z (implicit)
// 0x10004d80 ??1?$MfScListLinkData@H@@UAE@XZ (inline)

// 0x10004de0 ??0MfScDebugger@@QAE@XZ (implicit)
// 0x10004df0 ??1MfScDebugger@@UAE@XZ (inline)

MfAiPdbDebugger *MfScDebugger::pdbgTheDebugger = 0;

// 0x10004e00 ?Debugger@MfScDebugger@@SAPAVMfAiPdbDebugger@@XZ
MfAiPdbDebugger *MfScDebugger::Debugger()
{
    return pdbgTheDebugger;
}

// 0x10004e10 ?InitDebuggerPointer@MfScDebugger@@SAXPAVMfAiPdbDebugger@@@Z
void MfScDebugger::InitDebuggerPointer(MfAiPdbDebugger *dbg)
{
    pdbgTheDebugger = dbg;
}

// 0x10004e20 ?ClearDebuggerPointer@MfScDebugger@@SAXXZ
void MfScDebugger::ClearDebuggerPointer()
{
    pdbgTheDebugger = 0;
}

// 0x10004e30 ??0MfScDebugger@@QAE@ABV0@@Z (implicit)
// 0x10004e40 ??_EMfScDebugger@@UAEPAXI@Z (implicit)
// 0x10004e90 ??0MfScDiagnosticInfo@@QAE@XZ (inline)
// 0x10004eb0 ??1MfScDiagnosticInfo@@QAE@XZ (implicit)
// 0x10004ec0 ??0MfScDiagnosticInfo@@QAE@ABV0@@Z (implicit)
// 0x10004ee0 ??4MfScDiagnosticInfo@@QAEAAV0@ABV0@@Z (implicit)
// 0x10004f00 ??1MfScDiagnosticCallBack@@UAE@XZ (inline)


// Matched by this code:
// mfactorlib.dll 0x100108c0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// mfutil.dll 0x100068d0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// mfutil.dll 0x10008280 ??_GMfScDataArrayImp@@UAEPAXI@Z
// mfutil.dll 0x10008c80 ??_GMfScDataArrayImp@@UAEPAXI@Z
// mfutil.dll 0x10009da0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// mfutil.dll 0x1000a920 ??_GMfScDataArrayImp@@UAEPAXI@Z
// mfutil.dll 0x10010d80 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccolocomp.dll 0x10015700 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccolocomp.dll 0x10015780 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10006a20 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10007890 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10007a70 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10007b20 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10007cb0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10007d40 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10008270 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10008850 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10009b90 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10009c60 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10009c80 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000c0f0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000c140 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000c9f0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000cb60 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000ddb0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000e7c0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000e9c0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000e9e0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000f7f0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000f840 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000f890 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000f8e0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000f930 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000f9b0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000fa50 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000fb20 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000fb80 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000fc00 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000fc60 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1000feb0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10010310 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10010400 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x100105c0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x100105e0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10010de0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10011030 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x100114d0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10012b30 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10012c10 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10013480 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x100139a0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x100199d0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10019b40 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x10019c70 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1001a060 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1001a940 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1001aae0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// piccoloconsole.dll 0x1001bbe0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// 0x10006130 ??_GMfScDataArrayImp@@UAEPAXI@Z
// 0x10006150 ??_GMfScDataArrayImp@@UAEPAXI@Z
// 0x10006170 ??_GMfScDataArrayImp@@UAEPAXI@Z
// 0x100086e0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// 0x100088b0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// 0x10009880 ??_GMfScDataArrayImp@@UAEPAXI@Z
// 0x1000c060 ??_GMfScDataArrayImp@@UAEPAXI@Z
// 0x1000c3e0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// 0x1000fe10 ??_GMfScDataArrayImp@@UAEPAXI@Z
// 0x10012e60 ??_GMfScDataArrayImp@@UAEPAXI@Z
// 0x10016820 ??_GMfScDataArrayImp@@UAEPAXI@Z
// 0x1001c1a0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// 0x1001c480 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/psccontentinfo.dll 0x10001450 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/psccontentinfo.dll 0x10002d80 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/psccontentinfo.dll 0x10002ec0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/psccontentinfo.dll 0x10003ab0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/psccontentinfo.dll 0x10005770 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10001060 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x100011f0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10001300 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10003090 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x100059c0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10005c30 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10006a30 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10006c70 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10006c90 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10006cd0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10006ef0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10007000 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x100070a0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x100075e0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10007b60 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10007c20 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10007cb0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10008180 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10008640 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10009820 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x100099d0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x1000ae50 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x1000aec0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x1000b430 ??_GMfScDataArrayImp@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x1000b920 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x100047f0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x100197b0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1001c3c0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1001c830 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1001e1c0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10020d90 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1002ff60 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1004c320 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1004f9d0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10053290 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10053a80 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10053da0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10054140 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10055da0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10055fd0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10056180 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10058b60 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10063d60 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10063f60 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10064190 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x100643d0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10064780 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10064890 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x100696f0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10069e00 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1006a000 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1006a3d0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1006a780 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1006ab40 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1006ad30 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1006b1b0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1006b4f0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1006b6d0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1006b8e0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1006bb30 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1006bd10 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1006bda0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1006c330 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1006cdf0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1006f7f0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10070ea0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10071c30 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10078ed0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1007bd50 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1007dc70 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1007e010 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1007fa80 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1007fca0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1007fe90 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x100822c0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10082780 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10082990 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10082d60 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10083720 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x100846b0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10084ea0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10085380 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10085610 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10085c30 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10086330 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10086950 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10086fd0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10087690 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10087c40 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10087e10 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10088010 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10089110 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x100896d0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10089a30 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10089db0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1008a1a0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1008a560 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1008a5c0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1008a6f0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1008a8b0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1008aa90 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1008b390 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1008be50 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1008c000 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1008efd0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x10090ba0 ??_GMfScDataArrayImp@@UAEPAXI@Z
// runtimelib.dll 0x1009b3b0 ??_GMfScDataArrayImp@@UAEPAXI@Z