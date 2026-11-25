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
