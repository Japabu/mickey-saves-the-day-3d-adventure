#pragma once
#include "Gfx_Node.h"
#include "Graph_NodeTraverser.h"
#include "StdBase.h"
namespace Gfx { class PickTraverser; }
namespace Gfx { class RenderTraverser; }
namespace Gfx { class Traverser; }
namespace Gfx { class WriteTraverser; }
namespace Graph { class BasicNode; }
namespace Graph { class CallbackTraverser; }
namespace Graph { class Stream; }

namespace Gfx {
class IONode : public Gfx::Node {
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
protected:
    virtual ~IONode();
public:
    virtual Graph::NodeTraverser::Response Pick(Gfx::PickTraverser *);
    virtual Graph::NodeTraverser::Response HandleEvent(Gfx::Traverser *);
    virtual Graph::NodeTraverser::Response Render(Gfx::RenderTraverser *);
    virtual void Write(Gfx::WriteTraverser *);
    virtual void Read(Graph::Stream *);
    virtual void ReadVis(Graph::Stream *);
    virtual bool Open() = 0;
    virtual bool Close();
    Gfx::Node * GetCopyOfContents();
protected:
    IONode();
    static Graph::NodeTraverser::Response __cdecl BuildBBHierCB(Graph::CallbackTraverser *, Graph::BasicNode *, void *);
    void BuildBBHierTraversal();
    void ReadInternals(Graph::Stream *, int);
    void WriteInternals(Gfx::WriteTraverser *);
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    static StdBase::TypeInfo const s_TypeInfo;
private:
    static int const Version;
    int m_TypeHandle;
public:
    Gfx::Node * mCopyOfContents;
    bool field_144;
    bool field_145;
};
}
