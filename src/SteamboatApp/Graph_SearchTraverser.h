#pragma once
#include "Graph_PassiveTraverser.h"
#include "Graph_Path.h"
#include "Text.h"
#include "Vec.h"
namespace Graph { class ActiveNode; }
namespace Graph { class BasicNode; }

namespace Graph {
// Finds the paths to nodes of a type, to one given node, or with a name.
class SearchTraverser : public Graph::PassiveTraverser {
protected:
    static StdBase::TypeInfo const s_TypeInfo;
    int m_TypeHandle;
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    static StdBase::TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
    enum Find {
        All = 0,
        First = 1
    };
    enum NameMatch {
        Exact = 0,
        Contains = 1
    };
    SearchTraverser(const Graph::SearchTraverser &);
    SearchTraverser();
    virtual ~SearchTraverser();
    Find GetFind() { return m_Find; }
    void SetFind(Find find) { m_Find = find; }
    const StdBase::TypeInfo *GetType() { return m_Type; }
    void SetType(const StdBase::TypeInfo *type) { m_Type = type; }
    Graph::BasicNode *GetNode() { return m_Node; }
    void SetNode(Graph::BasicNode *node) { m_Node = node; }
    Text GetName() { return m_Name; }
    void SetName(const Text &name) { m_Name = name; }
    NameMatch GetNameMatch() { return m_NameMatch; }
    void SetNameMatch(NameMatch match) { m_NameMatch = match; }
    bool IsCaseSensitive() { return m_CaseSensitive; }
    void SetCaseSensitivity(bool caseSensitive) { m_CaseSensitive = caseSensitive; }
    bool GetDerived() { return m_Derived; }
    void SetDerived(bool derived) { m_Derived = derived; }
    const Vec<Graph::Path> &GetPaths() { return m_FindList; }
    void Traverse(Graph::ActiveNode &);
    virtual Graph::NodeTraverser::Response Visit(Graph::BasicNode &);
private:
    const StdBase::TypeInfo *m_Type;
    Graph::BasicNode *m_Node;
    Text m_Name;
    NameMatch m_NameMatch;
    bool m_CaseSensitive;
    Find m_Find;
    bool m_Derived;
    Vec<Graph::Path> m_FindList;
};
}
