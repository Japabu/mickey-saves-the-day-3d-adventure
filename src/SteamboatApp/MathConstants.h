#pragma once
// Angle constants of the graphics library. Every unit that includes this
// header has its own copies; the derived ones are computed by a static
// initializer (one $E pair per constant).
static const double PI = 3.141592653589793;
static const double HALF_PI = PI * 0.5;
static const double QUARTER_PI = PI * 0.25;
static const double INV_PI = 1.0 / PI;
static const double DEG_TO_RAD = PI * (1.0 / 180.0);
static const double RAD_TO_DEG = 180.0 / PI;
