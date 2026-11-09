#define TEST_TOLERANCE 1e-5f
#include "../test_common.h"

static void TestVec3UnionLayout(void) {
    Vec3 v = { 1.5f, -2.5f, 3.5f };

    EXPECT_FLOAT("Vec3.x/y/z", v.x + v.y + v.z, 2.5f);
    EXPECT_FLOAT("Vec3.r/g/b", v.r, 1.5f);
    EXPECT_FLOAT("Vec3.u/v/w", v.w, 3.5f);
    EXPECT_FLOAT("Vec3.values[1]", v.values[1], -2.5f);
    EXPECT_VEC2("Vec3.xy swizzle", v.xy, ((Vec2){ 1.5f, -2.5f }));
    EXPECT_VEC2("Vec3.yz swizzle", v.yz, ((Vec2){ -2.5f, 3.5f }));
}

static void TestVec3BasicArithmetic(void) {
    Vec3 a = { 1.0f, -2.0f, 3.0f };
    Vec3 b = { 4.0f,  5.0f, -6.0f };

    EXPECT_VEC3("V3Neg", V3Neg(a), ((Vec3){ -1.0f, 2.0f, -3.0f }));
    EXPECT_VEC3("V3Add", V3Add(a, b), ((Vec3){ 5.0f, 3.0f, -3.0f }));
    EXPECT_VEC3("V3Sub", V3Sub(a, b), ((Vec3){ -3.0f, -7.0f, 9.0f }));
    EXPECT_VEC3("V3MulS", V3MulS(a, 2.5f), ((Vec3){ 2.5f, -5.0f, 7.5f }));
    EXPECT_VEC3("V3DivS", V3DivS((Vec3){ 2.0f, -4.0f, 8.0f }, 2.0f), ((Vec3){ 1.0f, -2.0f, 4.0f }));
    EXPECT_VEC3("V3Mul", V3Mul(a, b), ((Vec3){ 4.0f, -10.0f, -18.0f }));
    EXPECT_VEC3("V3Div", V3Div((Vec3){ 8.0f, -9.0f, 10.0f }, (Vec3){ 2.0f, 3.0f, -5.0f }), ((Vec3){ 4.0f, -3.0f, -2.0f }));
}

static void TestVec3EqAndLerp(void) {
    Vec3 a = { 1.0f, 2.0f, 3.0f };
    Vec3 b = { 1.0f, 2.0f, 3.0f };
    Vec3 c = { 1.0f, 2.0f, -3.0f };

    EXPECT_TRUE("V3Eq equal", V3Eq(a, b));
    EXPECT_TRUE("V3Eq diff z", !V3Eq(a, c));
    EXPECT_VEC3("V3Lerp t=0", V3Lerp(a, 0.0f, c), a);
    EXPECT_VEC3("V3Lerp t=1", V3Lerp(a, 1.0f, c), c);
    EXPECT_VEC3("V3Lerp t=0.5", V3Lerp((Vec3){ 0.0f, 2.0f, 4.0f }, 0.5f, (Vec3){ 10.0f, -2.0f, 8.0f }),
                ((Vec3){ 5.0f, 0.0f, 6.0f }));
}

static void TestVec3GeometricOps(void) {
    Vec3 a = { 1.0f, 2.0f, 3.0f };
    Vec3 b = { 4.0f, -5.0f, 6.0f };

    EXPECT_FLOAT("V3Dot general", V3Dot(a, b), 12.0f);
    EXPECT_FLOAT("V3Dot orthogonal", V3Dot((Vec3){ 1.0f, 0.0f, 0.0f }, (Vec3){ 0.0f, 1.0f, 0.0f }), 0.0f);

    Vec3 v = { 2.0f, 3.0f, 6.0f };
    EXPECT_FLOAT("V3LengthSquared", V3LengthSquared(v), 49.0f);
    EXPECT_FLOAT("V3Length", V3Length(v), 7.0f);

    EXPECT_VEC3("V3Normalize (2,3,6)", V3Normalize(v), ((Vec3){ 2.0f / 7.0f, 3.0f / 7.0f, 6.0f / 7.0f }));
    EXPECT_VEC3("V3Normalize zero vector", V3Normalize((Vec3){ 0.0f, 0.0f, 0.0f }), ((Vec3){ 0.0f, 0.0f, 0.0f }));

    Vec3 ex = { 1.0f, 0.0f, 0.0f };
    Vec3 ey = { 0.0f, 1.0f, 0.0f };
    Vec3 ez = { 0.0f, 0.0f, 1.0f };
    EXPECT_VEC3("V3Cross X x Y = Z", V3Cross(ex, ey), ez);
    EXPECT_VEC3("V3Cross Y x X = -Z", V3Cross(ey, ex), V3Neg(ez));
    EXPECT_VEC3("V3Cross general", V3Cross(a, b), ((Vec3){ 27.0f, 6.0f, -13.0f }));
}

int main(void) {
    PrintSuiteHeader("Running Vec3 Unit Tests");
    TestVec3UnionLayout();
    TestVec3BasicArithmetic();
    TestVec3EqAndLerp();
    TestVec3GeometricOps();
    return TestSummary();
}
