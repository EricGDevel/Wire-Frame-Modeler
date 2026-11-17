#include "../test_common.h"
#include "../../primitives.h"
#include <raylib.h>

static void ValidateFaceIndicesAndNonDegenerate(const char *prefix, Model m) {
    bool allValidV = true;
    bool allValidVt = true;
    bool allValidVn = true;
    bool allNonDegenerate = true;

    for (int i = 0; i < m.faceCount; ++i) {
        TriangleFace f = m.faces[i];
        for (int k = 0; k < 3; ++k) {
            FaceElement e = f.elements[k];
            if (e.v < 1 || (int)e.v > m.vertexCount) {
                allValidV = false;
            }
            if (e.vt < 1 || (int)e.vt > m.textureCount) {
                allValidVt = false;
            }
            if (e.vn < 1 || (int)e.vn > m.normalCount) {
                allValidVn = false;
            }
        }
        if (f.a.v == f.b.v || f.b.v == f.c.v || f.a.v == f.c.v) {
            allNonDegenerate = false;
        }
    }

    char label[128];
    snprintf(label, sizeof(label), "%s: all face vertex indices in [1, %d]", prefix, m.vertexCount);
    EXPECT_TRUE(label, allValidV);

    snprintf(label, sizeof(label), "%s: all face texture indices in [1, %d]", prefix, m.textureCount);
    EXPECT_TRUE(label, allValidVt);

    snprintf(label, sizeof(label), "%s: all face normal indices in [1, %d]", prefix, m.normalCount);
    EXPECT_TRUE(label, allValidVn);

    snprintf(label, sizeof(label), "%s: all triangles have 3 distinct vertices", prefix);
    EXPECT_TRUE(label, allNonDegenerate);
}

static void TestPrimitiveCube(void) {
    Model cube = MakePrimativeCube();

    EXPECT_INT("Cube: vertexCount == 8", cube.vertexCount, 8);
    EXPECT_INT("Cube: arrlen(vertices) == 8", arrlen(cube.vertices), 8);
    EXPECT_INT("Cube: normalCount == 6", cube.normalCount, 6);
    EXPECT_INT("Cube: arrlen(normals) == 6", arrlen(cube.normals), 6);
    EXPECT_INT("Cube: textureCount == 14", cube.textureCount, 14);
    EXPECT_INT("Cube: faceCount == 12 (triangulated)", cube.faceCount, 12);
    EXPECT_INT("Cube: arrlen(faces) == 12", arrlen(cube.faces), 12);

    bool cornersAtOne = (cube.vertexCount == 8);
    for (int i = 0; i < cube.vertexCount; ++i) {
        Vec3 v = cube.vertices[i];
        if (!FloatNear(fabsf(v.x), 1.0f) || !FloatNear(fabsf(v.y), 1.0f) || !FloatNear(fabsf(v.z), 1.0f)) {
            cornersAtOne = false;
        }
    }
    EXPECT_TRUE("Cube: all 8 vertices at (+/-1, +/-1, +/-1)", cornersAtOne);

    bool normalsUnit = (cube.normalCount == 6);
    for (int i = 0; i < cube.normalCount; ++i) {
        if (!FloatNear(V3Length(cube.normals[i]), 1.0f)) {
            normalsUnit = false;
        }
    }
    EXPECT_TRUE("Cube: all 6 normals are unit length", normalsUnit);

    if (cube.faceCount == 12) {
        ValidateFaceIndicesAndNonDegenerate("Cube", cube);
    }

    FreeModel(&cube);
}

static void TestPrimitiveSphere(void) {
    Model sphere = MakePrimativeSphere();

    EXPECT_INT("Sphere: vertexCount == 482", sphere.vertexCount, 482);
    EXPECT_INT("Sphere: arrlen(vertices) == 482", arrlen(sphere.vertices), 482);
    EXPECT_INT("Sphere: normalCount == 512", sphere.normalCount, 512);
    EXPECT_INT("Sphere: textureCount == 559", sphere.textureCount, 559);
    EXPECT_INT("Sphere: faceCount == 960 (triangulated)", sphere.faceCount, 960);
    EXPECT_INT("Sphere: arrlen(faces) == 960", arrlen(sphere.faces), 960);

    bool onUnitSphere = (sphere.vertexCount == 482);
    for (int i = 0; i < sphere.vertexCount; ++i) {
        float r = V3Length(sphere.vertices[i]);
        if (fabsf(r - 1.0f) > 1e-3f) {
            onUnitSphere = false;
        }
    }
    EXPECT_TRUE("Sphere: all vertices lie on unit sphere (|v| == 1)", onUnitSphere);

    bool normalsUnit = (sphere.normalCount == 512);
    for (int i = 0; i < sphere.normalCount; ++i) {
        if (fabsf(V3Length(sphere.normals[i]) - 1.0f) > 1e-3f) {
            normalsUnit = false;
        }
    }
    EXPECT_TRUE("Sphere: all normals are unit length", normalsUnit);

    if (sphere.faceCount == 960) {
        ValidateFaceIndicesAndNonDegenerate("Sphere", sphere);
    }

    FreeModel(&sphere);
}

int main(void) {
    SetTraceLogLevel(LOG_ERROR);
    PrintSuiteHeader("Running Primitives Unit Tests");

    TestPrimitiveCube();
    TestPrimitiveSphere();

    return TestSummary();
}
