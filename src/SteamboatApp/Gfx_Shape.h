#pragma once
#include "Gfx_Node.h"
#include "Graph_NodeTraverser.h"
#include "StdBase.h"
#include <windows.h>
namespace Gfx { class PickTraverser; }
namespace Gfx { class WriteTraverser; }
namespace Graph { class Stream; }

namespace Gfx {
class Shape : public Gfx::Node {
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    static StdBase::TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
protected:
    virtual ~Shape();
public:
    virtual Graph::NodeTraverser::Response Pick(Gfx::PickTraverser *);
    virtual void Write(Gfx::WriteTraverser *);
    virtual void Read(Graph::Stream *);
    virtual void ReadVis(Graph::Stream *);
    virtual int GetAtom(int, Vec3<float> **) = 0;
protected:
    Shape();
    Shape(Gfx::Shape const &);
    void ReadInternals(Graph::Stream *, int);
    void WriteInternals(Gfx::WriteTraverser *);
    static StdBase::TypeInfo const s_TypeInfo;
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
private:
    static int const Version;
public:
    void SetCalculateScreenRect(bool calculate) { mCalculateScreenRect = calculate; }
    bool GetCalculateScreenRect() { return mCalculateScreenRect; }
    const RECT &GetScreenRect() { return mScreenRect; }
    void SetScreenRectValid(bool valid) { mScreenRectValid = valid; }
    bool GetScreenRectValid() { return mScreenRectValid; }
    int m_TypeHandle;                                   // +0x013c
    bool mCalculateScreenRect;                          // +0x0140
    RECT mScreenRect;                                   // +0x0144
    bool mScreenRectValid;                              // +0x0154
};
}
