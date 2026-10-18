// MBEngine.dll .\MBEngineRelease\CMBPref.obj
#include "CMBPref.h"

// 0x1004f010 ?MakeCMBPref@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBPref(void)
{
    IMBUnknown *p = new CMBPref;
    if (p)
        p->AddRef();
    return p;
}

// 0x1004f040 ??0CMBPref@@QAE@XZ
CMBPref::CMBPref(void)
    : mHasChanged(false)
{
    _mRefCnt = 0;
}

// 0x1004f0f0 ?QueryInterface@CMBPref@@UAEJABUMBGUID@@PAPAX@Z
long CMBPref::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBPref)
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

// 0x1004f1c0 ?AddRef@CMBPref@@UAEKXZ
unsigned long CMBPref::AddRef()
{
    return ++_mRefCnt;
}

// 0x1004f1d0 ?Release@CMBPref@@UAEKXZ
unsigned long CMBPref::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt) {
        if (this) {
            this->~CMBPref();
            MBfree_release(this);
        }
    }
    return r;
}

// 0x1004f200 ??1CMBPref@@IAE@XZ
CMBPref::~CMBPref(void)
{
    if (mHasChanged)
        ApplyChanges();
    CleanUpTree();
}

// EQUIV 0x1004f330 ?ApplyChanges@CMBPref@@MAE_NXZ: reviewed: inline choices of the tree iterator helpers only (the original inlines the outer _Inc and the inner one with an out-of-line _Min; VC6 calls _Inc), frame 4 bytes bigger; the stream writes and their order agree. Was: inline budget: original inlines the outer _Inc fully and the inner _Inc with an out-of-line _Min; for-loops here inline neither, while-loops both (77% similar)
bool CMBPref::ApplyChanges(void)
{
    mStream->Seek(0, kIMBStreamSeekStart);
    MBComPtr<IMBFile> file(mStream);
    file->SetSize(0);
    MBPrefTree::iterator it = mPrefTree.begin();
    while (it != mPrefTree.end()) {
        MBPrefSectionList *section = (*it).second;
        mStream->Write("[", 1);
        mStream->Write((*it).first.c_str(), (*it).first.length());
        mStream->Write("]", 1);
        mStream->Write("\r\n", 2);
        MBPrefSectionList::iterator k = section->begin();
        while (k != section->end()) {
            mStream->Write((*k).first.c_str(), (*k).first.length());
            // comment lines have no value
            if ((*k).first.c_str()[0] != ';') {
                mStream->Write(" = ", 3);
                mStream->Write((*k).second->c_str(), (*k).second->length());
            }
            mStream->Write("\r\n", 2);
            k++;
        }
        mStream->Write("\r\n", 2);
        it++;
    }
    mHasChanged = false;
    return true;
}

// 0x1004f530 ?CleanUpTree@CMBPref@@IAEXXZ
void CMBPref::CleanUpTree(void)
{
    MBPrefTree::iterator it = mPrefTree.begin();
    while (it != mPrefTree.end()) {
        MBPrefSectionList *section = (*it).second;
        MBPrefSectionList::iterator k = section->begin();
        while (k != section->end()) {
            MBString *value = (*k).second;
            k++;
            delete value;
        }
        it++;
        delete section;
    }
}

// EQUIV 0x1004f6b0 ?GetString@CMBPref@@UAEJPAVIMBStream@@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@1AAV?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@4@ABV54@@Z: reviewed: inline choices of the same helpers only: the original inlines the section list find (lower_bound and the key compare) and calls the string assign, VC6 calls find and inlines the assign; the lookups, the conversion and the returns agree. Was: inline budget: original inlines the section list find (lower_bound + compare), ours calls it (87% similar)
long CMBPref::GetString(IMBStream *stream, const MBString &section, const MBString &keyName, MBString16 &dest, const MBString16 &deflt)
{
    if (mStream != stream) {
        if (mHasChanged)
            ApplyChanges();
        mStream = stream;
        ReadStreamIntoTree();
    }
    MBPrefTree::iterator it = mPrefTree.find(section);
    if (it != mPrefTree.end()) {
        MBPrefSectionList *list = (*it).second;
        if (list) {
            MBPrefSectionList::iterator k = list->find(keyName);
            if (k != list->end()) {
                MB_utf8_to_ucs2(*(*k).second, dest);
                return dest.length();
            }
            dest = deflt;
            return dest.length();
        }
    }
    dest = deflt;
    return dest.length();
}

// EQUIV 0x1004f860 ?GetString@CMBPref@@UAEJPAVIMBStream@@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@1AAV34@1@Z: reviewed: ebp is saved only on the paths that use it; otherwise the same
long CMBPref::GetString(IMBStream *stream, const MBString &section, const MBString &keyName, MBString &dest, const MBString &deflt)
{
    if (mStream != stream) {
        if (mHasChanged)
            ApplyChanges();
        mStream = stream;
        ReadStreamIntoTree();
    }
    MBPrefTree::iterator it = mPrefTree.find(section);
    if (it != mPrefTree.end()) {
        MBPrefSectionList *list = (*it).second;
        if (list) {
            MBPrefSectionList::iterator k = list->find(keyName);
            if (k != list->end()) {
                dest = *(*k).second;
                return dest.length();
            }
            dest = deflt;
            return dest.length();
        }
    }
    dest = deflt;
    return dest.length();
}

// EQUIV 0x1004f9f0 ?GetLong@CMBPref@@UAEJPAVIMBStream@@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@1J@Z: reviewed: inline choices of the string helpers only (the original calls _Tidy(false) out of line and inlines assign, VC6 the other way round) and the zero register; the lookups and returns agree. Was: inline budget: original calls _Tidy(false) out of line and inlines assign for both string ctors; ours inlines _Tidy and calls assign for the first (66% similar)
long CMBPref::GetLong(IMBStream *stream, const MBString &section, const MBString &keyName, long deflt)
{
    MBString dest("");
    if (GetString(stream, section, keyName, dest, MBString("")))
        return MBatol(dest.c_str());
    return deflt;
}

// 0x1004fb80 ?SetString@CMBPref@@UAEJPAVIMBStream@@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@1AAV?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@4@@Z
long CMBPref::SetString(IMBStream *stream, const MBString &section, const MBString &keyName, MBString16 &value)
{
    MBString utf8;
    MB_ucs2_to_utf8(value, utf8);
    SetString(stream, section, keyName, utf8);
    return utf8.length();
}

// 0x1004fc20 ?SetString@CMBPref@@UAEJPAVIMBStream@@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@1AAV34@@Z
long CMBPref::SetString(IMBStream *stream, const MBString &section, const MBString &keyName, MBString &value)
{
    if (mStream != stream) {
        if (mHasChanged)
            ApplyChanges();
        mStream = stream;
        ReadStreamIntoTree();
    }
    MBPrefSectionList *list;
    MBPrefTree::iterator it = mPrefTree.find(section);
    if (it == mPrefTree.end()) {
        list = new MBPrefSectionList;
        mPrefTree.insert(MBPrefTree::value_type(section, list));
    } else {
        list = (*it).second;
    }
    MBPrefSectionList::iterator k = list->find(keyName);
    if (k == list->end()) {
        MBString *s = new MBString(value);
        list->insert(MBPrefSectionList::value_type(keyName, s));
    } else {
        delete (*k).second;
        (*k).second = new MBString(value);
    }
    mHasChanged = true;
    return value.length();
}

// 0x1004fe00 ?SetLong@CMBPref@@UAEJPAVIMBStream@@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@1J@Z
long CMBPref::SetLong(IMBStream *stream, const MBString &section, const MBString &keyName, long value)
{
    char buf[32];
    sprintf(buf, "%d", value);
    MBString svalue(buf);
    return SetString(stream, section, keyName, svalue);
}

// TODO 0x1004fee0 ?ReadStreamIntoTree@CMBPref@@IAEXXZ: inline budget/register allocation in the parser loops; structure reconstructed (91% similar)
void CMBPref::ReadStreamIntoTree(void)
{
    unsigned long length = mStream->GetStreamSize();
    mStream->Seek(0, kIMBStreamSeekStart);
    char *buffer = new char[length + 1];
    mStream->Read(buffer, length);
    buffer[length] = 0;
    MBString vBuffer(buffer);
    delete[] buffer;
    StripChar('\r', vBuffer);

    unsigned long pos = 0;
    MBString vSection("");
    while (pos < length) {
        // [section]
        vSection = "";
        bool inSection = false;
        while (pos != length) {
            if (inSection) {
                if (vBuffer[pos] == '\n' || vBuffer[pos] == ']')
                    break;
                if (vBuffer[pos] != ']')
                    vSection += vBuffer[pos];
            }
            if (vBuffer[pos] == '[')
                inSection = true;
            pos++;
        }
        pos++;
        TrimLeadingWhiteSpace(vSection);
        TrimTrailingWhiteSpace(vSection);
        if (vSection.length() == 0)
            break;

        MBPrefSectionList *list;
        MBPrefTree::iterator it = mPrefTree.find(vSection);
        if (it == mPrefTree.end()) {
            list = new MBPrefSectionList;
            mPrefTree.insert(MBPrefTree::value_type(vSection, list));
        } else {
            list = (*it).second;
        }

        // key = value lines up to the next section
        while ((vBuffer[pos] == 0 || vBuffer[pos] == ' ' || vBuffer[pos] == '\n') && pos < length)
            pos++;
        while (vBuffer[pos] != '[' && pos < length) {
            MBString lValue("");
            MBString *value = new MBString("");
            ReadTokenToEOL(vBuffer, pos, '=', lValue);
            TrimLeadingWhiteSpace(lValue);
            TrimTrailingWhiteSpace(lValue);
            ReadTokenToEOL(vBuffer, pos, '\n', *value);
            TrimLeadingWhiteSpace(*value);
            TrimTrailingWhiteSpace(*value);
            MBPrefSectionList::iterator k = list->find(lValue);
            if (k == list->end()) {
                list->insert(MBPrefSectionList::value_type(lValue, value));
            } else {
                delete (*k).second;
                (*k).second = value;
            }
            while ((vBuffer[pos] == 0 || vBuffer[pos] == ' ' || vBuffer[pos] == '\n') && pos < length)
                pos++;
        }
    }
}

// TODO 0x100504f0 ?ReadTokenToEOL@CMBPref@@IAE_NAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAKD0@Z: original loop is not rotated (length test only at the top, back edge to it); while/for(;;)/early return all rotate (87% similar)
bool CMBPref::ReadTokenToEOL(MBString &buffer, unsigned long &startPos, char stopChar, MBString &dest)
{
    MBString returnValue("");
    while (startPos < buffer.length()) {
        if (buffer[startPos] == stopChar || buffer[startPos] == '\n') {
            startPos++;
            break;
        }
        dest += buffer[startPos++];
    }
    return false;
}

// 0x100506c0 ?StripChar@CMBPref@@IAEXDAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
void CMBPref::StripChar(char vchar, MBString &buffer)
{
    unsigned long pos = 0;
    while (pos != buffer.length()) {
        if (buffer[pos] == vchar)
            buffer.erase(pos, 1);
        else
            pos++;
    }
}

// 0x10050820 ?TrimLeadingWhiteSpace@CMBPref@@IAEXAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
void CMBPref::TrimLeadingWhiteSpace(MBString &buffer)
{
    while (buffer.length() != 0 && buffer[0] == ' ')
        buffer.erase(0, 1);
}

// 0x10050940 ?TrimTrailingWhiteSpace@CMBPref@@IAEXAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
void CMBPref::TrimTrailingWhiteSpace(MBString &buffer)
{
    while (buffer.length() != 0 && buffer[buffer.length() - 1] == ' ')
        buffer.erase(buffer.length() - 1, 1);
}

// 0x10050ab0 ??0?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QAE@ABV01@@Z
// 0x10050bd0 ??Eiterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$less@V2553d6b2
// 0x10050c60 ??0?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE@ABUbdc83cb0
// 0x10050d00 ?_Freeze@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEXXZ
// 0x10050d80 ?begin@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$less@V?$basb22a5f6c
// 0x10050d90 ?insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$less@V?$ba5b1b1551
// 0x10051060 ?find@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$less@V?$basia930095a
// 0x100510f0 ?begin@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$le5cb1ad9
// 0x10051100 ?insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$a6f2feab
// 0x100513d0 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x100514d0 ?find@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$le34051a21
// 0x10051560 ??0?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@Ucecd4ac2
// 0x10051580 ??0?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@st662a3791
// 0x100515a0 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$less@V?$bas98b77b8f
// 0x10051ab0 ?_Erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$less@V?$ba7ffbd241
// 0x10051b40 ?_Insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$less@V?$b8fd0bedb
// 0x10051e20 ?_Inc@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@62d885aa
// 0x10051eb0 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@2@V?$MBAllocator@PAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE?AViterator@12@V312@@Z
// TODO 0x100523c0 ?lower_bound@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV174a4438a: only emitted out of line once GetString(MBString16) inlines the section list find
// 0x10052430 ?_Erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$b8cdddf1
// 0x100524c0 ?_Insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?b7ea22b5
// 0x100527a0 ?_Inc@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@s1b8f0ff3
// 0x10052830 ?_Lbound@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$less@V?$bcaa6f01f
// 0x100528d0 ?_Lrotate@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$less@V?$c5c20780
// 0x10052930 ?_Rrotate@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$less@V?$b67a172f
// 0x10052990 ?_Buynode@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$less@V?$e25858eb
// 0x100529b0 ?_Dec@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@a0b4f960
// 0x10052a50 ?_Lbound@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?3f9d175c
// 0x10052af0 ?_Lrotate@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U4a96fbe5
// 0x10052b50 ?_Min@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$le97873fdc
// 0x10052b90 ?_Rrotate@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U7bc5ab60
// 0x10052bf0 ?_Buynode@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U0fcf8806
// 0x10052c10 ?_Dec@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@sca641ea9
// 0x10052cb0 ?_Construct@std@@YAXPAU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV?$MBMap@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@1eb897ed
// 0x10052df0 ?_Construct@std@@YAXPAU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@PAV12@@1@ABU21@@Z
// 0x10052f30 ?compare@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBEHABV12@@Z
// 0x10052f80 ?compare@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBEHIIPBDI@Z
// 0x10052ff0 ?_QueryInterface@?$MBComPtr@VIMBFile@@@@AAEJABV?$MBComPtr@VIMBStream@@@@@Z
