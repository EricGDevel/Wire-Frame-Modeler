#include "../test_common.h"

static void TestMat4UnionLayout(void) {
    Mat4 m = {
        .r0 = {  1.0f,  2.0f,  3.0f,  4.0f },
        .r1 = {  5.0f,  6.0f,  7.0f,  8.0f },
        .r2 = {  9.0f, 10.0f, 11.0f, 12.0f },
        .r3 = { 13.0f, 14.0f, 15.0f, 16.0f },
    };

    EXPECT_FLOAT("Mat4.s00/s01/s02/s03 maps to r0", m.s00 + m.s01 + m.s02 + m.s03, 10.0f);
    EXPECT_FLOAT("Mat4.s10/s11/s12/s13 maps to r1", m.s10 + m.s11 + m.s12 + m.s13, 26.0f);
    EXPECT_FLOAT("Mat4.s20/s21/s22/s23 maps to r2", m.s20 + m.s21 + m.s22 + m.s23, 42.0f);
    EXPECT_FLOAT("Mat4.s30/s31/s32/s33 maps to r3", m.s30 + m.s31 + m.s32 + m.s33, 58.0f);
    EXPECT_VEC4("Mat4.rows[1] == r1", m.rows[1], ((Vec4){ 5.0f, 6.0f, 7.0f, 8.0f }));
    EXPECT_VEC4("Mat4.rows[3] == r3", m.rows[3], ((Vec4){ 13.0f, 14.0f, 15.0f, 16.0f }));
}

static void TestM4MulV4(void) {
    Mat4 zero = M4DiagS(0.0f);
    Mat4 identity = M4Identity();
    Mat4 a = {
        .r0 = {  1.0f,  2.0f,  3.0f,  4.0f },
        .r1 = {  5.0f,  6.0f,  7.0f,  8.0f },
        .r2 = { -1.0f,  0.0f,  2.0f, -3.0f },
        .r3 = {  2.0f, -1.0f,  0.0f,  1.0f },
    };
    Vec4 v = { 2.0f, -1.0f, 3.0f, 1.0f };
    Vec4 zeroV = { 0.0f, 0.0f, 0.0f, 0.0f };

    // General & edge cases
    EXPECT_VEC4("M4MulV4 general", M4MulV4(a, v), ((Vec4){ 13.0f, 33.0f, 1.0f, 6.0f }));
    EXPECT_VEC4("M4MulV4 with Identity (I * v == v)", M4MulV4(identity, v), v);
    EXPECT_VEC4("M4MulV4 with Zero matrix (0 * v == 0)", M4MulV4(zero, v), zeroV);
    EXPECT_VEC4("M4MulV4 with Zero vector (A * 0 == 0)", M4MulV4(a, zeroV), zeroV);

    // Standard basis vectors extract columns of A
    Vec4 e0 = { 1.0f, 0.0f, 0.0f, 0.0f };
    Vec4 e1 = { 0.0f, 1.0f, 0.0f, 0.0f };
    Vec4 e2 = { 0.0f, 0.0f, 1.0f, 0.0f };
    Vec4 e3 = { 0.0f, 0.0f, 0.0f, 1.0f };
    EXPECT_VEC4("M4MulV4 extracts col 0", M4MulV4(a, e0), ((Vec4){  1.0f, 5.0f, -1.0f,  2.0f }));
    EXPECT_VEC4("M4MulV4 extracts col 1", M4MulV4(a, e1), ((Vec4){  2.0f, 6.0f,  0.0f, -1.0f }));
    EXPECT_VEC4("M4MulV4 extracts col 2", M4MulV4(a, e2), ((Vec4){  3.0f, 7.0f,  2.0f,  0.0f }));
    EXPECT_VEC4("M4MulV4 extracts col 3", M4MulV4(a, e3), ((Vec4){  4.0f, 8.0f, -3.0f,  1.0f }));

    // Linearity: A * (u + v) == A * u + A * v
    Vec4 u = { -3.0f, 4.0f, 1.0f, 2.0f };
    EXPECT_VEC4("M4MulV4 linearity A*(u+v) == A*u + A*v",
                M4MulV4(a, V4Add(u, v)),
                V4Add(M4MulV4(a, u), M4MulV4(a, v)));
}

static void TestM4MulM4(void) {
    Mat4 zero = M4DiagS(0.0f);
    Mat4 identity = M4Identity();
    Mat4 a = {
        .r0 = { 1.0f, 2.0f, 0.0f, 1.0f },
        .r1 = { 0.0f, 1.0f, 1.0f, 0.0f },
        .r2 = { 2.0f, 0.0f, 1.0f, 3.0f },
        .r3 = { 1.0f, 1.0f, 0.0f, 2.0f },
    };
    Mat4 b = {
        .r0 = {  2.0f, 1.0f, 0.0f, -1.0f },
        .r1 = {  1.0f, 0.0f, 2.0f,  1.0f },
        .r2 = {  0.0f, 3.0f, 1.0f,  2.0f },
        .r3 = { -1.0f, 1.0f, 0.0f,  1.0f },
    };

    Mat4 expectedAB = {
        .r0 = { 3.0f, 2.0f, 4.0f, 2.0f },
        .r1 = { 1.0f, 3.0f, 3.0f, 3.0f },
        .r2 = { 1.0f, 8.0f, 1.0f, 3.0f },
        .r3 = { 1.0f, 3.0f, 2.0f, 2.0f },
    };
    Mat4 expectedBA = {
        .r0 = { 1.0f, 4.0f, 1.0f, 0.0f },
        .r1 = { 6.0f, 3.0f, 2.0f, 9.0f },
        .r2 = { 4.0f, 5.0f, 4.0f, 7.0f },
        .r3 = { 0.0f, 0.0f, 1.0f, 1.0f },
    };

    // M4MulM4 general & properties
    EXPECT_MAT4("M4MulM4 general (A * B)", M4MulM4(a, b), expectedAB);
    EXPECT_MAT4("M4MulM4 non-commutative (B * A)", M4MulM4(b, a), expectedBA);
    EXPECT_TRUE("M4MulM4 A*B != B*A", !Mat4Near(M4MulM4(a, b), M4MulM4(b, a)));
    EXPECT_MAT4("M4MulM4 right identity (A * I == A)", M4MulM4(a, identity), a);
    EXPECT_MAT4("M4MulM4 left identity (I * A == A)", M4MulM4(identity, a), a);
    EXPECT_MAT4("M4MulM4 right zero (A * 0 == 0)", M4MulM4(a, zero), zero);
    EXPECT_MAT4("M4MulM4 left zero (0 * A == 0)", M4MulM4(zero, a), zero);

    // Diagonal matrices
    Mat4 diag1 = M4Diag(2.0f, -3.0f, 4.0f, 5.0f);
    Mat4 diag2 = M4Diag(5.0f,  2.0f, -0.5f, 0.2f);
    EXPECT_MAT4("M4MulM4 diagonal matrices", M4MulM4(diag1, diag2), M4Diag(10.0f, -6.0f, -2.0f, 1.0f));

    // Associativity: (A * B) * C == A * (B * C)
    Mat4 c = {
        .r0 = {  0.0f,  1.0f, -2.0f, 1.0f },
        .r1 = {  2.0f, -1.0f,  0.0f, 3.0f },
        .r2 = {  1.0f,  0.0f,  1.0f, 2.0f },
        .r3 = { -1.0f,  2.0f,  0.0f, 1.0f },
    };
    EXPECT_MAT4("M4MulM4 associativity (A * B) * C == A * (B * C)",
                M4MulM4(M4MulM4(a, b), c),
                M4MulM4(a, M4MulM4(b, c)));

    // Compatibility with M4MulV4: (A * B) * v == A * (B * v)
    Vec4 v = { 1.0f, -2.0f, 3.0f, 4.0f };
    EXPECT_VEC4("M4MulM4 compatibility with M4MulV4 ((A * B) * v == A * (B * v))",
                M4MulV4(M4MulM4(a, b), v),
                M4MulV4(a, M4MulV4(b, v)));
}

static void TestM4Transpose(void) {
    Mat4 identity = M4Identity();
    Mat4 a = {
        .r0 = {  1.0f,  2.0f,  3.0f,  4.0f },
        .r1 = {  5.0f,  6.0f,  7.0f,  8.0f },
        .r2 = {  9.0f, 10.0f, 11.0f, 12.0f },
        .r3 = { 13.0f, 14.0f, 15.0f, 16.0f },
    };
    Mat4 aT = {
        .r0 = { 1.0f, 5.0f,  9.0f, 13.0f },
        .r1 = { 2.0f, 6.0f, 10.0f, 14.0f },
        .r2 = { 3.0f, 7.0f, 11.0f, 15.0f },
        .r3 = { 4.0f, 8.0f, 12.0f, 16.0f },
    };

    // General & involution
    EXPECT_MAT4("M4Transpose", M4Transpose(a), aT);
    EXPECT_MAT4("M4Transpose(M4Transpose(A)) == A", M4Transpose(M4Transpose(a)), a);
    EXPECT_MAT4("M4Transpose(Identity) == Identity", M4Transpose(identity), identity);

    // Diagonal matrix
    Mat4 diag = M4Diag(1.0f, -2.0f, 3.0f, 4.0f);
    EXPECT_MAT4("M4Transpose(Diag) == Diag", M4Transpose(diag), diag);

    // Symmetric matrix: S^T == S
    Mat4 sym = {
        .r0 = {  2.0f,  1.0f, -1.0f,  3.0f },
        .r1 = {  1.0f,  5.0f,  0.0f, -2.0f },
        .r2 = { -1.0f,  0.0f,  4.0f,  6.0f },
        .r3 = {  3.0f, -2.0f,  6.0f,  7.0f },
    };
    EXPECT_MAT4("M4Transpose(Symmetric) == Symmetric", M4Transpose(sym), sym);

    // Product transpose: (A * B)^T == B^T * A^T
    Mat4 b = {
        .r0 = {  2.0f, 0.0f, -1.0f,  1.0f },
        .r1 = {  1.0f, 3.0f,  2.0f,  0.0f },
        .r2 = { -2.0f, 1.0f,  0.0f,  4.0f },
        .r3 = {  0.0f, 2.0f, -3.0f,  1.0f },
    };
    EXPECT_MAT4("M4Transpose(A * B) == B^T * A^T",
                M4Transpose(M4MulM4(a, b)),
                M4MulM4(M4Transpose(b), M4Transpose(a)));
}

static void TestM4Det(void) {
    Mat4 zero = M4DiagS(0.0f);
    Mat4 identity = M4Identity();

    // Basic & diagonal matrices
    EXPECT_FLOAT("M4Det(Identity) == 1", M4Det(identity), 1.0f);
    EXPECT_FLOAT("M4Det(Diag(2, 3, -4, 5)) == -120", M4Det(M4Diag(2.0f, 3.0f, -4.0f, 5.0f)), -120.0f);

    // Singular matrix edge cases (det == 0)
    EXPECT_FLOAT("M4Det(Zero matrix) == 0 (singular)", M4Det(zero), 0.0f);

    Mat4 singularZeroRow = {
        .r0 = { 1.0f, 2.0f, 3.0f, 4.0f },
        .r1 = { 0.0f, 0.0f, 0.0f, 0.0f },
        .r2 = { 5.0f, 6.0f, 7.0f, 8.0f },
        .r3 = { 9.0f, 1.0f, 2.0f, 3.0f },
    };
    EXPECT_FLOAT("M4Det(zero row) == 0 (singular)", M4Det(singularZeroRow), 0.0f);

    Mat4 singularProportionalRows = {
        .r0 = { 1.0f, -2.0f, 3.0f,  4.0f },
        .r1 = { 2.0f, -4.0f, 6.0f,  8.0f }, // r1 = 2 * r0
        .r2 = { 5.0f,  1.0f, 0.0f, -1.0f },
        .r3 = { 3.0f,  2.0f, 1.0f,  7.0f },
    };
    EXPECT_FLOAT("M4Det(proportional rows r1 = 2*r0) == 0 (singular)", M4Det(singularProportionalRows), 0.0f);

    Mat4 singularLinearComb = {
        .r0 = { 1.0f, 2.0f, 3.0f,  4.0f },
        .r1 = { 5.0f, 6.0f, 7.0f,  8.0f },
        .r2 = { 2.0f, 0.0f, 1.0f, -1.0f },
        .r3 = { 8.0f, 8.0f, 11.0f, 11.0f }, // r3 = r0 + r1 + r2
    };
    EXPECT_FLOAT("M4Det(linearly dependent rows r3 = r0+r1+r2) == 0 (singular)", M4Det(singularLinearComb), 0.0f);

    // Triangular matrices (det == product of diagonal entries)
    Mat4 upperTriangular = {
        .r0 = { 2.0f,  3.0f, -1.0f, 5.0f },
        .r1 = { 0.0f,  4.0f,  2.0f, 1.0f },
        .r2 = { 0.0f,  0.0f, -2.0f, 7.0f },
        .r3 = { 0.0f,  0.0f,  0.0f, 3.0f },
    };
    EXPECT_FLOAT("M4Det(upper triangular) == product of diagonal", M4Det(upperTriangular), -48.0f);

    Mat4 lowerTriangular = {
        .r0 = {  3.0f, 0.0f, 0.0f, 0.0f },
        .r1 = {  2.0f, -1.0f, 0.0f, 0.0f },
        .r2 = {  4.0f,  5.0f, 2.0f, 0.0f },
        .r3 = { -1.0f,  3.0f, 7.0f, 4.0f },
    };
    EXPECT_FLOAT("M4Det(lower triangular) == product of diagonal", M4Det(lowerTriangular), -24.0f);

    // Invertible general matrix
    Mat4 m = {
        .r0 = { 1.0f, 0.0f, 2.0f, -1.0f },
        .r1 = { 3.0f, 0.0f, 0.0f,  5.0f },
        .r2 = { 2.0f, 1.0f, 4.0f, -3.0f },
        .r3 = { 1.0f, 0.0f, 5.0f,  0.0f },
    };
    EXPECT_FLOAT("M4Det(invertible general)", M4Det(m), 30.0f);

    // Det of transpose: det(A^T) == det(A)
    EXPECT_FLOAT("M4Det(A^T) == M4Det(A)", M4Det(M4Transpose(m)), 30.0f);

    // Multiplicative property: det(A * B) == det(A) * det(B)
    EXPECT_FLOAT("M4Det(A * B) == M4Det(A) * M4Det(B)",
                 M4Det(M4MulM4(upperTriangular, lowerTriangular)),
                 M4Det(upperTriangular) * M4Det(lowerTriangular));
}

static void TestM4RotateV(void) {
    Vec4 xVec4 = { 1.0f, 0.0f, 0.0f, 1.0f };
    Vec4 yVec4 = { 0.0f, 1.0f, 0.0f, 1.0f };
    Vec4 zVec4 = { 0.0f, 0.0f, 1.0f, 1.0f };

    // 90 deg rotation from +X to +Y
    Vec3 src = { 3.0f, 0.0f, 0.0f };
    Vec3 dst = { 0.0f, 5.0f, 0.0f };
    Mat4 rotXY = M4RotateV(src, dst);

    EXPECT_VEC4("M4RotateV (3,0,0) -> (0,5,0) applied to X", M4MulV4(rotXY, xVec4), yVec4);
    EXPECT_VEC4("M4RotateV (3,0,0) -> (0,5,0) applied to Y", M4MulV4(rotXY, yVec4), ((Vec4){ -1.0f, 0.0f, 0.0f, 1.0f }));
    EXPECT_VEC4("M4RotateV (3,0,0) -> (0,5,0) preserves rotation axis Z", M4MulV4(rotXY, zVec4), zVec4);
    EXPECT_FLOAT("M4Det(M4RotateV) == 1", M4Det(rotXY), 1.0f);

    // Rotation preserves length and w component
    EXPECT_FLOAT("M4RotateV preserves w=1", (M4MulV4(rotXY, xVec4)).w, 1.0f);

    // Arbitrary unit vectors
    Vec3 arbitrarySrc = V3Normalize((Vec3){  1.0f, 2.0f, -3.0f });
    Vec3 arbitraryDst = V3Normalize((Vec3){ -2.0f, 4.0f,  1.0f });
    Mat4 rotArb = M4RotateV(arbitrarySrc, arbitraryDst);

    EXPECT_VEC3("M4RotateV arbitrary unit vectors via M4MulV3", M4MulV3(rotArb, arbitrarySrc), arbitraryDst);
    EXPECT_FLOAT("M4Det(M4RotateV arbitrary) == 1", M4Det(rotArb), 1.0f);

    // Orthogonality: R^T * R == I
    EXPECT_MAT4("M4RotateV is orthogonal (R^T * R == I)",
                M4MulM4(M4Transpose(rotArb), rotArb),
                M4Identity());
}

static void TestM4Translate(void) {
    Vec3 v = { 3.0f, -4.0f, 5.0f };
    Mat4 t = M4Translate(v);

    Mat4 expectedT = {
        .r0 = { 1.0f, 0.0f, 0.0f,  3.0f },
        .r1 = { 0.0f, 1.0f, 0.0f, -4.0f },
        .r2 = { 0.0f, 0.0f, 1.0f,  5.0f },
        .r3 = { 0.0f, 0.0f, 0.0f,  1.0f },
    };
    EXPECT_MAT4("M4Translate explicit entries", t, expectedT);

    // Zero translation
    EXPECT_MAT4("M4Translate(0) == Identity", M4Translate((Vec3){ 0.0f, 0.0f, 0.0f }), M4Identity());

    // Translation of a point (w = 1)
    Vec4 p = { 1.0f, 2.0f, 3.0f, 1.0f };
    EXPECT_VEC4("M4Translate point (w=1)", M4MulV4(t, p), ((Vec4){ 4.0f, -2.0f, 8.0f, 1.0f }));

    // Translation of a direction vector (w = 0) is invariant
    Vec4 d = { 1.0f, 2.0f, 3.0f, 0.0f };
    EXPECT_VEC4("M4Translate vector (w=0) unaffected", M4MulV4(t, d), d);

    // Composition of translations: T(v1) * T(v2) == T(v1 + v2)
    Vec3 v2 = { -1.0f, 6.0f, -2.0f };
    EXPECT_MAT4("M4Translate composition T(v1) * T(v2) == T(v1 + v2)",
                M4MulM4(t, M4Translate(v2)),
                M4Translate(V3Add(v, v2)));

    // Inverse translation: T(v) * T(-v) == Identity
    EXPECT_MAT4("M4Translate inverse T(v) * T(-v) == I",
                M4MulM4(t, M4Translate(V3Neg(v))),
                M4Identity());

    // Determinant of translation matrix
    EXPECT_FLOAT("M4Det(M4Translate) == 1", M4Det(t), 1.0f);
}

static void TestM4Orthographic(void) {
    float left = -2.0f, right = 2.0f;
    float bottom = -1.0f, top = 3.0f;
    float zNear = 2.0f, zFar = 10.0f;

    Mat4 ortho = M4Orthographic(left, right, bottom, top, zNear, zFar);

    Mat4 expectedOrtho = {
        .r0 = { 0.5f, 0.0f,  0.0f,   0.0f },
        .r1 = { 0.0f, 0.5f,  0.0f,  -0.5f },
        .r2 = { 0.0f, 0.0f, -0.25f, -1.5f },
        .r3 = { 0.0f, 0.0f,  0.0f,   1.0f },
    };
    EXPECT_MAT4("M4Orthographic explicit entries", ortho, expectedOrtho);

    // Near plane center: (mid_x, mid_y, -zNear, 1) -> (0, 0, -1, 1)
    Vec4 nearCenter = { 0.0f, 1.0f, -2.0f, 1.0f };
    EXPECT_VEC4("M4Orthographic maps near center to (0, 0, -1, 1)",
                M4MulV4(ortho, nearCenter),
                ((Vec4){ 0.0f, 0.0f, -1.0f, 1.0f }));

    // Far plane center: (mid_x, mid_y, -zFar, 1) -> (0, 0, 1, 1)
    Vec4 farCenter = { 0.0f, 1.0f, -10.0f, 1.0f };
    EXPECT_VEC4("M4Orthographic maps far center to (0, 0, 1, 1)",
                M4MulV4(ortho, farCenter),
                ((Vec4){ 0.0f, 0.0f, 1.0f, 1.0f }));

    // Corners of the viewing frustum
    Vec4 nearBottomLeft = { left, bottom, -zNear, 1.0f };
    EXPECT_VEC4("M4Orthographic maps near bottom-left corner to (-1, -1, -1, 1)",
                M4MulV4(ortho, nearBottomLeft),
                ((Vec4){ -1.0f, -1.0f, -1.0f, 1.0f }));

    Vec4 farTopRight = { right, top, -zFar, 1.0f };
    EXPECT_VEC4("M4Orthographic maps far top-right corner to (1, 1, 1, 1)",
                M4MulV4(ortho, farTopRight),
                ((Vec4){ 1.0f, 1.0f, 1.0f, 1.0f }));

    // Determinant is non-zero
    EXPECT_FLOAT("M4Det(M4Orthographic) != 0",
                 M4Det(ortho),
                 (2.0f / (right - left)) * (2.0f / (top - bottom)) * (-2.0f / (zFar - zNear)));
}

static void TestM4Perspective(void) {
    float fov = PI_F / 2.0f; // 90 degrees: tan(fov/2) == 1
    float aspect = 16.0f / 9.0f;
    float zNear = 1.0f;
    float zFar = 10.0f;

    Mat4 persp = M4Perspective(fov, aspect, zNear, zFar);

    Mat4 expectedPersp = {
        .r0 = { 9.0f / 16.0f, 0.0f,   0.0f,             0.0f },
        .r1 = { 0.0f,         1.0f,   0.0f,             0.0f },
        .r2 = { 0.0f,         0.0f, -11.0f / 9.0f, -20.0f / 9.0f },
        .r3 = { 0.0f,         0.0f,  -1.0f,             0.0f },
    };
    EXPECT_MAT4("M4Perspective explicit entries", persp, expectedPersp);

    // Near plane center projects to NDC z = -1
    Vec3 nearCenter = { 0.0f, 0.0f, -zNear };
    EXPECT_VEC3("M4Perspective maps near center to (0, 0, -1)",
                M4MulV3(persp, nearCenter),
                ((Vec3){ 0.0f, 0.0f, -1.0f }));

    // Far plane center projects to NDC z = +1
    Vec3 farCenter = { 0.0f, 0.0f, -zFar };
    EXPECT_VEC3("M4Perspective maps far center to (0, 0, 1)",
                M4MulV3(persp, farCenter),
                ((Vec3){ 0.0f, 0.0f, 1.0f }));

    // Near plane corners project to NDC (-1, -1, -1) and (1, 1, -1)
    float top = zNear * tanf(fov / 2.0f);
    float right = top * aspect;
    Vec3 nearTopRight = { right, top, -zNear };
    Vec3 nearBottomLeft = { -right, -top, -zNear };

    EXPECT_VEC3("M4Perspective maps near top-right corner to (1, 1, -1)",
                M4MulV3(persp, nearTopRight),
                ((Vec3){ 1.0f, 1.0f, -1.0f }));
    EXPECT_VEC3("M4Perspective maps near bottom-left corner to (-1, -1, -1)",
                M4MulV3(persp, nearBottomLeft),
                ((Vec3){ -1.0f, -1.0f, -1.0f }));

    // Test underlying M4Frustum directly
    Mat4 frustum = M4Frustum(-right, right, -top, top, zNear, zFar);
    EXPECT_MAT4("M4Frustum matches M4Perspective for symmetric bounds", frustum, persp);
}

static void TestM4LookAt(void) {
    // Case 1: Camera placed on +Z looking at origin with up along +Y
    Vec3 eyeZ = { 0.0f, 0.0f, 5.0f };
    Vec3 target = { 0.0f, 0.0f, 0.0f };
    Vec3 upY = { 0.0f, 1.0f, 0.0f };
    Mat4 lookAtZ = M4LookAt(eyeZ, target, upY);

    Mat4 expectedLookAtZ = {
        .r0 = { 1.0f, 0.0f, 0.0f,  0.0f },
        .r1 = { 0.0f, 1.0f, 0.0f,  0.0f },
        .r2 = { 0.0f, 0.0f, 1.0f, -5.0f },
        .r3 = { 0.0f, 0.0f, 0.0f,  1.0f },
    };
    EXPECT_MAT4("M4LookAt +Z eye looking at origin", lookAtZ, expectedLookAtZ);
    EXPECT_VEC4("M4LookAt maps eye to camera origin (0, 0, 0, 1)",
                M4MulV4(lookAtZ, ((Vec4){ eyeZ.x, eyeZ.y, eyeZ.z, 1.0f })),
                ((Vec4){ 0.0f, 0.0f, 0.0f, 1.0f }));
    EXPECT_VEC4("M4LookAt maps target to (0, 0, -dist, 1)",
                M4MulV4(lookAtZ, ((Vec4){ target.x, target.y, target.z, 1.0f })),
                ((Vec4){ 0.0f, 0.0f, -5.0f, 1.0f }));
    EXPECT_FLOAT("M4Det(M4LookAt) == 1", M4Det(lookAtZ), 1.0f);

    // Case 2: Camera placed on +X looking at origin with up along +Y
    Vec3 eyeX = { 4.0f, 0.0f, 0.0f };
    Mat4 lookAtX = M4LookAt(eyeX, target, upY);

    Mat4 expectedLookAtX = {
        .r0 = { 0.0f, 0.0f, -1.0f,  0.0f },
        .r1 = { 0.0f, 1.0f,  0.0f,  0.0f },
        .r2 = { 1.0f, 0.0f,  0.0f, -4.0f },
        .r3 = { 0.0f, 0.0f,  0.0f,  1.0f },
    };
    EXPECT_MAT4("M4LookAt +X eye looking at origin", lookAtX, expectedLookAtX);
    EXPECT_VEC4("M4LookAt maps +X eye to camera origin",
                M4MulV4(lookAtX, ((Vec4){ eyeX.x, eyeX.y, eyeX.z, 1.0f })),
                ((Vec4){ 0.0f, 0.0f, 0.0f, 1.0f }));
    EXPECT_VEC4("M4LookAt maps target from +X view to (0, 0, -4, 1)",
                M4MulV4(lookAtX, ((Vec4){ target.x, target.y, target.z, 1.0f })),
                ((Vec4){ 0.0f, 0.0f, -4.0f, 1.0f }));

    // Case 3: Arbitrary view position and target
    Vec3 eyeArb = { 3.0f, 4.0f, 5.0f };
    Vec3 targetArb = { 1.0f, 1.0f, 1.0f };
    Mat4 lookAtArb = M4LookAt(eyeArb, targetArb, upY);

    EXPECT_VEC4("M4LookAt arbitrary eye maps to camera origin",
                M4MulV4(lookAtArb, ((Vec4){ eyeArb.x, eyeArb.y, eyeArb.z, 1.0f })),
                ((Vec4){ 0.0f, 0.0f, 0.0f, 1.0f }));

    Vec4 targetInCam = M4MulV4(lookAtArb, ((Vec4){ targetArb.x, targetArb.y, targetArb.z, 1.0f }));
    float dist = V3Length(V3Sub(eyeArb, targetArb));
    EXPECT_FLOAT("M4LookAt arbitrary target has x=0 in view space", targetInCam.x, 0.0f);
    EXPECT_FLOAT("M4LookAt arbitrary target has y=0 in view space", targetInCam.y, 0.0f);
    EXPECT_FLOAT("M4LookAt arbitrary target has z=-dist in view space", targetInCam.z, -dist);
    EXPECT_FLOAT("M4LookAt arbitrary target has w=1 in view space", targetInCam.w, 1.0f);
    EXPECT_FLOAT("M4Det(M4LookAt arbitrary) == 1", M4Det(lookAtArb), 1.0f);
}

int main(void) {
    PrintSuiteHeader("Running Mat4 Unit Tests");
    TestMat4UnionLayout();
    TestM4MulV4();
    TestM4MulM4();
    TestM4Transpose();
    TestM4Det();
    TestM4RotateV();
    TestM4Translate();
    TestM4Orthographic();
    TestM4Perspective();
    TestM4LookAt();
    return TestSummary();
}
