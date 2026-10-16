#pragma once
#include "Gfx_Node.h"
#include "Graph_NodeTraverser.h"
#include "StdBase.h"
#include "TMatrix4.h"
namespace Gfx { class WriteTraverser; }
namespace Graph { class PassiveTraverser; }
namespace Graph { class Stream; }

namespace Gfx {
// Transformation node: pushes its matrix (its own or an external one) onto the
// current model matrix for the traversal of its children. An absolute
// transformation replaces the model matrix instead of multiplying it (no PDB
// declaration; member names chosen from the use).
class MatrixTransform : public Gfx::Node {
protected:
    void IncrementTypeCount()
    {
        s_CurrentTypeCount++;
        m_TypeHandle = s_TotalTypeCount;
        s_TotalTypeCount++;
    }
    void DecrementTypeCount() { s_CurrentTypeCount--; }
    static StdBase::TypeInfo const s_TypeInfo;
    static int s_CurrentTypeCount;
    static int s_TotalTypeCount;
    int m_TypeHandle;
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    static StdBase::TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
    MatrixTransform();
    virtual ~MatrixTransform();
    const TMatrix4<float> &GetMatrix() const { return m_UseExternalMatrix ? *m_pExternalMatrix : m_Matrix; }
    void Push(const TMatrix4<float> &);
    void Pop();
    virtual Graph::NodeTraverser::Response Traverse(Graph::PassiveTraverser &);
    virtual void Write(Gfx::WriteTraverser *);
    virtual void ReadVis(Graph::Stream *);
    virtual void Read(Graph::Stream *);
protected:
    void WriteInternals(Gfx::WriteTraverser *);
    void ReadInternals(Graph::Stream *, int);
private:
    static int const Version;
    TMatrix4<float> m_Matrix;
    const TMatrix4<float> *m_pExternalMatrix;
    bool m_Absolute;
    bool m_Enabled;
    bool m_UseExternalMatrix;
};
}
