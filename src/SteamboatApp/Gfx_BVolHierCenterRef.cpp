// own copy: an out-of-line empty constructor, which the canonical BVolHierCenterRef.h
// (the layout the hierarchy code uses, with the implicit constructor) does not
// declare; declaring it there would make every Vec<BVolHierCenterRef> construct
// its elements through it.
namespace Gfx {
struct BVolHierCenterRef {
public:
    BVolHierCenterRef();
};
}

// 0x100e6160 ??0BVolHierCenterRef@Gfx@@QAE@XZ
Gfx::BVolHierCenterRef::BVolHierCenterRef()
{
}
