#if !defined(INTERFACE_HEADER)
#define INTERFACE_HEADER

#include <raylib.h>

#include "renderer.h"
#include "scene.h"
#include "primitives.h"

static inline Vec4 V4FromColor(Color c) {
    return (Vec4){(float)(c).r / 255.f, (float)(c).g / 255.f, (float)(c).b / 255.f, (float)(c).a / 255.f};
}

static inline Color ColorFromV4(Vec4 v) {
    return (Color){(int)(v.r * 255.f), (int)(v.g * 255.f), (int)(v.b * 255.f), (int)(v.a * 255.f)};
}

void FocusCameraOnPoint(Camera *cam, Vec3 targetPos);
Vec3 TransformDeltaByCamera(Vec3 localDelta, const Camera *cam, bool relativeToCamera);
void CreateInterface(RenderSettings *settings, Scene *scene);

#endif // INTERFACE_HEADER
