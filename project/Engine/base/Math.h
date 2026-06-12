#pragma once
#include <cmath>
#include <cstdint>
#include <map>
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
struct Matrix4x4 {
    float m[4][4];
};
struct AABB {
    Vector3 min;
    Vector3 max;
};
struct Transform {
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
};
struct VertexData {
    Vector4 position;
    Vector2 texcoord;
    Vector3 normal;
};
struct SkyboxVertexData {
    Vector4 position;
    Vector3 texcoord;
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
    float shininess;
    float evnironmentCoefficient;
};
struct ParticleMaterial {
    Matrix4x4 uvTransform;
    Vector4 color;
    int32_t enableLighting;
    int32_t useClampSampler; // ⭐️ 追加 (0: WRAP, 1: CLAMP)
    float padding[2];
};
struct TransformationMatrix {
    Matrix4x4 WVP;
    Matrix4x4 world;
    Matrix4x4 worldInverseTranspose;
};
struct DirectionalLight {
    Vector4 color;
    Vector3 direction;
    float intensity;
};
struct MaterialData {
    std::string textureFilePath;
    uint32_t textureIndex = 0;
};
struct Node {
    Matrix4x4 localMatrix;
    std::string name;
    std::vector<Node> childrem;
};

template <typename tValue>
struct keyframe {
    float time;
    tValue value;
};
using keyframeVector3 = keyframe<Vector3>;
using keyframeQuaternion = keyframe<Vector4>;

template <typename tValue>
struct AnimationCurve {
    std::vector<keyframe<tValue>> keyframes;
};

struct NodeAnimation {
    AnimationCurve<Vector3> translate;
    AnimationCurve<Vector4> rotate;
    AnimationCurve<Vector3> scale;
};

struct Animation {
    float duration; // アニメーション全体の尺
    std::map<std::string, NodeAnimation> nodeAnimations;
};

struct ModelData {
    std::vector<VertexData> vertices;
    std::vector<uint32_t> indices;
    MaterialData material;
    Node rootNode;
};
struct PointLigth {
    Vector4 color; // 色
    Vector3 position; // 位置
    float intensity; // 輝度
    float radius; // ライトの届く最大距離
    float decay; // 減衰率
    float Padding[2];
};

struct AccelerationField {
    Vector3 acceleration;
    AABB area;
};
struct Particle {
    Transform transform;
    Vector3 velocity;
    Vector4 startColor;
    Vector4 endColor;
    Vector4 color;
    float lifeTime;
    float currentTime;
    bool isInfinite;
};
enum BlendMode {
    kBlendModeNone, // ブレンドなし
    kBlendModeNormal, // 通常αブレンド
    kBlendModeAdd, // 加算
    kBlendModeSubtract, // 減算
    kBlendModeMultily, // 乗算
    kBlendModeScreen, // スクリーン
};
struct Emitter {
    Transform transform;
    uint32_t count;
    float frequency;
    float ferquencyTime;
};
struct CameraForGPU {
    Vector3 worldPosition;
};
struct SpotLigth {
    Vector4 color;
    Vector3 position;
    float intensity;
    Vector3 direction;
    float distance;
    float decay;
    float cosAngle;
    float cosFalloffStart; // スポットライトの内側の角度（減衰開始の余弦）
    float padding[2];
};

Vector3 CalculateValue(const std::vector<keyframeVector3>& keyframes, float time);

Vector4 CalculateValue(const std::vector<keyframeQuaternion>& keyframes, float time);

Vector3 Lerp(const Vector3& start, const Vector3& end, float t);

Vector4 Lerp(const Vector4& start, const Vector4& end, float t);

Vector4 Slerp(const Vector4& start, const Vector4& end, float t);

Matrix4x4 MakeIdentity4x4();

Matrix4x4 MakeRotateMatrix(const Vector4& quaternion);

Matrix4x4 MakeRotateXMatrix(float radian);

Matrix4x4 MakeRotateYMatrix(float radian);

Matrix4x4 MakeRotateZMatrix(float radian);

Matrix4x4 MakeScaleMatrix(const Vector3& scale);

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

Vector3 Multiply(const Vector3& m1, const float& m2);

Matrix4x4 Transpose(const Matrix4x4& m);

Matrix4x4 Inverse(const Matrix4x4& m);

Vector3 Normalize(const Vector3& v);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector4& rotate, const Vector3& translate);

Matrix4x4 MakePrespectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

bool IsCollision(const AABB& aabb, const Vector3& point);

Vector3 operator*(const Vector3& m1, const float& m2);
Vector3& operator+=(Vector3& lhv, const Vector3& rhv);

Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
