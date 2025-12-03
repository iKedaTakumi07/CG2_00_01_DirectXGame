#include "Math.h"

Matrix4x4 MakeIdentity4x4()
{
    Matrix4x4 num;
    num = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
    return num;
}
