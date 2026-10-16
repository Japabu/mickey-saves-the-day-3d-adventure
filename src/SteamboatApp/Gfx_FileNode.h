#pragma once
#include "Gfx_IONode.h"
#include "Graph_NodeTraverser.h"
#include "StdBase.h"
#include "Text.h"
#include "Vec.h"
namespace Graph { class FileStream; }
namespace Gfx { class RenderTraverser; }
namespace Gfx { class WriteTraverser; }
namespace Graph { class Stream; }

namespace Gfx {
// Loads its contents from a scene file, searched for in a list of
// directories, the first time it is rendered (no PDB declaration; member
// names chosen from the use).
class FileNode : public Gfx::IONode {
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
protected:
    virtual ~FileNode();
public:
    virtual Graph::NodeTraverser::Response Render(Gfx::RenderTraverser *);
    virtual void Write(Gfx::WriteTraverser *);
    virtual void Read(Graph::Stream *);
    virtual void ReadVis(Graph::Stream *);
    virtual bool Open();
    FileNode();
    void RemoveAllDirectoryPaths();
    void RemoveDirectoryPath(char *);
protected:
    void ReadInternals(Graph::Stream *, int);
    void WriteInternals(Gfx::WriteTraverser *);
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    static StdBase::TypeInfo const s_TypeInfo;
private:
    static int const Version;
    int m_TypeHandle;
public:
    Text m_FileName;
    Vec<Text> m_DirectoryPaths;
    Graph::FileStream *m_pStream;
    bool field_174;
};
}
