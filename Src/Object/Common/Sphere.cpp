#include <DxLib.h>
#include "Transform.h"
#include "Sphere.h"

Sphere::Sphere(std::weak_ptr<Transform> parent)
{
	transformParent_ = parent;
	radius_ = 0.0f;
	localPos_ = { 0.0f, 0.0f, 0.0f };
}

Sphere::Sphere(const std::weak_ptr<Sphere> base, const std::weak_ptr<Transform> parent)
{
	transformParent_ = parent;
	radius_ = base.lock()->GetRadius();
	localPos_ = base.lock()->GetLocalPos();
}

Sphere::~Sphere(void)
{
}

void Sphere::Draw(void)
{
	//	カプセルの中心
	DrawSphere3D(GetPos(), radius_, 10, COLOR, COLOR, true);
}

VECTOR Sphere::GetLocalPos(void) const
{
	return localPos_;
}


void Sphere::SetLocalPos(const VECTOR& pos)
{
	localPos_ = pos;
}

VECTOR Sphere::GetPos(void) const
{
	return VAdd(transformParent_.lock()->pos,localPos_);
}

float Sphere::GetRadius(void) const
{
	return radius_;
}

void Sphere::SetRadius(float radius)
{
	radius_ = radius;
}
