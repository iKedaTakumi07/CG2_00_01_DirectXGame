#pragma once
#include <cmath>
#include <cstdint>

struct Vector2 {
    float x;
    float y;
};
struct Vector3 {
    float x;
    float y;
    float z;
};
struct Vector4 {
    float x;
    float y;
    float z;
    float w;
};
struct Transform {
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
};
struct Matrix4x4 {
    float m[4][4];
};
struct VertexData {
    Vector4 position;
    Vector2 texcoord;
    Vector3 normal;
};
struct Material {
    Vector4 color;
    int32_t enableLighting;
    float padding[3];
    Matrix4x4 uvTransform;
};
struct TransformationMatrix {
    Matrix4x4 WVP;
    Matrix4x4 world;
};

Matrix4x4 MakeIdentity4x4();

Matrix4x4 MakeRotateXMatrix(float radian);

Matrix4x4 MakeRotateYMatrix(float radian);

Matrix4x4 MakeRotateZMatrix(float radian);

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

Matrix4x4 MakePrespectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

Vector3 Normalize(const Vector3& v);

Matrix4x4 Inverse(const Matrix4x4& m);
