#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <DxLib.h>
#include "Utility.h"

int Utility::Round(float v)
{
	return static_cast<int>(roundf(v));
}

float Utility::ScaleDownTen(float i)
{
	return i / 10.0f;
}

float Utility::ScaleDownHundled(float i)
{
	return i / 100.0f;
}

int Utility::ScaleDownTen(int i)
{
	return i / 10;
}

int Utility::ScaleDownHundled(int i)
{
	return i / 100;
}

int Utility::Minint(int a, int b)
{
	if (a <= b) return a;
	return b;
}

int Utility::Maxint(int a, int b)
{
	if (a >= b) return a;
	return b;
}

float Utility::Minfloat(float a, float b)
{
	if (a <= b) return a;
	return b;
}

float Utility::Maxfloat(float a, float b)
{
	if (a >= b) return a;
	return b;
}

std::vector<std::string> Utility::Split(std::string& line, char delimiter)
{

	std::istringstream stream(line);
	std::string field;
	std::vector<std::string> result;
	
	while (getline(stream, field, delimiter)) {
		result.push_back(field);
	}

	return result;

}

float Utility::Rad2DegD(float rad)
{
	return rad * (180.0 / DX_PI);
}

float Utility::Rad2DegF(float rad)
{
	return rad * (180.0f / DX_PI_F);
}

int Utility::Rad2DegI(int rad)
{
	return rad * Round(180.0f / DX_PI_F);
}

float Utility::Deg2RadD(float deg)
{
	return deg * (DX_PI / 180.0);
}

float Utility::Deg2RadF(float deg)
{
	return deg * (DX_PI_F / 180.0f);
}

int Utility::Deg2RadI(int deg)
{
	return deg * Round(DX_PI_F / 180.0f);
}

float Utility::DegIn360(float deg)
{
	deg = fmod(deg, 360.0);
	if (deg < 0.0f)
	{
		deg += 360.0;
	}
	return deg;
}

float Utility::RadIn2PI(float rad)
{
	rad = fmod(rad, (float)DX_TWO_PI);
	if (rad < 0.0)
	{
		rad += (float)DX_TWO_PI;
	}
	return rad;
}

int Utility::DirNearAroundRad(float from, float to)
{

	float ret = 1.0f;

	float diff = to - from;

	if (diff >= 0.0f)
	{

		//	比較元よりも時計回りに位置する

		if (diff > DX_PI_F)
		{
			//	でも、180度以上離れているので、反時計回りの方が近い
			ret = -1.0f;
		}
		else
		{
			//	時計回り
			ret = 1.0f;
		}

	}
	else
	{

		//	比較元よりも反時計回りに位置する

		if (diff < -DX_PI_F)
		{
			//	でも、180度以上離れているので、時計回りの方が近い
			ret = 1.0f;
		}
		else
		{
			//	反時計回り
			ret = -1.0f;
		}

	}

	return static_cast<int>(ret);

}

int Utility::DirNearAroundDeg(float from, float to)
{

	float ret = 1.0f;

	float diff = to - from;

	if (diff >= 0.0f)
	{

		//	比較元よりも時計回りに位置する

		if (diff > 180.0f)
		{
			//	でも、180度以上離れているので、反時計回りの方が近い
			ret = -1.0f;
		}
		else
		{
			//	時計回り
			ret = 1.0f;
		}

	}
	else
	{

		//	比較元よりも反時計回りに位置する

		if (diff < -180.0f)
		{
			//	でも、180度以上離れているので、時計回りの方が近い
			ret = 1.0f;
		}
		else
		{
			//	反時計回り
			ret = -1.0f;
		}

	}

	return static_cast<int>(ret);

}

int Utility::Lerp(int start, int end, float t)
{
	//	線形補間
	if (t >= 1.0f)
	{
		return end;
	}

	int ret = start;
	ret += Round(t * static_cast<float>(end - start));
	return ret;
}

double Utility::Lerp(double start, double end, double t)
{
	//	線形補間
	if (t >= 1.0f)
	{
		return end;
	}

	double ret = start;
	ret += t * (end - start);
	return ret;
}

float Utility::Lerp(float start, float end, float t)
{
	//	線形補間
	if (t >= 1.0)
	{
		return end;
	}

	float ret = start;
	ret += t * (end - start);
	return ret;
}

Vector2 Utility::Lerp(const Vector2& start, const Vector2& end, float t)
{
	//	線形補間
	if (t >= 1.0f)
	{
		return end;
	}

	Vector2 ret = start;
	ret.x += Round(t * static_cast<float>((end.x - start.x)));
	ret.y += Round(t * static_cast<float>((end.y - start.y)));
	return ret;
}

VECTOR Utility::Lerp(const VECTOR& start, const VECTOR& end, float t)
{
	//	線形補間
	if (t >= 1.0f)
	{
		return end;
	}

	VECTOR ret = start;
	ret.x += t * (end.x - start.x);
	ret.y += t * (end.y - start.y);
	ret.z += t * (end.z - start.z);

	return ret;
}

float Utility::LerpDeg(float start, float end, float t)
{

	float ret;

	float diff = end - start;
	if (diff < -180.0)
	{
		end += 360.0;
		ret = Lerp(start, end, t);
		if (ret >= 360.0)
		{
			ret -= 360.0;
		}
	}
	else if (diff > 180.0)
	{
		end -= 360.0;
		ret = Lerp(start, end, t);
		if (ret < 0.0)
		{
			ret += 360.0;
		}
	}
	else
	{
		ret = Lerp(start, end, t);
	}

	return ret;

}

COLOR_F Utility::Lerp(const COLOR_F& start, const COLOR_F& end, float t)
{
	//	線形補間
	if (t >= 1.0f)
	{
		return end;
	}

	COLOR_F ret = start;
	ret.r += t * (end.r - start.r);
	ret.g += t * (end.g - start.g);
	ret.b += t * (end.b - start.b);
	ret.a += t * (end.a - start.a);
	return ret;
}

Vector2 Utility::Bezier(const Vector2& p1, const Vector2& p2, const Vector2& p3, float t)
{
	Vector2 a = Lerp(p1, p2, t);
	Vector2 b = Lerp(p2, p3, t);
	return Lerp(a, b, t);
}

VECTOR Utility::Bezier(const VECTOR& p1, const VECTOR& p2, const VECTOR& p3, float t)
{
	VECTOR a = Lerp(p1, p2, t);
	VECTOR b = Lerp(p2, p3, t);
	return Lerp(a, b, t);
}

VECTOR Utility::RotXZPos(const VECTOR& centerPos, const VECTOR& radiusPos, float rad)
{
	float x = ((radiusPos.x - centerPos.x) * cosf(rad)) - ((radiusPos.z - centerPos.z) * sinf(rad));
	float z = ((radiusPos.x - centerPos.x) * sinf(rad)) + ((radiusPos.z - centerPos.z) * cosf(rad));
	return VGet(centerPos.x + x, radiusPos.y, centerPos.z + z);
}

float Utility::Magnitude(const Vector2& v)
{
	return sqrt((v.x * v.x) + (v.y * v.y));
}

float Utility::Magnitude(const VECTOR& v)
{
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

float Utility::MagnitudeF(const VECTOR& v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

int Utility::SqrMagnitude(const Vector2& v)
{
	return v.x * v.x + v.y * v.y;
}

float Utility::SqrMagnitudeF(const VECTOR& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

float Utility::SqrMagnitude(const VECTOR& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

float Utility::SqrMagnitude(const VECTOR& v1, const VECTOR& v2)
{
	return pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2) + pow(v2.z - v1.z, 2);
}

float Utility::Distance(const Vector2& v1, const Vector2& v2, bool doFabsf)
{
	float ret = sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2));
	if (doFabsf)
	{
		ret = fabsf(ret);
	}
	return ret;
}

float Utility::Distance(const VECTOR& v1, const VECTOR& v2, bool doFabsf)
{
	float ret = sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2) + pow(v2.z - v1.z, 2));
	if (doFabsf)
	{
		ret = fabsf(ret);
	}
	return ret;

}

VECTOR Utility::DistanceV(const VECTOR& v1, const VECTOR& v2, bool doFabsf)
{
	VECTOR ret;
	ret.x = v2.x - v1.x;
	ret.y = v2.y - v1.y;
	ret.z = v2.z - v1.z;
	if (doFabsf)
	{
		ret.x = fabsf(ret.x);
		ret.y = fabsf(ret.y);
		ret.z = fabsf(ret.z);
	}
	return ret;

}

bool Utility::IsHitSpheres(const VECTOR& pos1, float radius1, const VECTOR& pos2, float radius2)
{
	//	球体同士の衝突判定
	bool ret = false;

	//	お互いの半径の合計
	float radius = radius1 + radius2;

	//	座標の差からお互いの距離を取る
	VECTOR diff = VSub(pos2, pos1);

	//	三平方の定理で比較(SqrMagnitudeと同じ)
	float dis = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);
	if (dis < (radius * radius))
	{
		ret = true;
	}

	return ret;
}

bool Utility::IsHitSphereCapsule(
	const VECTOR& sphPos, float sphRadius, 
	const VECTOR& capPos1, const VECTOR& capPos2, float capRadius)
{

	bool ret = false;

	//	カプセル球体の中心を繋ぐベクトル
	VECTOR cap1to2 = VSub(capPos2, capPos1);

	//	ベクトルを正規化
	VECTOR cap1to2ENor = VNorm(cap1to2);

	//	カプセル繋ぎの単位ベクトルと、
	//	そのベクトル元から球体へのベクトルの内積を取る
	float dot = VDot(cap1to2ENor, VSub(sphPos, capPos1));

	//	内積で求めた射影距離を使って、カプセル繋ぎ上の座標を取る
	VECTOR capRidePos = VAdd(capPos1, VScale(cap1to2ENor, dot));

	//	カプセル繋ぎのベクトルの長さを取る
	float len = Utility::MagnitudeF(cap1to2);

	//	球体がカプセル繋ぎ上にいるか判別するため、比率を取る
	float rate = dot / len;

	VECTOR centerPos;

	//	球体の位置が３エリアに分割されたカプセル形状のどこにいるか判別
	if (rate > 0.0f && rate <= 1.0f)
	{
		//	①球体がカプセル繋ぎ上にいる
		centerPos = VAdd(capPos1, VScale(cap1to2ENor, dot));
	}
	else if (rate > 1.0f)
	{
		//	②球体がカプセルの終点側にいる
		centerPos = capPos2;
	}
	else if (rate < 0.0f)
	{
		//	③球体がカプセルの始点側にいる
		centerPos = capPos1;
	}
	else
	{
		//	ここにきてはいけない
	}

	//	球体同士の当たり判定
	if (Utility::IsHitSpheres(centerPos, capRadius, sphPos, sphRadius))
	{
		ret = true;
	}
	else
	{
		ret = false;
	}

	return ret;

}

bool Utility::Equals(const VECTOR& v1, const VECTOR& v2)
{
	if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)
	{
		return true;
	}
	return false;
}

bool Utility::EqualsVZero(const VECTOR& v1)
{
	const VECTOR& v2 = VECTOR_ZERO;
	if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)
	{
		return true;
	}
	return false;
}

VECTOR Utility::Normalize(const Vector2& v)
{
	VECTOR ret = VGet(
		static_cast<float>(v.x),
		static_cast<float>(v.y),
		0.0f
	);
	float len = static_cast<float>(Magnitude(v));
	ret.x /= len;
	ret.y /= len;
	ret.z /= len;
	return ret;
}

VECTOR Utility::VNormalize(const VECTOR& v)
{
	if (Utility::EqualsVZero(v))
	{
		//	Quaternion計算でゼロを渡して、
		//	エラー(-1, -1, -1)が返ってくると困る
		return v;
	}
	return VNorm(v);
}

VECTOR Utility::VDiv(const VECTOR& v, float i)
{
	return { v.x / i,v.y / i, v.z / i };
}

VECTOR Utility::VDiv(const VECTOR& v, const VECTOR v2)
{
	return { v.x / v2.x,v.y / v2.y, v.z / v2.z };
}

float Utility::AngleDeg(const VECTOR& from, const VECTOR& to)
{
	//	sqrt(a) * sqrt(b) = sqrt(a * b) -- valid for real numbers
	auto fLen = SqrMagnitude(from);
	auto tLen = SqrMagnitude(to);
	auto denominator = sqrt(fLen * tLen);
	if (denominator < kEpsilonNormalSqrt)
	{
		return 0.0f;
	}

	//	float dot = std::clamp(Dot(from, to) / denominator, -1.0f, 1.0f);
	//	auto dot = Dot(from, to) / denominator;
	auto dot = VDot(from, to) / denominator;
	if (dot < -1.0f)
	{
		dot = -1.0f;
	}
	if (dot > 1.0f)
	{
		dot = 1.0f;
	}

	return acos(dot) * (180.0 / DX_PI);
}

void Utility::DrawLineDir(const VECTOR& pos, const VECTOR& dir, int color, float len)
{
	auto nDir = Utility::VNormalize(dir);
	auto sPos = VAdd(pos, VScale(nDir, -len));
	auto ePos = VAdd(pos, VScale(nDir, len));
	DrawLine3D(sPos, ePos, color);
	DrawSphere3D(ePos, 5.0f, 5, color, color, true);
}

void Utility::DrawLineXYZ(const VECTOR& pos, const MATRIX& rot, float len)
{

	VECTOR dir;

	//	X
	dir = VTransform(Utility::DIR_R, rot);
	DrawLineDir(pos, dir, 0xff0000, len);

	//	Y
	dir = VTransform(Utility::DIR_U, rot);
	DrawLineDir(pos, dir, 0x00ff00, len);

	//	Z
	dir = VTransform(Utility::DIR_F, rot);
	DrawLineDir(pos, dir, 0x0000ff, len);

}

void Utility::DrawLineXYZ(const VECTOR& pos, const Quaternion& rot, float len)
{

	VECTOR dir;

	//	X
	dir = rot.GetRight();
	DrawLineDir(pos, dir, 0xff0000, len);

	//	Y
	dir = rot.GetUp();
	DrawLineDir(pos, dir, 0x00ff00, len);

	//	Z
	dir = rot.GetForward();
	DrawLineDir(pos, dir, 0x0000ff, len);

}


