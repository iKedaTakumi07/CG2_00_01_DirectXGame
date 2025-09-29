#pragma once
#include "Vector3.h"

class DebugCamera {
public:
private:
    // x,y,z軸周りのローカル
    Vecotr3 rotation_ = { 0, 0, 0 };

    // ローカル座標
    Vecotr3 translation = { 0, 0, -50 };

    // ビュー行列
    // 射影行列
};
