// mfutil.dll MfPathSearch (0x1000acf0-0x1000b100), with the local tokenizer
// callback that Search() uses (vtable 0x1001767c).
#include <windows.h>
#include "MfPathSearch_unit.h"

// Not exported: tries "<dir>\<name>" for every directory token.
// size 0xc: vptr, m_name +4, m_found +8
class MfPathSearchCallBack : public MfStringTokenizerCallBack {
public:
    virtual int CallBack(const MfString &token, MfString &rest);

    MfString m_name;    // +4
    MfString m_found;   // +8
};

// 0x1000acf0 ??0MfPathSearch@@QAE@ABV0@@Z
MfPathSearch::MfPathSearch(const MfPathSearch &o) : m_file(o.m_file), m_path(o.m_path)
{
}

// 0x1000ad50 ??_EMfPathSearch@@UAEPAXI@Z (compiler-generated)

// 0x1000ada0 ??0MfPathSearch@@QAE@ABVMfString@@0@Z
MfPathSearch::MfPathSearch(const MfString &file, const MfString &path) : m_file(file), m_path(path)
{
}

// 0x1000ae00 ??1MfPathSearch@@UAE@XZ
MfPathSearch::~MfPathSearch()
{
}

// 0x1000ae50 ?TestAccess@MfPathSearch@@SAHABVMfString@@@Z
int MfPathSearch::TestAccess(const MfString &file)
{
    int result = 0;
    HANDLE h = CreateFileA(file, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, 0);
    if (h != INVALID_HANDLE_VALUE) {
        CloseHandle(h);
        result = 1;
    }
    return result;
}

// 0x1000ae90 ?Search@MfPathSearch@@QAE?AVMfString@@XZ
MfString MfPathSearch::Search()
{
    if (TestAccess(m_file))
        return m_file;
    MfPathSearchCallBack cb;
    MfString ext;
    m_file.SplitPath(0, 0, &cb.m_name, &ext);
    cb.m_name << ext;
    m_path.Tokenize(";", cb);
    return cb.m_found;
}

// 0x1000afd0 ??1MfPathSearchCallBack@@UAE@XZ (implicit)
// 0x1000b020 ??_GMfPathSearchCallBack@@UAEPAXI@Z (compiler-generated)

// 0x1000b040 ?CallBack@MfPathSearchCallBack@@UAEHABVMfString@@AAV2@@Z
int MfPathSearchCallBack::CallBack(const MfString &token, MfString &rest)
{
    MfString file;
    file << token << "\\" << m_name;
    if (MfPathSearch::TestAccess(file)) {
        m_found = file;
        return 0;
    }
    return 1;
}
