#pragma optimize("a", on) // evidence: the constructor and destructor load the type counters before the stores (no-alias code)
#include "Gfx_MatrixTransform.h"
#include "MathConstants.h"
#include "Gfx_CurrentTransformationMatrix.h"
#include "Gfx_WriteTraverser.h"
#include "Graph_FileStream.h"
#include "VisMisc.h"
#include "StdBase.h"

// 0x10104040 ?GetTypeInfo@MatrixTransform@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::MatrixTransform::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x10104050 ?GetTypeName@MatrixTransform@Gfx@@UBEPBDXZ
char const * Gfx::MatrixTransform::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x10104060 ??0MatrixTransform@Gfx@@QAE@XZ
Gfx::MatrixTransform::MatrixTransform()
    : m_pExternalMatrix(0), m_Absolute(false), m_Enabled(true), m_UseExternalMatrix(false)
{
    m_Matrix.identity();
    IncrementTypeCount();
}

// 0x10104110 ??1MatrixTransform@Gfx@@UAE@XZ
Gfx::MatrixTransform::~MatrixTransform()
{
    DecrementTypeCount();
}

// 0x10104130 ?Traverse@MatrixTransform@Gfx@@UAE?AW4Response@NodeTraverser@Graph@@AAVPassiveTraverser@5@@Z
Graph::NodeTraverser::Response Gfx::MatrixTransform::Traverse(Graph::PassiveTraverser &traverser)
{
    if (m_Enabled)
        Push(GetMatrix());
    Graph::NodeTraverser::Response response = ActiveNode::Traverse(traverser);
    if (m_Enabled)
        Pop();
    return response;
}

// 0x10104190 ?Write@MatrixTransform@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::MatrixTransform::Write(Gfx::WriteTraverser *traverser)
{
    unsigned char header[25] = "GfxMatrixTransform";
    if (WriteHeaderInfo(traverser, header))
        WriteInternals(traverser);
}

// 0x10104210 ?ReadVis@MatrixTransform@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::MatrixTransform::ReadVis(Graph::Stream *stream)
{
    Node::ReadInternals(stream, 1);
    ReadInternals(stream, 0);
}

// 0x10104230 ?Read@MatrixTransform@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::MatrixTransform::Read(Graph::Stream *stream)
{
    int version;
    Node::Read(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x10104260 ?Push@MatrixTransform@Gfx@@QAEXABV?$TMatrix4@M@@@Z
void Gfx::MatrixTransform::Push(const TMatrix4<float> &matrix)
{
    if (!m_Absolute) {
        Gfx::CurrentTransformationMatrix::s_pInstance->PushLeft(matrix);
        return;
    }
    Gfx::CurrentTransformationMatrix *ctm = Gfx::CurrentTransformationMatrix::s_pInstance;
    ctm->m_ModelMatrixStack.push(matrix);
    ctm->m_ModelChanged = true;
}

// 0x10104420 ?Pop@MatrixTransform@Gfx@@QAEXXZ
void Gfx::MatrixTransform::Pop()
{
    Gfx::CurrentTransformationMatrix::s_pInstance->Pop();
}

// TODO 0x10104430 ?WriteInternals@MatrixTransform@Gfx@@IAEXPAVWriteTraverser@2@@Z: the original keeps this in esi and the stream in edi and pushes the stream in each branch; VC6 swaps the registers and pushes it before the test (if/else, early return and repeated GetStream() all give the same code)
void Gfx::MatrixTransform::WriteInternals(Gfx::WriteTraverser *traverser)
{
    Node::WriteInternals(traverser);
    Graph::FileStream *stream = traverser->GetStream();
    stream->Write(Version);
    stream->Write((short)(m_Absolute ? 1 : 0));
    if (m_UseExternalMatrix)
        writeMatrix4f(*m_pExternalMatrix, stream);
    else
        writeMatrix4f(m_Matrix, stream);
}

// 0x101044a0 ?ReadInternals@MatrixTransform@Gfx@@IAEXPAVStream@Graph@@H@Z
void Gfx::MatrixTransform::ReadInternals(Graph::Stream *stream, int version)
{
    short absolute;
    stream->Read(&absolute);
    m_Absolute = absolute != 0;
    readMatrix4f(m_Matrix, stream);
    ScheduleRefresh(false);
}

int const Gfx::MatrixTransform::Version = 0;

// 0x10103f50 _$E37
// 0x10103f60 _$E36
// 0x10103f80 _$E40
// 0x10103f90 _$E39
// 0x10103fb0 _$E43
// 0x10103fc0 _$E42
// 0x10103fe0 _$E46
// 0x10103ff0 _$E45
// 0x10104010 _$E49
// 0x10104020 _$E48

// Compiler-generated and template instances this unit also emits:
// 0x101040f0 ??_GMatrixTransform@Gfx@@UAEPAXI@Z
// 0x101044f0 ?_Destroy@?$vector@V?$TMatrix4@M@@V?$allocator@V?$TMatrix4@M@@@std@@@std@@IAEXPAV?$TMatrix4@M@@0@Z
// 0x10104500 ?_Ucopy@?$vector@V?$TMatrix4@M@@V?$allocator@V?$TMatrix4@M@@@std@@@std@@IAEPAV?$TMatrix4@M@@PBV3@0PAV3@@Z
// 0x10104540 ?_Ufill@?$vector@V?$TMatrix4@M@@V?$allocator@V?$TMatrix4@M@@@std@@@std@@IAEXPAV?$TMatrix4@M@@IABV3@@Z
// 0x10104570 ?fill@std@@YAXPAV?$TMatrix4@M@@0ABV2@@Z
// 0x101045a0 ?copy_backward@std@@YAPAV?$TMatrix4@M@@PAV2@00@Z
