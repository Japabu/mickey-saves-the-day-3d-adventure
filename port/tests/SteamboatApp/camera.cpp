// The scene camera (Gfx::Camera on EAICamera/EAIRefFrame).
#include "../test.h"
#include "Gfx_Camera.h"

// RenderManager::CreateStandardViewD3D sets up the view camera this way; setting the same
// up vector again must not move the from point (it turned into NaN, and with it the view
// matrix, so nothing 3D was drawn).
TEST(camera_setup_keeps_points)
{
    Gfx::Camera camera;
    camera.SetFrom(Vec3<float>(0, 0, 0));
    camera.SetAt(Vec3<float>(0, 0, -1));
    camera.SetUp(Vec3<float>(0, 1, 0));
    for (int i = 0; i < 3; i++)
        CHECK_NEAR(camera.GetFrom().e[i], 0, 1e-6);
    CHECK_NEAR(camera.GetAt().e[2], -1, 1e-6);
    CHECK_NEAR(camera.GetUp().e[1], 1, 1e-6);
}

// An up vector along the view is invalid: the frame turns instead.
TEST(camera_up_along_view_turns_frame)
{
    Gfx::Camera camera;
    camera.SetFrom(Vec3<float>(0, 0, 0));
    camera.SetAt(Vec3<float>(0, 0, -1));
    camera.SetUp(Vec3<float>(0, 0, 1));
    for (int i = 0; i < 3; i++) {
        CHECK(camera.GetFrom().e[i] == camera.GetFrom().e[i]);
        CHECK(camera.GetAt().e[i] == camera.GetAt().e[i]);
    }
}

// The world from/at points follow the world matrix.
TEST(camera_world_points)
{
    Gfx::Camera camera;
    TMatrix4<float> world;
    world.identity();
    world[3][1] = 0.5f;
    world[3][2] = 5.0f;
    camera.SetWorldMatrix(world);
    camera.SetFrom(Vec3<float>(0, 0, 0));
    camera.SetAt(Vec3<float>(0, 0, -1));
    camera.SetUp(Vec3<float>(0, 1, 0));
    Vec3<float> from = camera.GetWorldFrom(), at = camera.GetWorldAt();
    CHECK_NEAR(from.e[1], 0.5, 1e-6);
    CHECK_NEAR(from.e[2], 5, 1e-6);
    CHECK_NEAR(at.e[2], 4, 1e-6);
}
