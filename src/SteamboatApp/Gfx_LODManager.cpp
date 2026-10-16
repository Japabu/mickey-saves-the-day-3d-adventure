#include "Gfx_LODManager.h"
#include "MathConstants.h"
#include <math.h>
#include <stdlib.h>
#include "Gfx_LODNode.h"
#include "Gfx_MatrixTransform.h"
#include "Gfx_VisibilityList.h"
#include "Gfx_View.h"
#include "Gfx_CurrentTransformationMatrix.h"
#include "Gfx_BoundingVolume.h"
#include "Graph_SearchTraverser.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#pragma optimize("a", on)

// EQUIV 0x100ed9d0 ??0LODManager@Gfx@@QAE@XZ: reviewed: the original inlines _SS<char>() (empty body) and calls its base basic_string(const allocator &) with a temporary allocator in the frame, VC6 calls _SS<char>() here (inline budget); same stores in the same order around the same construction
Gfx::LODManager::LODManager()
    : m_SqrtHighCoverage(0.0f), m_SqrtLowCoverage(0.0f), m_LowCoverageFactor(0.0f), m_HighCoverageFactor(0.0f)
{
}

// EQUIV 0x100edb00 ??0LODManager@Gfx@@QAE@MMM@Z: reviewed: as the default constructor (_SS<char>() inlined in the original, called here); the float members, the call order and the vftable store are the same
Gfx::LODManager::LODManager(float fieldOfView, float lowCoverage, float highCoverage)
    : m_SqrtHighCoverage((float)sqrt(highCoverage)), m_SqrtLowCoverage((float)sqrt(lowCoverage)),
      m_LowCoverageFactor(0.0f), m_HighCoverageFactor(0.0f), m_FieldOfView(fieldOfView * 0.017453292f)
{
    ComputeCoverageFactors();
}

// 0x100edc30 ??1LODManager@Gfx@@UAE@XZ
Gfx::LODManager::~LODManager()
{
    for (int i = 0; i < m_GroupList.length(); i++)
        for (int j = 0; j < m_GroupList[i].length(); j++)
            delete m_GroupList[i][j];
}

// 0x100edd90 ?InitializeFieldOfViewCoverage@LODManager@Gfx@@QAEXMMM@Z
void Gfx::LODManager::InitializeFieldOfViewCoverage(float fieldOfView, float lowCoverage, float highCoverage)
{
    m_FieldOfView = fieldOfView * 0.017453292f;
    m_SqrtHighCoverage = (float)sqrt(highCoverage);
    m_SqrtLowCoverage = (float)sqrt(lowCoverage);
    ComputeCoverageFactors();
}

// TODO 0x100eddc0 ?SetRanges@LODManager@Gfx@@QAEXPAVNode@2@M@Z: the original calls Vec::append(const T *, int) out of line (as every unit of the binary does); VC6 inlines it here (in-class, out-of-class and auto_inline(off) definitions tried)
void Gfx::LODManager::SetRanges(Gfx::Node *root, float coverage)
{
    Graph::SearchTraverser traverser;
    traverser.SetType(Gfx::LODNode::GetStaticTypeInfo());
    traverser.Traverse(*root);
    int first = m_LodList.length();
    int last = first + traverser.GetPaths().length();
    m_LodList.verify(last);
    m_LodList.setLength(first);
    m_Coverage.verify(last);
    m_Coverage.setLength(first);
    int i;
    for (i = first; i < last; i++) {
        Gfx::LODNode *lodNode = (Gfx::LODNode *)((const Graph::Path *)traverser.GetPaths())[i].GetTop();
        m_LodList.append(&lodNode, 1);
        float sqrtCoverage = (float)sqrt(coverage);
        m_Coverage.append(&sqrtCoverage, 1);
    }
    Gfx::LODNode **lodNodes = m_LodList;
    float *coverages = m_Coverage;
    for (i = first; i < last; i++)
        ComputeRanges(lodNodes[i], coverages[i]);
}

// 0x100ee070 ?CreateGroup@LODManager@Gfx@@QAEHW4GroupManagementMethod@12@@Z
int Gfx::LODManager::CreateGroup(GroupManagementMethod method)
{
    m_GroupList.setLength(m_GroupList.length() + 1);
    m_GroupInfo.setLength(m_GroupInfo.length() + 1);
    switch (method) {
    case cQuota:
        m_GroupPriorityList.setLength(m_GroupPriorityList.length() + 1);
        m_GroupInfo[m_GroupInfo.length() - 1].groupManagementMethod = cQuota;
        m_GroupInfo[m_GroupInfo.length() - 1].offset = m_GroupPriorityList.length() - 1;
        break;
    case cVisibilityList:
        m_GroupVisibilityList.setLength(m_GroupVisibilityList.length() + 1);
        m_GroupInfo[m_GroupInfo.length() - 1].groupManagementMethod = cVisibilityList;
        m_GroupInfo[m_GroupInfo.length() - 1].offset = m_GroupVisibilityList.length() - 1;
        break;
    }
    return m_GroupList.length() - 1;
}

// TODO 0x100ee1a0 ?AddNode@LODManager@Gfx@@QAEXHPAVLODNode@2@@Z: Vec::append(const T *, int) called out of line in the original, inlined here (see SetRanges)
void Gfx::LODManager::AddNode(int group, Gfx::LODNode *lodNode)
{
    if (lodNode) {
        NodeInfo *info = new NodeInfo;
        info->lodNode = lodNode;
        info->numLevels = lodNode->CountChildren();
        info->distance = -1;
        info->inView = true;
        m_GroupList[group].append(&info, 1);
        GroupInfo &groupInfo = m_GroupInfo[group];
        switch (groupInfo.groupManagementMethod) {
        case cQuota: {
            for (int i = m_GroupPriorityList[groupInfo.offset].length(); i < info->numLevels; i++) {
                int priority = 0;
                m_GroupPriorityList[groupInfo.offset].append(&priority, 1);
            }
            break;
        }
        case cVisibilityList:
            if (groupInfo.offset >= m_GroupVisibilityList.length())
                m_GroupVisibilityList.append((Gfx::VisibilityList *)0);
            break;
        }
        Graph::BasicNode *node = lodNode;
        while (node->CountParents()) {
            node = node->GetParent(0);
            Gfx::MatrixTransform *transform = StdBaseCast(node, (Gfx::MatrixTransform *)0);
            if (transform)
                info->matList.append(transform);
        }
    }
}

// TODO 0x100ee3e0 ?InsertNode@LODManager@Gfx@@QAEXHPAVLODNode@2@H@Z: Vec::insert(const T *, int, int) called out of line in the original, inlined here (see SetRanges)
void Gfx::LODManager::InsertNode(int group, Gfx::LODNode *lodNode, int position)
{
    if (lodNode) {
        NodeInfo *info = new NodeInfo;
        info->lodNode = lodNode;
        info->numLevels = lodNode->CountChildren();
        info->distance = -1;
        info->inView = true;
        m_GroupList[group].insert(&info, 1, position);
        GroupInfo &groupInfo = m_GroupInfo[group];
        switch (groupInfo.groupManagementMethod) {
        case cQuota: {
            for (int i = m_GroupPriorityList[groupInfo.offset].length(); i < info->numLevels; i++) {
                int priority = 0;
                m_GroupPriorityList[groupInfo.offset].append(&priority, 1);
            }
            break;
        }
        case cVisibilityList:
            if (groupInfo.offset >= m_GroupVisibilityList.length())
                m_GroupVisibilityList.append((Gfx::VisibilityList *)0);
            break;
        }
        Graph::BasicNode *node = lodNode;
        while (node->CountParents()) {
            node = node->GetParent(0);
            Gfx::MatrixTransform *transform = StdBaseCast(node, (Gfx::MatrixTransform *)0);
            if (transform)
                info->matList.append(transform);
        }
    }
}

// 0x100ee620 ?SetCenterPoint@LODManager@Gfx@@QAEXABV?$Vec3@M@@@Z
void Gfx::LODManager::SetCenterPoint(const Vec3<float> &point)
{
    m_CenterPoint = point;
}

// 0x100ee640 ?UpdateLODs@LODManager@Gfx@@QAEXPAVView@2@@Z
void Gfx::LODManager::UpdateLODs(Gfx::View *view)
{
    for (int i = 0; i < m_GroupList.length(); i++) {
        switch (m_GroupInfo[i].groupManagementMethod) {
        case cQuota:
            CalculateDistanceFromCenterPoint(i, view);
            SortByDistance(i);
            SetRangesByForce(i);
            break;
        case cVisibilityList:
            SetRangesBasedOnVisibility(i);
            break;
        }
    }
}

// EQUIV 0x100ee6a0 ?SetGroupPriorityLevels@LODManager@Gfx@@QAEXHHH@Z: reviewed: registers renamed (this in ebx, group * 12 in edi, the length in ebp; here ebp/ebx/edi) with the prologue loads scheduled differently; same loads, stores and calls
void Gfx::LODManager::SetGroupPriorityLevels(int group, int level, int count)
{
    if (level > m_GroupPriorityList[m_GroupInfo[group].offset].length() - 1)
        m_GroupPriorityList[m_GroupInfo[group].offset].append(0);
    m_GroupPriorityList[m_GroupInfo[group].offset][level] = count;
}

// 0x100ee740 ?SetGroupVisibilityList@LODManager@Gfx@@QAEXHPAVVisibilityList@2@@Z
void Gfx::LODManager::SetGroupVisibilityList(int group, Gfx::VisibilityList *list)
{
    m_GroupVisibilityList[m_GroupInfo[group].offset] = list;
}

// 0x100ee760 ?SetMaxVisibleObjectsPerGroup@LODManager@Gfx@@QAEXHH@Z
void Gfx::LODManager::SetMaxVisibleObjectsPerGroup(int group, int count)
{
    m_GroupInfo[group].maxVisibleObjects = count;
}

// TODO 0x100ee780 ?SetRangesBasedOnVisibility@LODManager@Gfx@@AAEXH@Z: register allocation: the original keeps the visibility list in ebp and spills group * 24 (ours the other way round)
void Gfx::LODManager::SetRangesBasedOnVisibility(int group)
{
    int i;
    Gfx::VisibilityList *list = m_GroupVisibilityList[m_GroupInfo[group].offset];
    if (list->m_Enabled) {
        int chunk = list->m_CurChunk;
        for (i = 0; i < m_GroupList[group].length(); i++)
            m_GroupList[group][i]->lodNode->SetVisible(false);
        list->m_CurChunk = list->m_NextChunk;
        const char *state = list->GetVisChunkVis(chunk, list->m_Selected) ? "On" : "Off";
        int selected = list->m_Selected;
        m_VisibilityString.Format("CurChunk: %i , Selected %i is %s with LOD %i", chunk, selected, state,
                                  list->GetVisChunkLod(chunk, selected));
        char *entries = list->m_Chunks[list->m_CurChunk].GetBuffer(list->m_Chunks[list->m_CurChunk].GetLength());
        for (i = 1; i < list->m_Chunks[list->m_CurChunk].GetLength(); i += 4) {
            int index = atoi(&entries[i]);
            int lod = entries[i - 1] - 'A';
            if (index < m_GroupList[group].length()) {
                m_GroupList[group][index]->lodNode->SetVisible(true);
                m_GroupList[group][index]->lodNode->SetForceChild(lod - 1);
            }
        }
    } else {
        for (i = 0; i < m_GroupList[group].length(); i++)
            m_GroupList[group][i]->lodNode->SetVisible(true);
    }
}

// TODO 0x100ee950 ?CalculateDistanceFromCenterPoint@LODManager@Gfx@@AAEXHPAVView@2@@Z: the original calls SquareMatrix() and Vec3(x, y, z) out of line (inline budget) and caches the matrix list length
void Gfx::LODManager::CalculateDistanceFromCenterPoint(int group, Gfx::View *view)
{
    Vec<NodeInfo *> &list = m_GroupList[group];
    for (int i = 0; i < list.length(); i++) {
        TMatrix4<float> matrix;
        matrix.identity();
        NodeInfo *info = list[i];
        for (int j = 0; j < info->matList.length(); j++)
            matrix.multRight(info->matList[j]->GetMatrix());
        matrix.multRight(Gfx::CurrentTransformationMatrix::GetInstance()->GetViewMatrix());
        Gfx::BoundingBox box(info->lodNode->GetBoundingBox());
        box.Transform4(matrix);
        if (view->IsBoundingBoxInViewFrustum(box) == 0)
            info->inView = false;
        else
            info->inView = true;
        Vec3<float> d = m_CenterPoint - box.GetCenter();
        info->distance = (int)d.mag2();
    }
}

// EQUIV 0x100eec20 ?SortByDistance@LODManager@Gfx@@AAEXH@Z: reviewed: registers renamed (ebx/edi/esi/dl here edi/esi/edx/bl) and the flag clear swapped with an independent store; same loads, stores and tests
void Gfx::LODManager::SortByDistance(int group)
{
    Vec<NodeInfo *> &list = m_GroupList[group];
    bool sorted;
    do {
        sorted = true;
        for (int i = 0; i < list.length() - 1; i++) {
            if (list[i]->distance > list[i + 1]->distance) {
                NodeInfo *info = list[i];
                list[i] = list[i + 1];
                list[i + 1] = info;
                sorted = false;
            }
        }
    } while (!sorted);
}

// TODO 0x100eec80 ?SetRangesByForce@LODManager@Gfx@@AAEXH@Z: the original walks the node list with a pointer loaded once (no reload after SetVisible)
void Gfx::LODManager::SetRangesByForce(int group)
{
    GroupInfo &groupInfo = m_GroupInfo[group];
    int maxVisible = groupInfo.maxVisibleObjects;
    Vec<NodeInfo *> &list = m_GroupList[group];
    int count = list.length();
    if (maxVisible > count)
        maxVisible = count;
    int level = 0;
    int used = 0;
    int i = 0;
    while (i < maxVisible) {
        NodeInfo *info = list[i];
        if (info->inView) {
            if (used < m_GroupPriorityList[groupInfo.offset][level]) {
                if (level < info->numLevels)
                    info->lodNode->SetForceChild(level);
                else
                    info->lodNode->SetForceChild(info->numLevels - 1);
                info->lodNode->SetVisible(true);
                i++;
                used++;
            } else {
                level++;
                used = 0;
            }
        } else {
            info->lodNode->SetForceChild(info->numLevels - 1);
            info->lodNode->SetVisible(true);
            i++;
        }
    }
    for (i = maxVisible; i < count; i++)
        list[i]->lodNode->SetVisible(false);
}

// TODO 0x100eed90 ?ComputeRanges@LODManager@Gfx@@AAEXPAVLODNode@2@M@Z: the original calls SquareMatrix() and Vec3::operator- out of line (inline budget)
void Gfx::LODManager::ComputeRanges(Gfx::LODNode *lodNode, float coverage)
{
    if (lodNode) {
        TMatrix4<float> matrix;
        matrix.identity();
        Graph::BasicNode *node = lodNode;
        while (node->CountParents()) {
            node = node->GetParent(0);
            Gfx::MatrixTransform *transform = StdBaseCast(node, (Gfx::MatrixTransform *)0);
            if (transform)
                matrix *= *(const float *)transform->GetMatrix(); // the first element only, as in the original
        }
        lodNode->SetCenter(lodNode->GetBoundingBox().GetCenter());
        Gfx::BoundingBox box = lodNode->GetBoundingBox().TransformAffine(matrix);
        Vec3<float> diagonal = box.GetMaxima() - box.GetMinima();
        float size = diagonal.mag() * 0.5f * coverage;
        float high = size * m_HighCoverageFactor;
        float low = size * m_LowCoverageFactor;
        int levels = lodNode->CountChildren();
        if (levels > 0) {
            float delta = low - high;
            float last = (float)(levels - 1);
            for (int i = 0; i < levels; i++)
                lodNode->SetRangeLimit(i, i * delta / last + high);
        }
    }
}

// 0x100eef80 ?ComputeCoverageFactors@LODManager@Gfx@@AAEXXZ
void Gfx::LODManager::ComputeCoverageFactors()
{
    float t = (float)tan(m_FieldOfView * 0.5);
    m_LowCoverageFactor = 1.0f / (t * m_SqrtLowCoverage);
    m_HighCoverageFactor = 1.0f / (t * m_SqrtHighCoverage);
}

// Compiler-generated and template instances this unit also emits:
// 0x100edae0 ??_GLODManager@Gfx@@UAEPAXI@Z
// 0x100eefc0 ??0?$AVec@M@@QAE@HM@Z
// 0x100ef0a0 ??_G?$AVec@M@@UAEPAXI@Z
// 0x100ef0c0 ??1?$AVec@M@@UAE@XZ
// 0x100ef340 ??0?$Vec@PAVMatrixTransform@Gfx@@@@QAE@HM@Z
// 0x100ef420 ??1?$Vec@V?$Vec@PAUNodeInfo@LODManager@Gfx@@@@@@UAE@XZ
// 0x100ef470 ??1?$Vec@V?$IVec@H@@@@UAE@XZ
// 0x100ef4c0 ??1?$Vec@PAVVisibilityList@Gfx@@@@UAE@XZ
// 0x100ef4e0 ??1?$Vec@UGroupInfo@LODManager@Gfx@@@@UAE@XZ
// 0x100ef500 ??1?$Vec@PAVLODNode@Gfx@@@@UAE@XZ
// 0x100ef810 ??_G?$Vec@PAVMatrixTransform@Gfx@@@@UAEPAXI@Z
// 0x100ef830 ??1?$Vec@PAVMatrixTransform@Gfx@@@@UAE@XZ
// 0x100ef850 ??_G?$Vec@V?$Vec@PAUNodeInfo@LODManager@Gfx@@@@@@UAEPAXI@Z
// 0x100ef870 ??_G?$Vec@V?$IVec@H@@@@UAEPAXI@Z
// 0x100ef890 ??_G?$Vec@PAVVisibilityList@Gfx@@@@UAEPAXI@Z
// 0x100ef8b0 ??_G?$Vec@UGroupInfo@LODManager@Gfx@@@@UAEPAXI@Z
// 0x100ef8d0 ??_G?$Vec@PAVLODNode@Gfx@@@@UAEPAXI@Z
// 0x100efb40 ??1?$Vec@PAUNodeInfo@LODManager@Gfx@@@@UAE@XZ
// 0x100efb60 ?initElements@?$Vec@V?$Vec@PAUNodeInfo@LODManager@Gfx@@@@@@IAEXHH@Z
// 0x100eff50 ?reallocMem@?$Vec@PAVVisibilityList@Gfx@@@@IAEXHHH@Z
// 0x100f0000 ?reallocMem@?$Vec@UGroupInfo@LODManager@Gfx@@@@IAEXHHH@Z
// 0x100f0130 ?initElements@?$Vec@UGroupInfo@LODManager@Gfx@@@@IAEXHH@Z
// 0x100f0190 ?reallocMem@?$Vec@PAVLODNode@Gfx@@@@IAEXHHH@Z
// 0x100f0240 ?reallocMem@?$Vec@M@@IAEXHHH@Z
// 0x100f02f0 ?initElements@?$Vec@M@@IAEXHH@Z
// 0x100f0310 ?reallocMem@?$Vec@PAUNodeInfo@LODManager@Gfx@@@@IAEXHHH@Z
// 0x100f04a0 ??_E?$Vec@PAUNodeInfo@LODManager@Gfx@@@@UAEPAXI@Z
// 0x100f0500 ??_F?$Vec@PAUNodeInfo@LODManager@Gfx@@@@QAEXXZ
// 0x100f0510 ??_E?$IVec@H@@UAEPAXI@Z
// 0x100f0570 ??_F?$IVec@H@@QAEXXZ
// 0x100f0580 ?at@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAADI@Z
// 0x100f0610 ??0?$Vec@PAUNodeInfo@LODManager@Gfx@@@@QAE@HM@Z
// 0x100f06f0 ??4?$Vec@PAUNodeInfo@LODManager@Gfx@@@@QAEAAV0@ABV0@@Z
// 0x100ed8e0 _$E15
// 0x100ed8f0 _$E14
// 0x100ed910 _$E18
// 0x100ed920 _$E17
// 0x100ed940 _$E21
// 0x100ed950 _$E20
// 0x100ed970 _$E24
// 0x100ed980 _$E23
// 0x100ed9a0 _$E27
// 0x100ed9b0 _$E26
// 0x100f0450 _$E85
// 0x100f0480 _$E81
// 0x100f0490 _$E84

// Template and inline instances this unit also emits:
// TODO 0x100ef0e0 ?Format@?$_SS@D@@QAAXPBDZZ: 67% - the inline instance compiles differently here
// TODO 0x100ef520 ?append@?$Vec@PAVLODNode@Gfx@@@@QAEXPBQAVLODNode@Gfx@@H@Z: inline instance (body in the header); this unit does not emit an out-of-line copy yet
// TODO 0x100ef5c0 ?append@?$Vec@M@@QAEXPBMH@Z: inline instance (body in the header); this unit does not emit an out-of-line copy yet
// TODO 0x100ef660 ?append@?$Vec@PAUNodeInfo@LODManager@Gfx@@@@QAEXPBQAUNodeInfo@LODManager@Gfx@@H@Z: inline instance (body in the header); this unit does not emit an out-of-line copy yet
// TODO 0x100ef700 ?insert@?$Vec@PAUNodeInfo@LODManager@Gfx@@@@QAEXPBQAUNodeInfo@LODManager@Gfx@@HH@Z: inline instance (body in the header); this unit does not emit an out-of-line copy yet
// TODO 0x100ef8f0 ?reallocMem@?$Vec@V?$Vec@PAUNodeInfo@LODManager@Gfx@@@@@@IAEXHHH@Z: 50% - the inline instance compiles differently here
// TODO 0x100efc30 ?reallocMem@?$Vec@V?$IVec@H@@@@IAEXHHH@Z: 23% - the inline instance compiles differently here
// TODO 0x100efe80 ?initElements@?$Vec@V?$IVec@H@@@@IAEXHH@Z: 38% - the inline instance compiles differently here
// TODO 0x100f03c0 ?multRight@?$SquareMatrix@M$03@@QAEHABV1@@Z: inline instance (body in the header); this unit does not emit an out-of-line copy yet
