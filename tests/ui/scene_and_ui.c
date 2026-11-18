#include "../test_common.h"

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#define SCENE_IMPLEMENTATION
#include "../../scene.h"

#define RENDERER_IMPLEMENTATION
#include "../../renderer.h"

#define INTERFACE_IMPLEMENTATION
#include "../../interface.h"

static void TestMakeDefaultCamera(void) {
    Camera cam = MakeDefaultCamera();

    EXPECT_VEC3("Default Camera Position == (0, 0, 3)", cam.Position, ((Vec3){0.0f, 0.0f, 3.0f}));
    EXPECT_VEC3("Default Camera rotation == (0, 0, 0)", cam.rotation, ((Vec3){0.0f, 0.0f, 0.0f}));
    EXPECT_VEC3("Default Camera target == (0, 0, 0)", cam.target, ((Vec3){0.0f, 0.0f, 0.0f}));
    EXPECT_VEC3("Default Camera up == (0, 1, 0)", cam.up, ((Vec3){0.0f, 1.0f, 0.0f}));
    EXPECT_INT("Default Camera pType == PTPerspective", cam.pType, PTPerspective);
    EXPECT_FLOAT("Default Camera fov == pi/6", cam.fov, PI_F / 6.0f);
    EXPECT_FLOAT("Default Camera zNear == 0.1", cam.zNear, 0.1f);
    EXPECT_FLOAT("Default Camera zFar == 100.0", cam.zFar, 100.0f);
    EXPECT_FLOAT("Default Camera left == -1.0", cam.left, -1.0f);
    EXPECT_FLOAT("Default Camera right == 1.0", cam.right, 1.0f);
    EXPECT_FLOAT("Default Camera bottom == -1.0", cam.bottom, -1.0f);
    EXPECT_FLOAT("Default Camera top == 1.0", cam.top, 1.0f);
}

static void TestSceneModelManagement(void) {
    Scene scene = {};

    Model m0 = MakePrimativeCube();
    m0.position = (Vec3){1.0f, 0.0f, 0.0f};
    Model m1 = MakePrimativeCube();
    m1.position = (Vec3){2.0f, 0.0f, 0.0f};
    Model m2 = MakePrimativeCube();
    m2.position = (Vec3){3.0f, 0.0f, 0.0f};

    AddModel(&scene, m0);
    AddModel(&scene, m1);
    AddModel(&scene, m2);
    EXPECT_INT("AddModel: count == 3", arrlen(scene.models), 3);

    // Out-of-bounds removals should be ignored safely
    RemoveModel(&scene, -1);
    RemoveModel(&scene, 99);
    EXPECT_INT("RemoveModel out-of-bounds ignored", arrlen(scene.models), 3);

    // Remove middle model (m1)
    RemoveModel(&scene, 1);
    EXPECT_INT("RemoveModel middle: count == 2", arrlen(scene.models), 2);
    EXPECT_VEC3("RemoveModel middle: models[0] is m0", scene.models[0].position, ((Vec3){1.0f, 0.0f, 0.0f}));
    EXPECT_VEC3("RemoveModel middle: models[1] is m2", scene.models[1].position, ((Vec3){3.0f, 0.0f, 0.0f}));

    RemoveModel(&scene, 1);
    RemoveModel(&scene, 0);
    EXPECT_INT("RemoveModel all: count == 0", arrlen(scene.models), 0);
    arrfree(scene.models);
}

static void TestSceneCameraManagement(void) {
    Scene scene = {};

    Camera c0 = MakeDefaultCamera();
    c0.Position = (Vec3){0.0f, 0.0f, 1.0f};
    AddCamera(&scene, c0);
    EXPECT_INT("AddCamera: count == 1", arrlen(scene.cameras), 1);

    // Guard: cannot delete the last remaining camera
    RemoveCamera(&scene, 0);
    EXPECT_INT("RemoveCamera guards last remaining camera (count stays 1)", arrlen(scene.cameras), 1);

    Camera c1 = MakeDefaultCamera();
    c1.Position = (Vec3){0.0f, 0.0f, 2.0f};
    Camera c2 = MakeDefaultCamera();
    c2.Position = (Vec3){0.0f, 0.0f, 3.0f};
    AddCamera(&scene, c1);
    AddCamera(&scene, c2);
    EXPECT_INT("AddCamera: count == 3", arrlen(scene.cameras), 3);

    // Out-of-bounds removals ignored
    RemoveCamera(&scene, -1);
    RemoveCamera(&scene, 10);
    EXPECT_INT("RemoveCamera out-of-bounds ignored", arrlen(scene.cameras), 3);

    // Remove first camera (c0)
    RemoveCamera(&scene, 0);
    EXPECT_INT("RemoveCamera valid: count == 2", arrlen(scene.cameras), 2);
    EXPECT_VEC3("Remaining camera[0] is c1", scene.cameras[0].Position, ((Vec3){0.0f, 0.0f, 2.0f}));
    EXPECT_VEC3("Remaining camera[1] is c2", scene.cameras[1].Position, ((Vec3){0.0f, 0.0f, 3.0f}));

    arrfree(scene.cameras);
}

static void TestMakeDefaultRenderSettings(void) {
    RenderSettings s = MakeDefaultRenderSettings();

    EXPECT_VEC4("Default RenderSettings tint", s.tint, ((Vec4){0.42f, 0.20f, 0.47f, 1.0f}));
    EXPECT_TRUE("Default RenderSettings showProblem == false", !s.showProblem);
    EXPECT_INT("Default RenderSettings problemX == 300", s.problemX, 300);
    EXPECT_INT("Default RenderSettings problemY == 300", s.problemY, 300);
    EXPECT_TRUE("Default RenderSettings showDebugLayer == true", s.showDebugLayer);
    EXPECT_INT("Default RenderSettings renderingMethod == SOFTWARE", s.renderingMethod, RENDERING_METHOD_SOFTWARE);
}

static void TestColorConversions(void) {
    Color c = {255, 128, 0, 255};
    Vec4 v = V4FromColor(c);
    EXPECT_FLOAT("V4FromColor.r", v.r, 1.0f);
    EXPECT_FLOAT("V4FromColor.g", v.g, 128.0f / 255.0f);
    EXPECT_FLOAT("V4FromColor.b", v.b, 0.0f);
    EXPECT_FLOAT("V4FromColor.a", v.a, 1.0f);

    Color roundTrip = ColorFromV4((Vec4){1.0f, 0.0f, 1.0f, 1.0f});
    EXPECT_INT("ColorFromV4.r == 255", roundTrip.r, 255);
    EXPECT_INT("ColorFromV4.g == 0", roundTrip.g, 0);
    EXPECT_INT("ColorFromV4.b == 255", roundTrip.b, 255);
    EXPECT_INT("ColorFromV4.a == 255", roundTrip.a, 255);
}

static void TestFocusCameraAndTransformDelta(void) {
    Camera cam = MakeDefaultCamera();
    cam.Position = (Vec3){0.0f, 0.0f, 3.0f};
    cam.rotation = (Vec3){0.5f, -0.5f, 0.2f};

    // Focusing from (0, 0, 3) onto (0, 0, 0) looks straight down -Z -> rotation (0, 0, 0), target (0, 0, 0), up (0, 1, 0)
    FocusCameraOnPoint(&cam, (Vec3){0.0f, 0.0f, 0.0f});
    EXPECT_VEC3("FocusCameraOnPoint along -Z gives rotation (0,0,0)", cam.rotation, ((Vec3){0.0f, 0.0f, 0.0f}));
    EXPECT_VEC3("FocusCameraOnPoint sets cam.target", cam.target, ((Vec3){0.0f, 0.0f, 0.0f}));
    EXPECT_VEC3("FocusCameraOnPoint along -Z sets cam.up == (0,1,0)", cam.up, ((Vec3){0.0f, 1.0f, 0.0f}));

    // Focusing onto same position as camera should be a no-op (no NaN)
    cam.rotation = (Vec3){0.1f, 0.2f, 0.3f};
    FocusCameraOnPoint(&cam, cam.Position);
    EXPECT_VEC3("FocusCameraOnPoint on camera's own position is a safe no-op",
                cam.rotation, ((Vec3){0.1f, 0.2f, 0.3f}));

    // Focusing from (0, 0, 3) onto (0, 3, 3) looks straight up +Y -> pitch = +pi/2
    FocusCameraOnPoint(&cam, (Vec3){0.0f, 3.0f, 3.0f});
    EXPECT_FLOAT("FocusCameraOnPoint along +Y gives pitch +pi/2", cam.rotation.x, PI_F / 2.0f);

    // TransformDeltaByCamera: world mode vs camera-relative mode
    Vec3 delta = {1.0f, 2.0f, -3.0f};
    cam.rotation = (Vec3){0.7f, -1.2f, 0.4f};
    EXPECT_VEC3("TransformDeltaByCamera (relativeToCamera=false) preserves world delta",
                TransformDeltaByCamera(delta, &cam, false), delta);

    cam.rotation = (Vec3){0.0f, 0.0f, 0.0f};
    EXPECT_VEC3("TransformDeltaByCamera (identity camera) preserves delta",
                TransformDeltaByCamera(delta, &cam, true), delta);

    cam.rotation = (Vec3){0.0f, PI_F / 2.0f, 0.0f};
    Vec3 rotated = TransformDeltaByCamera((Vec3){1.0f, 0.0f, 0.0f}, &cam, true);
    EXPECT_FLOAT("TransformDeltaByCamera preserves vector length", V3Length(rotated), 1.0f);
}

int main(void) {
    SetTraceLogLevel(LOG_ERROR);
    PrintSuiteHeader("Running Scene, RenderSettings & UI Helper Unit Tests");

    TestMakeDefaultCamera();
    TestSceneModelManagement();
    TestSceneCameraManagement();
    TestMakeDefaultRenderSettings();
    TestColorConversions();
    TestFocusCameraAndTransformDelta();

    return TestSummary();
}
