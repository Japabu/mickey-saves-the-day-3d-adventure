#include <stdexcept>
#include <algorithm>
#include "Graph_BasicNode.h"
#include "Graph_BasicAttribute.h"
#include "StdBase.h"

// 0x100de460 ?GetTypeInfo@BasicNode@Graph@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Graph::BasicNode::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100de470 ?GetTypeName@BasicNode@Graph@@UBEPBDXZ
char const * Graph::BasicNode::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100de480 ??1BasicNode@Graph@@UAE@XZ
Graph::BasicNode::~BasicNode()
{
    RemoveAllChildren();
    RemoveAllAttributes();
    if (CountParents() != 0)
        throw std::logic_error("Error: deleting node with parents.");
    if (CountObservers() != 0)
        throw std::logic_error("Error: deleting node with observers.");
}

// 0x100de740 ?GetChild@BasicNode@Graph@@QBEPAV12@H@Z
Graph::BasicNode *Graph::BasicNode::GetChild(int index) const
{
    if ((unsigned)index >= m_ChildList.size())
        return 0;
    if (index == m_ChildIndex)
        return *m_ChildIterator;
    if (index < m_ChildIndex) {
        m_ChildIndex = 0;
        m_ChildIterator = m_ChildList.begin();
    }
    while (index > m_ChildIndex) {
        m_ChildIndex++;
        m_ChildIterator++;
    }
    return *m_ChildIterator;
}

// 0x100de7a0 ?FindChild@BasicNode@Graph@@QAE?AViterator@?$list@PAVBasicNode@Graph@@V?$allocator@PAVBasicNode@Graph@@@std@@@std@@PAV12@@Z
Graph::BasicNode::NodeIterator Graph::BasicNode::FindChild(Graph::BasicNode *node)
{
    return std::find(m_ChildList.begin(), m_ChildList.end(), node);
}

// 0x100de7d0 ?FindChild@BasicNode@Graph@@QBE?AVconst_iterator@?$list@PAVBasicNode@Graph@@V?$allocator@PAVBasicNode@Graph@@@std@@@std@@PAV12@@Z
Graph::BasicNode::ConstNodeIterator Graph::BasicNode::FindChild(Graph::BasicNode *node) const
{
    m_ChildIterator = m_ChildList.begin();
    m_ChildIndex = 0;
    while (m_ChildIterator != m_ChildList.end() && *m_ChildIterator != node) {
        m_ChildIterator++;
        m_ChildIndex++;
    }
    return m_ChildIterator;
}

// 0x100de820 ?InsertChild@BasicNode@Graph@@QAE_NViterator@?$list@PAVBasicNode@Graph@@V?$allocator@PAVBasicNode@Graph@@@std@@@std@@PAV12@@Z
bool Graph::BasicNode::InsertChild(Graph::BasicNode::NodeIterator where, Graph::BasicNode *child)
{
    if (CanAddChild(child) && child->CanAddParent(this)) {
        m_ChildList.insert(where, child);
        child->AddParent(this);
        ChildAdded(child);
        child->ParentAdded(this);
        m_ChildIndex = m_ChildList.size();
        return true;
    }
    return false;
}

// 0x100de8b0 ?RemoveChild@BasicNode@Graph@@QAEXViterator@?$list@PAVBasicNode@Graph@@V?$allocator@PAVBasicNode@Graph@@@std@@@std@@@Z
void Graph::BasicNode::RemoveChild(Graph::BasicNode::NodeIterator where)
{
    if (where == m_ChildList.end())
        throw std::runtime_error("Error: attempting to remove non-existent child.");
    Graph::BasicNode *child = *where;
    m_ChildList.erase(where);
    child->RemoveParent(child->FindParent(this));
    ChildRemoved(child);
    child->ParentRemoved(this);
    m_ChildIndex = m_ChildList.size();
}

// 0x100debe0 ?RemoveAllChildren@BasicNode@Graph@@QAEXXZ
void Graph::BasicNode::RemoveAllChildren()
{
    while (m_ChildList.size() != 0)
        RemoveChild(m_ChildList.begin());
    m_ChildIndex = 0;
}

// 0x100dec10 ?CanAddChild@BasicNode@Graph@@UAE_NPAV12@@Z
bool Graph::BasicNode::CanAddChild(Graph::BasicNode *)
{
    return true;
}

// 0x100dec20 ?GetAttribute@BasicNode@Graph@@QBEPAVBasicAttribute@2@H@Z
Graph::BasicAttribute *Graph::BasicNode::GetAttribute(int index) const
{
    if ((unsigned)index >= m_AttributeList.size())
        return 0;
    if (index == m_AttributeIndex)
        return *m_AttributeIterator;
    if (index < m_AttributeIndex) {
        m_AttributeIndex = 0;
        m_AttributeIterator = m_AttributeList.begin();
    }
    while (index > m_AttributeIndex) {
        m_AttributeIndex++;
        m_AttributeIterator++;
    }
    return *m_AttributeIterator;
}

// 0x100dec80 ?FindAttribute@BasicNode@Graph@@QAE?AViterator@?$list@PAVBasicAttribute@Graph@@V?$allocator@PAVBasicAttribute@Graph@@@std@@@std@@PAVBasicAttribute@2@@Z
Graph::BasicNode::AttributeIterator Graph::BasicNode::FindAttribute(Graph::BasicAttribute *attribute)
{
    return std::find(m_AttributeList.begin(), m_AttributeList.end(), attribute);
}

// 0x100decb0 ?FindAttribute@BasicNode@Graph@@QBE?AVconst_iterator@?$list@PAVBasicAttribute@Graph@@V?$allocator@PAVBasicAttribute@Graph@@@std@@@std@@PAVBasicAttribute@2@@Z
Graph::BasicNode::ConstAttributeIterator Graph::BasicNode::FindAttribute(Graph::BasicAttribute *attribute) const
{
    m_AttributeIterator = m_AttributeList.begin();
    m_AttributeIndex = 0;
    while (m_AttributeIterator != m_AttributeList.end() && *m_AttributeIterator != attribute) {
        m_AttributeIterator++;
        m_AttributeIndex++;
    }
    return m_AttributeIterator;
}

// 0x100ded00 ?InsertAttribute@BasicNode@Graph@@QAE_NViterator@?$list@PAVBasicAttribute@Graph@@V?$allocator@PAVBasicAttribute@Graph@@@std@@@std@@PAVBasicAttribute@2@@Z
bool Graph::BasicNode::InsertAttribute(Graph::BasicNode::AttributeIterator where, Graph::BasicAttribute *attribute)
{
    Graph::BasicNode *node = attribute;
    if (CanAddAttribute(attribute) && node->CanAddParent(this)) {
        m_AttributeList.insert(where, attribute);
        node->AddParent(this);
        AttributeAdded(attribute);
        node->ParentAdded(this);
        m_AttributeIndex = m_AttributeList.size();
        return true;
    }
    return false;
}

// 0x100ded90 ?RemoveAttribute@BasicNode@Graph@@QAEXViterator@?$list@PAVBasicAttribute@Graph@@V?$allocator@PAVBasicAttribute@Graph@@@std@@@std@@@Z
void Graph::BasicNode::RemoveAttribute(Graph::BasicNode::AttributeIterator where)
{
    if (where == m_AttributeList.end())
        throw std::runtime_error("Error: attempting to remove non-existent attribute.");
    Graph::BasicAttribute *attribute = *where;
    Graph::BasicNode *node = attribute;
    m_AttributeList.erase(where);
    node->RemoveParent(node->FindParent(this));
    AttributeRemoved(attribute);
    node->ParentRemoved(this);
    m_AttributeIndex = m_AttributeList.size();
}

// 0x100deec0 ?RemoveAllAttributes@BasicNode@Graph@@QAEXXZ
void Graph::BasicNode::RemoveAllAttributes()
{
    while (m_AttributeList.size() != 0)
        RemoveAttribute(m_AttributeList.begin());
    m_AttributeIndex = 0;
}

// 0x100deef0 ?CanAddAttribute@BasicNode@Graph@@UAE_NPAVBasicAttribute@2@@Z
bool Graph::BasicNode::CanAddAttribute(Graph::BasicAttribute *)
{
    return true;
}

// 0x100def00 ?GetObserver@BasicNode@Graph@@QBEPAV12@H@Z
Graph::BasicNode *Graph::BasicNode::GetObserver(int index) const
{
    if ((unsigned)index >= m_ObserverList.size())
        return 0;
    if (index == m_ObserverIndex)
        return *m_ObserverIterator;
    if (index < m_ObserverIndex) {
        m_ObserverIndex = 0;
        m_ObserverIterator = m_ObserverList.begin();
    }
    while (index > m_ObserverIndex) {
        m_ObserverIndex++;
        m_ObserverIterator++;
    }
    return *m_ObserverIterator;
}

// 0x100def60 ?FindObserver@BasicNode@Graph@@QAE?AViterator@?$list@PAVBasicNode@Graph@@V?$allocator@PAVBasicNode@Graph@@@std@@@std@@PAV12@@Z
Graph::BasicNode::NodeIterator Graph::BasicNode::FindObserver(Graph::BasicNode *observer)
{
    return std::find(m_ObserverList.begin(), m_ObserverList.end(), observer);
}

// 0x100def90 ?FindObserver@BasicNode@Graph@@QBE?AVconst_iterator@?$list@PAVBasicNode@Graph@@V?$allocator@PAVBasicNode@Graph@@@std@@@std@@PAV12@@Z
Graph::BasicNode::ConstNodeIterator Graph::BasicNode::FindObserver(Graph::BasicNode *observer) const
{
    m_ObserverIterator = m_ObserverList.begin();
    m_ObserverIndex = 0;
    while (m_ObserverIterator != m_ObserverList.end() && *m_ObserverIterator != observer) {
        m_ObserverIterator++;
        m_ObserverIndex++;
    }
    return m_ObserverIterator;
}

// 0x100defe0 ?InsertObserver@BasicNode@Graph@@QAE_NViterator@?$list@PAVBasicNode@Graph@@V?$allocator@PAVBasicNode@Graph@@@std@@@std@@PAV12@@Z
bool Graph::BasicNode::InsertObserver(Graph::BasicNode::NodeIterator where, Graph::BasicNode *observer)
{
    if (!CanAddObserver(observer))
        return false;
    m_ObserverList.insert(where, observer);
    ObserverAdded(observer);
    m_ObserverIndex = m_ObserverList.size();
    return true;
}

// 0x100df050 ?RemoveObserver@BasicNode@Graph@@QAEXViterator@?$list@PAVBasicNode@Graph@@V?$allocator@PAVBasicNode@Graph@@@std@@@std@@@Z
void Graph::BasicNode::RemoveObserver(Graph::BasicNode::NodeIterator where)
{
    if (where == m_ObserverList.end())
        throw std::runtime_error("Error: attempting to remove non-existent observer.");
    Graph::BasicNode *observer = *where;
    m_ObserverList.erase(where);
    ObserverRemoved(observer);
    m_ObserverIndex = m_ObserverList.size();
}

// 0x100df160 ?RemoveAllObservers@BasicNode@Graph@@QAEXXZ
void Graph::BasicNode::RemoveAllObservers()
{
    while (m_ObserverList.size() != 0)
        RemoveObserver(m_ObserverList.begin());
    m_ObserverIndex = 0;
}

// 0x100df190 ?CanAddObserver@BasicNode@Graph@@UAE_NPAV12@@Z
bool Graph::BasicNode::CanAddObserver(Graph::BasicNode *)
{
    return true;
}

// 0x100df1a0 ?GetParent@BasicNode@Graph@@QBEPAV12@H@Z
Graph::BasicNode *Graph::BasicNode::GetParent(int index) const
{
    if ((unsigned)index >= m_ParentList.size())
        return 0;
    if (index == m_ParentIndex)
        return *m_ParentIterator;
    if (index < m_ParentIndex) {
        m_ParentIndex = 0;
        m_ParentIterator = m_ParentList.begin();
    }
    while (index > m_ParentIndex) {
        m_ParentIndex++;
        m_ParentIterator++;
    }
    return *m_ParentIterator;
}

// 0x100df200 ?FindParent@BasicNode@Graph@@QAE?AViterator@?$list@PAVBasicNode@Graph@@V?$allocator@PAVBasicNode@Graph@@@std@@@std@@PAV12@@Z
Graph::BasicNode::NodeIterator Graph::BasicNode::FindParent(Graph::BasicNode *parent)
{
    return std::find(m_ParentList.begin(), m_ParentList.end(), parent);
}

// 0x100df230 ?FindParent@BasicNode@Graph@@QBE?AVconst_iterator@?$list@PAVBasicNode@Graph@@V?$allocator@PAVBasicNode@Graph@@@std@@@std@@PAV12@@Z
Graph::BasicNode::ConstNodeIterator Graph::BasicNode::FindParent(Graph::BasicNode *parent) const
{
    m_ParentIterator = m_ParentList.begin();
    m_ParentIndex = 0;
    while (m_ParentIterator != m_ParentList.end() && *m_ParentIterator != parent) {
        m_ParentIterator++;
        m_ParentIndex++;
    }
    return m_ParentIterator;
}

// 0x100df280 ??0BasicNode@Graph@@IAE@XZ
Graph::BasicNode::BasicNode()
    : m_ChildIndex(0), m_ParentIndex(0), m_AttributeIndex(0), m_ObserverIndex(0)
{
}

// 0x100df390 ??0BasicNode@Graph@@IAE@ABV01@@Z
Graph::BasicNode::BasicNode(Graph::BasicNode const &node)
    : m_ChildIndex(0), m_ParentIndex(0), m_AttributeIndex(0), m_ObserverIndex(0),
      m_Name(node.m_Name)
{
}

// 0x100df480 ?AddParent@BasicNode@Graph@@IAEXPAV12@@Z
void Graph::BasicNode::AddParent(Graph::BasicNode *parent)
{
    m_ParentList.push_back(parent);
    m_ParentIndex = m_ParentList.size();
}

// 0x100df4d0 ?InsertParent@BasicNode@Graph@@IAEXViterator@?$list@PAVBasicNode@Graph@@V?$allocator@PAVBasicNode@Graph@@@std@@@std@@PAV12@@Z
void Graph::BasicNode::InsertParent(Graph::BasicNode::NodeIterator where, Graph::BasicNode *parent)
{
    m_ParentList.insert(where, parent);
    m_ParentIndex = m_ParentList.size();
}

// 0x100df520 ?RemoveParent@BasicNode@Graph@@IAEXViterator@?$list@PAVBasicNode@Graph@@V?$allocator@PAVBasicNode@Graph@@@std@@@std@@@Z
void Graph::BasicNode::RemoveParent(Graph::BasicNode::NodeIterator where)
{
    m_ParentList.erase(where);
    m_ParentIndex = m_ParentList.size();
}

// 0x100df550 ?CanAddParent@BasicNode@Graph@@MAE_NPAV12@@Z
bool Graph::BasicNode::CanAddParent(Graph::BasicNode *)
{
    return true;
}

// 0x100df560 ?ParentAdded@BasicNode@Graph@@MAEXPAV12@@Z
void Graph::BasicNode::ParentAdded(Graph::BasicNode *)
{
    Ref();
}

// 0x100df570 ?ParentRemoved@BasicNode@Graph@@MAEXPAV12@@Z
void Graph::BasicNode::ParentRemoved(Graph::BasicNode *)
{
    Unref();
}

// 0x100df580 ?ChildAdded@BasicNode@Graph@@MAEXPAV12@@Z
void Graph::BasicNode::ChildAdded(Graph::BasicNode *)
{
}

// 0x100df590 ?ChildRemoved@BasicNode@Graph@@MAEXPAV12@@Z
void Graph::BasicNode::ChildRemoved(Graph::BasicNode *)
{
}

// 0x100df5a0 ?AttributeAdded@BasicNode@Graph@@MAEXPAVBasicAttribute@2@@Z
void Graph::BasicNode::AttributeAdded(Graph::BasicAttribute *)
{
}

// 0x100df5b0 ?AttributeRemoved@BasicNode@Graph@@MAEXPAVBasicAttribute@2@@Z
void Graph::BasicNode::AttributeRemoved(Graph::BasicAttribute *)
{
}

// 0x100df5c0 ?ObserverAdded@BasicNode@Graph@@MAEXPAV12@@Z
void Graph::BasicNode::ObserverAdded(Graph::BasicNode *)
{
}

// 0x100df5d0 ?ObserverRemoved@BasicNode@Graph@@MAEXPAV12@@Z
void Graph::BasicNode::ObserverRemoved(Graph::BasicNode *)
{
}


// Matched by this code:
// 0x10036db0 ??1StdBase@@MAE@XZ
// 0x100de720 ??_GBasicNode@Graph@@UAEPAXI@Z
// 0x100de9e0 ??1runtime_error@std@@UAE@XZ
// 0x100dea30 ?what@runtime_error@std@@UBEPBDXZ
// 0x100dea40 ?_Doraise@runtime_error@std@@MBEXXZ
// 0x100dea60 ??_Gruntime_error@std@@UAEPAXI@Z
// 0x100dea80 ??0runtime_error@std@@QAE@ABV01@@Z
// 0x100df370 ??_GText@@UAEPAXI@Z
// 0x100df5e0 ??1?$list@PAVBasicNode@Graph@@V?$allocator@PAVBasicNode@Graph@@@std@@@std@@QAE@XZ
// 0x100df630 ?begin@?$list@PAVBasicNode@Graph@@V?$allocator@PAVBasicNode@Graph@@@std@@@std@@QAE?AViterator@12@XZ
// 0x100df640 ??1?$list@PAVBasicAttribute@Graph@@V?$allocator@PAVBasicAttribute@Graph@@@std@@@std@@QAE@XZ
// 0x100df690 ?begin@?$list@PAVBasicAttribute@Graph@@V?$allocator@PAVBasicAttribute@Graph@@@std@@@std@@QAE?AViterator@12@XZ
// 0x100df6a0 ?erase@?$list@PAVBasicNode@Graph@@V?$allocator@PAVBasicNode@Graph@@@std@@@std@@QAE?AViterator@12@V312@0@Z
// 0x100df6f0 ?erase@?$list@PAVBasicAttribute@Graph@@V?$allocator@PAVBasicAttribute@Graph@@@std@@@std@@QAE?AViterator@12@V312@0@Z

// Template and inline instances this unit also emits:
// 0x1000bde0 ?assign@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@PBDI@Z
