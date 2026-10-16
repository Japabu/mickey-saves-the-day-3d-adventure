// PscContentInfo.dll 0x100049f0-0x10006520: the script object "Content"
// (PcContent) and the conversion of a content description to and from a
// script object.
#include "PscContentInfo.h"

// 0x100049f0 ?ToObject@MfContentDescription@@SAPAVMfScDataHandle@@ABU1@@Z
MfScDataHandle *MfContentDescription::ToObject(const MfContentDescription &desc)
{
    MfScDataObject *obj = new MfScDataObject(0x400);
    MfScDataHandle *h = MfScDataHandle::CreateData(obj);
    obj->SetProperty(h, "name", MfScDataHandle::CreateData(desc.name, 0), 1);
    obj->SetProperty(h, "vendor", MfScDataHandle::CreateData(desc.vendor, 0), 1);
    obj->SetProperty(h, "description", MfScDataHandle::CreateData(desc.description, 0), 1);
    obj->SetProperty(h, "version", MfScDataHandle::CreateData(desc.version, 0), 1);
    obj->SetProperty(h, "defaultSpace", MfScDataHandle::CreateData(desc.defaultSpace, 0), 1);
    obj->SetProperty(h, "contentPath", MfScDataHandle::CreateData(desc.path, 0), 1);
    return h;
}

// 0x10004c20 ??_GMfScDataObject@@UAEPAXI@Z

// 0x10004c40 ?FromObject@MfContentDescription@@QAEHPAVMfScDataHandle@@@Z
int MfContentDescription::FromObject(MfScDataHandle *h)
{
    MfScDataObject *obj = h ? dynamic_cast<MfScDataObject *>(h->GetData()) : 0;
    if (!obj)
        return 0;
    MfScObjString *s;
    if (obj->GetProperty("name", &h) == MfScDataObject::kPropertyOk) {
        s = h ? dynamic_cast<MfScObjString *>(h->GetData()) : 0;
        if (!s)
            return 0;
        name = *s;
    }
    if (obj->GetProperty("vendor", &h) == MfScDataObject::kPropertyOk) {
        s = h ? dynamic_cast<MfScObjString *>(h->GetData()) : 0;
        if (!s)
            return 0;
        vendor = *s;
    }
    if (obj->GetProperty("description", &h) == MfScDataObject::kPropertyOk) {
        s = h ? dynamic_cast<MfScObjString *>(h->GetData()) : 0;
        if (!s)
            return 0;
        description = *s;
    }
    if (obj->GetProperty("version", &h) == MfScDataObject::kPropertyOk) {
        s = h ? dynamic_cast<MfScObjString *>(h->GetData()) : 0;
        if (!s)
            return 0;
        version = *s;
    }
    if (obj->GetProperty("defaultSpace", &h) == MfScDataObject::kPropertyOk) {
        s = h ? dynamic_cast<MfScObjString *>(h->GetData()) : 0;
        if (!s)
            return 0;
        defaultSpace = *s;
    }
    return 1;
}

// 0x10004f70 _$E24
// 0x10004f80 _$E21
// 0x10004fa0 _$E23
// 0x10004fb0 _$E22
// 0x1000cb08
MfString g_contentName("Content");
// 0x1000cb0c
static PcContent *g_content;

// 0x10004fc0 ?GetClassNameA@PcContent@@UAE?AVMfString@@XZ
MfString PcContent::GetClassNameA()
{
    return g_contentName;
}

// 0x10004ff0 ??0PcContent@@QAE@VPcContentArg@@@Z
PcContent::PcContent(PcContentArg)
    : m_content(0)
{
    *m_className = g_contentName + MfString("<class>");
    MfScClassInfo *ci = new MfScClassInfo(g_contentName);
    {
        MfString full("Content::ChangeContentFolder");
        MfString name("ChangeContentFolder");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&PcContent::ChangeContentFolder, full, 1), 1);
    }
    *ci + &(*new MfScCiMethod("ChangeContentFolder") + new MfScMethodParameterInfo("String contentName", 0));
    {
        MfString full("Content::GetCurrentInfo");
        MfString name("GetCurrentInfo");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&PcContent::GetCurrentInfo, full, 0), 1);
    }
    *ci + new MfScCiMethod("GetCurrentInfo");
    {
        MfString full("Content::GetInfo");
        MfString name("GetInfo");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&PcContent::GetInfo, full, 1), 1);
    }
    *ci + &(*new MfScCiMethod("GetInfo") + new MfScMethodParameterInfo("String contentName", 0));
    {
        MfString full("Content::CurrentContentFolder");
        MfString name("CurrentContentFolder");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&PcContent::CurrentContentFolder, full, 0), 1);
    }
    {
        MfString full("Content::PickContent");
        MfString name("PickContent");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&PcContent::PickContent, full, 0), 1);
    }
    {
        MfString full("Content::ChangeContentFolderToPath");
        MfString name("ChangeContentFolderToPath");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&PcContent::ChangeContentFolderToPath, full, 1), 1);
    }
    {
        MfString full("Content::SaveContentInfoToPath");
        MfString name("SaveContentInfoToPath");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&PcContent::SaveContentInfoToPath, full, 3), 1);
    }
    {
        MfString full("Content::CreateContentSubFolders");
        MfString name("CreateContentSubFolders");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&PcContent::CreateContentSubFolders, full, 0), 1);
    }
    {
        MfString full("Content::GetContentNames");
        MfString name("GetContentNames");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&PcContent::GetContentNames, full, 0), 1);
    }
    {
        MfString full("Content::Refresh");
        MfString name("Refresh");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&PcContent::Refresh, full, 0), 1);
    }
    MfScDataObject::AddClassInfo(ci);
    m_content = new MfContent;
    MfInterfaceRegistry::AddInterface(m_content);
}

// 0x10005760 ??1PcContentArg@@QAE@XZ
// 0x10005770 ??_GPcContent@@UAEPAXI@Z
// 0x10005790 ??_GMfScClassInfo@@UAEPAXI@Z
// 0x100057b0 ??_GMfScCiMethod@@UAEPAXI@Z
// 0x100057d0 ??_GMfScMethodParameterInfo@@UAEPAXI@Z

// 0x100057f0 ??1PcContent@@UAE@XZ
PcContent::~PcContent()
{
    char name[256];
    int version;
    m_content->GetInterfaceName(name, 255, &version, &version);
    MfInterfaceRegistry::RemoveInterface(MfString(name));
    m_content->Release();
}

// 0x100058b0 ?CreateContentObject@@YAXXZ
void CreateContentObject()
{
    if (!g_content) {
        PcContentArg arg;
        g_content = new PcContent(arg);
        MfScDataHandle *h = MfScDataHandle::CreateData(g_content);
        h->m_flags |= 8;
        MfScVmEnv::SetGlobalVariable(g_contentName, h, 1, 0);
    }
}

// 0x10005940 ?DestroyContentObject@@YAXXZ
void DestroyContentObject()
{
    if (g_content) {
        MfScDataHandle *h;
        MfScVmEnv::GetGlobalVariable(g_contentName, &h);
        if (h) {
            h->m_flags &= ~8;
            MfScVmEnv::SetGlobalVariable(g_contentName, 0, 1, 0);
            h->Release();
        }
        g_content = 0;
    }
}

// 0x100059a0 ?ChangeContentFolder@PcContent@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *PcContent::ChangeContentFolder(MfScDataHandle *, int, MfScDataHandle **args)
{
    if (!args[0]) {
        MfScVmEnv *env = MfThread::GetThread()->m_env;
        MfString msg;
        msg << "Parameter number " << 0 << " must not be nil";
        env->ThrowException(6, msg);
        return 0;
    }
    if (!args[0]->IsType(0x100)) {
        MfScVmEnv *env = MfThread::GetThread()->m_env;
        MfString msg;
        msg << "Parameter number " << 0 << " must be a string.";
        env->ThrowException(11, msg);
        return 0;
    }
    MfString *name = (MfScObjString *)args[0]->GetData();
    return MfScDataHandle::CreateData(m_content->SelectContent(*name));
}

// 0x10005b00 ?GetCurrentInfo@PcContent@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *PcContent::GetCurrentInfo(MfScDataHandle *, int, MfScDataHandle **)
{
    MfScDataHandle *result = 0;
    MfContentDescription desc;
    if (m_content->GetCurrentContent(&desc))
        result = MfContentDescription::ToObject(desc);
    return result;
}

// 0x10005bf0 ?GetInfo@PcContent@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *PcContent::GetInfo(MfScDataHandle *, int, MfScDataHandle **args)
{
    MfScObjString *str = 0;
    MfScDataHandle *result = 0;
    if (!args[0]) {
        MfScVmEnv *env = MfThread::GetThread()->m_env;
        MfString msg;
        msg << "Parameter number " << 0 << " must not be nil";
        env->ThrowException(6, msg);
        return 0;
    }
    if (!args[0]->IsType(0x100)) {
        MfScVmEnv *env = MfThread::GetThread()->m_env;
        MfString msg;
        msg << "Parameter number " << 0 << " must be a string.";
        env->ThrowException(11, msg);
        return 0;
    }
    str = (MfScObjString *)args[0]->GetData();
    MfContentDescription desc;
    if (m_content->GetContent(*(MfString *)str, &desc))
        result = MfContentDescription::ToObject(desc);
    return result;
}

// 0x10005de0 ?CurrentContentFolder@PcContent@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *PcContent::CurrentContentFolder(MfScDataHandle *, int, MfScDataHandle **)
{
    return MfScDataHandle::CreateData(m_content->GetDataPath(), 0);
}

// EQUIV 0x10005e40 ?PickContent@PcContent@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: the null test compares with a register known to hold 0 instead of testing the value itself
MfScDataHandle *PcContent::PickContent(MfScDataHandle *, int, MfScDataHandle **)
{
    MfScDataHandle *result = 0;
    MfString name;
    if (m_content->ChooseContent(0, &name))
        result = MfScDataHandle::CreateData(name, 0);
    return result;
}

// 0x10005ec0 ?ChangeContentFolderToPath@PcContent@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *PcContent::ChangeContentFolderToPath(MfScDataHandle *, int, MfScDataHandle **args)
{
    if (!args[0]) {
        MfScVmEnv *env = MfThread::GetThread()->m_env;
        MfString msg;
        msg << "Parameter number " << 0 << " must not be nil";
        env->ThrowException(6, msg);
        return 0;
    }
    if (!args[0]->IsType(0x100)) {
        MfScVmEnv *env = MfThread::GetThread()->m_env;
        MfString msg;
        msg << "Parameter number " << 0 << " must be a string.";
        env->ThrowException(11, msg);
        return 0;
    }
    MfString *path = (MfScObjString *)args[0]->GetData();
    return MfScDataHandle::CreateData(m_content->SetContentPath(*path));
}

// 0x10006020 ?SaveContentInfoToPath@PcContent@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// The result handle is created but never returned.
MfScDataHandle *PcContent::SaveContentInfoToPath(MfScDataHandle *, int, MfScDataHandle **args)
{
    if (!args[0]) {
        MfScVmEnv *env = MfThread::GetThread()->m_env;
        MfString msg;
        msg << "Parameter number " << 0 << " must not be nil";
        env->ThrowException(6, msg);
        return 0;
    }
    if (!args[1]) {
        MfScVmEnv *env = MfThread::GetThread()->m_env;
        MfString msg;
        msg << "Parameter number " << 1 << " must not be nil";
        env->ThrowException(6, msg);
        return 0;
    }
    if (!args[2]) {
        MfScVmEnv *env = MfThread::GetThread()->m_env;
        MfString msg;
        msg << "Parameter number " << 2 << " must not be nil";
        env->ThrowException(6, msg);
        return 0;
    }
    if (!args[0]->IsType(0x100)) {
        MfScVmEnv *env = MfThread::GetThread()->m_env;
        MfString msg;
        msg << "Parameter number " << 0 << " must be a string.";
        env->ThrowException(11, msg);
        return 0;
    }
    if (!args[1]->IsType(0x100)) {
        MfScVmEnv *env = MfThread::GetThread()->m_env;
        MfString msg;
        msg << "Parameter number " << 1 << " must be a string.";
        env->ThrowException(11, msg);
        return 0;
    }
    MfScDataHandle *result = MfScDataHandle::CreateData(0);
    MfContentInfo info;
    if (info.FromObject(args[2])) {
        MfScObjString *path = (MfScObjString *)args[0]->GetData();
        MfScObjString *fileName = (MfScObjString *)args[1]->GetData();
        result->SetInt(m_content->SaveContentInfoToPath(*(MfString *)path, *(MfString *)fileName, &info));
    }
    return 0;
}

// 0x100063a0 ?CreateContentSubFolders@PcContent@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *PcContent::CreateContentSubFolders(MfScDataHandle *, int, MfScDataHandle **)
{
    return MfScDataHandle::CreateData(m_content->CreateDataFolders());
}

// EQUIV 0x100063c0 ?GetContentNames@PcContent@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: stack slot of the this copy only
MfScDataHandle *PcContent::GetContentNames(MfScDataHandle *, int, MfScDataHandle **)
{
    MfScDataHandle *result = 0;
    MfDblList *list;
    MfScStdLibListInterface *iface = MfScStdLib::pslliListInterface;
    if (iface) {
        MfStringList names;
        m_content->GetContentNames(&names);
        iface->CreateList(&list, &result);
        MfDblListItem *pos;
        for (MfString *name = (MfString *)names.GetFirst(pos); name; name = (MfString *)names.GetNext(pos)) {
            MfScDataHandle *h = MfScDataHandle::CreateData(*name, 0);
            list->Add(h);
            h->Release();
        }
    }
    return result;
}

// 0x100064f0 ?Refresh@PcContent@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *PcContent::Refresh(MfScDataHandle *, int, MfScDataHandle **)
{
    m_content->Refresh();
    return 0;
}

// 0x10006500 ??_EMfScClassInfo@@W7AEPAXI@Z
// EQUIV 0x10006510 ??_EMfScCiMethod@@W3AEPAXI@Z: reviewed: the 8-byte thunk (sub ecx, 4; jmp) is byte-identical; the tool's 32-byte extent also takes in the import thunks that follow it
