#pragma once
// MfVector3.h: Motivate's 3D vector (MfActorLib/mflinalg), as the game declares it
// (SteamboatApp.pdb). The constructors and destructor are inline: the game's units inline them
// or emit them as instances (Actor.obj, SteamboatNode.obj).

// own copy: the game calls mflinalg through its import thunks (no dllimport) and emits its own copies of
// inline members; the owner's header declares the class MFLINALG_API, dllimport here
// (Actor.obj and SteamboatNode.obj emit the constructors).
class MfVector3 {
public:
    MfVector3(const MfVector3 &src) : _x(src._x), _y(src._y), _z(src._z) {}
    MfVector3(float, float, float);
    MfVector3()
    {
        _x = 0.0f;
        _y = 0.0f;
        _z = 0.0f;
    }
    ~MfVector3() {}
    float &x() { return _x; }
    const float &x() const { return _x; }
    float &y() { return _y; }
    const float &y() const { return _y; }
    float &z() { return _z; }
    const float &z() const { return _z; }
private:
    float _x;                                           // +0x0000
    float _y;                                           // +0x0004
    float _z;                                           // +0x0008
};
