#include "Gfx_TraversalPath.h"
#include "MathConstants.h"
#include "Gfx_Node.h"
#include "Gfx_MatrixTransform.h"
#include "TMatrix4.h"
#include "StdBase.h"

// 0x100fcad0 ??0TraversalPath@Gfx@@QAE@XZ
Gfx::TraversalPath::TraversalPath()
{
}

// 0x100fcb10 ??0TraversalPath@Gfx@@QAE@ABVPath@Graph@@@Z
Gfx::TraversalPath::TraversalPath(const Graph::Path &path) : Graph::Path(path)
{
}

// 0x100fcb30 ??0TraversalPath@Gfx@@QAE@ABV01@@Z
Gfx::TraversalPath::TraversalPath(const Gfx::TraversalPath &path) : Graph::Path(path)
{
}

// 0x100fcb50 ??1TraversalPath@Gfx@@UAE@XZ
Gfx::TraversalPath::~TraversalPath()
{
}

// 0x100fcb60 ?IsVisible@TraversalPath@Gfx@@QBE_NXZ
bool Gfx::TraversalPath::IsVisible() const
{
    bool visible = true;
    for (int i = 0; i < GetLength(); i++) {
        Gfx::Node *node = StdBaseCast(GetNode(i), (Gfx::Node *)0);
        if (node) {
            visible = node->IsVisible();
            if (!visible)
                break;
        }
    }
    return visible;
}

// 0x100fcbc0 ?ComputeGlobalMatrix@TraversalPath@Gfx@@QBE?BV?$TMatrix4@M@@XZ
const TMatrix4<float> Gfx::TraversalPath::ComputeGlobalMatrix() const
{
    TMatrix4<float> matrix;
    matrix.identity();
    for (int i = GetLength() - 1; i >= 0; i--) {
        Gfx::MatrixTransform *transform = StdBaseCast(GetNode(i), (Gfx::MatrixTransform *)0);
        if (transform)
            matrix *= transform->GetMatrix()(0, 0); // literal: the original scales by the first element only
    }
    return matrix;
}

// 0x100fcaf0 ??_GTraversalPath@Gfx@@UAEPAXI@Z
// 0x100fc9e0 _$E2
// 0x100fc9f0 _$E1
// 0x100fca10 _$E5
// 0x100fca20 _$E4
// 0x100fca40 _$E8
// 0x100fca50 _$E7
// 0x100fca70 _$E11
// 0x100fca80 _$E10
// 0x100fcaa0 _$E14
// 0x100fcab0 _$E13