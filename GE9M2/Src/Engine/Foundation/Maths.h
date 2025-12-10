#pragma once
#include <cmath>
#include <algorithm>
#define M_PI 3.141592654f
#define SQ(x) ((x) * (x))

class Quaternion;

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

// =========================================================================
// Col-major 4x4 matrix
// Column Multiplication
// 
// v' = M * v
// 
// | m0  m4  m8  m12 |   | v.x |
// | m1  m5  m9  m13 | * | v.y |
// | m2  m6  m10 m14 |   | v.z |
// | m3  m7  m11 m15 |   | v.w |
//
// Left Hand Coordinate System
// =========================================================================
class Matrix {
public:
    float m[16];

    Matrix() { identity(); }

    void identity() {
        for (int i = 0; i < 16; i++) m[i] = 0;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }

    static Matrix Identity() { return Matrix(); }

    static Matrix Zero() {
        Matrix output;
        for (int i = 0; i < 16; i++) output.m[i] = 0;
        return output;
    }

	// Access element at (row, col)
    float& at(int row, int col) { return m[col * 4 + row]; }
    const float& at(int row, int col) const { return m[col * 4 + row]; }

    Vec4 operator*(const Vec4& v) const {
        return Vec4(
            m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12] * v.w,
            m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13] * v.w,
            m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w,
            m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w
        );
    }

    Matrix operator*(const Matrix& b) const {
        Matrix r = Zero();
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                r.at(row, col) =
                    at(row, 0) * b.at(0, col) +
                    at(row, 1) * b.at(1, col) +
                    at(row, 2) * b.at(2, col) +
                    at(row, 3) * b.at(3, col);
            }
        }
        return r;
    }

    static Matrix translation(const Vec3& v) {
        Matrix r = Identity();
        r.at(0, 3) = v.x;
        r.at(1, 3) = v.y;
        r.at(2, 3) = v.z;
        return r;
    }

    static Matrix scale(const Vec3& s) {
        Matrix r = Identity();
        r.at(0, 0) = s.x; r.at(1, 1) = s.y; r.at(2, 2) = s.z;
        return r;
    }

    static Matrix rotationX(float rad) {
        Matrix r = Identity();
        float c = std::cos(rad), s = std::sin(rad);
        r.at(1, 1) = c; r.at(2, 1) = s;
        r.at(1, 2) = -s; r.at(2, 2) = c;
        return r;
    }

    static Matrix rotationY(float rad) {
        Matrix r = Identity();
        float c = std::cos(rad), s = std::sin(rad);
        r.at(0, 0) = c;  r.at(2, 0) = -s;
        r.at(0, 2) = s;  r.at(2, 2) = c;
        return r;
    }

    static Matrix rotationZ(float rad) {
        Matrix r = Identity();
        float c = std::cos(rad), s = std::sin(rad);
        r.at(0, 0) = c; r.at(1, 0) = s;
        r.at(0, 1) = -s; r.at(1, 1) = c;
        return r;
    }

	// =====================================================================
	// Left Hand LookAt Matrix
	// =====================================================================
    static Matrix lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {

        Vec3 z = (target - eye).normalized();
        Vec3 x = up.cross(z).normalized();
        Vec3 y = z.cross(x);

        Matrix r = Identity();
        r.at(0, 0) = x.x; r.at(0, 1) = x.y; r.at(0, 2) = x.z;
        r.at(1, 0) = y.x; r.at(1, 1) = y.y; r.at(1, 2) = y.z;
        r.at(2, 0) = z.x; r.at(2, 1) = z.y; r.at(2, 2) = z.z;
        r.at(0, 3) = -x.dot(eye);
        r.at(1, 3) = -y.dot(eye);
        r.at(2, 3) = -z.dot(eye);

        r.at(3, 3) = 1.0f;

        return r;
    }

	// =====================================================================
	// Left Hand Perspective Projection Matrix
	// =====================================================================
    static Matrix perspective(float n, float f, float aspect, float fovDeg) {
        float fov = fovDeg * (M_PI / 180.0f);
        float yScale = 1.0f / std::tan(fov * 0.5f);
        float xScale = yScale / aspect;

        Matrix r = Zero();

        r.at(0, 0) = xScale;
        r.at(1, 1) = yScale;
        r.at(2, 2) = f / (f - n);
        r.at(3, 2) = 1.0f;
        r.at(2, 3) = -n * f / (f - n);

        return r;
    }

    Matrix transpose() const {
        Matrix out = Matrix::Zero();
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                out.at(col, row) = this->at(row, col);
            }
        }
        return out;
    }
};

class Quaternion {
public:
    float w, x, y, z;

    Quaternion() { Identity(); }

    Quaternion(float ww, float xx, float yy, float zz) : w(ww), x(xx), y(yy), z(zz) {}

    static Quaternion Identity() { return Quaternion(1, 0, 0, 0); }

    float magnitude() const {  return std::sqrt(w * w + x * x + y * y + z * z); }

    Quaternion conjugate() const { return Quaternion(w, -x, -y, -z); }

    static Quaternion fromAxisAngle(const Vec3& axis, float rad) {
        Vec3 n = axis.normalized();
        float s = std::sin(rad * 0.5f);
        return Quaternion(std::cos(rad * 0.5f), n.x * s, n.y * s, n.z * s);
    }

    Quaternion normalized() const {
        float m = std::sqrt(w * w + x * x + y * y + z * z);
        if (m <= 0) return Quaternion();
        float i = 1.0f / m;
        return Quaternion(w * i, x * i, y * i, z * i);
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

    static Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t) {
        Quaternion b = q2;
        float dot = q1.w * b.w + q1.x * b.x + q1.y * b.y + q1.z * b.z;

        if (dot < 0.0f) {
            dot = -dot;
            b.w = -b.w; b.x = -b.x; b.y = -b.y; b.z = -b.z;
        }

        const float EPS = 0.9995f;
        if (dot > EPS) {
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
        Quaternion q = normalized();
        float xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
        float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;
        float wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;

        Matrix m = Matrix::Identity();

        m.at(0, 0) = 1 - 2 * (yy + zz);
        m.at(1, 0) = 2 * (xy + wz);
        m.at(2, 0) = 2 * (xz - wy);

        m.at(0, 1) = 2 * (xy - wz);
        m.at(1, 1) = 1 - 2 * (xx + zz);
        m.at(2, 1) = 2 * (yz + wx);

        m.at(0, 2) = 2 * (xz + wy);
        m.at(1, 2) = 2 * (yz - wx);
        m.at(2, 2) = 1 - 2 * (xx + yy);

        return m;
    }

    static Quaternion fromMatrix(const Matrix& M) {
        float m00 = M.at(0, 0); float m01 = M.at(0, 1); float m02 = M.at(0, 2);
        float m10 = M.at(1, 0); float m11 = M.at(1, 1); float m12 = M.at(1, 2);
        float m20 = M.at(2, 0); float m21 = M.at(2, 1); float m22 = M.at(2, 2);

        float trace = m00 + m11 + m22;
        Quaternion q;

        if (trace > 0.0f)
        {
            float s = std::sqrt(trace + 1.0f) * 2.0f; // 4*q.w
            q.w = 0.25f * s;
            q.x = (m21 - m12) / s;
            q.y = (m02 - m20) / s;
            q.z = (m10 - m01) / s;
        }
        else if (m00 > m11 && m00 > m22)
        {
            float s = std::sqrt(1.0f + m00 - m11 - m22) * 2.0f; // 4*q.x
            q.w = (m21 - m12) / s;
            q.x = 0.25f * s;
            q.y = (m01 + m10) / s;
            q.z = (m02 + m20) / s;
        }
        else if (m11 > m22)
        {
            float s = std::sqrt(1.0f + m11 - m00 - m22) * 2.0f; // 4*q.y
            q.w = (m02 - m20) / s;
            q.x = (m01 + m10) / s;
            q.y = 0.25f * s;
            q.z = (m12 + m21) / s;
        }
        else
        {
            float s = std::sqrt(1.0f + m22 - m00 - m11) * 2.0f; // 4*q.z
            q.w = (m10 - m01) / s;
            q.x = (m02 + m20) / s;
            q.y = (m12 + m21) / s;
            q.z = 0.25f * s;
        }

        return q.normalized();
    }

};

class Colour {
public:
    float r, g, b, a;

    Colour() : r(0), g(0), b(0), a(1) {}
    Colour(float R, float G, float B, float A = 1.0f)
        : r(R), g(G), b(B), a(A) {
    }

    Colour operator+(const Colour& c) const {
        return Colour(r + c.r, g + c.g, b + c.b, a + c.a);
    }

    Colour operator*(const Colour& c) const {
        return Colour(r * c.r, g * c.g, b * c.b, a * c.a);
    }

    Colour operator*(float s) const {
        return Colour(r * s, g * s, b * s, a * s);
    }

    Colour operator/(float s) const {
        float inv = 1.0f / s;
        return Colour(r * inv, g * inv, b * inv, a * inv);
    }
};