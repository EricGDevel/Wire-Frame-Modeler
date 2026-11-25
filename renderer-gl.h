#if !defined(RENDERER_GL_HEADER)
#define RENDERER_GL_HEADER

#include <raylib.h>

#include "scene.h"
#include "renderer.h"

typedef enum {
    LOCATION_POSITION = 0,
    LOCATION_TEXCOORD = 1,
    LOCATION_NORMAL = 2,
} GpuVertexAttributeLocation;

typedef struct ShaderUniformLocations {
    int toScreenFromModel;
    int normalTransform;
    // TODO(student): add whatever uniforms you need in your model shaders
} ShaderUniformLocations;

typedef enum {
    SHADER_TYPE_FLAT,
    SHADER_TYPE_GOURAUD,
    SHADER_TYPE_PHONG,

    SHADER_TYPE_COUNT,
} ShaderType;

typedef struct Shader {
    unsigned int id;
    ShaderUniformLocations loc;
} Shader;

typedef struct RendererGL {
    RenderSettings *settings;

    struct {
        unsigned int id;
        int VAO;
        int VBO;
        int timeLoc;
    } backgroundShader;

    Shader shaders[SHADER_TYPE_COUNT];

    RenderTexture target;
} RendererGL;

void RendererLoadShaders(RendererGL *renderer);
void RendererUnloadShaders(RendererGL *renderer);
void RenderGLScene(Scene *scene, RendererGL *renderer, double deltaTime);

void LoadModelGLBuffers(Model *model);
void UnloadModelGLBuffers(Model *model);

#endif // RENDERER_GL_HEADER
