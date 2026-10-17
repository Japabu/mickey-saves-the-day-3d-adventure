// MBMinMax.h - the engine's MBmax and MBmin (references, as std::max/std::min); MBRect uses them.
#ifndef MBENGINE_MBMINMAX_H
#define MBENGINE_MBMINMAX_H

template <class T> inline const T &MBmax(const T &a, const T &b) { return a < b ? b : a; }
template <class T> inline const T &MBmin(const T &a, const T &b) { return b < a ? b : a; }

#endif
