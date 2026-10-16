// Mf2DVectorList: a list of 2D points that may own them (RTTI name; vtable
// 100b593c; unit 1007dc50-1007dd30), the points of a visual connector.
#ifndef RUNTIMELIB_MF2DVECTORLIST_H
#define RUNTIMELIB_MF2DVECTORLIST_H
#include "MfDblList.h"

struct Mf2DPoint {
    Mf2DPoint() { x = 0.0; y = 0.0; }
    Mf2DPoint(double px, double py) { x = px; y = py; }
    Mf2DPoint &operator=(const Mf2DPoint &p)
    {
        x = p.x;
        y = p.y;
        return *this;
    }
    double x, y;
};

// a point of the editor's drawing
struct Mf2DVector {
    Mf2DVector() {}
    // a point read by the parser (MfStateParser.cpp)
    Mf2DVector(double x, double y) : pos(x, y) {}
    ~Mf2DVector();

    Mf2DPoint pos;                                                              // 0x00
    Mf2DPoint m10;                                                              // 0x10
};

class Mf2DVectorList : public MfDblList {
public:
    Mf2DVectorList(int ownsItems);
    virtual ~Mf2DVectorList() { Clear(); }
    // deletes the points it owns
    virtual void Clear();

    int mOwnsItems;                                                             // 0x10
};

#endif
