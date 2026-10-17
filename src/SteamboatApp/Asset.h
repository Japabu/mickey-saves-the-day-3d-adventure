// EG2::Asset: the base of the engine's loadable assets (bitmaps): a resource
// loader and an instance count.
#ifndef ASSET_H
#define ASSET_H
namespace EG2 { class Resource; }

namespace EG2 {
class Asset {
public:
    virtual bool LoadFromResource(EG2::Resource *) = 0;
protected:
    virtual ~Asset();
    Asset();
public:
    int mInstanceCount;
};
}

#endif
