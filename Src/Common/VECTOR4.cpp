#include "VECTOR4.h"

//	コンストラクタ
VECTOR4::VECTOR4(void)
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
}

//	コンストラクタ
VECTOR4::VECTOR4(float vX, float vY, float vZ, float vW)
{
	x = vX;
	y = vY;
	z = vZ;
	w = vW;
}

VECTOR4::~VECTOR4(void)
{
}

VECTOR4 VECTOR4::VTransform4(const VECTOR4& v, const MATRIX& m)
{
	VECTOR4 result;
	result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0];
	result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1];
	result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2];
	result.w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3];
	return result;
}
