#if !defined(SCENE_HEADER)
#define SCENE_HEADER

#include "model.h"
#include "math.h"

typedef enum {
    PTPerspective = 0,
    PTOrthographic = 1,
} ProjectionType;

typedef struct Camera {
    Vec3 Position;
    Vec3 rotation;
    ProjectionType pType;
    Vec3 target;
    Vec3 up;

    // FoV in Radians
    float fov, zNear, zFar;
    float left, right, bottom, top;
} Camera;

typedef struct Scene {
    Model *models;
    Camera *cameras;

    int activeModelIndex;
    int activeCameraIndex;

    bool advanceTime;
    float step;
} Scene;

Camera MakeDefaultCamera(void);
void AddModel(Scene *scene, Model model);
void RemoveModel(Scene *scene, int modelIndex);
void AddCamera(Scene *scene, Camera camera);
void RemoveCamera(Scene *scene, int cameraIndex);

#endif // SCENE_HEADER

// Scene Implementation
#if defined(SCENE_IMPLEMENTATION)
#if !defined(SCENE_IMPLEMENTED)
#define SCENE_IMPLEMENTED

#include <math.h>
#include "stb_ds.h" // For dynamic arrays

Camera MakeDefaultCamera(void) {
    Camera cam = {};
    cam.Position = (Vec3){0.0f, 0.0f, 3.0f};
    cam.rotation = (Vec3){0.0f, 0.0f, 0.0f};
    cam.pType = PTPerspective;
    cam.fov = PI / 6; // 60 degrees in radians
    cam.target = (Vec3){0, 0, 0};
    cam.up = (Vec3){0, 1, 0};
    cam.zNear = 0.1f;
    cam.zFar = 100.0f;
    cam.left = -1.0f;
    cam.right = 1.0f;
    cam.bottom = -1.0f;
    cam.top = 1.0f;
    return cam;
}

void AddModel(Scene *scene, Model model) { arrput(scene->models, model); }

void RemoveModel(Scene *scene, int modelIndex) {
    if (modelIndex >= 0 && modelIndex < arrlen(scene->models)) {
        FreeModel(&scene->models[modelIndex]);
        arrdel(scene->models, modelIndex);
    }
}

void AddCamera(Scene *scene, Camera camera) { arrput(scene->cameras, camera); }

void RemoveCamera(Scene *scene, int cameraIndex) {
    if (arrlen(scene->cameras) > 1 && cameraIndex >= 0 && cameraIndex < arrlen(scene->cameras)) {
        arrdel(scene->cameras, cameraIndex);
    }
}

#endif // SCENE_IMPLEMENTED
#endif // SCENE_IMPLEMENTATION
