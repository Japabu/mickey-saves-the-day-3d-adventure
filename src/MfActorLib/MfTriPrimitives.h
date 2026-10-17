#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// Triangle sets of simple shapes (the handle, camera and light models of
// MfProtoNode::CreateColModel). Exported names only.
#include "MfTriangleSet.h"
class MfMaterial;
class MfVector3;

class MFACTOR_API MfTriSquare : public MfTriangleSet {
public:
    MfTriSquare(const MfMaterial &, float width, float height);
};

class MFACTOR_API MfTriCube : public MfTriangleSet {
public:
    MfTriCube(const MfMaterial &, const MfVector3 &size);
};

class MFACTOR_API MfTriSphere : public MfTriangleSet {
public:
    MfTriSphere(const MfMaterial &, float radius, int steps);
};

class MFACTOR_API MfTriCone : public MfTriangleSet {
public:
    MfTriCone(const MfMaterial &, float radius, float height, int steps);
};

class MFACTOR_API MfTriCylinder : public MfTriangleSet {
public:
    MfTriCylinder(const MfMaterial &, float radius, float height, int steps);
};

class MFACTOR_API MfTriLine : public MfTriangleSet {
public:
    MfTriLine(const MfMaterial &, const MfVector3 &from, const MfVector3 &to);
    void Update(const MfVector3 &from, const MfVector3 &to);
};

class MFACTOR_API MfTriBezier : public MfTriangleSet {
public:
    MfTriBezier(const MfMaterial &, MfVector3 *const points, int steps);
    void Update(MfVector3 *const points);
};

class MFACTOR_API MfTriCircle : public MfTriangleSet {
public:
    MfTriCircle(const MfMaterial &, float radius, int steps);
};

class MFACTOR_API MfTriRing : public MfTriangleSet {
public:
    MfTriRing(const MfMaterial &, float inner, float outer, float width, int steps);
};
