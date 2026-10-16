# mflinalg: source patterns that match (VC5 /O2 /GX /MD /GR)

- The operand order of commutative x87 operations depends on the headers the
  unit includes: MfVector2.cpp matches operator+, operator* and dot with
  math.h, MfMath.h, MfVector2.h, MfVector3.h (with MfVector2.h alone only
  normalize matches). Comments and extra forward declarations before a function
  do not change it. Search include sets (order matters) before rewriting code.
- Writing `a * b` or `b * a`, `*=` or `= x *` does not change the emitted order.
- The 3-byte accessors at 0x10001710-0x10001730 (x(), y(), z() and their folded
  twins) come from MfMatrix3.cpp's inline block.
- Wave 7 sweep (scripted, include block rewritten, TODOs checked as claims):
  the tie-breaks follow a symbol counter (adding `extern int dN;` lines cycles
  operator+/dot with period 8), but in MfVector2.cpp operator*(float) and
  normalize, and in MfVector3.cpp the second function and operator*=, flip
  together: no include set (256 for MfVector2, 24 for MfVector3) matches both, so
  one of each pair needs a different source shape, not other includes.
- Wave 10 (bodies for every open function):
  - Quaternion -> matrix (MfMatrix3(const MfRotation3 &), MfXformMatrix3::setValue):
    `r.getValue(w, x, y, z)` into locals, then `1.0f - 2.0f * (y * y + z * z)` on
    the diagonal and `2.0f * (x * y) - 2.0f * (w * z)` off it (the doubling is
    `fadd st, st` on each product, not on the difference). In setValue(r) the
    translation is cleared as `_m[2][3]`, `_m[1][3]`, `_m[0][3]` (that order).
  - Functions returning a vector built row by row (MfXformMatrix3::operator*
    (const MfVector3 &), rotate) use `MfVector3 r; float x = v.x(), ...;
    r.x() = ...; return r;`: this gives the 0x14 frame with the dead zero
    store into the argument slot. `return MfVector3(a, b, c)` does not.
  - Code that reads the quaternion of another object (MfTransform3::Multiply)
    accesses `_rotation._w` etc. directly (MfRotation3.h declares
    `friend class MfTransform3`); copying through getValue() keeps all four in
    locals and is far off. Writing `a._rotation._x` in full rather than through
    a `const MfRotation3 &q` reference also changes the canonical order.
  - Euler angles (getEuler) match with atan2/cos/fabs and double 0.001
    comparisons; `rx += TWO_PI` compiles to `fsub -2pi`.
  - The static MfRotation3::_math (_$E12/_$E8/_$E11/_$E9 at 0x10003a70-0x10003aa0)
    matches when defined at the end of MfRotation3.cpp with
    `inline MfMath::~MfMath() {}` before it. Defined at the top it shifts the
    symbol counter (two operators flip); writing `~MfMath() {}` inside the
    class in MfRotation3_unit.h instead breaks five functions.
  - A float variable reused for |v|^2, |v| and 1/|v| (NormalizeCross:
    `float l = lengthSquared(); l = 1.0f / (float)sqrt(l);`) gives the three
    `fst` to one slot.
- Wave 11 (TODO sweep):
  - VC5 reassociates float sums and products without parentheses; the grouping
    it picks follows the same tie-breaks as the operand order (include set,
    declarations). Parentheses are respected: writing a sum in the grouping
    the original evaluates fixes the rounding (MfTransform3::Multiply(a, b)
    matched with parentheses on the y and z rows only; several matrix
    products are EQUIV with fully parenthesized rows).
  - Extra standard headers before the class headers flip the tie-breaks:
    MfXformMatrix3.cpp `<stdio.h>`; MfMatrix3.cpp `<stdio.h> <stdlib.h>
    <float.h> <assert.h> <limits.h>`; MfTransform3.cpp `<stdio.h> <float.h>
    <ctype.h>`; MfVector2.cpp `<stdio.h> <stdlib.h> <memory.h> <ctype.h>`
    (found by sweeping header subsets, checking every claim of the file).
  - MfMatrix3::eigenValues matches with the locals declared a..i in order.
