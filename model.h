#if !defined(MODEL_HEADER)
#define MODEL_HEADER

#include "math.h"

typedef struct Model {
    // Model geometry data
    int vertexCount;
    Vec3 *vertices;
    Vec3 *normals;
    // TODO(student): Add more vertex attributes ... 
    
    // Model transformation data
    Vec3 position;
    bool hidden;
    // TODO(student) ...
    
    // GPU handles (HW3)
    unsigned int gpuVertexBufferId;
    unsigned int gpuNormalBufferId;
    
    unsigned int gpuVertexArrayId;
} Model;


Model LoadModelFromObjFile(const char *filename);
void FreeModel(Model *model);

#endif // MODEL_HEADER

