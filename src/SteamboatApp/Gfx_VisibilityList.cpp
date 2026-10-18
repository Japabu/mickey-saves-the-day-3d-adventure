#pragma optimize("a", on) // evidence: the constructor stores the string allocator before m_CurChunk and the destructor's inlined _Tidy loads the reference count first only with no-alias code
#include "Gfx_VisibilityList.h"
#include "MathConstants.h"
#include "Gfx_Camera.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream.h>
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x1010d660 ??0VisibilityList@Gfx@@QAE@XZ
Gfx::VisibilityList::VisibilityList()
    : m_Enabled(true), m_Selected(0), m_SelectedLod(-1), m_CurChunk(0), m_NextChunk(-1), m_Chunks(0), m_NumChunks(0)
{
}

// 0x1010d6c0 ??1VisibilityList@Gfx@@UAE@XZ
Gfx::VisibilityList::~VisibilityList()
{
    delete [] m_Chunks;
}

// 0x1010d7b0 ?ToggleVisibilityList@VisibilityList@Gfx@@QAEXXZ
void Gfx::VisibilityList::ToggleVisibilityList()
{
    m_Enabled = !m_Enabled;
}

// 0x1010d7c0 ?ToggleChunkDetail@VisibilityList@Gfx@@QAEXXZ
void Gfx::VisibilityList::ToggleChunkDetail()
{
    if (m_Enabled) {
        if (++m_SelectedLod > 2)
            m_SelectedLod = -1;
        AddVisChunk(m_CurChunk, m_Selected, m_SelectedLod);
    }
}

// 0x1010d7f0 ?ToggleChunkVis@VisibilityList@Gfx@@QAEXXZ
void Gfx::VisibilityList::ToggleChunkVis()
{
    if (m_Enabled) {
        if (GetVisChunkVis(m_CurChunk, m_Selected))
            DelVisChunk(m_CurChunk, m_Selected);
        else
            AddVisChunk(m_CurChunk, m_Selected, m_SelectedLod);
    }
}

// 0x1010e6b0 ?UpdateVisibilityList@VisibilityList@Gfx@@QAEXPAVCamera@2@@Z
void Gfx::VisibilityList::UpdateVisibilityList(Gfx::Camera *)
{
}

#pragma optimize("a", off) // evidence: the file code reloads m_Chunks and the file name after every stream call

// TODO 0x1010d840 ?LoadVisList@VisibilityList@Gfx@@QAEXV?$_SS@D@@@Z: same calls and order; register allocation of the default-list loops (the original keeps i in ebp and the chunk offset on the stack) and the stream state load differ (90% similar)
void Gfx::VisibilityList::LoadVisList(_SS<char> fileName)
{
    m_FileName = fileName;
    delete [] m_Chunks;
    int count = m_NumChunks;
    if (count < 4)
        count = 100;
    delete [] m_Chunks;
    m_Chunks = new _SS<char>[count];
    if (m_Chunks) {
        ifstream in;
        int size = 0;
        FILE *fp = fopen(m_FileName.c_str(), "rt");
        if (fp) {
            fseek(fp, 0, SEEK_END);
            size = ftell(fp);
            fclose(fp);
        }
        in.open(m_FileName.c_str(), ios::in);
        if (!in.fail()) {
            if (size < 10) {
                // No list yet: every chunk sees its neighbours (the chunks
                // wrap around) at the default LOD, and the list is written out.
                for (int i = 0; i < count; i++) {
                    m_Chunks[i] = "";
                    for (int j = 0; j < count; j++) {
                        int distance = abs(j - i);
                        if (distance > m_NumChunks / 2)
                            distance = m_NumChunks - distance;
                        if (abs(distance) < 5) {
                            _SS<char> entry;
                            entry.Format("%02i ", j);
                            m_Chunks[i] += "A";
                            m_Chunks[i] += entry;
                        }
                    }
                }
                in.close();
                SaveVisList();
            } else {
                while (!in.eof()) {
                    char line[256];
                    in.getline(line, 256);
                    char *found = strstr(line, "CHUNK");
                    if (found == line) {
                        int chunk = atoi(line + 5);
                        if (found && chunk >= 0 && chunk < count)
                            m_Chunks[chunk] = line + 8;
                    }
                }
                in.close();
            }
        }
    }
}

// 0x1010de20 ?SaveVisList@VisibilityList@Gfx@@QAEXXZ
void Gfx::VisibilityList::SaveVisList()
{
    int numChunks;
    if (m_Chunks && (numChunks = m_NumChunks) > 0) {
        ofstream out;
        out.open(m_FileName.c_str(), ios::out);
        out << "VisList V0.99\n";
        out << "each line must begin with 'CHUNK##' in UpperCase\n";
        out << "where ## are 2 digit representing a chunk number\n";
        out << "the formating must be a letter(form A to D) in UpperCase\n";
        out << "where A=default LOD, B,C,D => LOD level 0,1,2\n";
        out << "followed by 2 digit, representing track number\n";
        out << "Ex : CHUNK03 A01 A02 C03 D04\n\n";
        for (int i = 0; i < numChunks; i++) {
            if (m_Chunks[i].GetLength()) {
                out << "CHUNK";
                if (i < 10)
                    out << '0';
                out << i;
                out << " ";
                out << m_Chunks[i].c_str();
                out << "\n";
            }
        }
        out.close();
    }
}

#pragma optimize("a", on)

// 0x1010dfa0 ?AddVisChunk@VisibilityList@Gfx@@QAEXHHH@Z
void Gfx::VisibilityList::AddVisChunk(int chunk, int index, int lod)
{
    if (chunk < m_NumChunks) {
        if (GetVisChunkVis(chunk, index))
            DelVisChunk(chunk, index);
        _SS<char> entry;
        entry.Format("%c%02i ", lod + 'B', index);
        m_Chunks[chunk] += entry;
    }
}

// EQUIV 0x1010e0e0 ?DelVisChunk@VisibilityList@Gfx@@QAEXHH@Z: proven (renaming-equivalent); bytes differ: inlining differs: the original calls assign(const char *, size_t) and _Tidy(true) for the Right() copy and inlines them for the Left() copy; VC6 does the opposite
void Gfx::VisibilityList::DelVisChunk(int chunk, int index)
{
    if (chunk < m_NumChunks) {
        _SS<char> key;
        key.Format("%02i", index);
        int pos = m_Chunks[chunk].Find(key);
        if (pos != -1 && pos > 0) {
            key = m_Chunks[chunk].Right(m_Chunks[chunk].GetLength() - pos - 3);
            m_Chunks[chunk] = m_Chunks[chunk].Left(pos - 1);
            m_Chunks[chunk] += key;
        }
    }
}

// TODO 0x1010e360 ?GetVisChunkVis@VisibilityList@Gfx@@QAE_NHH@Z: (86% similar) the original folds basic_string::npos to -1 in the inlined find/Find (as in Graph::DataBase::VerifyFile); VC6 loads it from memory
bool Gfx::VisibilityList::GetVisChunkVis(int chunk, int index)
{
    if (chunk < m_NumChunks) {
        _SS<char> key;
        key.Format("%02i", index);
        int pos = m_Chunks[chunk].Find(key);
        if (pos != -1 && pos > 0)
            return true;
    }
    return false;
}

// TODO 0x1010e4d0 ?GetVisChunkLod@VisibilityList@Gfx@@QAEHHH@Z: (54% similar) npos folded in the original (see GetVisChunkVis)
int Gfx::VisibilityList::GetVisChunkLod(int chunk, int index)
{
    if (chunk < m_NumChunks) {
        _SS<char> key;
        key.Format("%02i", index);
        int pos = m_Chunks[chunk].Find(key);
        if (pos != -1 && pos > 0)
            return m_Chunks[chunk].GetBuffer(m_Chunks[chunk].GetLength())[pos - 1] - 'A' - 1;
    }
    return -1;
}

// 0x1010e6c0 ?DecChunk@VisibilityList@Gfx@@QAEXXZ
void Gfx::VisibilityList::DecChunk()
{
    if (--m_Selected < 0)
        m_Selected = m_NumChunks - 1;
    m_SelectedLod = GetVisChunkLod(m_CurChunk, m_Selected);
}

// 0x1010e6f0 ?IncChunk@VisibilityList@Gfx@@QAEXXZ
void Gfx::VisibilityList::IncChunk()
{
    if (++m_Selected >= m_NumChunks)
        m_Selected = 0;
    m_SelectedLod = GetVisChunkLod(m_CurChunk, m_Selected);
}

// 0x1010d6a0 ??_GVisibilityList@Gfx@@UAEPAXI@Z
// 0x1010d750 ??_E?$_SS@D@@QAEPAXI@Z
// 0x1010de00 ??_Difstream@@QAEXXZ
// 0x1010e720 ?_Freeze@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXXZ
// 0x1010e780 ??0?$_SS@D@@QAE@XZ
// 0x1010d570 _$E15
// 0x1010d580 _$E14
// 0x1010d5a0 _$E18
// 0x1010d5b0 _$E17
// 0x1010d5d0 _$E21
// 0x1010d5e0 _$E20
// 0x1010d600 _$E24
// 0x1010d610 _$E23
// 0x1010d630 _$E27
// 0x1010d640 _$E26
// 0x1010e7a0 _$E91
// 0x1010e7d0 _$E87
// 0x1010e7e0 _$E90
