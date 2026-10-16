#pragma once
#include <memory>
#include <stack>
#include <vector>
#include "Gfx_Attribute.h"
#include "Graph_NodeContext.h"
#include "StdBase.h"
#include "Text.h"
namespace Gfx { class RefreshTraverser; }
namespace Gfx { class WriteTraverser; }
namespace Graph { class PassiveTraverser; }
namespace Graph { class Stream; }

namespace Gfx {
// Texture of the nodes below it: a file, its colour key, filtering, blending
// and wrapping, and the texture stage it is applied to (one attribute stack
// per stage).
class TextureAttribute : public Gfx::Attribute {
protected:
    void IncrementTypeCount();
    void DecrementTypeCount();
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
    enum {
        MaxTextureStages = 8
    };
    enum FilterType {
        c_NoFilter = 0,
        c_Bilinear = 1,
        c_Trilinear = 2
    };
    enum BlendMode {
        c_Normal = 0,
        c_Additive = 1
    };
    enum TextureStageCompositeMode {
        c_Default = 0,
        c_ModulateTextureWithPreviousStage = 1,
        c_ModulateTextureWithPreviousStageX2 = 2,
        c_ModulateTextureWithPreviousStageX4 = 3,
        c_AddTextureToPreviousStage = 4,
        c_AddTextureToPreviousStageSigned = 5,
        c_AddTextureToPreviousStageSignedX2 = 6,
        c_AddTextureToPreviousStageSmooth = 7
    };
    TextureAttribute(const Gfx::TextureAttribute &);
    TextureAttribute();
    static std::stack<Gfx::TextureAttribute const *, std::vector<Gfx::TextureAttribute const *, std::allocator<Gfx::TextureAttribute const *> > > &GetStack(int stage) { return s_Stack[stage]; }
    bool WantTexturing() const;
    const char *GetFilename() const { return m_Filename; }
    void SetFilename(const char *filename) { m_Filename = filename; SetTexture(); }
    bool HasColorKey() const { return m_HasColorKey; }
    void SetHasColorKey(bool hasColorKey) { m_HasColorKey = hasColorKey; SetTexture(); }
    int GetColorKey() const { return m_ColorKey; }
    void SetColorKey(unsigned long colorKey) { m_ColorKey = colorKey; SetTexture(); }
    FilterType GetFilter() const { return m_Filter; }
    void SetFilter(FilterType filter) { m_Filter = filter; }
    bool GetMipMap() const { return m_MipMap; }
    void SetMipMap(bool mipMap) { m_MipMap = mipMap; SetTexture(); }
    bool HasAlpha() const { return m_Alpha; }
    void SetAlpha(bool alpha) { m_Alpha = alpha; SetTexture(); }
    BlendMode GetBlendMode() const { return m_BlendMode; }
    void SetBlendMode(BlendMode blendMode) { m_BlendMode = blendMode; }
    void SetWrapU(bool wrap) { m_WrapU = wrap; }
    void SetWrapV(bool wrap) { m_WrapV = wrap; }
    bool GetWrapU() const { return m_WrapU; }
    bool GetWrapV() const { return m_WrapV; }
    int GetTextureStage() const { return m_TextureStage; }
    void SetTextureStage(int stage) { m_TextureStage = stage; }
    TextureStageCompositeMode GetTextureStageCompositeMode() { return m_TextureStageCompositeMode; }
    bool GetMultiPassOverride() const { return m_MultiPassOverride; }
    void SetMultiPassOverride(bool multiPassOverride) { m_MultiPassOverride = multiPassOverride; }
    bool GetEnable() const;
    void SetEnable(bool);
    void operator=(const Gfx::TextureAttribute &);
    bool operator==(const Gfx::TextureAttribute &) const;
    bool operator!=(const Gfx::TextureAttribute &texture) const { return !(*this == texture); }
    static void SetTextureAttributeEnableContext(Graph::NodeContext<Gfx::TextureAttribute, bool> *context) { m_TextureAttributeEnableContext = context; }
    static Graph::NodeContext<Gfx::TextureAttribute, bool> *GetTextureAttributeEnableContext() { return m_TextureAttributeEnableContext; }
    unsigned long GetTextureColor(Gfx::RefreshTraverser &, float, float);
    virtual void SetTexture() = 0;
    virtual void SetTextureStageCompositeMode(TextureStageCompositeMode) = 0;
    virtual void Apply(Graph::PassiveTraverser *) const = 0;
    virtual void Push(Graph::PassiveTraverser *) const;
    virtual void Pop(Graph::PassiveTraverser *) const;
    virtual void Write(Gfx::WriteTraverser *);
    virtual void ReadVis(Graph::Stream *);
    virtual void Read(Graph::Stream *);
protected:
    virtual ~TextureAttribute();
    void WriteInternals(Gfx::WriteTraverser *);
    void ReadInternals(Graph::Stream *, int);
    virtual unsigned char *GetTextureCache(Gfx::RefreshTraverser &) = 0;
    BlendMode m_BlendMode;
    unsigned long m_ColorKey;
    bool m_HasColorKey;
    Text m_Filename;
    FilterType m_Filter;
    bool m_MipMap;
    bool m_Alpha;
    bool m_WrapU;
    bool m_WrapV;
    int m_TextureStage;
    TextureStageCompositeMode m_TextureStageCompositeMode;
    bool m_MultiPassOverride;
    bool m_BitsAccessable;
    bool m_Enable;
    unsigned char *m_CachedSurface;
    int m_CachedSurfaceWidth;
    int m_CachedSurfaceHeight;
    int m_CachedSurfaceDepth;
private:
    static int const Version;
    static Graph::NodeContext<Gfx::TextureAttribute, bool> *m_TextureAttributeEnableContext;
    static std::stack<Gfx::TextureAttribute const *, std::vector<Gfx::TextureAttribute const *, std::allocator<Gfx::TextureAttribute const *> > > s_Stack[8];
};
}
