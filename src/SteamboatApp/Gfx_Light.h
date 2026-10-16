#pragma once
#include "Gfx_Node.h"
#include "StdBase.h"
#include "Vec3.h"
#include "Vec4.h"
namespace Gfx { class WriteTraverser; }
namespace Graph { class Stream; }

namespace Gfx {
// Light source node; the device-specific subclass holds the light.
class Light : public Gfx::Node {
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
    enum LightType {
        Directional = 0,
        Point = 1,
        ParallelPoint = 2
    };
    virtual ~Light() {}
    virtual const Vec3<float> GetPosition() = 0;
    virtual void SetPosition(const Vec3<float> &) = 0;
    virtual void SetDirection(const Vec3<float> &) = 0;
    virtual const Vec3<float> GetDirection() = 0;
    virtual LightType GetType() = 0;
    virtual void SetType(LightType) = 0;
    virtual void SetColor(const Vec4<float> &) = 0;
    virtual const Vec4<float> GetColor() = 0;
    virtual void SetAmbient(const Vec4<float> &) = 0;
    virtual const Vec4<float> GetAmbient() = 0;
    virtual void TurnOn() = 0;
    virtual void TurnOff() = 0;
    virtual void Write(Gfx::WriteTraverser *);
    virtual void Read(Graph::Stream *);
protected:
    Light(const Gfx::Light &);
    Light() {}
    void WriteInternals(Gfx::WriteTraverser *);
    void ReadInternals(Graph::Stream *, int);
private:
    static int const Version;
};
}
