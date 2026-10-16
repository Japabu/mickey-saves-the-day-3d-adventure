#include "Gfx_WriteTraverser.h"
#include "MathConstants.h"
#include "Gfx_Node.h"
#include "Graph_FileStream.h"
#include "StdBase.h"
#include <string.h>

// 0x101046d0 ?GetTypeInfo@WriteTraverser@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::WriteTraverser::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x101046e0 ?GetTypeName@WriteTraverser@Gfx@@UBEPBDXZ
char const * Gfx::WriteTraverser::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x101046f0 ??0WriteTraverser@Gfx@@QAE@XZ
Gfx::WriteTraverser::WriteTraverser() : m_pStream(0)
{
    m_pStream = new Graph::FileStream;
}

// 0x10104780 ??0WriteTraverser@Gfx@@QAE@PBD@Z
Gfx::WriteTraverser::WriteTraverser(char const *fileName) : m_pStream(0)
{
    try {
        m_pStream = new Graph::FileStream(fileName, Graph::FileStream::WriteMode);
    }
    catch (...) {
        throw;
    }
}

// 0x10104810 ??0WriteTraverser@Gfx@@QAE@PAVFileStream@Graph@@@Z
Gfx::WriteTraverser::WriteTraverser(Graph::FileStream *stream) : m_pStream(stream)
{
}

// 0x10104830 ??1WriteTraverser@Gfx@@UAE@XZ
Gfx::WriteTraverser::~WriteTraverser()
{
    if (m_pStream)
        delete m_pStream;
}

// 0x10104890 ?Traverse@WriteTraverser@Gfx@@UAEXAAVNode@2@@Z
void Gfx::WriteTraverser::Traverse(Gfx::Node &node)
{
    char header[40] = "*VisFly Project #00.25  bin*";
    Graph::Stream *stream = m_pStream;
    int length = strlen(header);
    stream->Write(length);
    stream->Write((unsigned char *)header, length);
    node.Write(this);
    m_pStream->Close();
}

// 0x10104900 ?IsInstance@WriteTraverser@Gfx@@QAEHPAVNode@2@@Z
int Gfx::WriteTraverser::IsInstance(Gfx::Node *node)
{
    return GetTraversalPath().FindNode(node);
}

// 0x10104760 ??_GWriteTraverser@Gfx@@UAEPAXI@Z
// 0x101045e0 _$E13
// 0x101045f0 _$E12
// 0x10104610 _$E16
// 0x10104620 _$E15
// 0x10104640 _$E19
// 0x10104650 _$E18
// 0x10104670 _$E22
// 0x10104680 _$E21
// 0x101046a0 _$E25
// 0x101046b0 _$E24
