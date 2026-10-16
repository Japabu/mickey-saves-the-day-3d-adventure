#pragma once
#include "Gfx_Shape.h"
#include "StdBase.h"
#include "AVec.h"
#include "Vec3.h"
#include "TMatrix4.h"
namespace Gfx { class Camera; }
namespace Gfx { class WriteTraverser; }
namespace Gfx { class VertexClusterAttribute; }
namespace Graph { class Stream; }
namespace Gfx { class IndexedTriangleSet; }

namespace Gfx {
// Shape whose vertex data is packed per primitive (strips, lines) in m_CList;
// m_Binding packs one Bind value per vertex array.
class PackedVertexShape : public Gfx::Shape {
    // IndexedTriangleSet::ConvertPackedTriStripSetToIndexedTriangleSet reads
    // the arrays directly
    friend class Gfx::IndexedTriangleSet;
    // VertexClusterAttribute::ApplyClustersToPackedShape moves the
    // coordinates directly
    friend class Gfx::VertexClusterAttribute;
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
    static int GetCurrentTypeCount();
    static int GetTotalTypeCount();
    int GetTypeHandle() const { return m_TypeHandle; }
    enum Bind {
        BindNone = 1,
        BindVertex = 2,
        BindPrimitive = 4,
        BindPrimitiveIndex = 8,
        BindShape = 16
    };
    static short const NormalMask;
    static short const ColorMask;
    static short const TextureMask;
    static short const NormalShift;
    static short const ColorShift;
    static short const TextureShift;
    short GetBinding() const { return m_Binding; }
    Bind GetNormalBinding() const { return (Bind)(short)((m_Binding & NormalMask) >> NormalShift); }
    Bind GetColorBinding() const { return (Bind)(short)((m_Binding & ColorMask) >> ColorShift); }
    Bind GetTextureBinding() const { return (Bind)(short)((m_Binding & TextureMask) >> TextureShift); }
    int CountPrimitives() const;
    IVec<int> &GetPrim() { return m_Prim; }
    int GetNumCoords();
    const Vec3<float> &GetCoord(int);
    void SetCoord(int, const Vec3<float> &, bool);
    void AppendCoord(const Vec3<float> &, bool);
    void RemoveCoords(int, int, bool);
    int GetNumNormals();
    const Vec3<float> &GetNormal(int);
    void SetNormal(int, const Vec3<float> &, bool);
    void AppendNormal(const Vec3<float> &, bool);
    void RemoveNormals(int, int, bool);
    int GetNumDiffuseColors();
    const unsigned long GetDiffuseColor(int);
    void SetDiffuseColor(int, unsigned long, bool);
    void AppendDiffuseColor(unsigned long, bool);
    void RemoveDiffuseColors(int, int, bool);
    int GetNumSpecularColors();
    const unsigned long GetSpecularColor(int);
    void SetSpecularColor(int, unsigned long, bool);
    void AppendSpecularColor(unsigned long, bool);
    void RemoveSpecularColors(int, int, bool);
    int GetNumTextureStages();
    int GetNumTCoords(int);
    const Vec3<float> &GetTCoord(int, int);
    void SetTCoord(int, int, const Vec3<float> &, bool);
    void RemoveTCoords(int, int, int, bool);
    void AppendTextureStages(int);
    void AppendTCoordToStage(const Vec3<float> &, int, bool);
    void SetChangedVertices(int);
    void Shrink();
    void Clear();
    void SetBindings(int, Bind, Bind, Bind);
    void PackIndexedPrimitives(const IVec<int> &, const IVec<int> &, const Vec3<float> *, const Vec3<float> *, const Vec3<float> *, const Vec3<float> *);
    void PackPrimitives(const IVec<int> &, const Vec3<float> *, const Vec3<float> *, const Vec3<float> *, const Vec3<float> *);
    void UnpackPrimitives(IVec<int> &, AVec<Vec3<float> > &, AVec<Vec3<float> > &, AVec<Vec3<float> > &, AVec<Vec3<float> > &) const;
    void AppendPrimitive(int, const Vec3<float> *, const Vec3<float> *, const Vec3<float> *, const Vec3<float> *);
    void AppendPrims(const int *, int, bool);
    void SetPrim(int, int, bool);
    virtual void Write(Gfx::WriteTraverser *);
    virtual void ReadVis(Graph::Stream *);
    virtual void Read(Graph::Stream *);
    virtual void SetDrawFunction();
    void CalculateVertsPerStrip(int);
protected:
    PackedVertexShape(const Gfx::PackedVertexShape &);
    PackedVertexShape();
    virtual ~PackedVertexShape();
    void SetBinding(short);
    void SetNormalBinding(Bind);
    void SetColorBinding(Bind);
    void SetTextureBinding(Bind);
    void WriteInternals(Gfx::WriteTraverser *);
    void ReadInternals(Graph::Stream *, int);
    void InitOffsets(int);
    void PackCListColorRef();
    void UnPackCListColorRef();
    void SeparateVertexInfo();
    void SeparateVertexInfoV3();
    void ApplySphereMap(Gfx::Camera *, int);
    IVec<int> m_Prim;
    int m_ChangedVertices;
    AVec<Vec3<float> > m_Coords;
    AVec<Vec3<float> > m_Normals;
    Vec<unsigned long> m_DiffuseColors;
    Vec<unsigned long> m_SpecularColors;
    Vec<AVec<Vec3<float> > > m_TCoords;
    TMatrix4<float> S;
    TMatrix4<float> SS;
    TMatrix4<float> P;
    TMatrix4<float> ST;
    TMatrix4<float> SST;
    int m_nVerts;
    short m_Binding;
    RVec<float> m_CList;
    int m_Stride;
    int m_CoordStride;
    int m_NormalStride;
    int m_ColorStride;
    int m_TextureStride;
    int m_NormalOffset;
    int m_ColorOffset;
    int m_TextureOffset;
private:
    static int const Version;
    RVec<float> m_Coordsv3;
    RVec<float> m_Normalsv3;
    Vec<unsigned long> m_Colorsv3;
    AVec<RVec<float> > m_TCoordsv3;
};
}
