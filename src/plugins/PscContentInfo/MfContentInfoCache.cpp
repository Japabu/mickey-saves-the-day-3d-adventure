// PscContentInfo.dll 0x10002c50-0x100049ef: the cache of content information
// files (MfContentInfoCache), the file reader (MfContentInfoFile) and the
// MfContentInfo helpers.
#include "PscContentInfo.h"

// 0x10002c50 _$E24
// 0x10002c60 _$E21
// 0x10002ca0 _$E23
// 0x10002cb0 _$E22
// Folders of a content that are never searched for *.cif files.
static MfString aStandardFolders[4] = { "Actors", "Piccolo", "Sounds", "Spaces" };

// 0x10002cd0 ??0MfContentInfoCache@@QAE@XZ
MfContentInfoCache::MfContentInfoCache()
    : MfDictionary(0x35)
{
}

// 0x10002d80 ??_GMfContentInfoCache@@UAEPAXI@Z
// 0x10002da0 ??1MfContentInfoCache@@UAE@XZ
MfContentInfoCache::~MfContentInfoCache()
{
    MfContentInfoCacheClearCB clear;
    ClearItems(&clear);
    m_names.Clear();
}

// 0x10002ec0 ??_GMfContentInfoCacheClearCB@MfContentInfoCache@@UAEPAXI@Z
// 0x10002ee0 ??1MfContentInfoCacheClearCB@MfContentInfoCache@@UAE@XZ

// 0x10002ef0 ?AddPath@MfContentInfoCache@@QAEXABVMfString@@@Z
void MfContentInfoCache::AddPath(const MfString &path)
{
    if (!m_paths.Search(path))
        m_paths.Add(path);
}

// 0x10002f20 ?Add@MfContentInfoCache@@QAEHABUMfContentInfo@@@Z
int MfContentInfoCache::Add(const MfContentInfo &info)
{
    if (!(info.name == NULL_STRING)) {
        MfContentInfo *entry = new MfContentInfo;
        *entry = info;
        if (!MfDictionary::Add(info.name, entry)) {
            m_names.Add(info.name);
            return 1;
        }
    }
    return 0;
}

// 0x10003010 ?Find@MfContentInfoCache@@QAEHABVMfString@@AAUMfContentInfo@@@Z
int MfContentInfoCache::Find(const MfString &name, MfContentInfo &info)
{
    MfContentInfo *entry = (MfContentInfo *)MfDictionary::Find(name);
    if (entry) {
        info = *entry;
        return 1;
    }
    return 0;
}

// 0x10003040 ?Refresh@MfContentInfoCache@@QAEXXZ
void MfContentInfoCache::Refresh()
{
    MfContentInfoCacheClearCB clear;
    ClearItems(&clear);
    m_names.Clear();
    MfDblListItem *pos;
    for (MfString *path = m_paths.First(pos); path; path = m_paths.Next(pos))
        ScanFolder(*path);
}

// 0x100030c0 ?GetNames@MfContentInfoCache@@QAEXAAVMfStringList@@@Z
void MfContentInfoCache::GetNames(MfStringList &names)
{
    MfCopyNameListCB copy;
    names.Clear();
    names.Copy(m_names, copy);
}

// 0x100030f0 ?Iterate@MfListItemInterface@@UAEHPAX@Z
// 0x10003100 ?Delete@MfListItemInterface@@UAEXPAX@Z
// 0x10003110 ?Matches@MfListItemInterface@@UAEHPAX0@Z

// 0x10003120 ?ScanFolder@MfContentInfoCache@@AAEXABVMfString@@@Z
void MfContentInfoCache::ScanFolder(const MfString &folder)
{
    MfRegularExpression cif(".*\\.cif$");
    cif.Optimize();
    MfDirectory dir;
    MfDirectoryFileInfo *info;
    if (dir.Open(folder) && (info = dir.First()) != 0) {
        do {
            MfString name = info->GetFileName();
            if (info->QueryAttributes() & FILE_ATTRIBUTE_DIRECTORY) {
                if (name != "." && name != ".." && !IsStandardFolder(name))
                    ScanFolder(info->GetFilePath());
            } else if (cif.Matches(name)) {
                MfContentInfoFile file(info->GetFilePath());
                MfContentInfo content;
                file.Read(content);
                if (!Add(content)) {
                    MfContentInfo other;
                    Find(content.name, other);
                    MfString function("File Reader");
                    MfString module("Content Info");
                    MfLogger::Log(1, module, function,
                                  MfString("Duplicate Content Name\n") +
                                  MfString("The content information named '") + content.name +
                                  MfString("' in the file '") + content.file + MfString("'\n") +
                                  MfString("has the same name as the information in the file '") +
                                  other.file + MfString("'"));
                }
            }
            MfDirectoryFileInfo *next = dir.Next();
            info = next;
        } while (info);
    }
}

// 0x100037e0 ?IsStandardFolder@MfContentInfoCache@@AAEHABVMfString@@@Z
int MfContentInfoCache::IsStandardFolder(const MfString &name)
{
    int result = 0;
    for (int i = 0; i < 4; i++) {
        if (aStandardFolders[i].CaseCompare(name) == 0) {
            result = 1;
            break;
        }
    }
    return result;
}

// 0x10003820 ?CallBack@MfContentInfoCacheClearCB@MfContentInfoCache@@UAEHPAX@Z
int MfContentInfoCache::MfContentInfoCacheClearCB::CallBack(void *data)
{
    delete (MfContentInfo *)data;
    return 1;
}

// 0x10003850 ?Copy@MfCopyNameListCB@MfContentInfoCache@@UAEPAXPAX@Z
void *MfContentInfoCache::MfCopyNameListCB::Copy(void *item)
{
    return new MfString(*(MfString *)item);
}

// 0x100038c0 ??0MfContentInfoFile@@QAE@ABVMfString@@@Z
MfContentInfoFile::MfContentInfoFile(const MfString &fileName)
    : m_fileName(fileName), m_line(0), m_open(0)
{
    m_in.open(fileName, ios::in | ios::nocreate, filebuf::openprot);
    if (!m_in.is_open()) {
        MfString function("File Reader");
        MfString module("Content Info");
        MfLogger::Log(2, module, function,
                      MfString("Error opening content file\n") +
                      MfString("There was an error opening the content information file.\n") +
                      MfString("The file name is '") + fileName + MfString("'"));
    } else {
        m_open = 1;
    }
}

// 0x10003ab0 ??_GMfContentInfoFile@@UAEPAXI@Z
// 0x10003ad0 ??1MfContentInfoFile@@UAE@XZ
MfContentInfoFile::~MfContentInfoFile()
{
    if (m_open)
        m_in.close();
}

// EQUIV 0x10003b60 ?Read@MfContentInfoFile@@QAEHAAUMfContentInfo@@@Z: reviewed: register choice only (ebp/edi swapped for the hoisted MfString(const char *) and operator+ import pointers in the loop)
int MfContentInfoFile::Read(MfContentInfo &info)
{
    info.Clear();
    info.file = m_fileName;
    info.path = info.GetFolder();
    if (!m_open) {
        MfString function("File Reader");
        MfString module("Content Info");
        MfLogger::Log(2, module, function,
                      MfString("File not open\n") + MfString("Attempt to read file '") +
                      m_fileName + MfString("' that was not open"));
        return 0;
    }
    while (NextLine()) {
        MfString keyword;
        switch (ParseDirective(keyword)) {
        case kEmpty:
            break;
        case kVendor:
            info.vendor = m_text;
            break;
        case kName:
            info.name = m_text;
            break;
        case kDescription:
            info.description = m_text;
            break;
        case kVersion:
            info.version = m_text;
            break;
        case kDefaultSpace:
            info.defaultSpace = m_text;
            break;
        case kUnknown: {
            MfString function("File Reader");
            MfString module("Content Info");
            MfLogger::Log(1, module, function,
                          MfString("Unknown CIF directive\n") + MfString("The directive '") + keyword +
                          MfString("' was encountered on line ") + MfString(m_line) +
                          MfString("of the content information file.\n") +
                          MfString("The CIF name is '") + m_fileName + MfString("'"));
            break;
        }
        }
    }
    return 1;
}

// 0x10004070 ?NextLine@MfContentInfoFile@@AAEHXZ
int MfContentInfoFile::NextLine()
{
    if (m_in.fail())
        return 0;
    m_text = NULL_STRING;
    MfString line(MfStringSize(0x200));
    int done = 0;
    do {
        m_in >> line;
        m_line++;
        line.NoTrailingWhiteSpace();
        if (line.GetCharacter(line.Length() - 1) != '\\')
            done = 1;
        else
            line.Truncate(line.Length() - 1);
        int comment = line.HaveCharactersInSet("#", 0);
        if (comment >= 0)
            line.Truncate(comment);
        m_text << line;
    } while (!done);
    return 1;
}

// 0x100043c0 ??_GMfStringRef@@UAEPAXI@Z

static struct {
    const char *name;
    int directive;
} aDirectives[] = {
    { "vendor", MfContentInfoFile::kVendor },
    { "name", MfContentInfoFile::kName },
    { "description", MfContentInfoFile::kDescription },
    { "version", MfContentInfoFile::kVersion },
    { "default-space", MfContentInfoFile::kDefaultSpace },
    { 0, 0 }
};

// EQUIV 0x100043f0 ?ParseDirective@MfContentInfoFile@@AAEHAAVMfString@@@Z: reviewed: register choice only (the original keeps the constant 1 in ebp for the refcount compares, the EH state and the kEmpty return, and decrements nRefs through a copy register)
int MfContentInfoFile::ParseDirective(MfString &keyword)
{
    int directive = kUnknown;
    m_text.NoLeadingWhiteSpace();
    m_text.NoTrailingWhiteSpace();
    if (m_text == NULL_STRING)
        return kEmpty;
    int i = m_text.HaveCharactersInSet("\t\n ", 0);
    MfString word;
    if (i >= 0) {
        word = m_text.LeftString(i);
        m_text = m_text.RightString(m_text.Length() - i);
        m_text.NoLeadingWhiteSpace();
        m_text.NoTrailingWhiteSpace();
    } else {
        word = m_text;
    }
    for (int n = 0; aDirectives[n].name; n++) {
        if (word.CaseCompare(aDirectives[n].name) == 0) {
            directive = aDirectives[n].directive;
            break;
        }
    }
    keyword = word;
    return directive;
}

// 0x10004820 ??4MfContentInfo@@QAEAAU0@ABU0@@Z
MfContentInfo &MfContentInfo::operator=(const MfContentInfo &other)
{
    name = other.name;
    vendor = other.vendor;
    description = other.description;
    version = other.version;
    defaultSpace = other.defaultSpace;
    path = other.path;
    file = other.file;
    return *this;
}

// 0x10004870 ?GetDescription@MfContentInfo@@QAEXAAUMfContentDescription@@@Z
void MfContentInfo::GetDescription(MfContentDescription &desc)
{
    desc.name = name;
    desc.vendor = vendor;
    desc.description = description;
    desc.version = version;
    desc.defaultSpace = defaultSpace;
    desc.path = path;
}

// 0x100048c0 ?Clear@MfContentInfo@@QAEXXZ
void MfContentInfo::Clear()
{
    name = NULL_STRING;
    vendor = NULL_STRING;
    description = NULL_STRING;
    version = NULL_STRING;
    defaultSpace = NULL_STRING;
    path = NULL_STRING;
}

// 0x10004910 ?GetFolder@MfContentInfo@@QAE?AVMfString@@XZ
MfString MfContentInfo::GetFolder()
{
    MfString drive;
    MfString dir;
    file.SplitPath(&drive, &dir, 0, 0);
    dir = dir.LeftString(dir.Length() - 1);
    return drive + dir;
}
