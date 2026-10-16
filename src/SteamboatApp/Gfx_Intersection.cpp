// evidence: IntersectionStack::operator= keeps the source length and array in registers across
// the element copies (no-alias code, as in the other Graphics.lib units)
#pragma optimize("a", on)
#include "Gfx_Intersection.h"
#include "MathConstants.h"

// 0x100e21e0 _$E7
// 0x100e21f0 _$E6
// 0x100e2210 _$E10
// 0x100e2220 _$E9
// 0x100e2240 _$E13
// 0x100e2250 _$E12
// 0x100e2270 _$E16
// 0x100e2280 _$E15
// 0x100e22a0 _$E19
// 0x100e22b0 _$E18

// 0x100e22d0 ??0IntersectionRecord@Gfx@@QAE@XZ
Gfx::IntersectionRecord::IntersectionRecord()
{
}

// TODO 0x100e23d0 ??0IntersectionRecord@Gfx@@QAE@ABV01@@Z: member copies and EH states laid out differently (open)
Gfx::IntersectionRecord::IntersectionRecord(const Gfx::IntersectionRecord &record)
    : m_GroupPath(record.m_GroupPath), m_Path1(record.m_Path1), m_Path2(record.m_Path2),
      m_Matrix1(record.m_Matrix1), m_Matrix2(record.m_Matrix2), m_AtomList(record.m_AtomList),
      m_Point1(record.m_Point1), m_Point2(record.m_Point2), m_Normal(record.m_Normal),
      m_Sphere1(record.m_Sphere1), m_Sphere2(record.m_Sphere2)
{
}

// TODO 0x100e25f0 ??0IntersectionRecord@Gfx@@QAE@ABVTraversalPath@1@00ABV?$TMatrix4@M@@1ABV?$Vec@H@@ABV?$Vec3@M@@33ABVBoundingVolume@1@4@Z: member copies and EH states laid out differently (open)
Gfx::IntersectionRecord::IntersectionRecord(const Gfx::TraversalPath &groupPath, const Gfx::TraversalPath &path1,
                                            const Gfx::TraversalPath &path2, const TMatrix4<float> &matrix1,
                                            const TMatrix4<float> &matrix2, const Vec<int> &atomList,
                                            const Vec3<float> &point1, const Vec3<float> &point2,
                                            const Vec3<float> &normal, const Gfx::BoundingVolume &volume1,
                                            const Gfx::BoundingVolume &volume2)
    : m_GroupPath(groupPath), m_Path1(path1), m_Path2(path2), m_Matrix1(matrix1), m_Matrix2(matrix2),
      m_AtomList(atomList), m_Point1(point1), m_Point2(point2), m_Normal(normal), m_Sphere1(volume1),
      m_Sphere2(volume2)
{
}

// TODO 0x100e27c0 ?Set@IntersectionRecord@Gfx@@QAEXABVTraversalPath@2@00ABV?$TMatrix4@M@@1ABV?$Vec@H@@ABV?$Vec3@M@@33ABVBoundingVolume@2@4@Z: the original reads the BoundingSphere temporaries from the stack, here through the constructor result
void Gfx::IntersectionRecord::Set(const Gfx::TraversalPath &groupPath, const Gfx::TraversalPath &path1,
                                  const Gfx::TraversalPath &path2, const TMatrix4<float> &matrix1,
                                  const TMatrix4<float> &matrix2, const Vec<int> &atomList,
                                  const Vec3<float> &point1, const Vec3<float> &point2, const Vec3<float> &normal,
                                  const Gfx::BoundingVolume &volume1, const Gfx::BoundingVolume &volume2)
{
    m_GroupPath = groupPath;
    m_Path1 = path1;
    m_Path2 = path2;
    m_Matrix1 = matrix1;
    m_Matrix2 = matrix2;
    m_AtomList = atomList;
    m_Point1 = point1;
    m_Point2 = point2;
    m_Normal = normal;
    m_Sphere1 = Gfx::BoundingSphere(volume1);
    m_Sphere2 = Gfx::BoundingSphere(volume2);
}

// TODO 0x100e2920 ??1IntersectionRecord@Gfx@@QAE@XZ: the original has no EH state for the atom list (Vec<int>) while it is destroyed
Gfx::IntersectionRecord::~IntersectionRecord()
{
}

// TODO 0x100e29b0 ??4IntersectionRecord@Gfx@@QAEAAV01@ABV01@@Z: the original keeps &m_Matrix1/&m_Matrix2 (operator= results) in eax
Gfx::IntersectionRecord &Gfx::IntersectionRecord::operator=(const Gfx::IntersectionRecord &record)
{
    m_GroupPath = record.m_GroupPath;
    m_Path1 = record.m_Path1;
    m_Path2 = record.m_Path2;
    m_Matrix1 = record.m_Matrix1;
    m_Matrix2 = record.m_Matrix2;
    m_AtomList = record.m_AtomList;
    m_Point1 = record.m_Point1;
    m_Point2 = record.m_Point2;
    m_Normal = record.m_Normal;
    m_Sphere1 = record.m_Sphere1;
    m_Sphere2 = record.m_Sphere2;
    return *this;
}

// 0x100e2b00 ??0IntersectionStack@Gfx@@QAE@XZ
Gfx::IntersectionStack::IntersectionStack()
{
}

// TODO 0x100e2b50 ??0IntersectionStack@Gfx@@QAE@ABV01@@Z: Vec copy constructor laid out differently (open)
Gfx::IntersectionStack::IntersectionStack(const Gfx::IntersectionStack &stack)
    : m_Intersections(stack.m_Intersections)
{
}

// 0x100e2c50 ??1IntersectionStack@Gfx@@QAE@XZ
Gfx::IntersectionStack::~IntersectionStack()
{
}

// 0x100e2cd0 ?Pop@IntersectionStack@Gfx@@QAE?AVIntersectionRecord@2@XZ
Gfx::IntersectionRecord Gfx::IntersectionStack::Pop()
{
    Gfx::IntersectionRecord record = m_Intersections[m_Intersections.length() - 1];
    m_Intersections.setLength(m_Intersections.length() - 1);
    return record;
}

// 0x100e2f00 ??4IntersectionStack@Gfx@@QAEAAV01@ABV01@@Z
Gfx::IntersectionStack &Gfx::IntersectionStack::operator=(const Gfx::IntersectionStack &stack)
{
    m_Intersections = stack.m_Intersections;
    return *this;
}

// Template and inline instances this unit also emits:
// 0x100e2f60 ??1?$Vec@VIntersectionRecord@Gfx@@@@UAE@XZ
// 0x100e2fe0 ??_G?$Vec@VIntersectionRecord@Gfx@@@@UAEPAXI@Z
// 0x100e3000 ?reallocMem@?$Vec@VIntersectionRecord@Gfx@@@@IAEXHHH@Z
