// MBEngine.dll .\MBEngineRelease\CMBWinFileSpec.obj
#include "CMBWinFileSpec.h"

// 0x1000d090 ?MakeCMBWinFileSpec@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBWinFileSpec(void)
{
    IMBUnknown *p = new CMBWinFileSpec;
    if (p)
        p->AddRef();
    return p;
}

// 0x1000d0c0 ??0CMBWinFileSpec@@QAE@XZ
CMBWinFileSpec::CMBWinFileSpec(void) : mPath(""), mFoundFile(false)
{
    _mRefCnt = 0;
}

// 0x1000d1a0 ?QueryInterface@CMBWinFileSpec@@UAEJABUMBGUID@@PAPAX@Z
long CMBWinFileSpec::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBFileSpec)
        p = this;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1000d270 ?AddRef@CMBWinFileSpec@@UAEKXZ
unsigned long CMBWinFileSpec::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1000d280 ?Release@CMBWinFileSpec@@UAEKXZ
unsigned long CMBWinFileSpec::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}

// 0x1000d2b0 ??1CMBWinFileSpec@@QAE@XZ
CMBWinFileSpec::~CMBWinFileSpec(void)
{
}

// 0x1000d300 ?SetPath@CMBWinFileSpec@@UAEXPBD@Z
void CMBWinFileSpec::SetPath(const char *path)
{
    mFoundFile = false;
    mPath = path;
    int len = mPath.length();
    if (len > 0 && mPath[len] == '\\')
        mPath.resize(len - 1);
    Update();
}

// EQUIV 0x1000d4b0 ?SetPath@CMBWinFileSpec@@UAEXPBVIMBFileSpec@@PBD@Z: reviewed: two lea sums with the operands in the other order
void CMBWinFileSpec::SetPath(const IMBFileSpec *parent, const char *path)
{
    MBString fullpath = parent->GetPath();
    if (fullpath.at(fullpath.length() - 1) != '\\')
        fullpath += "\\";
    fullpath += path;
    SetPath(fullpath.c_str());
}

// 0x1000d640 ?Update@CMBWinFileSpec@@IAEXXZ
void CMBWinFileSpec::Update(void)
{
    void *handle;
    char *name;
    char buffer[128];
    memset(&mFileInfo, 0, sizeof(mFileInfo));
    handle = FindFirstFileA(mPath.c_str(), &mFileInfo);
    if (handle == INVALID_HANDLE_VALUE) {
        mFoundFile = false;
        return;
    }
    mFoundFile = true;
    if (GetRoot().length() == 0) {
        GetFullPathNameA(mFileInfo.cFileName, 128, buffer, &name);
        mPath = buffer;
    }
    FindClose(handle);
}

// 0x1000d7a0 ?GetName@CMBWinFileSpec@@UBE?AV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@XZ
MBString CMBWinFileSpec::GetName(void) const
{
    MBString name;
    unsigned int pos = mPath.rfind(kMBDirectorySeparator);
    if (pos != -1)
        name = mPath.substr(pos + 1);
    else
        name = mPath;
    return name;
}

// 0x1000d8e0 ?GetExtension@CMBWinFileSpec@@UBE?AV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@XZ
MBString CMBWinFileSpec::GetExtension(void) const
{
    MBString extension("");
    unsigned int pos = mPath.rfind(kMBExtensionSeparator);
    if (pos != -1)
        extension = mPath.substr(pos + 1);
    return extension;
}

// 0x1000da40 ?GetPath@CMBWinFileSpec@@UBE?AV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@XZ
MBString CMBWinFileSpec::GetPath(void) const
{
    return mPath;
}

// 0x1000db60 ?GetTitle@CMBWinFileSpec@@UBE?AV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@XZ
MBString CMBWinFileSpec::GetTitle(void) const
{
    MBString title;
    unsigned int start = mPath.rfind(kMBDirectorySeparator) + 1;
    unsigned int end = mPath.rfind(kMBExtensionSeparator);
    title = mPath.substr(start, end - start);
    return title;
}

// 0x1000dc90 ?GetRoot@CMBWinFileSpec@@IBE?AV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@XZ
MBString CMBWinFileSpec::GetRoot(void) const
{
    MBString root("");
    unsigned int pos = mPath.rfind(kMBDirectorySeparator);
    if (pos != -1)
        root = mPath.substr(0, pos + 1);
    return root;
}

// EQUIV 0x1000ddf0 ?GetType@CMBWinFileSpec@@UBEKXZ: reviewed: the default for a null name is selected with the other branch form and the name is reloaded for the string release; same calls
unsigned long CMBWinFileSpec::GetType(void) const
{
    unsigned long type = 0;
    MBString extension = GetExtension();
    const char *ext = extension.c_str();
    int len = strlen(ext);
    if (len > 4)
        len = 4;
    int i, j = 3;
    for (i = 0; i < len; i++, j--)
        ((char *)&type)[j] = tolower(ext[i]);
    for (; i < 4; i++, j--)
        ((char *)&type)[j] = ' ';
    return type;
}

// 0x1000dea0 ?GetParent@CMBWinFileSpec@@UBEXPAPAVIMBFileSpec@@@Z
void CMBWinFileSpec::GetParent(IMBFileSpec **fileSpec) const
{
    *fileSpec = 0;
    if (MBFileSpecExists(this)) {
        MBString parentPath = GetRoot();
        if (MBstricmp(parentPath.c_str(), mPath.c_str()) != 0) {
            IMBUnknown *unknown;
            if (MBComCreateInstance(0, MBIID_CMBWinFileSpec, &unknown) == 0) {
                if (unknown->QueryInterface(MBIID_IMBFileSpec, (void **)fileSpec) == 0)
                    (*fileSpec)->SetPath(parentPath.c_str());
                unknown->Release();
            }
        }
    }
}

// 0x1000df80 ?Hash@CMBWinFileSpec@@UBEKXZ
unsigned long CMBWinFileSpec::Hash(void) const
{
    return MBHashString(GetTitle().c_str());
}


// Matched by this code:
// 0x1000dfe0 ?assign@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAEAAV12@ABV12@II@Z
// 0x1000e1f0 ?rfind@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBEIPBDII@Z
// 0x1000e270 ?_Split@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEXXZ
// 0x1000e340 ?max_size@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBEIXZ
// 0x1000e350 _$E20
// 0x1000e370 _$E19
