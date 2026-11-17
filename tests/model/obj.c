#include "../test_common.h"
#include "../../model.h"
#include <raylib.h>

static bool FaceElementEq(FaceElement a, FaceElement b) {
    return a.v == b.v && a.vt == b.vt && a.vn == b.vn;
}

#define EXPECT_FACE_ELEM(name, actual, expected) do { \
    g_tests_run++; \
    FaceElement _a = (actual); \
    FaceElement _e = (expected); \
    if (!FaceElementEq(_a, _e)) { \
        g_tests_failed++; \
        printf(FAIL_TAG " %s (" ANSI_YELLOW "%s:%d" ANSI_RESET "): got " ANSI_RED "(v=%u, vt=%u, vn=%u)" ANSI_RESET \
               ", expected " ANSI_GREEN "(v=%u, vt=%u, vn=%u)" ANSI_RESET "\n", \
               name, __FILE__, __LINE__, _a.v, _a.vt, _a.vn, _e.v, _e.vt, _e.vn); \
    } else { \
        printf(PASS_TAG " %s\n", name); \
    } \
} while (0)

// Helper: writes a small inline OBJ string to a temporary file, loads it via
// LoadModelFromObjFile, and removes the temporary file immediately.
static Model LoadModelFromObjString(const char *objText) {
    const char *tmpPath = "/tmp/cg_unit_test_temp.obj";
    FILE *f = fopen(tmpPath, "wb");
    if (f) {
        fputs(objText, f);
        fclose(f);
    }
    Model m = LoadModelFromObjFile(tmpPath);
    remove(tmpPath);
    return m;
}

static void TestTriangleFaceUnionLayout(void) {
    TriangleFace face = {
        .a = {1, 2, 3},
        .b = {4, 5, 6},
        .c = {7, 8, 9},
    };

    EXPECT_FACE_ELEM("TriangleFace.elements[0] == a", face.elements[0], ((FaceElement){1, 2, 3}));
    EXPECT_FACE_ELEM("TriangleFace.elements[1] == b", face.elements[1], ((FaceElement){4, 5, 6}));
    EXPECT_FACE_ELEM("TriangleFace.elements[2] == c", face.elements[2], ((FaceElement){7, 8, 9}));
}

static void TestParseVertices(void) {
    const char *obj =
        "# Vertex parsing test\n"
        "v 1.0 2.0 3.0\n"
        "v -4.5 0.0 0.25\n"
        "v -0.75 10 -2\n";

    Model m = LoadModelFromObjString(obj);

    EXPECT_INT("Vertices count", arrlen(m.vertices), 3);
    if (arrlen(m.vertices) == 3) {
        EXPECT_VEC3("Vertex 0 standard floats", m.vertices[0], ((Vec3){1.0f, 2.0f, 3.0f}));
        EXPECT_VEC3("Vertex 1 negative & zero", m.vertices[1], ((Vec3){-4.5f, 0.0f, 0.25f}));
        EXPECT_VEC3("Vertex 2 decimals & integers", m.vertices[2], ((Vec3){-0.75f, 10.0f, -2.0f}));
    }
    EXPECT_INT("No normals loaded", arrlen(m.normals), 0);
    EXPECT_INT("No textures loaded", arrlen(m.textures), 0);
    EXPECT_INT("No faces loaded", arrlen(m.faces), 0);

    FreeModel(&m);
}

static void TestParseNormalsAndTextures(void) {
    const char *obj =
        "vn 0.0 1.0 0.0\n"
        "vn -0.57735 0.57735 -0.57735\n"
        "vt 0.25 0.75\n"
        "vt 1.0 0.0\n";

    Model m = LoadModelFromObjString(obj);

    EXPECT_INT("Normals count", arrlen(m.normals), 2);
    if (arrlen(m.normals) == 2) {
        EXPECT_VEC3("Normal 0", m.normals[0], ((Vec3){0.0f, 1.0f, 0.0f}));
        EXPECT_VEC3("Normal 1", m.normals[1], ((Vec3){-0.57735f, 0.57735f, -0.57735f}));
    }

    EXPECT_INT("Textures count", arrlen(m.textures), 2);
    if (arrlen(m.textures) == 2) {
        EXPECT_VEC2("Texture 0", m.textures[0], ((Vec2){0.25f, 0.75f}));
        EXPECT_VEC2("Texture 1", m.textures[1], ((Vec2){1.0f, 0.0f}));
    }

    FreeModel(&m);
}

static void TestFacesFullSpec(void) {
    // Spec: 'f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3'
    const char *obj =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "vt 0 0\n"
        "vt 1 1\n"
        "vn 0 0 1\n"
        "f 10/20/30 40/50/60 70/80/90\n";

    Model m = LoadModelFromObjString(obj);

    EXPECT_INT("Faces count (v/vt/vn)", arrlen(m.faces), 1);
    if (arrlen(m.faces) == 1) {
        EXPECT_FACE_ELEM("Face 0.a (10/20/30)", m.faces[0].a, ((FaceElement){10, 20, 30}));
        EXPECT_FACE_ELEM("Face 0.b (40/50/60)", m.faces[0].b, ((FaceElement){40, 50, 60}));
        EXPECT_FACE_ELEM("Face 0.c (70/80/90)", m.faces[0].c, ((FaceElement){70, 80, 90}));
    }

    FreeModel(&m);
}

static void TestFacesVertexNormalNoTextureSpec(void) {
    // Spec: 'f v1//vn1 v2//vn2 v3//vn3' -> vt must be 0, vn populated
    const char *obj =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "vn 0 0 1\n"
        "vn 0 1 0\n"
        "f 1//2 2//1 3//2\n";

    Model m = LoadModelFromObjString(obj);

    EXPECT_INT("Faces count (v//vn)", arrlen(m.faces), 1);
    if (arrlen(m.faces) == 1) {
        EXPECT_FACE_ELEM("Face 0.a (v=1, vt=0, vn=2)", m.faces[0].a, ((FaceElement){1, 0, 2}));
        EXPECT_FACE_ELEM("Face 0.b (v=2, vt=0, vn=1)", m.faces[0].b, ((FaceElement){2, 0, 1}));
        EXPECT_FACE_ELEM("Face 0.c (v=3, vt=0, vn=2)", m.faces[0].c, ((FaceElement){3, 0, 2}));
    }

    FreeModel(&m);
}

static void TestFacesVertexTextureSpec(void) {
    // Spec: 'f v1/vt1 v2/vt2 v3/vt3' -> vn must default to 0
    const char *obj =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "vt 0 0\n"
        "vt 1 0\n"
        "vt 0 1\n"
        "f 1/3 2/1 3/2\n";

    Model m = LoadModelFromObjString(obj);

    EXPECT_INT("Faces count (v/vt)", arrlen(m.faces), 1);
    if (arrlen(m.faces) == 1) {
        EXPECT_FACE_ELEM("Face 0.a (v=1, vt=3, vn=0)", m.faces[0].a, ((FaceElement){1, 3, 0}));
        EXPECT_FACE_ELEM("Face 0.b (v=2, vt=1, vn=0)", m.faces[0].b, ((FaceElement){2, 1, 0}));
        EXPECT_FACE_ELEM("Face 0.c (v=3, vt=2, vn=0)", m.faces[0].c, ((FaceElement){3, 2, 0}));
    }

    FreeModel(&m);
}

static void TestObjEdgeCasesFormattingAndComments(void) {
    // Edge cases: leading tabs/spaces, CRLF (\r\n), comments, unknown directives, no trailing newline at EOF
    const char *obj =
        "# Header comment\r\n"
        "\t  # Indented comment\r\n"
        "\r\n"
        "o TestObject\r\n"
        "mtllib dummy.mtl\r\n"
        "   v   1.5   -2.5   3.5  \r\n"
        "\tv\t4.0\t5.0\t6.0\r\n"
        "v 7.0 8.0 9.0\r\n"
        "s off\r\n"
        "usemtl Default\r\n"
        "  f   1/2/3   2/3/1   3/1/2"; // Notice: no trailing '\n' at EOF

    Model m = LoadModelFromObjString(obj);

    EXPECT_INT("Edge cases: vertex count", arrlen(m.vertices), 3);
    if (arrlen(m.vertices) == 3) {
        EXPECT_VEC3("Edge cases: whitespace-padded vertex", m.vertices[0], ((Vec3){1.5f, -2.5f, 3.5f}));
        EXPECT_VEC3("Edge cases: tab-separated vertex", m.vertices[1], ((Vec3){4.0f, 5.0f, 6.0f}));
        EXPECT_VEC3("Edge cases: third vertex", m.vertices[2], ((Vec3){7.0f, 8.0f, 9.0f}));
    }

    EXPECT_INT("Edge cases: face count without trailing newline", arrlen(m.faces), 1);
    if (arrlen(m.faces) == 1) {
        EXPECT_FACE_ELEM("Edge cases: face.a", m.faces[0].a, ((FaceElement){1, 2, 3}));
        EXPECT_FACE_ELEM("Edge cases: face.b", m.faces[0].b, ((FaceElement){2, 3, 1}));
        EXPECT_FACE_ELEM("Edge cases: face.c", m.faces[0].c, ((FaceElement){3, 1, 2}));
    }

    FreeModel(&m);
}

static void TestFacesVertexOnlySpec(void) {
    // Spec: 'f v1 v2 v3' -> vt and vn must default to 0
    const char *obj =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "v 0 0 1\n"
        "f 1 2 3\n"
        "f 2 4 3\n";

    Model m = LoadModelFromObjString(obj);

    EXPECT_INT("Faces count (v only)", arrlen(m.faces), 2);
    if (arrlen(m.faces) == 2) {
        EXPECT_FACE_ELEM("Face 0.a (1,0,0)", m.faces[0].a, ((FaceElement){1, 0, 0}));
        EXPECT_FACE_ELEM("Face 0.b (2,0,0)", m.faces[0].b, ((FaceElement){2, 0, 0}));
        EXPECT_FACE_ELEM("Face 0.c (3,0,0)", m.faces[0].c, ((FaceElement){3, 0, 0}));

        EXPECT_FACE_ELEM("Face 1.a (2,0,0)", m.faces[1].a, ((FaceElement){2, 0, 0}));
        EXPECT_FACE_ELEM("Face 1.b (4,0,0)", m.faces[1].b, ((FaceElement){4, 0, 0}));
        EXPECT_FACE_ELEM("Face 1.c (3,0,0)", m.faces[1].c, ((FaceElement){3, 0, 0}));
    }

    FreeModel(&m);
}

int main(void) {
    SetTraceLogLevel(LOG_ERROR); // Silence expected OBJ loader warnings during edge-case tests
    PrintSuiteHeader("Running Model & OBJ Loader Unit Tests");

    TestTriangleFaceUnionLayout();
    TestParseVertices();
    TestParseNormalsAndTextures();
    TestFacesFullSpec();
    TestFacesVertexNormalNoTextureSpec();
    TestFacesVertexTextureSpec();
    TestObjEdgeCasesFormattingAndComments();
    TestFacesVertexOnlySpec();

    return TestSummary();
}
