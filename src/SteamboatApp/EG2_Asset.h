#pragma once
namespace EG2 { class Resource; }

namespace EG2 {
class Asset {
public:
    virtual bool LoadFromResource(EG2::Resource *) = 0;
protected:
    virtual ~Asset();
    Asset();
public:
    int field_4;
};
}
