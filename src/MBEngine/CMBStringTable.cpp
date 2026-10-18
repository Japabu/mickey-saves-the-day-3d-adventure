// CMBStringTable.cpp - MBEngine.dll CMBStringTable.obj
#include "CMBStringTable.h"

// 0x100483f0 ?MakeCMBStringTable@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBStringTable(void)
{
    IMBUnknown *p = new CMBStringTable;
    if (p)
        p->AddRef();
    return p;
}

// 0x10048420 ??0CMBStringTable@@QAE@XZ
CMBStringTable::CMBStringTable(void)
    : mLocale(MBString("ENG"))
{
    _mRefCnt = 0;
}

// 0x10048520 ?QueryInterface@CMBStringTable@@UAEJABUMBGUID@@PAPAX@Z
long CMBStringTable::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBStringTable)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x100485a0 ?AddRef@CMBStringTable@@UAEKXZ
unsigned long CMBStringTable::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x100485b0 ?Release@CMBStringTable@@UAEKXZ
unsigned long CMBStringTable::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x100485e0 ?Load@CMBStringTable@@UAE_NKPAVIMBStream@@@Z
bool CMBStringTable::Load(unsigned long id, IMBStream *in)
{
    if (!in)
        return false;
    mStream = in;
    mPref.CreateObject(MBIID_CMBPref);
    return true;
}

// TODO 0x100486a0 ?Load@CMBStringTable@@UAE_NPBD@Z: Load(const char*): original calls MBComPtr<IMBPref>::Release in both CreateObject Attach paths and inlines the string dtors; ours the reverse (inline budget); 5 compiles (85% similar)
bool CMBStringTable::Load(const char *filename)
{
    MBComPtr<IMBFileSpec> fileSpec(MBIID_IMBFileSpec);
    MBString sCDResourceDir; // unused (PDB locals; constructed first, at the higher slot)
    MBString sHDResourceDir;
    if (fileSpec) {
        fileSpec->SetPath(filename);
        {
            IMBStream *tempStream;
            MBOpenFile(fileSpec, 5, &tempStream);
            mStream.Attach(tempStream);
        }
        mPref.CreateObject(MBIID_CMBPref);
        return true;
    }
    return false;
}

// EQUIV 0x10048830 ?GetNamedString@CMBStringTable@@UAE_NABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAV?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@3@@Z: reviewed: the original never inlines the substr constructor, VC6 inlines the third one (frame 4 bytes bigger); calls, arguments and order agree. Was: GetNamedString: original never inlines the substr ctor and keeps find's char temp in a param slot; ours inlines the 3rd ctor (+4 frame); 3 compiles (74% similar)
bool CMBStringTable::GetNamedString(const MBString &name, MBString16 &ustring)
{
    if (!mPref)
        return false;
    MBString section("Strings.Named");
    mLocale.GetLCV(section);
    if (mPref->GetString(mStream, section, name, ustring) == 0) {
        section = mLocale.mLCV.substr(0, mLocale.mLCV.find('_', mLocale.mLCV.find('_') + 1));
        if (mPref->GetString(mStream, section, name, ustring) == 0) {
            section = mLocale.mLCV.substr(0, mLocale.mLCV.find('_'));
            if (mPref->GetString(mStream, section, name, ustring) == 0)
                return false;
        }
    }
    return true;
}

// EQUIV 0x10048ac0 ?GetString@CMBStringTable@@UAE_NKAAV?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@@Z: reviewed: inline choices of the string helpers only: the original calls substr out of line for the second lookup and the _Tidy of the temporary, VC6 inlines them; the aligned call sequences agree otherwise. Was: GetString: original calls substr out of line for the 2nd lookup; ours inlines it (+4 frame); 3 compiles (60% similar)
bool CMBStringTable::GetString(unsigned long id, MBString16 &ustring)
{
    if (!mPref)
        return false;
    static char buff[32];
    sprintf(buff, "%i", id);
    MBString key(buff);
    MBString section;
    mLocale.GetLCV(section);
    if (mPref->GetString(mStream, section, key, ustring) == 0) {
        section = mLocale.mLCV.substr(0, mLocale.mLCV.find('_', mLocale.mLCV.find('_') + 1));
        if (mPref->GetString(mStream, section, key, ustring) == 0) {
            section = mLocale.mLCV.substr(0, mLocale.mLCV.find('_'));
            if (mPref->GetString(mStream, section, key, ustring) == 0)
                return false;
        }
    }
    return true;
}

// 0x10048d90 ?SetLocale@CMBStringTable@@UAE_NABVMBLocale@@@Z
bool CMBStringTable::SetLocale(const MBLocale &locale)
{
    mLocale = locale;
    return true;
}

// 0x10048f70 ??1CMBStringTable@@QAE@XZ
CMBStringTable::~CMBStringTable(void)
{
}


// Matched by this code:
// 0x100101e0 ?find@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBEIPBDII@Z
// 0x100491a0 ??0?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAE@ABV01@IIABV?$MBAllocator@D@@@Z
// 0x10049360 ?Release@?$MBComPtr@VIMBPref@@@@QAEXXZ
// 0x10049380 $E21
// 0x100493a0 $E20
// steamboatapp.dll 0x100254d0 ?Release@?$MBComPtr@VIMBPref@@@@QAEXXZ

// TODO 0x10049010 ?substr@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBE?AV12@II@Z: STL instance (body in the VC6 <xstring>), not emitted: our compile inlines both substr calls (see GetNamedString, GetString)
