// MBStl.h - STL configuration of the muffin code: the library templates are used
// without exceptions, so _TRY_BEGIN/_CATCH compile to plain blocks. Evidence: the
// binaries are built without /GX, yet basic_string::_Copy has no EH frame and is
// inlined into _Grow (VC6 never inlines a function containing try); stock <xstddef>
// gives _Copy an EH frame. Include before any STL header.
#ifndef MBSTL_H
#define MBSTL_H
#include <xstddef>
#undef _TRY_BEGIN
#undef _CATCH
#undef _CATCH_ALL
#undef _CATCH_END
#define _TRY_BEGIN {
#define _CATCH(x) } if (0) {
#define _CATCH_ALL } if (0) {
#define _CATCH_END }
#endif
