#include "../test_common.h"

static void TestMat3UnionLayout(void) {
    Mat3 m = {
        .r0 = { 1.0f, 2.0f, 3.0f },
        .r1 = { 4.0f, 5.0f, 6.0f },
        .r2 = { 7.0f, 8.0f, 9.0f },
    };

    EXPECT_FLOAT("Mat3.s00/s01/s02 maps to r0", m.s00 + m.s01 + m.s02, 6.0f);
    EXPECT_FLOAT("Mat3.s10/s11/s12 maps to r1", m.s10 + m.s11 + m.s12, 15.0f);
    EXPECT_FLOAT("Mat3.s20/s21/s22 maps to r2", m.s20 + m.s21 + m.s22, 24.0f);
    EXPECT_VEC3("Mat3.rows[1] == r1", m.rows[1], ((Vec3){ 4.0f, 5.0f, 6.0f }));
}

static void TestMat3BasicElementwise(void) {
    Mat3 a = {
        .r0 = { 1.0f, 2.0f, 3.0f },
        .r1 = { 4.0f, 5.0f, 6.0f },
        .r2 = { 7.0f, 8.0f, 9.0f },
    };
    Mat3 b = {
        .r0 = { 9.0f, 8.0f, 7.0f },
        .r1 = { 6.0f, 5.0f, 4.0f },
        .r2 = { 3.0f, 2.0f, 1.0f },
    };

    Mat3 expectedAdd = {
        .r0 = { 10.0f, 10.0f, 10.0f },
        .r1 = { 10.0f, 10.0f, 10.0f },
        .r2 = { 10.0f, 10.0f, 10.0f },
    };
    Mat3 expectedSub = {
        .r0 = { -8.0f, -6.0f, -4.0f },
        .r1 = { -2.0f,  0.0f,  2.0f },
        .r2 = {  4.0f,  6.0f,  8.0f },
    };
    Mat3 expectedMulS = {
        .r0 = { 2.0f,  4.0f,  6.0f },
        .r1 = { 8.0f, 10.0f, 12.0f },
        .r2 = { 14.0f, 16.0f, 18.0f },
    };

    EXPECT_MAT3("M3Add", M3Add(a, b), expectedAdd);
    EXPECT_MAT3("M3Sub", M3Sub(a, b), expectedSub);
    EXPECT_MAT3("M3MulS", M3MulS(a, 2.0f), expectedMulS);
    EXPECT_MAT3("M3DivS", M3DivS(expectedMulS, 2.0f), a);
    EXPECT_TRUE("M3Eq equal", M3Eq(a, a));
    EXPECT_TRUE("M3Eq not equal", !M3Eq(a, b));
}

static void TestMat3MulAndTranspose(void) {
    Mat3 zero = M3DiagS(0.0f);
    Mat3 identity = M3DiagS(1.0f);
    Mat3 a = {
        .r0 = { 1.0f, 2.0f, 3.0f },
        .r1 = { 4.0f, 5.0f, 6.0f },
        .r2 = { 7.0f, 8.0f, 9.0f },
    };
    Vec3 v = { 1.0f, 2.0f, 3.0f };
    Vec3 zeroV = { 0.0f, 0.0f, 0.0f };

    // M3MulV3 general & edge cases
    EXPECT_VEC3("M3MulV3 general", M3MulV3(a, v), ((Vec3){ 14.0f, 32.0f, 50.0f }));
    EXPECT_VEC3("M3MulV3 with Identity (I * v == v)", M3MulV3(identity, v), v);
    EXPECT_VEC3("M3MulV3 with Zero matrix (0 * v == 0)", M3MulV3(zero, v), zeroV);
    EXPECT_VEC3("M3MulV3 with Zero vector (A * 0 == 0)", M3MulV3(a, zeroV), zeroV);

    // M3Transpose general & involution (A^T)^T == A
    Mat3 aT = {
        .r0 = { 1.0f, 4.0f, 7.0f },
        .r1 = { 2.0f, 5.0f, 8.0f },
        .r2 = { 3.0f, 6.0f, 9.0f },
    };
    EXPECT_MAT3("M3Transpose", M3Transpose(a), aT);
    EXPECT_MAT3("M3Transpose(M3Transpose(A)) == A", M3Transpose(M3Transpose(a)), a);
    EXPECT_MAT3("M3Transpose(Identity) == Identity", M3Transpose(identity), identity);

    // M3MulM3 general & edge cases
    Mat3 b = {
        .r0 = { 1.0f, 0.0f, 2.0f },
        .r1 = { -1.0f, 3.0f, 1.0f },
        .r2 = { 0.0f, 2.0f, -2.0f },
    };
    Mat3 expectedAB = {
        .r0 = { -1.0f, 12.0f, -2.0f },
        .r1 = { -1.0f, 27.0f,  1.0f },
        .r2 = { -1.0f, 42.0f,  4.0f },
    };
    Mat3 expectedBA = {
        .r0 = { 15.0f, 18.0f, 21.0f },
        .r1 = { 18.0f, 21.0f, 24.0f },
        .r2 = { -6.0f, -6.0f, -6.0f },
    };
    EXPECT_MAT3("M3MulM3 general (A * B)", M3MulM3(a, b), expectedAB);
    EXPECT_MAT3("M3MulM3 non-commutative (B * A)", M3MulM3(b, a), expectedBA);
    EXPECT_TRUE("M3MulM3 A*B != B*A", !Mat3Near(M3MulM3(a, b), M3MulM3(b, a)));
    EXPECT_MAT3("M3MulM3 right identity (A * I == A)", M3MulM3(a, identity), a);
    EXPECT_MAT3("M3MulM3 left identity (I * A == A)", M3MulM3(identity, a), a);
    EXPECT_MAT3("M3MulM3 right zero (A * 0 == 0)", M3MulM3(a, zero), zero);
    EXPECT_MAT3("M3MulM3 left zero (0 * A == 0)", M3MulM3(zero, a), zero);

    Mat3 diag1 = M3Diag(2.0f, -3.0f, 4.0f);
    Mat3 diag2 = M3Diag(5.0f,  2.0f, -0.5f);
    EXPECT_MAT3("M3MulM3 diagonal matrices", M3MulM3(diag1, diag2), M3Diag(10.0f, -6.0f, -2.0f));
}

static void TestMat3DetAndInverse(void) {
    Mat3 zero = M3DiagS(0.0f);
    Mat3 identity = M3DiagS(1.0f);

    EXPECT_FLOAT("M3Det(Identity) == 1", M3Det(identity), 1.0f);
    EXPECT_FLOAT("M3Det(Diag(2, 3, -4)) == -24", M3Det(M3Diag(2.0f, 3.0f, -4.0f)), -24.0f);

    // Singular matrix edge cases (det == 0)
    EXPECT_FLOAT("M3Det(Zero matrix) == 0 (singular)", M3Det(zero), 0.0f);

    Mat3 singularZeroRow = {
        .r0 = { 1.0f, 2.0f, 3.0f },
        .r1 = { 0.0f, 0.0f, 0.0f },
        .r2 = { 4.0f, 5.0f, 6.0f },
    };
    EXPECT_FLOAT("M3Det(zero row) == 0 (singular)", M3Det(singularZeroRow), 0.0f);

    Mat3 singularProportionalRows = {
        .r0 = { 1.0f, -2.0f,  3.0f },
        .r1 = { 2.0f, -4.0f,  6.0f },
        .r2 = { 5.0f,  1.0f, -1.0f },
    };
    EXPECT_FLOAT("M3Det(proportional rows r1 = 2*r0) == 0 (singular)", M3Det(singularProportionalRows), 0.0f);

    Mat3 singularLinearComb = {
        .r0 = { 1.0f, 2.0f, 3.0f },
        .r1 = { 4.0f, 5.0f, 6.0f },
        .r2 = { 7.0f, 8.0f, 9.0f }, // r2 = 2*r1 - r0
    };
    EXPECT_FLOAT("M3Det(linearly dependent rows 1..9) == 0 (singular)", M3Det(singularLinearComb), 0.0f);

    // Invertible matrix
    Mat3 m = {
        .r0 = { 2.0f, 0.0f, 1.0f },
        .r1 = { 1.0f, 3.0f, -1.0f },
        .r2 = { 0.0f, 2.0f, 4.0f },
    };
    EXPECT_FLOAT("M3Det(invertible general)", M3Det(m), 30.0f);

    // M3Adj tests
    EXPECT_MAT3("M3Adj(Identity) == Identity", M3Adj(identity), identity);
    EXPECT_MAT3("M3Adj(Diag(2, 3, 4)) == Diag(12, 8, 6)",
                M3Adj(M3Diag(2.0f, 3.0f, 4.0f)),
                M3Diag(12.0f, 8.0f, 6.0f));

    Mat3 expectedAdjM = {
        .r0 = { 14.0f,  2.0f, -3.0f },
        .r1 = { -4.0f,  8.0f,  3.0f },
        .r2 = {  2.0f, -4.0f,  6.0f },
    };
    EXPECT_MAT3("M3Adj(m) explicit entries", M3Adj(m), expectedAdjM);
    EXPECT_MAT3("M3MulM3(m, M3Adj(m)) == det(m)*I", M3MulM3(m, M3Adj(m)), M3DiagS(30.0f));
    EXPECT_MAT3("M3MulM3(M3Adj(m), m) == det(m)*I", M3MulM3(M3Adj(m), m), M3DiagS(30.0f));
    EXPECT_MAT3("M3MulM3(singular, M3Adj(singular)) == 0",
                M3MulM3(singularLinearComb, M3Adj(singularLinearComb)),
                zero);

    Mat3 mInv = M3Inverse(m);
    EXPECT_MAT3("M3MulM3(m, M3Inverse(m)) == I", M3MulM3(m, mInv), identity);
    EXPECT_MAT3("M3MulM3(M3Inverse(m), m) == I", M3MulM3(mInv, m), identity);
}

static void TestMat3Rotations(void) {
    Vec3 xAxis = { 1.0f, 0.0f, 0.0f };
    Vec3 yAxis = { 0.0f, 1.0f, 0.0f };
    Vec3 zAxis = { 0.0f, 0.0f, 1.0f };

    Mat3 rotZ90 = M3Rotate(PI_F / 2.0f, zAxis);
    EXPECT_VEC3("M3Rotate +90 deg around Z on X", M3MulV3(rotZ90, xAxis), yAxis);
    EXPECT_VEC3("M3Rotate +90 deg around Z on Y", M3MulV3(rotZ90, yAxis), V3Neg(xAxis));
    EXPECT_VEC3("M3Rotate +90 deg around Z on Z", M3MulV3(rotZ90, zAxis), zAxis);

    Mat3 rotDiag = M3Rotate(2.0f * PI_F / 3.0f, (Vec3){ 2.0f, 2.0f, 2.0f });
    EXPECT_VEC3("M3Rotate 120 deg around (2,2,2) on X", M3MulV3(rotDiag, xAxis), yAxis);
    EXPECT_FLOAT("M3Det(M3Rotate) == 1", M3Det(rotDiag), 1.0f);

    Vec3 src = { 3.0f, 0.0f, 0.0f };
    Vec3 dst = { 0.0f, 5.0f, 0.0f };
    Mat3 rotXY = M3RotateV(src, dst);
    EXPECT_VEC3("M3RotateV (3,0,0) -> (0,5,0) applied to X", M3MulV3(rotXY, xAxis), yAxis);
    EXPECT_FLOAT("M3Det(M3RotateV) == 1", M3Det(rotXY), 1.0f);

    Vec3 arbitrarySrc = V3Normalize((Vec3){ 1.0f, 2.0f, -3.0f });
    Vec3 arbitraryDst = V3Normalize((Vec3){ -2.0f, 4.0f, 1.0f });
    Mat3 rotArb = M3RotateV(arbitrarySrc, arbitraryDst);
    EXPECT_VEC3("M3RotateV arbitrary unit vectors", M3MulV3(rotArb, arbitrarySrc), arbitraryDst);
    EXPECT_MAT3("M3RotateV is orthogonal (R^T * R == I)", M3MulM3(M3Transpose(rotArb), rotArb), M3DiagS(1.0f));
}

int main(void) {
    PrintSuiteHeader("Running Mat3 Unit Tests");
    TestMat3UnionLayout();
    TestMat3BasicElementwise();
    TestMat3MulAndTranspose();
    TestMat3DetAndInverse();
    TestMat3Rotations();
    return TestSummary();
}
