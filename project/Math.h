#pragma once
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

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
struct AABB {
    Vector3 min;
    Vector3 max;
};
struct VertexData {
    Vector4 position;
    Vector2 texcoord;
    Vector3 normal;
};
struct ParticleForGPU {
    Matrix4x4 WVP;
    Matrix4x4 world;
    Vector4 color;
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
struct MaterialData {
    std::string textureFilePath;
    uint32_t textureIndex = 0;
};
struct ModelData {
    std::vector<VertexData> vertices;
    MaterialData material;
};
struct Particle {
    Transform transform;
    Vector3 velocity;
    Vector4 color;
    float lifeTime;
    float currentTime;
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

bool IsCollision(const AABB& aabb, const Vector3& point);

Vector3 Multiply(const Vector3& m1, const float& m2);

Vector3 operator*(const Vector3& m1, const float& m2);
Vector3& operator+=(Vector3& lhv, const Vector3& rhv);