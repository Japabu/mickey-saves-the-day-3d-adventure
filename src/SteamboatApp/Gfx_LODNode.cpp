#include "Gfx_LODNode.h"
#include "MathConstants.h"
#include "Gfx_Traverser.h"
#include "Gfx_View.h"
#include "Gfx_Camera.h"
#include "Gfx_WriteTraverser.h"
#include "Gfx_CurrentTransformationMatrix.h"
#include "Graph_BasicRefreshTraverser.h"
#include "Graph_FileStream.h"
#include "VisMisc.h"

// 0x100ec440 ?GetTypeInfo@LODNode@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::LODNode::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100ec450 ?GetTypeName@LODNode@Gfx@@UBEPBDXZ
char const * Gfx::LODNode::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100ec460 ??0LODNode@Gfx@@QAE@XZ
Gfx::LODNode::LODNode() : m_RangeLimits(0, 1.0f), m_Center(0.0f, 0.0f, 0.0f), m_ForceChild(-1), m_Distance(0.0f),
    m_Child(-1), m_Locked(false), m_Slave(false), m_pMasterNode(0), m_CurrentLOD(-1), m_DistanceBasedCulling(false)
{
}

// 0x100ec530 ?GetRangeLimit@LODNode@Gfx@@QAEMH@Z
float Gfx::LODNode::GetRangeLimit(int i)
{
    if (i >= 0 && i < m_RangeLimits.length())
        return m_RangeLimits[i];
    return -1.0f;
}

// 0x100ec560 ?GetRangeLimits@LODNode@Gfx@@QAEABV?$RVec@M@@XZ
const RVec<float> &Gfx::LODNode::GetRangeLimits()
{
    return m_RangeLimits;
}

// TODO 0x100ec570 ?SetRangeLimit@LODNode@Gfx@@QAEXHM@Z: the inlined Vec::verify/reallocMem keep len and e in registers (as with /Oa); a pragma here or around Vec.h does not give it
void Gfx::LODNode::SetRangeLimit(int i, float limit)
{
    if (!m_Locked)
        m_RangeLimits.setValueVerify(i, limit);
}

// TODO 0x100ec670 ?SetRangeLimits@LODNode@Gfx@@QAEXABV?$RVec@M@@@Z: the inlined RVec assignment copies with rep movsd after the length test in the original
void Gfx::LODNode::SetRangeLimits(const RVec<float> &limits)
{
    if (!m_Locked)
        m_RangeLimits = limits;
}

// 0x100ec6e0 ?GetRange@LODNode@Gfx@@QAEHM@Z
int Gfx::LODNode::GetRange(float distance)
{
    int range = -1;
    if (!m_DistanceBasedCulling)
        range = CountChildren() - 1;
    for (int i = 0; i < m_RangeLimits.length(); i++)
        if (distance < m_RangeLimits[i])
            return i;
    return range;
}

// 0x100ec730 ?Traverse@LODNode@Gfx@@UAE?AW4Response@NodeTraverser@Graph@@AAVPassiveTraverser@5@@Z
Graph::NodeTraverser::Response Gfx::LODNode::Traverse(Graph::PassiveTraverser &traverser)
{
    if (CountChildren() == 0)
        return Graph::NodeTraverser::Continue;
    if (!traverser.IsOfType(Graph::BasicRefreshTraverser::GetStaticTypeInfo())) {
        Gfx::Traverser *gfxTraverser = StdBaseCast(&traverser, (Gfx::Traverser *)0);
        if (gfxTraverser) {
            SelectLOD(gfxTraverser);
            if (m_CurrentLOD == -1)
                return Graph::NodeTraverser::Continue;
            Graph::BasicNode *child = GetChild(m_CurrentLOD);
            return traverser.Visit(*child);
        }
    }
    return ActiveNode::Traverse(traverser);
}

// TODO 0x100ec7c0 ?SelectLOD@LODNode@Gfx@@QAEXPAVTraverser@2@@Z: the original calls multVec3PointLeftAffine out of line (here inlined)
void Gfx::LODNode::SelectLOD(Gfx::Traverser *traverser)
{
    if (m_Slave) {
        m_pMasterNode->SelectLOD(traverser);
        m_CurrentLOD = m_pMasterNode->m_CurrentLOD;
        return;
    }
    if (m_ForceChild > -1) {
        m_CurrentLOD = m_ForceChild;
    } else {
        if (m_RangeLimits.length() == 0) {
            m_CurrentLOD = 0;
            return;
        }
        Vec3<float> eye = traverser->GetView()->GetCamera()->GetWorldFrom();
        m_Distance = GetEyeDistance(eye);
        if (m_Distance < 0.0f) {
            m_CurrentLOD = -1;
            return;
        }
        m_CurrentLOD = GetRange(m_Distance * traverser->GetLODScale());
    }
    m_CurrentLOD = (m_CurrentLOD < CountChildren() - 1) ? m_CurrentLOD : CountChildren() - 1;
}

// 0x100ec8c0 ?Write@LODNode@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::LODNode::Write(Gfx::WriteTraverser *traverser)
{
    unsigned char header[12] = "GfxLODNode";
    if (WriteHeaderInfo(traverser, header))
        WriteInternals(traverser);
}

// 0x100ec920 ?ReadVis@LODNode@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::LODNode::ReadVis(Graph::Stream *stream)
{
    Node::ReadInternals(stream, 1);
    ReadInternals(stream, 0);
}

// 0x100ec940 ?Read@LODNode@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::LODNode::Read(Graph::Stream *stream)
{
    int version;
    Node::Read(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x100ec970 ?SetMasterLOD@LODNode@Gfx@@QAEXPAV12@@Z
void Gfx::LODNode::SetMasterLOD(Gfx::LODNode *master)
{
    m_Slave = true;
    m_pMasterNode = master;
}

// 0x100ec990 ??1LODNode@Gfx@@MAE@XZ
Gfx::LODNode::~LODNode()
{
}

// 0x100ec9d0 ?WriteInternals@LODNode@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::LODNode::WriteInternals(Gfx::WriteTraverser *traverser)
{
    Node::WriteInternals(traverser);
    Graph::FileStream *stream = traverser->GetStream();
    stream->Write(Version);
    writeVecf(m_RangeLimits, stream);
    writeVec3f(m_Center, stream);
    stream->Write(m_ForceChild);
}

// 0x100eca20 ?ReadInternals@LODNode@Gfx@@IAEXPAVStream@Graph@@H@Z
void Gfx::LODNode::ReadInternals(Graph::Stream *stream, int version)
{
    readVecf(m_RangeLimits, stream);
    readVec3f(m_Center, stream);
    stream->Read(&m_ForceChild);
}

// TODO 0x100eca60 ?GetEyeDistance@LODNode@Gfx@@AAEMABV?$Vec3@M@@@Z: the inlined multVec3PointLeftAffine sums each row starting with elements[3][j] in the original
float Gfx::LODNode::GetEyeDistance(const Vec3<float> &eye)
{
    Vec3<float> center = Gfx::CurrentTransformationMatrix::GetInstance()->GetModelMatrix().multVec3PointLeftAffine(m_Center);
    Vec3<float> d = center - eye;
    return d.mag();
}

int const Gfx::LODNode::Version = 0;

// 0x100ec510 ??_GLODNode@Gfx@@MAEPAXI@Z
// 0x100ec350 _$E37
// 0x100ec360 _$E36
// 0x100ec380 _$E40
// 0x100ec390 _$E39
// 0x100ec3b0 _$E43
// 0x100ec3c0 _$E42
// 0x100ec3e0 _$E46
// 0x100ec3f0 _$E45
// 0x100ec410 _$E49
// 0x100ec420 _$E48

// Template and inline instances this unit also emits:
// 0x100ecb70 ??_G?$RVec@M@@UAEPAXI@Z
// 0x100ecb90 ??1?$RVec@M@@UAE@XZ
// 0x100ecc40 ??_G?$Vec@M@@UAEPAXI@Z
// 0x100ecc60 ??1?$Vec@M@@UAE@XZ
// TODO 0x100ecbb0 ?multVec3PointLeftAffine@?$TMatrix4@M@@QBE?AV?$Vec3@M@@ABV2@@Z: inline instance (body in TMatrix4.h); this unit does not emit an out-of-line copy yet
