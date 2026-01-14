#pragma once
#include <DxLib.h>

class VECTOR4
{
public:

	float x;
	float y;
	float z;
	float w;

	//	コンストラクタ
	VECTOR4(void);

	//	コンストラクタ
	VECTOR4(float vX, float vY, float vZ, float vW);

	//	デストラクタ
	~VECTOR4(void);

	static VECTOR4 VTransform4(const VECTOR4& v, const MATRIX& m);
};

