#pragma once
#include <cmath>
#include <algorithm>
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define M_PI 3.141592654f
#define SQ(x) ((x) * (x))

class Vec3 {
public:
    union {
        float v[3];
        struct { float x, y, z; };
    };

    Vec3(float xx = 0, float yy = 0, float zz = 0)
        : x(xx), y(yy), z(zz) {
    }

    Vec3 operator-() const { return Vec3(-x, -y, -z); }

    Vec3 operator+(const Vec3& r) const { return Vec3(x + r.x, y + r.y, z + r.z); }

    Vec3 operator-(const Vec3& r) const { return Vec3(x - r.x, y - r.y, z - r.z); }

    Vec3 operator*(const Vec3& r) const { return Vec3(x * r.x, y * r.y, z * r.z); }

    Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }

    Vec3 operator/(float s) const { float inverse = 1.0f / s; return Vec3(x * inverse, y * inverse, z * inverse); }

    Vec3& operator+=(const Vec3& r) { x += r.x; y += r.y; z += r.z; return *this; }

    Vec3& operator-=(const Vec3& r) { x -= r.x; y -= r.y; z -= r.z; return *this; }

    Vec3& operator*=(const Vec3& r) { x *= r.x; y *= r.y; z *= r.z; return *this; }

    Vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }

    Vec3& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }

    float length() const { return std::sqrt(x * x + y * y + z * z); }

    float lengthSqrt() const { return x * x + y * y + z * z; }

    // return a normalized copy of this vector
    Vec3 normalized() const {
        float lsq = lengthSqrt();
        if (lsq <= 0.0f) return Vec3(0, 0, 0);

        float inverse = 1.0f / std::sqrt(lsq);
        return Vec3(x * inverse, y * inverse, z * inverse);
    }

    // normalize in-place and return original length before normalization
    float normalize() {
        float len = length();
        if (len <= 0.0f) {
            x = y = z = 0;
            return 0;
        }

        float inv = 1.0f / len;
        x *= inv;
        y *= inv;
        z *= inv;
        return len;
    }

    // dot & cross
    float dot(const Vec3& r) const { return x * r.x + y * r.y + z * r.z; }

    Vec3 cross(const Vec3& r) const { return Vec3(y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x); }

    static float dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

    static Vec3 cross(const Vec3& a, const Vec3& b) { return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }

    // max function
    float max() const { return std::max(x, std::max(y, z)); }

    static Vec3 max(const Vec3& a, const Vec3& b) {
        return Vec3(
            std::max(a.x, b.x),
            std::max(a.y, b.y),
            std::max(a.z, b.z));
    }
};

class Vec4 {
public:
    union {
        float v[4];
        struct { float x, y, z, w; };
    };

    Vec4(float xx = 0, float yy = 0, float zz = 0, float ww = 0) : x(xx), y(yy), z(zz), w(ww) {}

    Vec4(const Vec3& p, float ww) : x(p.x), y(p.y), z(p.z), w(ww) {}

    Vec4 operator*(float s) const { return Vec4(x * s, y * s, z * s, w * s); }

    float max() const {
        return std::max(x, std::max(y, z));
    }
};

class Matrix {
public:
    union {
        float m[16];
        float a[4][4];
    };

    Matrix() { identity(); }

    void identity() {
        for (int col = 0; col < 4; ++col)
            for (int row = 0; row < 4; ++row)
                a[col][row] = (col == row ? 1.0f : 0.0f);
    }

    static Matrix Identity() { return Matrix(); }

    static Matrix Zero() {
        Matrix r;
        for (int i = 0; i < 16; ++i) r.m[i] = 0;
        return r;
    }

    Vec4 operator*(const Vec4& v) const {
        return Vec4(
            a[0][0] * v.x + a[1][0] * v.y + a[2][0] * v.z + a[3][0] * v.w,
            a[0][1] * v.x + a[1][1] * v.y + a[2][1] * v.z + a[3][1] * v.w,
            a[0][2] * v.x + a[1][2] * v.y + a[2][2] * v.z + a[3][2] * v.w,
            a[0][3] * v.x + a[1][3] * v.y + a[2][3] * v.z + a[3][3] * v.w
        );
    }

    Matrix operator*(const Matrix& rhs) const {
        Matrix out;
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                out.a[col][row] =
                    a[0][row] * rhs.a[col][0] +
                    a[1][row] * rhs.a[col][1] +
                    a[2][row] * rhs.a[col][2] +
                    a[3][row] * rhs.a[col][3];
            }
        }
        return out;
    }

    static Matrix translation(const Vec3& v) {
        Matrix out;
        out.a[3][0] = v.x;
        out.a[3][1] = v.y;
        out.a[3][2] = v.z;
        return out;
    }

    static Matrix scale(const Vec3& s) {
        Matrix out;
        out.a[0][0] = s.x;
        out.a[1][1] = s.y;
        out.a[2][2] = s.z;
        return out;
    }

    static Matrix rotationX(float rad) {
        Matrix M = Identity();
        float c = std::cos(rad), s = std::sin(rad);

        M.a[1][1] = c;  M.a[2][1] = -s;
        M.a[1][2] = s; M.a[2][2] = c;
        return M;
    }

    static Matrix rotationY(float rad) {
        Matrix out = Matrix::Identity();
        float c = std::cos(rad);
        float s = std::sin(rad);

        out.a[0][0] = c;  out.a[2][0] = -s;
        out.a[0][2] = s; out.a[2][2] = c;
        return out;
    }

    static Matrix rotationZ(float rad) {
        Matrix out = Matrix::Identity();
        float c = std::cos(rad);
        float s = std::sin(rad);

        out.a[0][0] = c;  out.a[1][0] = s;
        out.a[0][1] = -s;  out.a[1][1] = c;
        return out;
    }

    // Left Hand
    static Matrix lookAt(const Vec3& from, const Vec3& to, const Vec3& up) {
        Vec3 zaxis = (to - from).normalized();
        Vec3 xaxis = Vec3::cross(up, zaxis).normalized(); 
        Vec3 yaxis = Vec3::cross(zaxis, xaxis);

        Matrix out = Matrix::Identity();

        out.a[0][0] = xaxis.x; out.a[1][0] = yaxis.x; out.a[2][0] = zaxis.x;
        out.a[0][1] = xaxis.y; out.a[1][1] = yaxis.y; out.a[2][1] = zaxis.y;
        out.a[0][2] = xaxis.z; out.a[1][2] = yaxis.z; out.a[2][2] = zaxis.z;

        out.a[3][0] = -xaxis.dot(from);
        out.a[3][1] = -yaxis.dot(from);
        out.a[3][2] = -zaxis.dot(from);
        return out;
    }

    // Left Hand
    static Matrix perspective(float zn, float zf, float aspect, float fovDeg) {
        float fov = fovDeg * (M_PI / 180.0f);
        float h = 1.0f / std::tan(fov / 2.0f);

        Matrix out = Matrix::Zero();
        out.a[0][0] = h / aspect;
        out.a[1][1] = h; 
        out.a[2][2] = zf / (zf - zn);
        out.a[3][2] = (-zn * zf) / (zf - zn);
        out.a[2][3] = 1.0f;
        return out;
    }

    Matrix transpose() const {
        Matrix out = Matrix::Zero();
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                out.a[col][row] = a[row][col];
            }
        }
        return out;
    }

    Matrix inverse() const {
        Matrix invMat;
        float inv[16];

        const float* m = this->m;

        inv[0] = m[5] * m[10] * m[15] -
            m[5] * m[11] * m[14] -
            m[9] * m[6] * m[15] +
            m[9] * m[7] * m[14] +
            m[13] * m[6] * m[11] -
            m[13] * m[7] * m[10];

        inv[4] = -m[4] * m[10] * m[15] +
            m[4] * m[11] * m[14] +
            m[8] * m[6] * m[15] -
            m[8] * m[7] * m[14] -
            m[12] * m[6] * m[11] +
            m[12] * m[7] * m[10];

        inv[8] = m[4] * m[9] * m[15] -
            m[4] * m[11] * m[13] -
            m[8] * m[5] * m[15] +
            m[8] * m[7] * m[13] +
            m[12] * m[5] * m[11] -
            m[12] * m[7] * m[9];

        inv[12] = -m[4] * m[9] * m[14] +
            m[4] * m[10] * m[13] +
            m[8] * m[5] * m[14] -
            m[8] * m[6] * m[13] -
            m[12] * m[5] * m[10] +
            m[12] * m[6] * m[9];

        inv[1] = -m[1] * m[10] * m[15] +
            m[1] * m[11] * m[14] +
            m[9] * m[2] * m[15] -
            m[9] * m[3] * m[14] -
            m[13] * m[2] * m[11] +
            m[13] * m[3] * m[10];

        inv[5] = m[0] * m[10] * m[15] -
            m[0] * m[11] * m[14] -
            m[8] * m[2] * m[15] +
            m[8] * m[3] * m[14] +
            m[12] * m[2] * m[11] -
            m[12] * m[3] * m[10];

        inv[9] = -m[0] * m[9] * m[15] +
            m[0] * m[11] * m[13] +
            m[8] * m[1] * m[15] -
            m[8] * m[3] * m[13] -
            m[12] * m[1] * m[11] +
            m[12] * m[3] * m[9];

        inv[13] = m[0] * m[9] * m[14] -
            m[0] * m[10] * m[13] -
            m[8] * m[1] * m[14] +
            m[8] * m[2] * m[13] +
            m[12] * m[1] * m[10] -
            m[12] * m[2] * m[9];

        inv[2] = m[1] * m[6] * m[15] -
            m[1] * m[7] * m[14] -
            m[5] * m[2] * m[15] +
            m[5] * m[3] * m[14] +
            m[13] * m[2] * m[7] -
            m[13] * m[3] * m[6];

        inv[6] = -m[0] * m[6] * m[15] +
            m[0] * m[7] * m[14] +
            m[4] * m[2] * m[15] -
            m[4] * m[3] * m[14] -
            m[12] * m[2] * m[7] +
            m[12] * m[3] * m[6];

        inv[10] = m[0] * m[5] * m[15] -
            m[0] * m[7] * m[13] -
            m[4] * m[1] * m[15] +
            m[4] * m[3] * m[13] +
            m[12] * m[1] * m[7] -
            m[12] * m[3] * m[5];

        inv[14] = -m[0] * m[5] * m[14] +
            m[0] * m[6] * m[13] +
            m[4] * m[1] * m[14] -
            m[4] * m[2] * m[13] -
            m[12] * m[1] * m[6] +
            m[12] * m[2] * m[5];

        inv[3] = -m[1] * m[6] * m[11] +
            m[1] * m[7] * m[10] +
            m[5] * m[2] * m[11] -
            m[5] * m[3] * m[10] -
            m[9] * m[2] * m[7] +
            m[9] * m[3] * m[6];

        inv[7] = m[0] * m[6] * m[11] -
            m[0] * m[7] * m[10] -
            m[4] * m[2] * m[11] +
            m[4] * m[3] * m[10] +
            m[8] * m[2] * m[7] -
            m[8] * m[3] * m[6];

        inv[11] = -m[0] * m[5] * m[11] +
            m[0] * m[7] * m[9] +
            m[4] * m[1] * m[11] -
            m[4] * m[3] * m[9] -
            m[8] * m[1] * m[7] +
            m[8] * m[3] * m[5];

        inv[15] = m[0] * m[5] * m[10] -
            m[0] * m[6] * m[9] -
            m[4] * m[1] * m[10] +
            m[4] * m[2] * m[9] +
            m[8] * m[1] * m[6] -
            m[8] * m[2] * m[5];

        float det =
            m[0] * inv[0] +
            m[1] * inv[4] +
            m[2] * inv[8] +
            m[3] * inv[12];

        if (det == 0.0f)  return Matrix::Identity();

        float invDet = 1.0f / det;
        for (int i = 0; i < 16; i++)
            invMat.m[i] = inv[i] * invDet;

        return invMat;
    }
};

class Quaternion {
public:
    float w, x, y, z;

    Quaternion() : w(1), x(0), y(0), z(0) {}
    Quaternion(float ww, float xx, float yy, float zz)
        : w(ww), x(xx), y(yy), z(zz) {
    }

    static Quaternion Identity() {
        return Quaternion(1, 0, 0, 0);
    }

    float magnitude() const {
        return std::sqrt(w * w + x * x + y * y + z * z);
    }

    Quaternion normalized() const {
        float mag = magnitude();
        if (mag <= 0.0f) return Identity();
        float inv = 1.0f / mag;
        return Quaternion(w * inv, x * inv, y * inv, z * inv);
    }

    Quaternion conjugate() const {
        return Quaternion(w, -x, -y, -z);
    }

    Quaternion inverse() const {
        float lsq = w * w + x * x + y * y + z * z;
        if (lsq <= 0.0f) return Identity();
        float inv = 1.0f / lsq;
        return Quaternion(w * inv, -x * inv, -y * inv, -z * inv);
    }

    Quaternion operator*(const Quaternion& r) const {
        return Quaternion(
            w * r.w - x * r.x - y * r.y - z * r.z,
            w * r.x + x * r.w + y * r.z - z * r.y,
            w * r.y - x * r.z + y * r.w + z * r.x,
            w * r.z + x * r.y - y * r.x + z * r.w
        );
    }

    static Quaternion fromAxisAngle(const Vec3& axis, float angleRad) {
        Vec3 n = axis.normalized();
        float half = angleRad * 0.5f;
        float s = std::sin(half);
        return Quaternion(std::cos(half), n.x * s, n.y * s, n.z * s);
    }

    static Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t) {
        Quaternion b = q2;
        float dot = q1.w * b.w + q1.x * b.x + q1.y * b.y + q1.z * b.z;

        if (dot < 0.0f)
        {
            dot = -dot;
            b.w = -b.w; b.x = -b.x; b.y = -b.y; b.z = -b.z;
        }

        const float EPS = 0.9995f;
        if (dot > EPS)
        {
            Quaternion r(
                q1.w + t * (b.w - q1.w),
                q1.x + t * (b.x - q1.x),
                q1.y + t * (b.y - q1.y),
                q1.z + t * (b.z - q1.z)
            );
            return r.normalized();
        }

        float theta0 = std::acos(dot);
        float theta = theta0 * t;

        float sin0 = std::sin(theta0);
        float s0 = std::cos(theta) - dot * std::sin(theta) / sin0;
        float s1 = std::sin(theta) / sin0;

        return Quaternion(
            s0 * q1.w + s1 * b.w,
            s0 * q1.x + s1 * b.x,
            s0 * q1.y + s1 * b.y,
            s0 * q1.z + s1 * b.z
        );
    }

    Vec3 rotate(const Vec3& v) const {
        Quaternion p(0, v.x, v.y, v.z);
        Quaternion r = (*this) * p * conjugate();
        return Vec3(r.x, r.y, r.z);
    }

    Matrix toMatrix() const {
        float xx = x * x;
        float yy = y * y;
        float zz = z * z;
        float xy = x * y;
        float xz = x * z;
        float yz = y * z;
        float wx = w * x;
        float wy = w * y;
        float wz = w * z;

        Matrix M;

        M.m[0] = 1.0f - 2.0f * (yy + zz);
        M.m[1] = 2.0f * (xy + wz);
        M.m[2] = 2.0f * (xz - wy);
        M.m[3] = 0.0f;

        M.m[4] = 2.0f * (xy - wz);
        M.m[5] = 1.0f - 2.0f * (xx + zz); 
        M.m[6] = 2.0f * (yz + wx);
        M.m[7] = 0.0f;

        M.m[8] = 2.0f * (xz + wy); 
        M.m[9] = 2.0f * (yz - wx);
        M.m[10] = 1.0f - 2.0f * (xx + yy);
        M.m[11] = 0.0f;

        M.m[12] = 0.0f;
        M.m[13] = 0.0f;
        M.m[14] = 0.0f;
        M.

};