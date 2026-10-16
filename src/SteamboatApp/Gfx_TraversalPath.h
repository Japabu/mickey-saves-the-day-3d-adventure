#pragma once
#include "Graph_Path.h"
template <class T0> class TMatrix4;

namespace Gfx {
// A path through a graphics scene: visibility and the accumulated transform.
class TraversalPath : public Graph::Path {
public:
    TraversalPath(const Gfx::TraversalPath &);
    TraversalPath(const Graph::Path &);
    TraversalPath();
    virtual ~TraversalPath();
    bool IsVisible() const;
    const TMatrix4<float> ComputeGlobalMatrix() const;
    Gfx::TraversalPath &operator=(const Gfx::TraversalPath &path)
    {
        if (this != &path)
            Path::operator=(path);
        return *this;
    }
    Gfx::TraversalPath &operator=(const Graph::Path &path)
    {
        Path::operator=(path);
        return *this;
    }
    bool operator==(const Graph::Path &) const;
};
}
