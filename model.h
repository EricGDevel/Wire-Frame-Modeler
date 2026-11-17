#if !defined(MODEL_HEADER)
#define MODEL_HEADER

#include "math.h"

// Reference numbers start at 1 according to spec
// Set value to 0 if no reference number provided for specific field
typedef struct {
    unsigned int v;
    unsigned int vt;
    unsigned int vn;
} FaceElement;


typedef union {
    struct { FaceElement a, b, c; };
    FaceElement elements[3];
} TriangleFace;


typedef struct Model {
    // Model geometry data
    int vertexCount;
    Vec3 *vertices;

    int normalCount;
    Vec3 *normals;

    // Currently only supporting 2D textures
    // Later can be changed to a Vec3, and set the last coordinate's value to 0 by default
    int textureCount;
    Vec2 *textures;

    // Currently only supporting triangles, so each face has 3 vertices.
    // Each vertex has a vertex, normal and texture reference number.
    int faceCount;
    TriangleFace *faces;

    // Model transformation data
    Vec3 position;
    Vec3 rotation; // Euler angles
    Vec3 scale;
    Vec3 minBound, maxBound;

    bool hidden;
    bool showVertexNormals;
    bool showFaceNormals;
    bool showBoundingBox;

    // GPU handles (HW3)
    unsigned int gpuVertexBufferId;
    unsigned int gpuNormalBufferId;

    unsigned int gpuVertexArrayId;
} Model;


Model LoadModelFromStr(const char *str);
Model LoadModelFromObjFile(const char *filename);
void FreeModel(Model *model);

#endif // MODEL_HEADER
