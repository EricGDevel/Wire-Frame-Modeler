#include "model.h"
#include "raylib.h"

#include <ctype.h> // For character checking functions
#include "stb_ds.h" // For dynamic arrays


//
// Provided helper functions
//

static inline bool iswhitespace(char c) { return (isspace(c) && c != '\n'); }

static int ParseNextInt(const char *str, int *start, int len) {
    int sign = 1;
    int result = 0;

    int i = *start;
    for (; i < len; ++i) {
        if (str[i] == '-') {
            sign = -1;
            break;
        } else if (str[i] == '+') {
            ++i;
            sign = 1;
            break;
        } else if (str[i] >= '0' && str[i] <= '9') {
            break;
        } else if (iswhitespace(str[i])) {
            continue;
        }

        // Empty number, either reached newline, EOF or non-number character
        return 0;
    }

    for (; i < len; ++i) {
        if (str[i] < '0' || str[i] > '9') {
            break;
        }

        result *= 10;
        result += (int)(str[i] - '0');
    }

    result *= sign;
    *start = i;

    return result;
}

static float ParseNextFloat(const char *str, int *start, int len) {
    bool parse_before_dot = true;
    int sign = 1;

    int i = *start;
    for (; i < len; ++i) {
        if (str[i] == '.') {
            ++i;
            parse_before_dot = false;
            break;
        } else if (str[i] == '-') {
            ++i;
            sign = -1;
            break;
        } else if (str[i] == '+') {
            ++i;
            sign = 1;
            break;
        } else if (str[i] >= '0' && str[i] <= '9') {
            break;
        } else if (iswhitespace(str[i])) {
            continue;
        }

        // Empty number, either reached newline, EOF or non-number character
        return 0.f;
    }

    int before_dot = 0;
    int after_dot = 0;
    int fraction_digits = 0;

    if (parse_before_dot) {
        for (; i < len; ++i) {
            if (str[i] < '0' || str[i] > '9') {
                break;
            }

            before_dot *= 10;
            before_dot += (int)(str[i] - '0');
        }
    }
    if (str[i] == '.') {
        ++i;
        for (; i < len; ++i) {
            if (str[i] < '0' || str[i] > '9') {
                break;
            }

            after_dot *= 10;
            after_dot += (int)(str[i] - '0');
            fraction_digits++;
        }
    }

    float result = (float)before_dot + (float)after_dot / pow(10, fraction_digits);

    result *= (float)sign;
    *start = i;

    return result;
}

static Vec2 ParseNextVec2(const char *str, int *start, int len) {
    Vec2 v;
    int i = *start;
    v.x = ParseNextFloat(str, &i, len);
    v.y = ParseNextFloat(str, &i, len);
    *start = i;
    return v;
}

static Vec3 ParseNextVec3(const char *str, int *start, int len) {
    Vec3 v;
    int i = *start;
    v.x = ParseNextFloat(str, &i, len);
    v.y = ParseNextFloat(str, &i, len);
    v.z = ParseNextFloat(str, &i, len);
    *start = i;
    return v;
}
// Helper functions end


static FaceElement ParseNextFaceElement(const char *str, int *start, int len) {
    FaceElement e = {.v = 0, .vt = 0, .vn = 0};
    int i = *start;

    e.v = ParseNextInt(str, &i, len);
    if (str[i] != '/') {
        // End of element
        goto cleanup;
    }
    // assert(str[i] == '/');
    i++;

    // If vt is missing (x//z), then it will read a empty int
    e.vt = ParseNextInt(str, &i, len);
    if (str[i] != '/') {
        // End of element
        goto cleanup;
    }
    i++;

    e.vn = ParseNextInt(str, &i, len);

cleanup:
    *start = i;
    return e;
}

// Currently assume that all faces are triangles (have exactly 3 verticies)
// TODO: Create generic face struct that contains dynamic array of FaceElements
// and update parsing logic
static TriangleFace ParseFace(const char *str, int *start, int len) {
    TriangleFace f;
    int i = *start;

    for (int elem = 0; elem < 3; elem++) {
        f.elements[elem] = ParseNextFaceElement(str, &i, len);
    }

    *start = i;
    return f;
}


static void updateBoundsVectors(Vec3 *v, Vec3 *min, Vec3 *max) {
    for (int i = 0; i < 3; i++) {
        if (v->values[i] > max->values[i]) {
            max->values[i] = v->values[i];
        }
        if (v->values[i] < min->values[i]) {
            min->values[i] = v->values[i];
        }
    }
}


static Model LoadModel(const char *data, int dataSize) {
    Model model = {};

    Vec3 minBound = (Vec3){INFINITY, INFINITY, INFINITY}, maxBound = (Vec3){-INFINITY, -INFINITY, -INFINITY};
    int lineIndex = 0;
    for (int i = 0; i < dataSize; ++i) {
        // Skip whitespace
        while (iswhitespace(data[i]) && i < dataSize) {
            ++i;
        }

        // Parsed using the following specification:
        // https://www.martinreddy.net/gfx/3d/OBJ.spec
        switch (data[i]) {
            case '#': {
                // OBJ comment, ignore line
            } break;

            case 'v': {
                ++i;
                if (iswhitespace(data[i])) {
                    // Geometric Vertex
                    Vec3 v = ParseNextVec3(data, &i, dataSize);
                    updateBoundsVectors(&v, &minBound, &maxBound);
                    arrput(model.vertices, v);
                } else if (data[i] == 'n') {
                    // Vertex Normal
                    ++i;
                    Vec3 normal = ParseNextVec3(data, &i, dataSize);
                    arrput(model.normals, normal);
                } else if (data[i] == 't') {
                    // Vertex tetures
                    ++i;
                    Vec2 texture = ParseNextVec2(data, &i, dataSize);
                    arrput(model.textures, texture);
                } else {
                    TraceLog(LOG_WARNING,
                             "%d: OBJ Loader: Unhandled character '%c' when parsing a vertex, skipping the rest of "
                             "the line",
                             lineIndex, data[i]);
                }
                break;
            }

            case 'f': {
                ++i;
                TriangleFace f = ParseFace(data, &i, dataSize);
                arrput(model.faces, f);
                break;
            }

            default: {
                TraceLog(LOG_WARNING,
                         "%d: OBJ Loader: Unhandled character '%c' when parsing a line, skipping the rest of the line",
                         lineIndex, data[i]);
            } break;
        }

        // Ignore the rest of the line
        while ((data[i] != '\n') && (i < dataSize)) {
            ++i;
        }

        lineIndex += 1;
    }

    model.position = (Vec3){0, 0, 0};
    model.rotation = (Vec3){0, 0, 0};
    model.scale = (Vec3){1, 1, 1};
    model.minBound = minBound;
    model.maxBound = maxBound;

    model.vertexCount = arrlen(model.vertices);
    model.textureCount = arrlen(model.textures);
    model.normalCount = arrlen(model.normals);
    model.faceCount = arrlen(model.faces);

    model.hidden = false;
    model.showBoundingBox = false;
    model.showVertexNormals = false;
    model.showFaceNormals = false;

    return model;
}

Model LoadModelFromStr(const char *str) { return LoadModel(str, strlen(str)); }

Model LoadModelFromObjFile(const char *filename) {
    int dataSize = -1;
    const char *data = (char *)LoadFileData(filename, &dataSize);

    Model model = LoadModel(data, dataSize);

    UnloadFileData((unsigned char *)data);
    return model;
}

void FreeModel(Model *model) {
    arrfree(model->vertices);
    arrfree(model->textures);
    arrfree(model->normals);
    arrfree(model->faces);
}
