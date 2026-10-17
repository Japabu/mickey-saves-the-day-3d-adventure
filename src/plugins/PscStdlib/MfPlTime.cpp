// PscStdlib.dll 0x10007810-0x100082d0: the script class "Time".
#include "PscStdlib.h"

// 0x10007810 _$E6
// 0x10007820 _$E3
// 0x10007840 _$E5
// 0x10007850 _$E4
// 0x1000f298
static MfString fsName("Time");
// 0x1000f29c: the class object
static MfPlTime *pptClass;

// 0x10007860 ?GetClassNameA@MfPlTime@@UAE?AVMfString@@XZ
MfString MfPlTime::GetClassNameA()
{
    return fsName;
}

// 0x10007890 ??0MfPlTime@@QAE@VMfPlClassArg@@@Z
MfPlTime::MfPlTime(MfPlClassArg)
{
    *m_name = fsName + MfString("<class>");
    {
        MfString full("Time::TimeStamp");
        MfString name("TimeStamp");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlTime::TimeStamp, full, 0), 1);
    }
    {
        MfString full("Time::GetDayOfWeek");
        MfString name("GetDayOfWeek");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlTime::GetDayOfWeek, full, 0), 1);
    }
    {
        MfString full("Time::GetDay");
        MfString name("GetDay");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlTime::GetDay, full, 0), 1);
    }
    {
        MfString full("Time::GetMonth");
        MfString name("GetMonth");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlTime::GetMonth, full, 0), 1);
    }
    {
        MfString full("Time::GetYear");
        MfString name("GetYear");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlTime::GetYear, full, 0), 1);
    }
    {
        MfString full("Time::GetHour");
        MfString name("GetHour");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlTime::GetHour, full, 0), 1);
    }
    {
        MfString full("Time::GetMinute");
        MfString name("GetMinute");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlTime::GetMinute, full, 0), 1);
    }

    MfScClassInfo *ci = new MfScClassInfo("Time");
    *ci + new MfScCiMethod("TimeStamp");
    *ci + new MfScCiMethod("GetDayOfWeek");
    *ci + new MfScCiMethod("GetDay");
    *ci + new MfScCiMethod("GetMonth");
    *ci + new MfScCiMethod("GetYear");
    *ci + new MfScCiMethod("GetHour");
    *ci + new MfScCiMethod("GetMinute");
    MfScDataObject::AddClassInfo(ci);
}

// 0x10007fa0 ??_GMfPlTime@@UAEPAXI@Z
// 0x10007fc0 ??1MfPlTime@@UAE@XZ

// 0x10007fd0 ?TimeStamp@MfPlTime@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlTime::TimeStamp(MfScDataHandle *, int, MfScDataHandle **)
{
    MfSystemTime now(0);
    GetLocalTime(&now);
    MfString stamp;
    stamp.FormatTime("%#m/%#d/%y %#I:%M:%S %p", now);
    return MfScDataHandle::CreateData(stamp, 0);
}

// 0x10008060 ?GetDayOfWeek@MfPlTime@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlTime::GetDayOfWeek(MfScDataHandle *, int, MfScDataHandle **)
{
    MfSystemTime now(0);
    GetLocalTime(&now);
    struct tm t;
    now.Convert(&t);
    return MfScDataHandle::CreateData(t.tm_wday);
}

// 0x100080a0 ?GetDay@MfPlTime@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlTime::GetDay(MfScDataHandle *, int, MfScDataHandle **)
{
    MfSystemTime now(0);
    GetLocalTime(&now);
    struct tm t;
    now.Convert(&t);
    return MfScDataHandle::CreateData(t.tm_mday);
}

// 0x100080e0 ?GetMonth@MfPlTime@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlTime::GetMonth(MfScDataHandle *, int, MfScDataHandle **)
{
    MfSystemTime now(0);
    GetLocalTime(&now);
    struct tm t;
    now.Convert(&t);
    return MfScDataHandle::CreateData(t.tm_mon);
}

// 0x10008120 ?GetYear@MfPlTime@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlTime::GetYear(MfScDataHandle *, int, MfScDataHandle **)
{
    MfSystemTime now(0);
    GetLocalTime(&now);
    struct tm t;
    now.Convert(&t);
    return MfScDataHandle::CreateData(t.tm_year);
}

// 0x10008160 ?GetHour@MfPlTime@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlTime::GetHour(MfScDataHandle *, int, MfScDataHandle **)
{
    MfSystemTime now(0);
    GetLocalTime(&now);
    struct tm t;
    now.Convert(&t);
    return MfScDataHandle::CreateData(t.tm_hour);
}

// 0x100081a0 ?GetMinute@MfPlTime@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlTime::GetMinute(MfScDataHandle *, int, MfScDataHandle **)
{
    MfSystemTime now(0);
    GetLocalTime(&now);
    struct tm t;
    now.Convert(&t);
    return MfScDataHandle::CreateData(t.tm_min);
}

// 0x100081e0 ?Initialize@MfPlTime@@SAXXZ
void MfPlTime::Initialize()
{
    if (!pptClass) {
        MfPlClassArg arg;
        pptClass = new MfPlTime(arg);
        MfScDataHandle *h = MfScDataHandle::CreateData(pptClass);
        h->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(fsName, h, 1, 0);
    }
}

// 0x10008270 ?Shutdown@MfPlTime@@SAXXZ
void MfPlTime::Shutdown()
{
    if (pptClass) {
        MfScDataHandle *h;
        MfScVmEnv::GetGlobalVariable(fsName, &h);
        if (h) {
            h->SetReadWrite();
            MfScVmEnv::SetGlobalVariable(fsName, 0, 1, 0);
            h->Release();
        }
        pptClass = 0;
    }
}
