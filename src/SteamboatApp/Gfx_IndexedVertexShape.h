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

namespace Gfx {
// Shape whose primitives index into per-vertex arrays (coordinates, normals,
// colours, texture coordinates); m_Binding packs one Bind value per array.
class IndexedVertexShape : public Gfx::Shape {
    // VertexClusterAttribute::ApplyClustersToIndexedShape moves the
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
    void SetBindings(int, Bind, Bind, Bind);
    void SetNormalBinding(Bind binding) { m_Binding = (m_Binding & ~NormalMask) | (binding << NormalShift); }
    void SetColorBinding(Bind binding) { m_Binding = (m_Binding & ~ColorMask) | (binding << ColorShift); }
    void SetTextureBinding(Bind binding) { m_Binding = (m_Binding & ~TextureMask) | (binding << TextureShift); }
    Bind GetNormalBinding() const { return (Bind)(short)((m_Binding & NormalMask) >> NormalShift); }
    Bind GetColorBinding() const { return (Bind)(short)((m_Binding & ColorMask) >> ColorShift); }
    Bind GetTextureBinding() const { return (Bind)(short)((m_Binding & TextureMask) >> TextureShift); }
    int CountPrimitives() const;
    IVec<int> &GetPrim() { return m_Prim; }
    IVec<int> &GetIndices() { return m_Indices; }
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
    void Shrink();
    void Clear();
    virtual void Write(Gfx::WriteTraverser *);
    virtual void Read(Graph::Stream *);
    void NormalizeNormals();
    void AppendIndices(int const *, int, bool);
    void AppendPrims(int const *, int, bool);
    virtual void SetDrawFunction();
protected:
    IndexedVertexShape(const Gfx::IndexedVertexShape &);
    IndexedVertexShape(const Gfx::Shape &);
    IndexedVertexShape();
    virtual ~IndexedVertexShape();
    void SetBinding(short);
    void WriteInternals(Gfx::WriteTraverser *);
    void ReadInternals(Graph::Stream *, int);
    // (the default is how IndexedTriangleSet's constructors pass this class's
    // private Version)
    void InitOffsets(int version = Version);
    void PackCListColorRef();
    void UnPackCListColorRef();
    void SeparateVertexInfo();
    void SeparateVertexInfoV3();
    void ApplySphereMap(Gfx::Camera *, int);
    IVec<int> m_Prim;
    IVec<int> m_Indices;
    RVec<float> m_CList;
    int m_Stride;
    AVec<Vec3<float> > m_Coords;
    AVec<Vec3<float> > m_Normals;
    Vec<unsigned long> m_DiffuseColors;
    Vec<unsigned long> m_SpecularColors;
    Vec<AVec<Vec3<float> > > m_TCoords;
    TMatrix4<float> S;
    TMatrix4<float> P;
    TMatrix4<float> ST;
    int m_nVerts;
    int m_NormalOffset;
    int m_ColorOffset;
    int m_TextureOffset;
    short m_Binding;
private:
    static int const Version;
    RVec<float> m_Coordsv3;
    RVec<float> m_Normalsv3;
    Vec<unsigned long> m_Colorsv3;
    AVec<RVec<float> > m_TCoordsv3;
    int m_CoordStride;
    int m_NormalStride;
    int m_ColorStride;
    int m_TextureStride;
};
}
