#pragma once
// Triangle sets of simple shapes (the handle, camera and light models of
// MfProtoNode::CreateColModel). Exported names only.
#include "MfTriangleSet.h"
class MfMaterial;
class MfVector3;

class __declspec(dllexport) MfTriSquare : public MfTriangleSet {
public:
    MfTriSquare(const MfMaterial &, float width, float height);
};

class __declspec(dllexport) MfTriCube : public MfTriangleSet {
public:
    MfTriCube(const MfMaterial &, const MfVector3 &size);
};

class __declspec(dllexport) MfTriSphere : public MfTriangleSet {
public:
    MfTriSphere(const MfMaterial &, float radius, int steps);
};

class __declspec(dllexport) MfTriCone : public MfTriangleSet {
public:
    MfTriCone(const MfMaterial &, float radius, float height, int steps);
};

class __declspec(dllexport) MfTriCylinder : public MfTriangleSet {
public:
    MfTriCylinder(const MfMaterial &, float radius, float height, int steps);
};

class __declspec(dllexport) MfTriLine : public MfTriangleSet {
public:
    MfTriLine(const MfMaterial &, const MfVector3 &from, const MfVector3 &to);
    void Update(const MfVector3 &from, const MfVector3 &to);
};

class __declspec(dllexport) MfTriBezier : public MfTriangleSet {
public:
    MfTriBezier(const MfMaterial &, MfVector3 *const points, int steps);
    void Update(MfVector3 *const points);
};

class __declspec(dllexport) MfTriCircle : public MfTriangleSet {
public:
    MfTriCircle(const MfMaterial &, float radius, int steps);
};

class __declspec(dllexport) MfTriRing : public MfTriangleSet {
public:
    MfTriRing(const MfMaterial &, float inner, float outer, float width, int steps);
};
