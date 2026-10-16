#pragma once
#include "Graph_BasicRefreshTraverser.h"
namespace Gfx { class View; }

namespace Gfx {
// The refresh traverser of the scene library: a basic refresh traverser
// that knows the view it refreshes for (declaration from SteamboatApp.pdb).
class RefreshTraverser : public Graph::BasicRefreshTraverser {
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
    RefreshTraverser();
    virtual ~RefreshTraverser();
    Gfx::View *GetView() { return m_pView; }
    void SetView(Gfx::View *view) { m_pView = view; }
private:
    Gfx::View *m_pView;
};
}
