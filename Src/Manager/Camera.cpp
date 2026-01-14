#include <math.h>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Object/Common/Transform.h"
#include "../Object/Common/CollisionManager.h"
#include "Camera.h"

Camera::Camera(void) : colMng_(CollisionManager::GetInstance())
{
	angles_ = VECTOR();
	cameraUp_ = VECTOR();
	mode_ = MODE::NONE;
	pos_ = AsoUtility::VECTOR_ZERO;
	targetPos_ = AsoUtility::VECTOR_ZERO;
	followTransform_ = nullptr;
}

Camera::~Camera(void)
{
}

void Camera::Init(void)
{
	ChangeMode(MODE::FIXED_POINT);
	midRayDestinationPos_ = AsoUtility::VECTOR_ZERO;
}

void Camera::Update(void)
{
}

void Camera::SetBeforeDraw(void)
{

	//	クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);
	
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		SetBeforeDrawFixedPoint();
		break;
	case Camera::MODE::FOLLOW:
		SetBeforeDrawFollow();
		break;
	}

	//	カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		pos_, 
		targetPos_, 
		cameraUp_
	);


	//DrawSphere3D(pos_, 3, 1, 0xff0000, 0xff0000, true);
	//DrawSphere3D(targetPos_, 3, 1, 0xff0000, 0xff0000, true);


	midRayDestinationPos_ = VAdd(pos_, VScale(GetForward(), MIDPOS_STRETCH_POW));

	//	DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();

}

void Camera::Draw(void)
{
}

void Camera::SetFollow(const Transform* follow)
{
	followTransform_ = follow;
}

VECTOR Camera::GetMidRayDestantionPos(void)
{
	return midRayDestinationPos_;
}

VECTOR Camera::GetPos(void) const
{
	return pos_;
}

VECTOR Camera::GetAngles(void) const
{
	return angles_;
}

VECTOR Camera::GetTargetPos(void) const
{
	return targetPos_;
}

Quaternion Camera::GetQuaRot(void) const
{
	return rot_;
}

Quaternion Camera::GetQuaRotOutX(void) const
{
	return rotOutX_;
}

VECTOR Camera::GetForward(void) const
{
	return VNorm(VSub(targetPos_, pos_));
}

void Camera::ChangeMode(MODE mode)
{

	//	カメラの初期設定
	SetDefault();

	//	カメラモードの変更
	mode_ = mode;

	//	変更時の初期化処理
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		pos_ = AsoUtility::VECTOR_ZERO;
		targetPos_ = AsoUtility::VECTOR_ZERO;
		break;
	case Camera::MODE::FOLLOW:
		break;

	}

}

void Camera::SetDefault(void)
{

	//	カメラの初期設定
	pos_ = DEFAULT_CAMERA_POS;

	//	注視点
	targetPos_ = AsoUtility::VECTOR_ZERO;

	//	カメラの上方向
	cameraUp_ = AsoUtility::DIR_U;

	angles_.x = AsoUtility::Deg2RadF(30.0f);
	angles_.y = 0.0f;
	angles_.z = 0.0f;

	rot_ = Quaternion();

}

void Camera::SyncFollow(void)
{

	//	同期先の位置
	VECTOR pos = followTransform_->pos;

	//	重力の方向制御に従う
	//	正面から設定されたY軸分、回転させる
	rotOutX_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);

	//	正面から設定されたX軸分、回転させる
	rot_ = rotOutX_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	VECTOR localPos;

	//	注視点(通常重力でいうところのY値を追従対象と同じにする)
	localPos = rotOutX_.PosAxis(LOCAL_F2T_POS);
	targetPos_ = VAdd(pos, localPos);

	//	カメラ位置
	localPos = rot_.PosAxis(LOCAL_F2C_POS);

	VECTOR tmpPos = VAdd(pos, localPos);


	//	カメラの上方向
	cameraUp_ = AsoUtility::DIR_U;

}

void Camera::ProcessRot(void)
{
	auto& ins = InputManager::GetInstance();
	if (angles_.x <= LIMIT_X_UP_RAD)
	{
		if (ins.IsNew(KEY_INPUT_UP))
		{
			angles_.x += AsoUtility::Deg2RadF(CAMERA_ROT_SPEED);
		}
	}
	if (angles_.x >= LIMIT_X_DW_RAD)
	{
		if (ins.IsNew(KEY_INPUT_DOWN))
		{
			angles_.x -= AsoUtility::Deg2RadF(CAMERA_ROT_SPEED);
		}
	}

	if (ins.IsNew(KEY_INPUT_LEFT))
	{
		angles_.y -= AsoUtility::Deg2RadF(CAMERA_ROT_SPEED);
	}
	if (ins.IsNew(KEY_INPUT_RIGHT))
	{
		angles_.y += AsoUtility::Deg2RadF(CAMERA_ROT_SPEED);
	}

}


void Camera::SetBeforeDrawFixedPoint(void)
{
	auto& ins = InputManager::GetInstance();

	/*if (ins.IsNew(KEY_INPUT_UP))
	{
		pos_.y += 1.0f;
	}
	if (ins.IsNew(KEY_INPUT_DOWN))
	{
		pos_.y -= 1.0f;
	}

	if (ins.IsNew(KEY_INPUT_LEFT))
	{
		pos_.z -= 1.0f;
	}
	if (ins.IsNew(KEY_INPUT_RIGHT))
	{
		pos_.z += 1.0f;
	}
	pos_ = { 0.0f,pos_.y,pos_.z };*/

	pos_ = { 0.0f,400.0f,-500.0f };
	targetPos_ = { 0.0f,0.0f,-50.0f };

	//	何もしない
}

void Camera::SetBeforeDrawFollow(void)
{
	//	カメラ操作
	ProcessRot();

	//	追従対象との相対位置を同期
	SyncFollow();
}
