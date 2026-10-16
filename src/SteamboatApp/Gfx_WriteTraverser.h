#pragma once
#include "Gfx_Traverser.h"
#include "StdBase.h"
namespace Gfx { class Node; }
namespace Graph { class FileStream; }

namespace Gfx {
// Writes a node graph to a VisFly binary scene file.
class WriteTraverser : public Gfx::Traverser {
protected:
    static StdBase::TypeInfo const s_TypeInfo;
    static int s_CurrentTypeCount;
    static int s_TotalTypeCount;
    int m_TypeHandle;
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    static StdBase::TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
    WriteTraverser();
    WriteTraverser(Graph::FileStream *);
    WriteTraverser(char const *);
    virtual ~WriteTraverser();
    virtual void Traverse(Gfx::Node &);
    int IsInstance(Gfx::Node *);
    Graph::FileStream *GetStream() { return m_pStream; }
private:
    Graph::FileStream *m_pStream;
};
}
