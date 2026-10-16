// The model changed these declarations for this unit; they are its own and are not generated again.
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
