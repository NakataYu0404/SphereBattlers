#include <DxLib.h>
#include "Transform.h"
#include "Capsule.h"

Capsule::Capsule(std::weak_ptr<Transform> parent)
{
	transformParent_ = parent;
	radius_ = 0.0f;
	localPosTop_ = { 0.0f, 0.0f, 0.0f };
	localPosDown_ = { 0.0f, 0.0f, 0.0f };
}

Capsule::Capsule(const std::weak_ptr<Capsule> base, const std::weak_ptr<Transform> parent)
{
	transformParent_ = parent;
	radius_ = base.lock()->GetRadius();
	localPosTop_ = base.lock()->GetLocalPosTop();
	localPosDown_ = base.lock()->GetLocalPosDown();
}

Capsule::~Capsule(void)
{
}

void Capsule::Draw(void)
{

	//	ã‚Ì‹…‘Ì
	VECTOR pos1 = GetPosTop();
	DrawSphere3D(pos1, radius_, 5, COLOR, COLOR, false);

	//	‰º‚Ì‹…‘Ì
	VECTOR pos2 = GetPosDown();
	DrawSphere3D(pos2, radius_, 5, COLOR, COLOR, false);

	VECTOR dir;
	VECTOR s;
	VECTOR e;

	//	‹…‘Ì‚ðŒq‚®ü(X+)
	dir = transformParent_.lock()->GetRight();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, COLOR);

	//	‹…‘Ì‚ðŒq‚®ü(X-)
	dir = transformParent_.lock()->GetLeft();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, COLOR);

	//	‹…‘Ì‚ðŒq‚®ü(Z+)
	dir = transformParent_.lock()->GetForward();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, COLOR);

	//	‹…‘Ì‚ðŒq‚®ü(Z-)
	dir = transformParent_.lock()->GetBack();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, COLOR);

	//	ƒJƒvƒZƒ‹‚Ì’†S
	DrawSphere3D(GetCenter(), 5.0f, 10, COLOR, COLOR, true);

}

VECTOR Capsule::GetLocalPosTop(void) const
{
	return localPosTop_;
}

VECTOR Capsule::GetLocalPosDown(void) const
{
	return localPosDown_;
}

void Capsule::SetLocalPosTop(const VECTOR& pos)
{
	localPosTop_ = pos;
}

void Capsule::SetLocalPosDown(const VECTOR& pos)
{
	localPosDown_ = pos;
}

VECTOR Capsule::GetPosTop(void) const
{
	return GetRotPos(localPosTop_);
}

VECTOR Capsule::GetPosDown(void) const
{
	return GetRotPos(localPosDown_);
}

VECTOR Capsule::GetRotPos(const VECTOR& localPos) const
{
	VECTOR localRotPos = transformParent_.lock()->quaRot.PosAxis(localPos);
	return VAdd(transformParent_.lock()->pos, localRotPos);
}

float Capsule::GetRadius(void) const
{
	return radius_;
}

void Capsule::SetRadius(float radius)
{
	radius_ = radius;
}

float Capsule::GetHeight(void) const
{
	return localPosTop_.y;
}

VECTOR Capsule::GetCenter(void) const
{
	VECTOR top = GetPosTop();
	VECTOR down = GetPosDown();

	VECTOR diff = VSub(top, down);
	return VAdd(down, VScale(diff, 0.5f));
}
