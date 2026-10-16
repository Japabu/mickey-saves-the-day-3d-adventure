#pragma once
#include <memory>
#include <list>
#include <functional>
#include "StdBase.h"
#include "Text.h"
namespace Graph { class BasicAttribute; }

namespace Graph {
// A scene-graph node: children, parents, attributes and observers, each kept
// in a list with a cached position for indexed access.
class BasicNode : public StdBase {
protected:
    void IncrementTypeCount();
    void DecrementTypeCount();
    static StdBase::TypeInfo const s_TypeInfo;
    static int s_CurrentTypeCount;
    static int s_TotalTypeCount;
    int m_TypeHandle;
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    static StdBase::TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
    static int GetCurrentTypeCount();
    static int GetTotalTypeCount();
    int GetTypeHandle() const;
    typedef std::list<BasicNode *> NodeList;
    typedef std::list<BasicAttribute *> AttributeList;
    typedef NodeList::iterator NodeIterator;
    typedef AttributeList::iterator AttributeIterator;
    typedef NodeList::const_iterator ConstNodeIterator;
    typedef AttributeList::const_iterator ConstAttributeIterator;
    virtual ~BasicNode();
    int CountChildren() const { return m_ChildList.size(); }
    Graph::BasicNode * GetChild(int) const;
    ConstNodeIterator GetBeginChildIterator() const { return m_ChildList.begin(); }
    NodeIterator GetBeginChildIterator() { return m_ChildList.begin(); }
    ConstNodeIterator GetEndChildIterator() const { return m_ChildList.end(); }
    NodeIterator GetEndChildIterator() { return m_ChildList.end(); }
    ConstNodeIterator FindChild(Graph::BasicNode *) const;
    NodeIterator FindChild(Graph::BasicNode *);
    bool AddChild(Graph::BasicNode *child) { return InsertChild(m_ChildList.end(), child); }
    bool InsertChild(NodeIterator, Graph::BasicNode *);
    void RemoveChild(NodeIterator);
    void RemoveAllChildren();
    int CountAttributes() const { return m_AttributeList.size(); }
    Graph::BasicAttribute * GetAttribute(int) const;
    ConstAttributeIterator GetBeginAttributeIterator() const { return m_AttributeList.begin(); }
    AttributeIterator GetBeginAttributeIterator() { return m_AttributeList.begin(); }
    ConstAttributeIterator GetEndAttributeIterator() const { return m_AttributeList.end(); }
    AttributeIterator GetEndAttributeIterator() { return m_AttributeList.end(); }
    ConstAttributeIterator FindAttribute(Graph::BasicAttribute *) const;
    AttributeIterator FindAttribute(Graph::BasicAttribute *);
    bool AddAttribute(Graph::BasicAttribute *attribute) { return InsertAttribute(m_AttributeList.end(), attribute); }
    bool InsertAttribute(AttributeIterator, Graph::BasicAttribute *);
    void RemoveAttribute(AttributeIterator);
    void RemoveAllAttributes();
    int CountObservers() const { return m_ObserverList.size(); }
    Graph::BasicNode * GetObserver(int) const;
    ConstNodeIterator GetBeginObserverIterator() const { return m_ObserverList.begin(); }
    NodeIterator GetBeginObserverIterator() { return m_ObserverList.begin(); }
    ConstNodeIterator GetEndObserverIterator() const { return m_ObserverList.end(); }
    NodeIterator GetEndObserverIterator() { return m_ObserverList.end(); }
    ConstNodeIterator FindObserver(Graph::BasicNode *) const;
    NodeIterator FindObserver(Graph::BasicNode *);
    bool AddObserver(Graph::BasicNode *observer) { return InsertObserver(m_ObserverList.end(), observer); }
    bool InsertObserver(NodeIterator, Graph::BasicNode *);
    void RemoveObserver(NodeIterator);
    void RemoveAllObservers();
    int CountParents() const { return m_ParentList.size(); }
    Graph::BasicNode * GetParent(int) const;
    ConstNodeIterator FindParent(Graph::BasicNode *) const;
    NodeIterator FindParent(Graph::BasicNode *);
    void SetName(const Text &name) { m_Name = name; }
    const Text &GetName() const { return m_Name; }
    virtual bool CanAddChild(Graph::BasicNode *);
    virtual bool CanAddAttribute(Graph::BasicAttribute *);
    virtual bool CanAddObserver(Graph::BasicNode *);
protected:
    BasicNode(Graph::BasicNode const &);
    BasicNode();
    void AddParent(Graph::BasicNode *);
    void InsertParent(NodeIterator, Graph::BasicNode *);
    void RemoveParent(NodeIterator);
    void SortChildren(std::greater<Graph::BasicNode *>);
    virtual bool CanAddParent(Graph::BasicNode *);
    virtual void ParentAdded(Graph::BasicNode *);
    virtual void ParentRemoved(Graph::BasicNode *);
    virtual void ChildAdded(Graph::BasicNode *);
    virtual void ChildRemoved(Graph::BasicNode *);
    virtual void AttributeAdded(Graph::BasicAttribute *);
    virtual void AttributeRemoved(Graph::BasicAttribute *);
    virtual void ObserverAdded(Graph::BasicNode *);
    virtual void ObserverRemoved(Graph::BasicNode *);
private:
    NodeList m_ChildList;
    NodeList m_ParentList;
    AttributeList m_AttributeList;
    NodeList m_ObserverList;
    mutable int m_ChildIndex;
    mutable int m_ParentIndex;
    mutable int m_AttributeIndex;
    mutable int m_ObserverIndex;
    mutable ConstNodeIterator m_ChildIterator;
    mutable ConstNodeIterator m_ParentIterator;
    mutable ConstAttributeIterator m_AttributeIterator;
    mutable ConstNodeIterator m_ObserverIterator;
    Text m_Name;
};
}
