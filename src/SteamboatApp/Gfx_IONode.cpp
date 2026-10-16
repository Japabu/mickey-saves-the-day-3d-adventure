#include "Gfx_IONode.h"
#include "Gfx_PickTraverser.h"
#include "MathConstants.h"
#include "Gfx_Node.h"
#include "StdBase.h"
#include "Gfx_Shape.h"
#include "Gfx_WriteTraverser.h"
#include "Graph_FileStream.h"
#include "Graph_CallbackTraverser.h"

// 0x10111e40 ?GetTypeInfo@IONode@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::IONode::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x10111e50 ?GetTypeName@IONode@Gfx@@UBEPBDXZ
char const * Gfx::IONode::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x10111e60 ??0IONode@Gfx@@IAE@XZ
Gfx::IONode::IONode() : mCopyOfContents(0), field_144(false), field_145(true)
{
}

// 0x10111eb0 ??1IONode@Gfx@@MAE@XZ
Gfx::IONode::~IONode()
{
}

// 0x10111ec0 ?GetCopyOfContents@IONode@Gfx@@QAEPAVNode@2@XZ
Gfx::Node * Gfx::IONode::GetCopyOfContents()
{
    return mCopyOfContents;
}


// 0x10111ed0 ?Close@IONode@Gfx@@UAE_NXZ
bool Gfx::IONode::Close()
{
    Gfx::Node *contents = mCopyOfContents;
    if (contents) {
        contents->RemoveAllChildren();
        RemoveChild(FindChild(contents));
        mCopyOfContents = 0;
    }
    return true;
}

// TODO 0x10111f10 ?Pick@IONode@Gfx@@UAE?AW4Response@NodeTraverser@Graph@@PAVPickTraverser@2@@Z: 48% - the original keeps this in a stack slot and the inlined Vec append/remove use other registers
Graph::NodeTraverser::Response Gfx::IONode::Pick(Gfx::PickTraverser *traverser)
{
    if (!mCopyOfContents)
        return Graph::NodeTraverser::Continue;
    traverser->PushNode(*mCopyOfContents);
    Graph::NodeTraverser::Response response = mCopyOfContents->Pick(traverser);
    traverser->PopNode();
    return response;
}

// 0x10112070 ?Render@IONode@Gfx@@UAE?AW4Response@NodeTraverser@Graph@@PAVRenderTraverser@2@@Z
Graph::NodeTraverser::Response Gfx::IONode::Render(Gfx::RenderTraverser *traverser)
{
    if (!mCopyOfContents)
        return Graph::NodeTraverser::Continue;
    return mCopyOfContents->Render(traverser);
}

// 0x10112090 ?HandleEvent@IONode@Gfx@@UAE?AW4Response@NodeTraverser@Graph@@PAVTraverser@2@@Z
Graph::NodeTraverser::Response Gfx::IONode::HandleEvent(Gfx::Traverser *traverser)
{
    if (!mCopyOfContents)
        return Graph::NodeTraverser::Continue;
    return mCopyOfContents->HandleEvent(traverser);
}

// 0x101120b0 ?ReadVis@IONode@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::IONode::ReadVis(Graph::Stream *stream)
{
    Node::ReadInternals(stream, 0);
    ReadInternals(stream, 0);
}

// 0x101120d0 ?Read@IONode@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::IONode::Read(Graph::Stream *stream)
{
    int version;
    Node::Read(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x10112100 ?ReadInternals@IONode@Gfx@@IAEXPAVStream@Graph@@H@Z
void Gfx::IONode::ReadInternals(Graph::Stream *stream, int)
{
    stream->Read((short *)&field_144);
}

// 0x10112120 ?Write@IONode@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::IONode::Write(Gfx::WriteTraverser *traverser)
{
    unsigned char header[10] = "IONode";
    if (WriteHeaderInfo(traverser, header))
        WriteInternals(traverser);
}

// 0x10112180 ?WriteInternals@IONode@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::IONode::WriteInternals(Gfx::WriteTraverser *traverser)
{
    Node::WriteInternals(traverser);
    traverser->GetStream()->Write((int)field_144);
}

// 0x101121b0 ?BuildBBHierTraversal@IONode@Gfx@@IAEXXZ
void Gfx::IONode::BuildBBHierTraversal()
{
    Graph::CallbackTraverser traverser;
    traverser.SetType(Gfx::Node::GetStaticTypeInfo());
    traverser.SetPreCallback(BuildBBHierCB, 0);
    traverser.Traverse(*mCopyOfContents);
}

// 0x10112230 ?BuildBBHierCB@IONode@Gfx@@KA?AW4Response@NodeTraverser@Graph@@PAVCallbackTraverser@5@PAVBasicNode@5@PAX@Z
Graph::NodeTraverser::Response Gfx::IONode::BuildBBHierCB(Graph::CallbackTraverser *, Graph::BasicNode *node, void *)
{
    node->IsOfType(Gfx::Shape::GetStaticTypeInfo());
    return Graph::NodeTraverser::Continue;
}

int const Gfx::IONode::Version = 0;

// 0x10111e90 ??_GIONode@Gfx@@MAEPAXI@Z
// 0x10111d50 _$E41
// 0x10111d60 _$E40
// 0x10111d80 _$E44
// 0x10111d90 _$E43
// 0x10111db0 _$E47
// 0x10111dc0 _$E46
// 0x10111de0 _$E50
// 0x10111df0 _$E49
// 0x10111e10 _$E53
// 0x10111e20 _$E52

// the run-time type of Gfx::IONode (values from the original's data section)
StdBase::TypeInfo const Gfx::IONode::s_TypeInfo = { "Gfx::IONode", &Gfx::Node::s_TypeInfo };
