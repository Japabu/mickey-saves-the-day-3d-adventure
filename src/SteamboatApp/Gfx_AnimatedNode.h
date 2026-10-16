#pragma once
#include "Gfx_Node.h"
#include "Graph_NodeTraverser.h"
#include "StdBase.h"
namespace Gfx { class WriteTraverser; }
namespace Graph { class PassiveTraverser; }
namespace Graph { class Stream; }

namespace Gfx {
// Shows one child at a time, stepping through the children at m_FrameRate
// frames per second (looping or stopping at the last); a fixed frame shows
// child m_StartFrame (no PDB declaration; member names chosen from the use).
class AnimatedNode : public Gfx::Node {
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
protected:
    virtual ~AnimatedNode();
public:
    virtual Graph::NodeTraverser::Response Traverse(Graph::PassiveTraverser &);
    virtual void Write(Gfx::WriteTraverser *);
    virtual void Read(Graph::Stream *);
    AnimatedNode();
protected:
    void ReadInternals(Graph::Stream *, int);
    void WriteInternals(Gfx::WriteTraverser *);
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    static StdBase::TypeInfo const s_TypeInfo;
private:
    static int const Version;
    int m_TypeHandle;
    int m_FixedFrame;
    int field_144;
    int field_148;
    int m_FrameRate;
    float m_CurrentFrame;
    float m_StartFrame;
    bool field_158;
    unsigned long m_LastTime;
    int field_160;
    float m_TimeScale;
    bool m_FirstFrame;
    bool m_Running;
    bool m_Loop;
};
}
