#if !defined(RENDERER_HEADER)
#define RENDERER_HEADER

#include "scene.h"
#include "math.h"

typedef enum {
    RENDERING_METHOD_SOFTWARE = 0, // HW1-2
    RENDERING_METHOD_OPENGL = 1, // HW3
    RENDERING_METHOD_RAYTRACING = 2, // HW4

    RENDERING_METHOD_COUNT,
} RenderingMethod;

// RenderSettings is pointed by the renderer and not stored on it directly,
// so we can share it between different renderers.
typedef struct RenderSettings {
    int width, height;

    bool showDebugLayer;
    Vec4 tint;
    bool showProblem;
    int problemX, problemY;

    RenderingMethod renderingMethod;
} RenderSettings;

typedef struct Renderer {
    RenderSettings *settings;
    Vec4 *pixels;
} Renderer;

RenderSettings MakeDefaultRenderSettings(void);
void RenderScene(Scene *scene, Renderer *renderer, double time);

#endif // RENDERER_HEADER
