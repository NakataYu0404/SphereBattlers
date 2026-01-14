#include "Collider.h"

Collider::Collider(Category cate, TYPE type, int modelId)
{
	category_ = cate;
	type_ = type;
	modelId_ = modelId;
	hitInfo_ = {};
}

Collider::~Collider(void)
{
}
