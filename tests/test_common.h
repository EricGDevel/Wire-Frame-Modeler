#if !defined(TEST_COMMON_H)
#define TEST_COMMON_H

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "../math.h"

#if !defined(TEST_TOLERANCE)
#define TEST_TOLERANCE 1e-4f
#endif

#define PI_F 3.14159265358979323846f

// ANSI color codes
#define ANSI_RESET   "\033[0m"
#define ANSI_BOLD    "\033[1m"
#define ANSI_RED     "\033[1;31m"
#define ANSI_GREEN   "\033[1;32m"
#define ANSI_YELLOW  "\033[1;33m"
#define ANSI_CYAN    "\033[1;36m"

#define PASS_TAG ANSI_GREEN "[PASS]" ANSI_RESET
#define FAIL_TAG ANSI_RED   "[FAIL]" ANSI_RESET

static int g_tests_run = 0;
static int g_tests_failed = 0;

static inline void PrintSuiteHeader(const char *title) {
    printf(ANSI_CYAN "=== %s ===" ANSI_RESET "\n", title);
}

static inline bool FloatNear(float a, float b) {
    return fabsf(a - b) <= TEST_TOLERANCE;
}

static inline bool Vec2Near(Vec2 a, Vec2 b) {
    return FloatNear(a.x, b.x) && FloatNear(a.y, b.y);
}

static inline bool Vec3Near(Vec3 a, Vec3 b) {
    return FloatNear(a.x, b.x) && FloatNear(a.y, b.y) && FloatNear(a.z, b.z);
}

static inline bool Vec4Near(Vec4 a, Vec4 b) {
    return FloatNear(a.x, b.x) && FloatNear(a.y, b.y) && FloatNear(a.z, b.z) && FloatNear(a.w, b.w);
}

static inline bool Mat2Near(Mat2 a, Mat2 b) {
    return Vec2Near(a.r0, b.r0) && Vec2Near(a.r1, b.r1);
}

static inline bool Mat3Near(Mat3 a, Mat3 b) {
    return Vec3Near(a.r0, b.r0) && Vec3Near(a.r1, b.r1) && Vec3Near(a.r2, b.r2);
}

static inline bool Mat4Near(Mat4 a, Mat4 b) {
    return Vec4Near(a.r0, b.r0) && Vec4Near(a.r1, b.r1) && Vec4Near(a.r2, b.r2) && Vec4Near(a.r3, b.r3);
}

#define EXPECT_TRUE(name, cond) do { \
    g_tests_run++; \
    if (!(cond)) { \
        g_tests_failed++; \
        printf(FAIL_TAG " %s (" ANSI_YELLOW "%s:%d" ANSI_RESET "): expected true\n", name, __FILE__, __LINE__); \
    } else { \
        printf(PASS_TAG " %s\n", name); \
    } \
} while (0)

#define EXPECT_FLOAT(name, actual, expected) do { \
    g_tests_run++; \
    float _a = (actual); \
    float _e = (expected); \
    if (!FloatNear(_a, _e)) { \
        g_tests_failed++; \
        printf(FAIL_TAG " %s (" ANSI_YELLOW "%s:%d" ANSI_RESET "): got " ANSI_RED "%.6f" ANSI_RESET ", expected " ANSI_GREEN "%.6f" ANSI_RESET "\n", \
               name, __FILE__, __LINE__, _a, _e); \
    } else { \
        printf(PASS_TAG " %s\n", name); \
    } \
} while (0)

#define EXPECT_VEC2(name, actual, expected) do { \
    g_tests_run++; \
    Vec2 _a = (actual); \
    Vec2 _e = (expected); \
    if (!Vec2Near(_a, _e)) { \
        g_tests_failed++; \
        printf(FAIL_TAG " %s (" ANSI_YELLOW "%s:%d" ANSI_RESET "): got " ANSI_RED "(%.6f, %.6f)" ANSI_RESET ", expected " ANSI_GREEN "(%.6f, %.6f)" ANSI_RESET "\n", \
               name, __FILE__, __LINE__, _a.x, _a.y, _e.x, _e.y); \
    } else { \
        printf(PASS_TAG " %s\n", name); \
    } \
} while (0)

#define EXPECT_VEC3(name, actual, expected) do { \
    g_tests_run++; \
    Vec3 _a = (actual); \
    Vec3 _e = (expected); \
    if (!Vec3Near(_a, _e)) { \
        g_tests_failed++; \
        printf(FAIL_TAG " %s (" ANSI_YELLOW "%s:%d" ANSI_RESET "): got " ANSI_RED "(%.6f, %.6f, %.6f)" ANSI_RESET ", expected " ANSI_GREEN "(%.6f, %.6f, %.6f)" ANSI_RESET "\n", \
               name, __FILE__, __LINE__, _a.x, _a.y, _a.z, _e.x, _e.y, _e.z); \
    } else { \
        printf(PASS_TAG " %s\n", name); \
    } \
} while (0)

#define EXPECT_VEC4(name, actual, expected) do { \
    g_tests_run++; \
    Vec4 _a = (actual); \
    Vec4 _e = (expected); \
    if (!Vec4Near(_a, _e)) { \
        g_tests_failed++; \
        printf(FAIL_TAG " %s (" ANSI_YELLOW "%s:%d" ANSI_RESET "): got " ANSI_RED "(%.6f, %.6f, %.6f, %.6f)" ANSI_RESET ", expected " ANSI_GREEN "(%.6f, %.6f, %.6f, %.6f)" ANSI_RESET "\n", \
               name, __FILE__, __LINE__, _a.x, _a.y, _a.z, _a.w, _e.x, _e.y, _e.z, _e.w); \
    } else { \
        printf(PASS_TAG " %s\n", name); \
    } \
} while (0)

#define EXPECT_MAT2(name, actual, expected) do { \
    g_tests_run++; \
    Mat2 _a = (actual); \
    Mat2 _e = (expected); \
    if (!Mat2Near(_a, _e)) { \
        g_tests_failed++; \
        printf(FAIL_TAG " %s (" ANSI_YELLOW "%s:%d" ANSI_RESET "):\n" \
               "  got rows:      " ANSI_RED   "r0=(%.4f,%.4f) r1=(%.4f,%.4f)" ANSI_RESET "\n" \
               "  expected rows: " ANSI_GREEN "r0=(%.4f,%.4f) r1=(%.4f,%.4f)" ANSI_RESET "\n", \
               name, __FILE__, __LINE__, \
               _a.r0.x, _a.r0.y, _a.r1.x, _a.r1.y, \
               _e.r0.x, _e.r0.y, _e.r1.x, _e.r1.y); \
    } else { \
        printf(PASS_TAG " %s\n", name); \
    } \
} while (0)

#define EXPECT_MAT3(name, actual, expected) do { \
    g_tests_run++; \
    Mat3 _a = (actual); \
    Mat3 _e = (expected); \
    if (!Mat3Near(_a, _e)) { \
        g_tests_failed++; \
        printf(FAIL_TAG " %s (" ANSI_YELLOW "%s:%d" ANSI_RESET "):\n" \
               "  got rows:      " ANSI_RED   "r0=(%.4f,%.4f,%.4f) r1=(%.4f,%.4f,%.4f) r2=(%.4f,%.4f,%.4f)" ANSI_RESET "\n" \
               "  expected rows: " ANSI_GREEN "r0=(%.4f,%.4f,%.4f) r1=(%.4f,%.4f,%.4f) r2=(%.4f,%.4f,%.4f)" ANSI_RESET "\n", \
               name, __FILE__, __LINE__, \
               _a.r0.x, _a.r0.y, _a.r0.z, _a.r1.x, _a.r1.y, _a.r1.z, _a.r2.x, _a.r2.y, _a.r2.z, \
               _e.r0.x, _e.r0.y, _e.r0.z, _e.r1.x, _e.r1.y, _e.r1.z, _e.r2.x, _e.r2.y, _e.r2.z); \
    } else { \
        printf(PASS_TAG " %s\n", name); \
    } \
} while (0)

#define EXPECT_MAT4(name, actual, expected) do { \
    g_tests_run++; \
    Mat4 _a = (actual); \
    Mat4 _e = (expected); \
    if (!Mat4Near(_a, _e)) { \
        g_tests_failed++; \
        printf(FAIL_TAG " %s (" ANSI_YELLOW "%s:%d" ANSI_RESET "):\n" \
               "  got rows:      " ANSI_RED   "r0=(%.4f,%.4f,%.4f,%.4f) r1=(%.4f,%.4f,%.4f,%.4f) r2=(%.4f,%.4f,%.4f,%.4f) r3=(%.4f,%.4f,%.4f,%.4f)" ANSI_RESET "\n" \
               "  expected rows: " ANSI_GREEN "r0=(%.4f,%.4f,%.4f,%.4f) r1=(%.4f,%.4f,%.4f,%.4f) r2=(%.4f,%.4f,%.4f,%.4f) r3=(%.4f,%.4f,%.4f,%.4f)" ANSI_RESET "\n", \
               name, __FILE__, __LINE__, \
               _a.r0.x, _a.r0.y, _a.r0.z, _a.r0.w, _a.r1.x, _a.r1.y, _a.r1.z, _a.r1.w, \
               _a.r2.x, _a.r2.y, _a.r2.z, _a.r2.w, _a.r3.x, _a.r3.y, _a.r3.z, _a.r3.w, \
               _e.r0.x, _e.r0.y, _e.r0.z, _e.r0.w, _e.r1.x, _e.r1.y, _e.r1.z, _e.r1.w, \
               _e.r2.x, _e.r2.y, _e.r2.z, _e.r2.w, _e.r3.x, _e.r3.y, _e.r3.z, _e.r3.w); \
    } else { \
        printf(PASS_TAG " %s\n", name); \
    } \
} while (0)

static inline int TestSummary(void) {
    int passed = g_tests_run - g_tests_failed;
    if (g_tests_failed == 0) {
        printf("\n" ANSI_GREEN "Summary: %d/%d tests passed." ANSI_RESET "\n", passed, g_tests_run);
        return 0;
    } else {
        printf("\n" ANSI_RED "Summary: %d/%d tests passed (%d failed)." ANSI_RESET "\n", passed, g_tests_run, g_tests_failed);
        return 1;
    }
}

#endif // TEST_COMMON_H
