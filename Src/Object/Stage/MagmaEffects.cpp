#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../Common/EffectController.h"
#include "MagmaEffects.h"

MagmaEffects::MagmaEffects(VECTOR magmaPos)
{
	pos_ = magmaPos;
}

MagmaEffects::~MagmaEffects()
{

}

void MagmaEffects::Init(void)
{
	transform_ = std::make_shared<Transform>();
	effectController_ = std::make_shared<EffectController>();
	
	SetParam();
}

void MagmaEffects::Update(void)
{
	effectController_->Play((int)EFFECT_TYPE::MAGMA_BUBBLE,true);
	effectController_->TransUpdate((int)EFFECT_TYPE::MAGMA_BUBBLE, transform_->pos, {15.0f,15.0f,15.0f});


	//effectController_->TransUpdate((int)EFFECT_TYPE::MAGMA_BUBBLE, transform_->pos);
}

void MagmaEffects::Draw(void)
{

}

void MagmaEffects::SetParam(void)
{
	auto& resIns = resMng_.GetInstance();

	transform_->SetModel(resIns.LoadModelDuplicate(ResourceManager::SRC::MDL_NONE));

	transform_->pos = pos_;
	transform_->quaRot = Quaternion();
	transform_->scl = { 1.0f,1.0f,1.0f };

	transform_->Update();

	effectController_->Add((int)EFFECT_TYPE::MAGMA_JUMP, resIns.Load(ResourceManager::SRC::EFF_MAGMA_JUMP).handleId_);

	effectController_->Add((int)EFFECT_TYPE::MAGMA_BUBBLE, resIns.Load(ResourceManager::SRC::EFF_MAGMA_BUBBLE).handleId_);
}
