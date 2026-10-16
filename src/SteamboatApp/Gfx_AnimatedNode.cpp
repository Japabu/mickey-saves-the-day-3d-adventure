#include "Gfx_AnimatedNode.h"
#include "MathConstants.h"
#include "StdBase.h"
#include "Gfx_Traverser.h"
#include "Gfx_WriteTraverser.h"
#include "Graph_FileStream.h"
#include "Graph_BasicRefreshTraverser.h"

// 0x1010e8e0 ?GetTypeInfo@AnimatedNode@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::AnimatedNode::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x1010e8f0 ?GetTypeName@AnimatedNode@Gfx@@UBEPBDXZ
char const * Gfx::AnimatedNode::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x1010e900 ??0AnimatedNode@Gfx@@QAE@XZ
Gfx::AnimatedNode::AnimatedNode()
    : m_FixedFrame(-1), field_148(-1), m_FrameRate(10), m_CurrentFrame(0.0f), m_StartFrame(0.0f), field_158(false),
      m_LastTime(0), m_TimeScale(0.001f), m_FirstFrame(true), m_Running(true), m_Loop(false)
{
}

// TODO 0x1010e990 ?Traverse@AnimatedNode@Gfx@@UAE?AW4Response@NodeTraverser@Graph@@AAVPassiveTraverser@5@@Z: VC6 keeps 0 in ebx (cmp/stores with bl) where the original uses test and immediates; the fixed-frame Visit is not merged into the tail in the original
Graph::NodeTraverser::Response Gfx::AnimatedNode::Traverse(Graph::PassiveTraverser &traverser)
{
    int nChildren = CountChildren();
    if (nChildren == 0)
        return Graph::NodeTraverser::Continue;
    if (!traverser.IsOfType(Graph::BasicRefreshTraverser::GetStaticTypeInfo())) {
        Gfx::Traverser *gfxTraverser = StdBaseCast(&traverser, (Gfx::Traverser *)0);
        if (gfxTraverser) {
            if (m_FixedFrame > -1 && nChildren > m_StartFrame)
                return traverser.Visit(*GetChild((int)m_StartFrame));
            if (m_FirstFrame) {
                m_CurrentFrame = m_StartFrame;
                m_FirstFrame = false;
                m_LastTime = traverser.GetFrameTime();
            }
            unsigned long time = traverser.GetFrameTime();
            float elapsed = (float)(time - m_LastTime);
            m_LastTime = time;
            if (m_Running) {
                m_CurrentFrame += elapsed * (m_FrameRate * m_TimeScale);
                if (m_Loop) {
                    while (m_CurrentFrame > CountChildren() - 1)
                        m_CurrentFrame -= CountChildren();
                    while (m_CurrentFrame < 0.0f)
                        m_CurrentFrame += CountChildren();
                } else {
                    float last = (float)(CountChildren() - 1);
                    if (m_CurrentFrame > last)
                        m_CurrentFrame = last;
                    if (m_CurrentFrame < 0.0f)
                        m_CurrentFrame = 0.0f;
                }
            }
            return traverser.Visit(*GetChild((int)m_CurrentFrame));
        }
    }
    return ActiveNode::Traverse(traverser);
}

// 0x1010eb90 ?Write@AnimatedNode@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::AnimatedNode::Write(Gfx::WriteTraverser *traverser)
{
    unsigned char header[16] = "GfxAnimatedNode";
    if (WriteHeaderInfo(traverser, header))
        WriteInternals(traverser);
}

// 0x1010ebf0 ?Read@AnimatedNode@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::AnimatedNode::Read(Graph::Stream *stream)
{
    int version;
    Node::Read(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x1010ec20 ??1AnimatedNode@Gfx@@MAE@XZ
Gfx::AnimatedNode::~AnimatedNode()
{
}

// 0x1010ec30 ?WriteInternals@AnimatedNode@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::AnimatedNode::WriteInternals(Gfx::WriteTraverser *traverser)
{
    Node::WriteInternals(traverser);
    Graph::FileStream *stream = traverser->GetStream();
    stream->Write(Version);
    stream->Write(m_FixedFrame);
    stream->Write(m_StartFrame);
    stream->Write(m_FrameRate);
    stream->Write((short)m_Loop);
}

// 0x1010ec90 ?ReadInternals@AnimatedNode@Gfx@@IAEXPAVStream@Graph@@H@Z
void Gfx::AnimatedNode::ReadInternals(Graph::Stream *stream, int version)
{
    stream->Read(&m_FixedFrame);
    stream->Read(&m_StartFrame);
    stream->Read(&m_FrameRate);
    if (version > 0) {
        short loop;
        stream->Read(&loop);
        m_Loop = loop != 0;
    }
}

// the run-time type of Gfx::AnimatedNode (values from the original's data section)
StdBase::TypeInfo const Gfx::AnimatedNode::s_TypeInfo = { "Gfx::AnimatedNode", &Gfx::Node::s_TypeInfo };
int const Gfx::AnimatedNode::Version = 1;

// 0x1010e970 ??_GAnimatedNode@Gfx@@MAEPAXI@Z
// 0x1010e7f0 _$E37
// 0x1010e800 _$E36
// 0x1010e820 _$E40
// 0x1010e830 _$E39
// 0x1010e850 _$E43
// 0x1010e860 _$E42
// 0x1010e880 _$E46
// 0x1010e890 _$E45
// 0x1010e8b0 _$E49
// 0x1010e8c0 _$E48
