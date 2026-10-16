// PscStdlib.dll 0x10004eb0-0x10005750: the script class "Logger".
#include "PscStdlib.h"

// 0x10004eb0 _$E6
// 0x10004ec0 _$E3
// 0x10004ee0 _$E5
// 0x10004ef0 _$E4
// 0x1000f288
static MfString fsName("Logger");
// 0x1000f28c: the class object
static MfPlLogger *pplClass;

// 0x10004f00 ?GetClassNameA@MfPlLogger@@UAE?AVMfString@@XZ
MfString MfPlLogger::GetClassNameA()
{
    return fsName;
}

// 0x10004f30 ??0MfPlLogger@@QAE@VMfPlClassArg@@@Z
MfPlLogger::MfPlLogger(MfPlClassArg)
{
    *m_name = fsName + MfString("<class>");
    {
        MfString full("Logger::Log");
        MfString name("Log");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlLogger::Log, full, -1), 1);
    }
    {
        MfString full("Logger::SetOptions");
        MfString name("SetOptions");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlLogger::SetOptions, full, 1), 1);
    }

    MfScClassInfo *ci = new MfScClassInfo("Logger");
    MfScCiMethod *method = new MfScCiMethod("Log");
    *ci + method;
    *method + new MfScMethodParameterInfo("<variable>", 0);
    method = new MfScCiMethod("SetOptions");
    *ci + method;
    *method + new MfScMethodParameterInfo("options", 0);
    MfScDataObject::AddClassInfo(ci);
}

// 0x10005320 ??_GMfPlLogger@@UAEPAXI@Z
// 0x10005340 ??1MfPlLogger@@UAE@XZ

// 0x10005350 ?Log@MfPlLogger@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlLogger::Log(MfScDataHandle *, int argc, MfScDataHandle **argv)
{
    MfStringList lines;
    for (int i = 0; i < argc; i++) {
        MfScDataHandle *h = argv[i];
        MfString *s;
        if (h && h->IsType(MfScDataHandle::kString)) {
            s = (MfScObjString *)h->GetData();
            lines.Add(*s);
        }
    }
    if (lines.GetCount())
        MfLogger::Log(0, "Piccolo Logger", "Logged Message", lines);
    return 0;
}

// 0x100054a0 ?SetOptions@MfPlLogger@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlLogger::SetOptions(MfScDataHandle *, int, MfScDataHandle **argv)
{
    if (argv[0]->IsType(MfScDataHandle::kInteger)) {
        int options = argv[0]->GetInt();
        if (options & PL_ENABLE)
            MfLogger::EnableWindowOutput(1);
        else if (options & PL_DISABLE)
            MfLogger::EnableWindowOutput(0);
    }
    return 0;
}

// 0x100054e0 ?GetConstants@MfPlLogger@@SA?AVMfString@@XZ
MfString MfPlLogger::GetConstants()
{
    return MfString("global PL_ENABLE = ") + MfString(PL_ENABLE) + MfString("; ")
        + MfString("global PL_DISABLE = ") + MfString(PL_DISABLE) + MfString("; ");
}

// 0x10005660 ?Initialize@MfPlLogger@@SAXXZ
void MfPlLogger::Initialize()
{
    if (!pplClass) {
        MfPlClassArg arg;
        pplClass = new MfPlLogger(arg);
        MfScDataHandle *h = MfScDataHandle::CreateData(pplClass);
        h->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(fsName, h, 1, 0);
    }
}

// 0x100056f0 ?Shutdown@MfPlLogger@@SAXXZ
void MfPlLogger::Shutdown()
{
    if (pplClass) {
        MfScDataHandle *h;
        MfScVmEnv::GetGlobalVariable(fsName, &h);
        if (h) {
            h->SetReadWrite();
            MfScVmEnv::SetGlobalVariable(fsName, 0, 1, 0);
            h->Release();
        }
        pplClass = 0;
    }
}
