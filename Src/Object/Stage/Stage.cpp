#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../Common/CollisionManager.h"
#include "../Common/Collider.h"
#include "../../Utility/AsoUtility.h"
#include "Stage.h"

Stage::Stage()
{
}

Stage::~Stage()
{
}

void Stage::Init(void)
{
	transform_ = std::make_shared<Transform>();


	auto& resIns = resMng_.GetInstance();

	transform_->SetModel(resIns.LoadModelDuplicate(ResourceManager::SRC::MDL_STAGE));



	for (int x = 0; x < 5; x++)
	{
		std::shared_ptr<Transform> rock;
		rock = std::make_shared<Transform>();
		rock->SetModel(resIns.LoadModelDuplicate(ResourceManager::SRC::MDL_ROCK));

		rock_.push_back(rock);
	}



	SetParam();

}

void Stage::Update(void)
{
}

void Stage::Draw(void)
{
	MV1DrawModel(transform_->modelId);

	for (auto& rock : rock_)
	{
		MV1DrawModel(rock->modelId);
	}

}

void Stage::SetParam(void)
{
	transform_->pos = { 0.0f,0.0f,0.0f };
	transform_->quaRot = Quaternion();
	transform_->scl = { 0.25f,1.0f,0.25f };

	transform_->MakeCollider(Collider::Category::STAGE, Collider::TYPE::MODEL);

	transform_->Update();

	int x = 0;

	for (auto& rock : rock_)
	{
		
		float random = static_cast<float>(rand()) / 700.0f;

		if (x == 0)
		{
			rock->pos = { -1400.0f ,-400.0f,1000.0f };
			rock->quaRot = Quaternion();
			rock->quaRot = Quaternion().Mult(rock->quaRot, Quaternion().AngleAxis(AsoUtility::Deg2RadF(90.0f), AsoUtility::AXIS_Z));
			rock->scl = { 0.3f,0.3f,0.3f };
		}
		else if (x <= 1)
		{
			rock->pos = { -800.0f ,-400.0f ,1500.0f };
			rock->quaRot = Quaternion();
			rock->quaRot = Quaternion().Mult(rock->quaRot, Quaternion().AngleAxis(AsoUtility::Deg2RadF(180.0f), AsoUtility::AXIS_Z));

			rock->scl = { 0.5f,0.5f,0.5f };
		}
		else if (x <= 2)
		{
			rock->pos = { 900.0f ,-400.0f ,300.0f };
			rock->quaRot = Quaternion();
			rock->quaRot = Quaternion().Mult(rock->quaRot, Quaternion().AngleAxis(AsoUtility::Deg2RadF(270.0f), AsoUtility::AXIS_Z));

			rock->scl = { 0.3f,0.3f,0.3f };
		}
		else if (x <= 3)
		{
			rock->pos = { -800.0f ,-400.0f ,0.0f };
			rock->quaRot = Quaternion();
			rock->quaRot = Quaternion().Mult(rock->quaRot, Quaternion().AngleAxis(AsoUtility::Deg2RadF(0.0f), AsoUtility::AXIS_Z));

			rock->scl = { 0.5f,0.5f,0.5f };
		}
		else if (x <= 4)
		{
			rock->pos = { 600.0f ,-400.0f ,1500.0f };
			rock->quaRot = Quaternion();
			rock->quaRot = Quaternion().Mult(rock->quaRot, Quaternion().AngleAxis(AsoUtility::Deg2RadF(90.0f), AsoUtility::AXIS_Z));

			rock->scl = { 0.5f,0.5f,0.5f };
		}
		else
		{
			rock->pos = { 0.0f ,-500.0f ,1500.0f };
			rock->quaRot = Quaternion();
			rock->scl = { 0.3f,0.3f,0.3f };
		}


		rock->MakeCollider(Collider::Category::STAGE, Collider::TYPE::MODEL);

		rock->Update();

		x++;
	}
}
