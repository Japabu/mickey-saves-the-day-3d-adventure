// MfPath2: the headings along a planned path.
// port-test-sources: MfPath2.cpp
#include "../test.h"
#include "MfPath2.h"

// A walk to where the actor already stands: the planner's points lie on one spot, the
// last one a float crumb off (2.4e-7, measured in the main menu). The headings must not
// turn a quarter (atan2 of the crumb); the menu's Mickey ended up sideways.
TEST(path2_update_same_spot_no_quarter_turn)
{
    MfPath2 path;
    path.SetNumPoints(3);
    path.Point(0).translation() = MfVector2(12.8283081f, -1.64587998f);
    path.Point(1).translation() = MfVector2(12.8283081f, -1.64587998f);
    path.Point(2).translation() = MfVector2(12.8283081f, -1.64588022f);
    path.Update();
    for (int i = 0; i < 3; i++)
        CHECK_NEAR(path.Point(i).rotation().angle(), 0.0, 1e-6);
}

// A real walk keeps its direction: along +y is a quarter turn.
TEST(path2_update_heading_follows_path)
{
    MfPath2 path;
    path.SetNumPoints(3);
    path.Point(0).translation() = MfVector2(0, 0);
    path.Point(1).translation() = MfVector2(0, 1);
    path.Point(2).translation() = MfVector2(0, 2);
    path.Update();
    for (int i = 0; i < 3; i++)
        CHECK_NEAR(path.Point(i).rotation().angle(), 1.5707963, 1e-5);
}
