#include "Gfx_Node.h"
#include "MathConstants.h"
#include "Gfx_PickTraverser.h"
#include "Gfx_RenderTraverser.h"
#include "Gfx_Traverser.h"
#include "Graph_NodeTraverser.h"
#include "Graph_PassiveTraverser.h"
#include "Graph_Stream.h"
#include "Graph_FileStream.h"
#include "Graph_DataBase.h"
#include "Gfx_Attribute.h"
#include "Gfx_WriteTraverser.h"
#include "VisMisc.h"
#include "Gfx_Assert.h"
#include <float.h>
#include "Gfx_Polygon3f.h"
#include "Gfx_Shape.h"
#include "Gfx_MatrixTransform.h"
#include <string.h>
#include "StdBase.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100102b0 ?ReadVis2@Node@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::Node::ReadVis2(Graph::Stream *stream)
{
    stream = stream;
}

// 0x100e3300 ?GetTypeInfo@Node@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::Node::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100e3310 ?GetTypeName@Node@Gfx@@UBEPBDXZ
char const * Gfx::Node::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100e3930 ?Pick@Node@Gfx@@UAE?AW4Response@NodeTraverser@Graph@@PAVPickTraverser@2@@Z
Graph::NodeTraverser::Response Gfx::Node::Pick(Gfx::PickTraverser *pPickTraverser)
{
    pPickTraverser = pPickTraverser;
    return Graph::NodeTraverser::Response(1);
}

// 0x100e3940 ?HandleEvent@Node@Gfx@@UAE?AW4Response@NodeTraverser@Graph@@PAVTraverser@2@@Z
Graph::NodeTraverser::Response Gfx::Node::HandleEvent(Gfx::Traverser *pTraverser)
{
    pTraverser = pTraverser;
    return Graph::NodeTraverser::Response(1);
}

// 0x100e3950 ?Render@Node@Gfx@@UAE?AW4Response@NodeTraverser@Graph@@PAVRenderTraverser@2@@Z
Graph::NodeTraverser::Response Gfx::Node::Render(Gfx::RenderTraverser *pTraverser)
{
    pTraverser = pTraverser;
    return Graph::NodeTraverser::Response(1);
}

// 0x100e3320 ??0Node@Gfx@@QAE@XZ
Gfx::Node::Node() : m_UseBoundingBox(true), m_BBoxHierNodeParent(0), m_Data(0), m_Visible(true),
    m_EnableBBoxHierIntersections(false), m_EnableAtomBoundingBoxIntersections(false),
    m_UseSimpleIntersections(false), m_BBoxHierIntersectionLevel(0),
    m_AtomBoundingBoxIntersectionScale(1.0f, 1.0f, 1.0f), m_IntersectionTestType(None)
{
    IncrementTypeCount();
}

// 0x100e3600 ??0Node@Gfx@@QAE@ABV01@@Z
Gfx::Node::Node(const Gfx::Node &node) : Graph::ActiveNode(node), m_UseBoundingBox(node.m_UseBoundingBox),
    m_BBoxHierNodeParent(0), m_Data(node.m_Data), m_Visible(node.m_Visible),
    m_EnableBBoxHierIntersections(node.m_EnableBBoxHierIntersections),
    m_EnableAtomBoundingBoxIntersections(node.m_EnableAtomBoundingBoxIntersections),
    m_UseSimpleIntersections(node.m_UseSimpleIntersections),
    m_BBoxHierIntersectionLevel(node.m_BBoxHierIntersectionLevel),
    m_AtomBoundingBoxIntersectionScale(node.m_AtomBoundingBoxIntersectionScale),
    m_IntersectionTestType(node.m_IntersectionTestType)
{
    IncrementTypeCount();
}

// EQUIV 0x100e3780 ??1Node@Gfx@@UAE@XZ: reviewed: the inlined ~BVolHierNodeFactory loop test compares with ebx, which holds 0 there, where VC6 compares with an immediate 0; the rest is identical
Gfx::Node::~Node()
{
    DecrementTypeCount();
}

// 0x100e38b0 ?UseBoundingBox@Node@Gfx@@QAEX_N@Z
void Gfx::Node::UseBoundingBox(bool use)
{
    if (use != m_UseBoundingBox) {
        int i;
        if (m_UseBoundingBox) {
            for (i = 0; i < CountParents(); i++)
                RemoveObserver(FindObserver(GetParent(i)));
        } else {
            for (i = 0; i < CountParents(); i++)
                AddObserver(GetParent(i));
        }
        m_UseBoundingBox = use;
    }
}

// 0x100e3960 ?Write@Node@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::Node::Write(Gfx::WriteTraverser *traverser)
{
    unsigned char header[10] = "GfxNode";
    if (WriteHeaderInfo(traverser, header))
        WriteInternals(traverser);
}

// 0x100e39b0 ?ReadVis@Node@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::Node::ReadVis(Graph::Stream *stream)
{
    Graph::BasicNode *parent = Graph::DataBase::GetInstance()->GetCurrentParentNode();
    if (parent)
        parent->AddChild(this);
    if (IsOfType(Gfx::Shape::GetStaticTypeInfo()))
        ReadInternals(stream, 0);
    else
        ReadInternals(stream, 1);
}

// 0x100e3a70 ?Read@Node@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::Node::Read(Graph::Stream *stream)
{
    int version;
    Graph::BasicNode *parent = Graph::DataBase::GetInstance()->GetCurrentParentNode();
    if (parent)
        parent->AddChild(this);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x100e3ab0 ?SetVisible@Node@Gfx@@QAEX_N@Z
void Gfx::Node::SetVisible(bool visible)
{
    if (IsVisible() != visible) {
        if (m_VisibilityContext)
            m_VisibilityContext->GetValue(this) = visible;
        else
            m_Visible = visible;
    }
}

// 0x100e3b00 ?IsVisible@Node@Gfx@@QAE_NXZ
bool Gfx::Node::IsVisible()
{
    if (m_VisibilityContext)
        return m_VisibilityContext->GetValue(this);
    return m_Visible;
}

// 0x100e3b40 ?GetNumberOfAtoms@Node@Gfx@@UBEHXZ
int Gfx::Node::GetNumberOfAtoms() const
{
    return CountChildren();
}

// 0x100e3b50 ?GetNumberOfAtomBoundingBoxes@Node@Gfx@@UBEHXZ
int Gfx::Node::GetNumberOfAtomBoundingBoxes() const
{
    return CountChildren();
}

// EQUIV 0x100e3b60 ?GetAtomBoundingBox@Node@Gfx@@UAE?AVBoundingBox@2@H@Z: reviewed: the returned box's copy also stores the BoundingVolume vftable first (overwritten by the BoundingBox one) and orders the independent stores differently
Gfx::BoundingBox Gfx::Node::GetAtomBoundingBox(int i)
{
    // a child's box, in this node's coordinates when the child is a transform
    Gfx::Node *child = StdBaseCast(GetChild(i), (Gfx::Node *)0);
    Gfx::MatrixTransform *transform = StdBaseCast(GetChild(i), (Gfx::MatrixTransform *)0);
    if (transform && child->GetBoundingBox().IsValid())
        return transform->GetBoundingBox().TransformAffine(transform->GetMatrix());
    return child->GetBoundingBox();
}

// 0x100e3c60 ?SetBBoxHierNodeParent@Node@Gfx@@QAEXPAV?$BVolHierNode@VBoundingBox@Gfx@@@2@@Z
void Gfx::Node::SetBBoxHierNodeParent(Gfx::BVolHierNode<Gfx::BoundingBox> *parent)
{
    m_BBoxHierNodeParent = parent;
}

// 0x100e3c70 ?GetBBoxHierNodeParent@Node@Gfx@@QAEPAV?$BVolHierNode@VBoundingBox@Gfx@@@2@XZ
Gfx::BVolHierNode<Gfx::BoundingBox> *Gfx::Node::GetBBoxHierNodeParent()
{
    return m_BBoxHierNodeParent;
}

// 0x100e3c80 ?CanAddChild@Node@Gfx@@MAE_NPAVBasicNode@Graph@@@Z
bool Gfx::Node::CanAddChild(Graph::BasicNode *child)
{
    return StdBaseCast(child, (Gfx::Node *)0) != 0;
}

// 0x100e3cd0 ?CanAddAttribute@Node@Gfx@@MAE_NPAVBasicAttribute@Graph@@@Z
bool Gfx::Node::CanAddAttribute(Graph::BasicAttribute *attribute)
{
    return StdBaseCast(attribute, (Gfx::Attribute *)0) != 0;
}

// 0x100e3d20 ?ChildAdded@Node@Gfx@@MAEXPAVBasicNode@Graph@@@Z
void Gfx::Node::ChildAdded(Graph::BasicNode *child)
{
    ((Gfx::Node *)child)->ScheduleRefresh(false);
    ActiveNode::ChildAdded(child);
}

// 0x100e3d40 ?ChildRemoved@Node@Gfx@@MAEXPAVBasicNode@Graph@@@Z
void Gfx::Node::ChildRemoved(Graph::BasicNode *child)
{
    ScheduleRefresh(false);
    ActiveNode::ChildRemoved(child);
}

// 0x100e3d60 ?ParentAdded@Node@Gfx@@MAEXPAVBasicNode@Graph@@@Z
void Gfx::Node::ParentAdded(Graph::BasicNode *parent)
{
    if (m_UseBoundingBox)
        AddObserver(parent);
    ActiveNode::ParentAdded(parent);
}

// 0x100e3d90 ?ParentRemoved@Node@Gfx@@MAEXPAVBasicNode@Graph@@@Z
void Gfx::Node::ParentRemoved(Graph::BasicNode *parent)
{
    if (m_UseBoundingBox)
        RemoveObserver(FindObserver(parent));
    ActiveNode::ParentRemoved(parent);
}

// 0x100e3dd0 ?Refresh@Node@Gfx@@MAEXAAVBasicRefreshTraverser@Graph@@@Z
void Gfx::Node::Refresh(Graph::BasicRefreshTraverser &traverser)
{
    if (m_UseBoundingBox)
        m_BBoxHierarchy.Refresh(this, traverser.GetRefreshType());
    ActiveNode::Refresh(traverser);
}

// TODO 0x100e3e30 ?WriteHeaderInfo@Node@Gfx@@IAEHPAVWriteTraverser@2@PAE@Z: PushNode's inlined Vec::append keeps len in a register across operator new (as with /Oa); everything before it matches
int Gfx::Node::WriteHeaderInfo(Gfx::WriteTraverser *traverser, unsigned char *header)
{
    char name[256];
    int instance = traverser->IsInstance(this);
    strcpy(name, instance > -1 ? "INSTANCE" : (char *)header);
    Graph::FileStream *stream = traverser->GetStream();
    short length = strlen(name);
    stream->Write(length);
    stream->Write((unsigned char *)name, length);
    if (instance > -1) {
        stream->Write(instance);
        return 0;
    }
    traverser->PushNode(*this);
    return 1;
}

// 0x100e3f50 ?WriteInternals@Node@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::Node::WriteInternals(Gfx::WriteTraverser *traverser)
{
    Graph::FileStream *stream = traverser->GetStream();
    stream->Write(Version);
    writeText(GetName(), stream);
    stream->Write((short)!IsVisible());
    int nAttributes = CountAttributes();
    stream->Write(nAttributes);
    int i;
    for (i = 0; i < nAttributes; i++)
        ((Gfx::Attribute *)GetAttribute(i))->Write(traverser);
    int nChildren = CountChildren();
    traverser->GetStream()->Write(nChildren);
    for (i = 0; i < nChildren; i++)
        ((Gfx::Node *)GetChild(i))->Write(traverser);
}

// 0x100e3fe0 ?ReadInternals@Node@Gfx@@IAEXPAVStream@Graph@@H@Z
void Gfx::Node::ReadInternals(Graph::Stream *stream, int version)
{
    SetName(readText(stream));
    short hidden;
    stream->Read(&hidden);
    SetVisible(hidden == 0);
    if (version > 0) {
        int nAttributes = 0;
        stream->Read(&nAttributes);
        int i;
        for (i = 0; i < nAttributes; i++)
            Graph::DataBase::GetInstance()->ReadAttribute(stream, this);
        int nChildren = 0;
        stream->Read(&nChildren);
        for (i = 0; i < nChildren; i++)
            Graph::DataBase::GetInstance()->ReadNode(stream, this);
    }
}

// 0x100e4170 ?PushAttributes@Node@Gfx@@QAEXPAVPassiveTraverser@Graph@@@Z
void Gfx::Node::PushAttributes(Graph::PassiveTraverser *traverser)
{
    for (int i = 0; i < CountAttributes(); i++)
        ((Gfx::Attribute *)GetAttribute(i))->Push(traverser);
}

// 0x100e41a0 ?PushAttribute@Node@Gfx@@QAEXPAVPassiveTraverser@Graph@@H@Z
void Gfx::Node::PushAttribute(Graph::PassiveTraverser *traverser, int i)
{
    ((Gfx::Attribute *)GetAttribute(i))->Push(traverser);
}

// 0x100e41c0 ?PopAttributes@Node@Gfx@@QAEXPAVPassiveTraverser@Graph@@@Z
void Gfx::Node::PopAttributes(Graph::PassiveTraverser *traverser)
{
    for (int i = 0; i < CountAttributes(); i++)
        ((Gfx::Attribute *)GetAttribute(i))->Pop(traverser);
}

// the run-time type of Gfx::Node (values from the original's data section)
StdBase::TypeInfo const Gfx::Node::s_TypeInfo = { "Gfx::Node", &Graph::ActiveNode::s_TypeInfo };
int const Gfx::Node::Version = 1;

// 0x100e3a00 ?GetInstance@DataBase@Graph@@SAPAV12@XZ
// 0x100e35e0 ??_GNode@Gfx@@UAEPAXI@Z
// TODO 0x100e3490 ??0BoundingBox@Gfx@@QAE@XZ: the out-of-line copy loads GetMaximum() per element (fld/fstp) and drops the BoundingVolume vptr store; the inlined copies in Node() match
// 0x100e34d0 ??1BoundingBox@Gfx@@UAE@XZ
// 0x100e35c0 ??_GBoundingBox@Gfx@@UAEPAXI@Z
// 0x100e4260 ??1?$BVolHierNode@VBoundingBox@Gfx@@@Gfx@@UAE@XZ
// 0x100e4580 ??_G?$BVolHierNode@VBoundingBox@Gfx@@@Gfx@@UAEPAXI@Z
// 0x100e45a0 ??_G?$BVolHierarchy@VBoundingBox@Gfx@@@Gfx@@UAEPAXI@Z
// 0x100e45c0 ??1?$BVolHierarchy@VBoundingBox@Gfx@@@Gfx@@UAE@XZ
// 0x100e5090 ?clamp@@YAMMMM@Z
// 0x100e5db0 ??0?$Vec@H@@QAE@HM@Z


// 0x100e3210 _$E33
// 0x100e3220 _$E32
// 0x100e3240 _$E36
// 0x100e3250 _$E35
// 0x100e3270 _$E39
// 0x100e3280 _$E38
// 0x100e32a0 _$E42
// 0x100e32b0 _$E41
// 0x100e32d0 _$E45
// 0x100e32e0 _$E44
// 0x100e4f00 _$E104
// 0x100e4f30 _$E100
// 0x100e4f40 _$E103

// Template and inline instances this unit also emits:
// 0x1000eb50 ?_Eos@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXI@Z
// 0x1000ed00 ?_Split@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXXZ
// 0x1000ee10 ?max_size@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QBEIXZ
// 0x100e41f0 ??1?$Vec@UBVolHierCenterRef@Gfx@@@@UAE@XZ
// 0x100e4210 ??1?$Vec@V?$Vec@UBVolHierCenterRef@Gfx@@@@@@UAE@XZ
// 0x100e4560 ??_G?$Vec@V?$Vec@UBVolHierCenterRef@Gfx@@@@@@UAEPAXI@Z
// 0x100e46c0 ?_Xran@?$vector@_NV?$allocator@_N@std@@@std@@IBEXXZ
// 0x100e47a0 ??0?$Vec@V?$Vec@UBVolHierCenterRef@Gfx@@@@@@QAE@HM@Z
// 0x100e6170 ??_E?$Vec@UBVolHierCenterRef@Gfx@@@@UAEPAXI@Z
// 0x100e61d0 ??_F?$Vec@UBVolHierCenterRef@Gfx@@@@QAEXXZ
// 0x100e61e0 ??0?$Vec@UBVolHierCenterRef@Gfx@@@@QAE@HM@Z

// qsort comparators of BVolHierarchy::Build: the atoms' centres along x, y and z
// 0x100e5cc0 ?BVolHierCenterRefCompareFunc0@@YAHPBX0@Z
int BVolHierCenterRefCompareFunc0(const void *a, const void *b)
{
    const Gfx::BVolHierCenterRef *refA = (const Gfx::BVolHierCenterRef *)a;
    const Gfx::BVolHierCenterRef *refB = (const Gfx::BVolHierCenterRef *)b;
    if (refA->vCenter.e[0] < refB->vCenter.e[0])
        return -1;
    if (refA->vCenter.e[0] > refB->vCenter.e[0])
        return 1;
    return 0;
}

// 0x100e5cf0 ?BVolHierCenterRefCompareFunc1@@YAHPBX0@Z
int BVolHierCenterRefCompareFunc1(const void *a, const void *b)
{
    const Gfx::BVolHierCenterRef *refA = (const Gfx::BVolHierCenterRef *)a;
    const Gfx::BVolHierCenterRef *refB = (const Gfx::BVolHierCenterRef *)b;
    if (refA->vCenter.e[1] < refB->vCenter.e[1])
        return -1;
    if (refA->vCenter.e[1] > refB->vCenter.e[1])
        return 1;
    return 0;
}

// 0x100e5d20 ?BVolHierCenterRefCompareFunc2@@YAHPBX0@Z
int BVolHierCenterRefCompareFunc2(const void *a, const void *b)
{
    const Gfx::BVolHierCenterRef *refA = (const Gfx::BVolHierCenterRef *)a;
    const Gfx::BVolHierCenterRef *refB = (const Gfx::BVolHierCenterRef *)b;
    if (refA->vCenter.e[2] < refB->vCenter.e[2])
        return -1;
    if (refA->vCenter.e[2] > refB->vCenter.e[2])
        return 1;
    return 0;
}

// BVolHierarchy.cxx of the graphics library (its assertions name that file)
// 0x100e60b0 ?Prune@?$BVolHierarchy@VBoundingBox@Gfx@@@Gfx@@AAEXPAV?$BVolHierNode@VBoundingBox@Gfx@@@2@@Z
template <class T>
void Gfx::BVolHierarchy<T>::Prune(Gfx::BVolHierNode<T> *pHierNode2)
{
    GFX_ASSERT(pHierNode2 != NULL, "D:\\tugboat\\code\\SDK\\Scene\\Graphics\\BVolHierarchy.cxx", 1428)
    if (pHierNode2->GetChild(0)) {
        Prune(pHierNode2->GetChild(0));
        pHierNode2->SetChild(0, 0);
    }
    if (pHierNode2->GetChild(1)) {
        Prune(pHierNode2->GetChild(1));
        pHierNode2->SetChild(1, 0);
    }
}

// TODO 0x100e48a0 ?Refresh@?$BVolHierNode@VBoundingBox@Gfx@@@Gfx@@IAEXXZ: 26% similar; the original walks the atom indices with a pointer and a down-counter, keeps this in ebp and calls BoundingBox()'s constructor for the new volume out of line
template <class T>
void Gfx::BVolHierNode<T>::Refresh()
{
    // the volume of this node's atoms again, then the children's, and the parent grows by it
    T volume;
    int nAtoms = m_AtomIndices.length();
    if (nAtoms && !m_Node->CountChildren())
        return;
    for (int i = 0; i < nAtoms; i++)
        volume += m_Node->GetAtomBoundingBox(m_AtomIndices[i]);
    if (!m_AtomBoundingVolume)
        m_AtomBoundingVolume = new T;
    *m_AtomBoundingVolume = volume;
    *(T *)this = volume;
    if (m_apChild[0] || m_apChild[1]) {
        for (int child = 0; child < 2; child++)
            if (m_apChild[child])
                m_apChild[child]->Refresh();
    }
    if (m_pParent)
        *m_pParent += *this;
}

// TODO 0x100e5f30 ?InitAtomCenterRef@?$BVolHierarchy@VBoundingBox@Gfx@@@Gfx@@AAEXPAVNode@2@AAVBoundingBox@2@@Z: 41% similar; registers renamed (this in edi, the vector in ebp) and the inlined initElements fills by an overlapping copy in the original
template <class T>
void Gfx::BVolHierarchy<T>::InitAtomCenterRef(Gfx::Node *node, T &volume)
{
    // every atom's centre and index, and the volume around all of them
    m_vAtomCenterRef.setLength(m_nAtoms);
    Gfx::BVolHierCenterRef *ref = m_vAtomCenterRef;
    for (int i = 0; i < m_nAtoms; i++) {
        T atomVolume = node->GetAtomBoundingBox(i);
        volume += atomVolume;
        ref->vCenter = Vec3<float>((atomVolume.GetMinima().e[0] + atomVolume.GetMaxima().e[0]) * 0.5f,
                                   (atomVolume.GetMinima().e[1] + atomVolume.GetMaxima().e[1]) * 0.5f,
                                   (atomVolume.GetMinima().e[2] + atomVolume.GetMaxima().e[2]) * 0.5f);
        ref->atomIndex = i;
        ref++;
    }
}

// EQUIV 0x100e42e0 ?Intersects@?$BVolHierNode@VBoundingBox@Gfx@@@Gfx@@UBE_NABVRay3f@2@AAM1AAV?$Vec3@M@@@Z: reviewed: same calls, tests and stores; this, tNear and the loop counters sit in other registers (the original reloads tNear from its argument slot)
template <class T>
bool Gfx::BVolHierNode<T>::Intersects(const Gfx::Ray3f &ray, float &tNear, float &tFar, Vec3<float> &point) const
{
    // the nearest hit among the children and this node's own atoms
    if (!T::Intersects(ray, tNear, tFar, point))
        return false;
    bool hit = false;
    float t, t2;
    Vec3<float> hitPoint;
    tNear = FLT_MAX;
    for (int i = 0; i < 2; i++) {
        if (m_apChild[i] && m_apChild[i]->Intersects(ray, t, t2, hitPoint) && t < tNear) {
            tNear = t;
            point = hitPoint;
            hit = true;
        }
    }
    if (m_AtomBoundingVolume && m_AtomBoundingVolume->Intersects(ray, t, t2, hitPoint)) {
        Gfx::Node *node = m_Node;
        Gfx::Shape *shape = dynamic_cast<Gfx::Shape *>(node);
        if (shape) {
            // the atoms of a shape are its polygons
            const int *atom = m_AtomIndices;
            for (int n = m_AtomIndices.length(); n > 0; n--) {
                Gfx::Polygon3f polygon;
                Vec3<float> *corners[128];
                polygon.Share(corners, shape->GetAtom(*atom, corners));
                if (polygon.Intersects(ray, t) && t < tNear) {
                    tNear = t;
                    point = polygon.GetPlane().GetNormal();
                    hit = true;
                }
                atom++;
            }
        } else {
            // the atoms of any other node are its children
            for (int i = 0; i < m_AtomIndices.length(); i++) {
                Gfx::Node *child = (Gfx::Node *)node->GetChild(((const int *)m_AtomIndices)[i]);
                if (child->GetBoundingBox().Intersects(ray, t, t2, hitPoint) && t < tNear) {
                    tNear = t;
                    point = hitPoint;
                    hit = true;
                }
            }
        }
    }
    return hit;
}

// EQUIV 0x100e4a60 ?Build@?$BVolHierarchy@VBoundingBox@Gfx@@@Gfx@@AAEXPAVNode@2@@Z: reviewed: the node count is doubled with shl eax,1 where VC6 gives add eax,eax; flags unused, rest identical
template <class T>
void Gfx::BVolHierarchy<T>::Build(Gfx::Node *node)
{
    Prune(this);
    m_vAtomCenterRef.setLength(0);
    if (node->GetNumberOfAtoms() > 0) {
        if (m_nAtoms != node->GetNumberOfAtoms()) {
            m_nAtoms = node->GetNumberOfAtoms();
            delete m_pBVolHierNodeFactory;
            m_pBVolHierNodeFactory = new Gfx::BVolHierNodeFactory<T>(m_nAtoms);
        } else
            m_pBVolHierNodeFactory->Reset();
        int levels = (int)(log((double)m_nAtoms) / log(2.0)) + 1;
        m_vvStackAtomCenterRef.setLength(levels);
        m_vvStackAtomCenterRef.setLength(0);
        T volume;
        InitAtomCenterRef(node, volume);
        m_MaxLevels = 0;
        BVolHierNode<T>::Build(node, 1, 0, m_MaxLevels, volume, m_vAtomCenterRef, m_vvStackAtomCenterRef, *m_pBVolHierNodeFactory);
    } else {
        m_nAtoms = 0;
        delete m_pBVolHierNodeFactory;
        m_pBVolHierNodeFactory = 0;
        GetAtomIndices().setLength(0);
    }
}

// takes over a volume; the hierarchy links stay as they are
template <class T>
Gfx::BVolHierNode<T> &Gfx::BVolHierNode<T>::operator=(const T &volume)
{
    T::operator=(volume);
    return *this;
}

// TODO 0x100e53e0 ?Build@?$BVolHierNode@VBoundingBox@Gfx@@@Gfx@@IAEXPAVNode@2@HHAAHAAVBoundingBox@2@AAV?$Vec@UBVolHierCenterRef@Gfx@@@@AAV?$Vec@V?$Vec@UBVolHierCenterRef@Gfx@@@@@@AAV?$BVolHierNodeFactory@VBoundingBox@Gfx@@@2@@Z: the original calls BoundingVolume() and Vec<int>::append out of line where VC6 inlines them here, and inlines the last box's maxima (56%)
template <class T>
void Gfx::BVolHierNode<T>::Build(Gfx::Node *node, int nLeafAtoms, int level, int &maxLevels, T &volume,
                                 Vec<Gfx::BVolHierCenterRef> &vCenterRef,
                                 Vec<Vec<Gfx::BVolHierCenterRef> > &vvStack,
                                 Gfx::BVolHierNodeFactory<T> &factory)
{
    m_Node = node;
    m_ShapeIsDirty = true;
    int nRefs = vCenterRef.length();
    GFX_ASSERT(nLeafAtoms > 0, "D:\\tugboat\\code\\SDK\\Scene\\Graphics\\BVolHierarchy.cxx", 1139)
    m_Level = level;
    if (level > maxLevels)
        maxLevels = level;
    *this = volume;
    m_AtomIndices.setLength(0);
    if (nRefs <= nLeafAtoms) {
        // few enough atoms: this node is a leaf
        m_AtomIndices.setLength(nRefs);
        int *index = m_AtomIndices;
        for (int i = 0; i < nRefs; i++)
            index[i] = vCenterRef[i].atomIndex;
    } else {
        T lowVolume;
        T highVolume;
        // split the atoms at their median along the longest axis
        Vec3<float> diagonal = volume.GetDiagonal();
        int axis = 0;
        if (diagonal.e[1] > diagonal.e[0])
            axis = 1;
        if (diagonal.e[2] > diagonal.e[axis])
            axis = 2;
        if (nRefs == 2) {
            if (vCenterRef[0].vCenter.e[axis] > vCenterRef[1].vCenter.e[axis]) {
                Gfx::BVolHierCenterRef ref = vCenterRef[0];
                vCenterRef[0] = vCenterRef[1];
                vCenterRef[1] = ref;
            }
        } else if (axis == 0)
            qsort((Gfx::BVolHierCenterRef *)vCenterRef, nRefs, sizeof(Gfx::BVolHierCenterRef), BVolHierCenterRefCompareFunc0);
        else if (axis == 1)
            qsort((Gfx::BVolHierCenterRef *)vCenterRef, nRefs, sizeof(Gfx::BVolHierCenterRef), BVolHierCenterRefCompareFunc1);
        else
            qsort((Gfx::BVolHierCenterRef *)vCenterRef, nRefs, sizeof(Gfx::BVolHierCenterRef), BVolHierCenterRefCompareFunc2);
        vvStack.verify(level);
        int half = nRefs >> 1;
        vvStack[level].setLength(half);
        // an atom too big for either half stays with this node
        float maxArea = volume.GetSurfaceArea() * 0.7f;
        int n = 0;
        int i;
        for (i = 0; i < half; i++) {
            int atom = vCenterRef[i].atomIndex;
            T atomVolume = m_Node->GetAtomBoundingBox(atom);
            if (atomVolume.GetSurfaceArea() > maxArea) {
                m_AtomIndices.append(&atom, 1);
                Gfx::Node *child = m_Node->CountChildren() ? StdBaseCast(m_Node->GetChild(atom), (Gfx::Node *)0) : 0;
            } else {
                vvStack[level][n++] = vCenterRef[i];
                lowVolume += atomVolume;
            }
        }
        vvStack[level].setLength(n);
        n = 0;
        for (i = half; i < nRefs; i++) {
            int atom = vCenterRef[i].atomIndex;
            T atomVolume = m_Node->GetAtomBoundingBox(atom);
            if (atomVolume.GetSurfaceArea() > maxArea) {
                m_AtomIndices.append(&atom, 1);
                Gfx::Node *child = m_Node->CountChildren() ? StdBaseCast(m_Node->GetChild(atom), (Gfx::Node *)0) : 0;
            } else {
                vCenterRef[n++] = vCenterRef[i];
                highVolume += atomVolume;
            }
        }
        vCenterRef.setLength(n);
        if (n > 0) {
            Gfx::BVolHierNode<T> *child = factory.NextNode();
            child->m_pParent = this;
            m_apChild[0] = child;
            child->Build(m_Node, nLeafAtoms, level + 1, maxLevels, highVolume, vCenterRef, vvStack, factory);
        }
        if (vvStack[level].length() > 0) {
            Gfx::BVolHierNode<T> *child = factory.NextNode();
            child->m_pParent = this;
            m_apChild[1] = child;
            child->Build(m_Node, nLeafAtoms, level + 1, maxLevels, lowVolume, vvStack[level], vvStack, factory);
        }
    }
    // the volume around the atoms this node keeps itself
    int nAtoms = m_AtomIndices.length();
    if (nAtoms) {
        T atomsVolume;
        const int *atom = m_AtomIndices;
        for (int k = nAtoms; k > 0; k--) {
            atomsVolume += m_Node->GetAtomBoundingBox(*atom);
            atom++;
        }
        if (!m_AtomBoundingVolume)
            m_AtomBoundingVolume = new T;
        *m_AtomBoundingVolume = atomsVolume;
    }
}

// TODO 0x100e34e0 ?GetCenter@BoundingBox@Gfx@@UBEABV?$Vec3@M@@XZ: header instance; the object holds two copies of it, so the tools cannot choose one
// TODO 0x100e3540 ?GetRadius@BoundingBox@Gfx@@UBEMXZ: header instance; the object holds two copies of it, so the tools cannot choose one
// 0x100e4d80 ??0?$BVolHierNode@VBoundingBox@Gfx@@@Gfx@@QAE@XZ
// TODO 0x100e4ec0 ??1?$BVolHierNodeFactory@VBoundingBox@Gfx@@@Gfx@@QAE@XZ: the out-of-line copy walks the nodes with a pointer and a down-counter loaded once; the inlined copies (~Node) reload the count like this one (28%)
// TODO 0x100e4f50 ?reallocMem@?$Vec@UBVolHierCenterRef@Gfx@@@@IAEXHHH@Z: header instance compiled differently here (45%)
// EQUIV 0x100e50e0 ?reallocMem@?$Vec@V?$Vec@UBVolHierCenterRef@Gfx@@@@@@IAEXHHH@Z: reviewed: one length copy goes through esi instead of eax with the argument load scheduled between, and the deleting-destructor call uses eax instead of edx; same loads, stores and calls
// EQUIV 0x100e5d50 ??4BoundingBox@Gfx@@QAEAAV01@ABV01@@Z: reviewed: the implicit copy assignment loads the last maxima component before storing the one before it; the two touch different members, nothing else differs
// TODO 0x100e5e90 ?setLength@?$Vec@H@@QAEXH@Z: header instance compiled differently here (35%)
// TODO 0x100e62c0 ?initElements@?$Vec@UBVolHierCenterRef@Gfx@@@@IAEXHH@Z: header instance compiled differently here (19%)
// TODO 0x100e6320 ??4?$Vec@UBVolHierCenterRef@Gfx@@@@QAEAAV0@ABV0@@Z: header instance compiled differently here (21%)
// TODO 0x100e6370 ?initElements@?$Vec@V?$Vec@UBVolHierCenterRef@Gfx@@@@@@IAEXHH@Z: header instance compiled differently here (53%)
// TODO 0x100e64b0 ?append@?$Vec@H@@QAEXPBHH@Z: header instance; the object holds two copies of it, so the tools cannot choose one (Build inlines it where the original calls it)

// the hierarchy of the nodes' bounding boxes is this unit's instance
template class Gfx::BVolHierarchy<Gfx::BoundingBox>;
// 0x100e4d60 ??0BoundingVolume@Gfx@@IAE@XZ

// static data (zero in the original, .bss)
int Gfx::Node::s_CurrentTypeCount = 0;
int Gfx::Node::s_TotalTypeCount = 0;
Graph::NodeContext<Gfx::Node, bool> *Gfx::Node::m_VisibilityContext = 0;
