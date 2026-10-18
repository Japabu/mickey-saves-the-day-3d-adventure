#pragma optimize("a", on) // evidence: WriteInternals keeps the path list pointer across the writeText calls (no-alias code)
#include "Gfx_FileNode.h"
#include "MathConstants.h"
#include "StdBase.h"
#include "Gfx_WriteTraverser.h"
#include "Graph_FileStream.h"
#include "VisMisc.h"
#include "Graph_DataBase.h"
#include <string.h>
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x1010ede0 ?GetTypeInfo@FileNode@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::FileNode::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x1010edf0 ?GetTypeName@FileNode@Gfx@@UBEPBDXZ
char const * Gfx::FileNode::GetTypeName() const
{
    return s_TypeInfo.typeName;
}


// 0x1010ee00 ??0FileNode@Gfx@@QAE@XZ
Gfx::FileNode::FileNode() : field_174(false)
{
    m_pStream = new Graph::FileStream;
}

// 0x1010ef00 ??1FileNode@Gfx@@MAE@XZ
Gfx::FileNode::~FileNode()
{
    Close();
    delete m_pStream;
}

// TODO 0x1010efa0 ?RemoveDirectoryPath@FileNode@Gfx@@QAEXPAD@Z: 84% - the inlined Vec::remove shifts the Text elements with the refcount code in another register order
void Gfx::FileNode::RemoveDirectoryPath(char *path)
{
    for (int i = 0; i < m_DirectoryPaths.length(); i++) {
        if (!strcmp(m_DirectoryPaths[i].getConstPtr(), path))
            m_DirectoryPaths.remove(i, 1);
    }
}

// 0x1010f0f0 ?RemoveAllDirectoryPaths@FileNode@Gfx@@QAEXXZ
void Gfx::FileNode::RemoveAllDirectoryPaths()
{
    m_DirectoryPaths.setLength(0);
    m_DirectoryPaths.shrink();
}

#pragma optimize("a", off) // evidence: Open reloads m_pStream after every call
// TODO 0x1010f160 ?Open@FileNode@Gfx@@UAE_NXZ: 80% - the inlined Vec::append(Text) loads len and alloclen in another order (as ReadInternals)
bool Gfx::FileNode::Open()
{
    if (!mCopyOfContents) {
        int length = strlen(m_FileName.getConstPtr());
        if (length) {
            if (!m_pStream)
                m_pStream = new Graph::FileStream;
            if (m_pStream->IsOpen())
                m_pStream->Close();
            if (!m_DirectoryPaths.length())
                m_DirectoryPaths.append(Text("."));
            if (!m_pStream->Open(m_FileName, m_DirectoryPaths, Graph::FileStream::ReadMode)) {
                field_174 = true;
                return false;
            }
            mCopyOfContents = (Gfx::Node *)Graph::DataBase::GetInstance()->ReadDB(m_pStream);
            if (mCopyOfContents) {
                InsertChild(GetEndChildIterator(), mCopyOfContents);
                m_pStream->Close();
                if (field_145)
                    BuildBBHierTraversal();
                return true;
            }
            m_pStream->Close();
            field_174 = true;
        }
    }
    return false;
}
#pragma optimize("a", on)

// 0x1010f3c0 ?Render@FileNode@Gfx@@UAE?AW4Response@NodeTraverser@Graph@@PAVRenderTraverser@2@@Z
Graph::NodeTraverser::Response Gfx::FileNode::Render(Gfx::RenderTraverser *traverser)
{
    if (field_144 && !mCopyOfContents) {
        Open();
        return Graph::NodeTraverser::Continue;
    }
    return IONode::Render(traverser);
}

// 0x1010f3f0 ?ReadVis@FileNode@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::FileNode::ReadVis(Graph::Stream *stream)
{
    IONode::ReadVis(stream);
    ReadInternals(stream, 0);
}

#pragma optimize("a", off) // evidence: ReadInternals reloads the path list length after the inlined reallocMem
// 0x1010f410 ?Read@FileNode@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::FileNode::Read(Graph::Stream *stream)
{
    IONode::Read(stream);
    int version;
    stream->Read(&version);
    ReadInternals(stream, version);
}

// TODO 0x1010f440 ?ReadInternals@FileNode@Gfx@@IAEXPAVStream@Graph@@H@Z: (97% similar) one instruction choice in the inlined append (len + 1 as lea from a reloaded len in the original, inc here)
void Gfx::FileNode::ReadInternals(Graph::Stream *stream, int)
{
    m_FileName = readText(stream);
    int nPaths;
    stream->Read(&nPaths);
    for (int i = 0; i < nPaths; i++)
        m_DirectoryPaths.append(readText(stream));
}

#pragma optimize("a", on) // back to no-alias code
// 0x1010f660 ?Write@FileNode@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::FileNode::Write(Gfx::WriteTraverser *traverser)
{
    unsigned char header[10] = "FileNode";
    if (WriteHeaderInfo(traverser, header))
        WriteInternals(traverser);
}

// 0x1010f6b0 ?WriteInternals@FileNode@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::FileNode::WriteInternals(Gfx::WriteTraverser *traverser)
{
    IONode::WriteInternals(traverser);
    Graph::FileStream *stream = traverser->GetStream();
    writeText(m_FileName, stream);
    int nPaths = m_DirectoryPaths.length();
    stream->Write(nPaths);
    for (int i = 0; i < nPaths; i++)
        writeText(m_DirectoryPaths[i], stream);
}

int const Gfx::FileNode::Version = 0;

// 0x1010eee0 ??_GFileNode@Gfx@@MAEPAXI@Z
// 0x1010ecf0 _$E41
// 0x1010ed00 _$E40
// 0x1010ed20 _$E44
// 0x1010ed30 _$E43
// 0x1010ed50 _$E47
// 0x1010ed60 _$E46
// 0x1010ed80 _$E50
// 0x1010ed90 _$E49
// 0x1010edb0 _$E53
// 0x1010edc0 _$E52
// 0x1010f8f0 _$E83
// 0x1010f920 _$E79
// 0x1010f930 _$E82

// Template and inline instances this unit also emits:
// TODO 0x1010f390 ??0Text@@QAE@ABV0@@Z: (78% similar) inline instance (body in the header); this unit does not emit an out-of-line copy yet
// TODO 0x1010f700 ??0?$Vec@VText@@@@QAE@ABV0@@Z: (62% similar) inline instance (body in the header); this unit does not emit an out-of-line copy yet
// TODO 0x1010f830 ?initElements@?$Vec@VText@@@@IAEXHH@Z: 75% - the inline instance compiles differently here

// the run-time type of Gfx::FileNode (values from the original's data section)
StdBase::TypeInfo const Gfx::FileNode::s_TypeInfo = { "Gfx::FileNode", &Gfx::IONode::s_TypeInfo };
