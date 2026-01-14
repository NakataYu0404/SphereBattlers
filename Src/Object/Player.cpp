#include "Player.h"
#include "../Manager/InputManager.h"
#include "Common/Sphere.h"
#include "../Manager/SceneManager.h"
#include "../Utility/AsoUtility.h"
#include "Common/CollisionManager.h"
#include "../Manager/Camera.h"

VECTOR VECTOR_ZERO = { 0.0f,0.0f,0.0f };

Player::Player() : input_(InputManager::GetInstance())
{
	transform_ = make_shared<Transform>();
	transform_->pos = { 0.0f,0.0f,0.0f };
	Init();
	InitModel();
}

Player::Player(const VECTOR& initPos) : input_(InputManager::GetInstance())
{
	transform_ = make_shared<Transform>();
	transform_->pos = initPos;
	Init();
	InitModel();
}

Player::~Player()
{
}

void Player::Init()
{
	sphere_ = make_shared<Sphere>(transform_);
	sphere_->SetRadius(100.0f);
	state_ = State::Move;
}

void Player::InitModel()
{
	//	TODO:ResourceManagerを無視して勝手に読み込み始めないで下さい。
	transform_->modelId = MV1LoadModel("PlayerData/Model/X bot.mv1");
	transform_->quaRot = Quaternion();
	transform_->quaRotLocal = Quaternion::Euler({ 0.0f,AsoUtility::Deg2RadF(180.0f),0.0f });
}

void Player::Update()
{
	(this->*updateFunc_)();

	CollisionStage();
	transform_->Update();
}

void Player::Draw()
{
	// モデル座標補正
	MV1SetPosition(transform_->modelId, VAdd(transform_->pos, MODEL_CORRECTION_POS));	//	transform->localPosを実装した方が確実に直感的だと思います。

	// モデル表示
	MV1DrawModel(transform_->modelId);

	DebugDraw();
}

void Player::DebugDraw()
{
	DrawSphere3D(transform_->pos, sphere_->GetRadius(), 10, 0xffffff, 0xffffff, false);
	VECTOR StageCenterPos = { 0.0f,0.0f,100.0f };
	float StageRadius = 700.0f;
	DrawSphere3D(StageCenterPos, StageRadius, 10, 0xffffff, 0xffffff, false);
	DrawFormatString(0, 32, 0xffffff, "%f", speed_);
}

const VECTOR& Player::GetMoveDir()
{
	return moveDir_;
}

const float Player::GetMoveAcc()
{
	return moveAcc_;
}

void Player::SetMoveDir(const VECTOR& dir)
{
	moveDir_ = dir;
}

void Player::UpdateMove()
{
	// 移動
	CheckMoveDirection();
	ProcessMove();

	// 回転
	Rotation();
}

void Player::UpdateKnockBack()
{
	// ノックバック
	KnockBack();
}

void Player::UpdateFall()
{
	// 落下
	Gravity();
}

const bool& Player::IsInputMove()
{
	// 仮(パッド対応)
	if (input_.IsNew(KEY_INPUT_W)) return true;
	if (input_.IsNew(KEY_INPUT_A)) return true;
	if (input_.IsNew(KEY_INPUT_S)) return true;
	if (input_.IsNew(KEY_INPUT_D)) return true;
	return false;
}

void Player::CheckMoveDirection()
{
	//	Checkと書いてあるのに何かを確認する関数ではないですね。

	if (input_.IsNew(KEY_INPUT_W)) {
		moveDir_ = { 0.0f,0.0f,1.0f };
	}
	if (input_.IsNew(KEY_INPUT_A)) {
		moveDir_ = { -1.0f,0.0f,0.0f };
		moveAcc_ = 0.0f;
	}
	if (input_.IsNew(KEY_INPUT_S)) {
		moveDir_ = { 0.0f,0.0f,-1.0f };
	}
	if (input_.IsNew(KEY_INPUT_D)) {
		moveDir_ = { 1.0f,0.0f,0.0f };
	}
}

//	全く必要となっていない関数です。
void Player::ProcessMove()
{
	Move(moveDir_, MOVE_SPEED);
}

void Player::Move(const VECTOR& dir, float speed)
{
	//	まず、moveAcc_をX軸とZ軸、それぞれに実装しないと、滑る感じはおそらく表現できません。
	//	この実装だと、右にかかっていたはずの慣性が上キーを押した瞬間上にかかるはずです。

	// 加速処理
	float deltaTime = SceneManager::GetInstance().GetDeltaTime();
	if (IsInputMove()){
		moveAcc_ += speed * deltaTime;
		if (moveAcc_ > MOVE_ACC_MAX) {
			moveAcc_ = MOVE_ACC_MAX;
		}
	}
	transform_->pos = VAdd(transform_->pos, VScale(moveDir_, moveAcc_));

	// 減速処理
	moveAcc_ -= speed * 0.05f * deltaTime;
	if (moveAcc_ < 0.0f) {
		moveAcc_ = 0.0f;
		moveDir_ = { 0.0f,0.0f,0.0f };
	}
}

void Player::ProcessKnockBack(const VECTOR& dir, float pow)
{
	knockBackDir_ = dir;
	KnockBackPow_ = pow;
	updateFunc_ = &Player::UpdateKnockBack;
	state_ = State::KnockBack;
	moveAcc_ = 0.0f;

	// ノックバック完了時間
	knockBackComTime_ = KnockBackPow_ / (KNOCKBACK_SPEED * 0.15);
	knockBackTotalTime_ = 0.0f;
}

void Player::KnockBack()
{
	//	ノックバック中に操作できなくする理由を感じられません。ノックバックなどでつるつる滑る中移動しようというゲーム性なのに…

	float deltaTime = SceneManager::GetInstance().GetDeltaTime();
	knockBackTotalTime_ += deltaTime;

	// 移動
	transform_->pos = VAdd(transform_->pos, VScale(knockBackDir_, KnockBackPow_));

	// 減速
	KnockBackPow_ -= KNOCKBACK_SPEED * 0.15f * deltaTime;

	// 移動切り替え可能処理
	// 経過時間で遷移可能か決める(55パー)
	if (knockBackTotalTime_ >= knockBackComTime_ * 0.55) {
		CheckMoveDirection();
		if (IsInputMove()) {
			updateFunc_ = &Player::UpdateMove;
			state_ = State::Move;
		}
	}

	// 強制終了
	if (KnockBackPow_ < 0.0f) {
		KnockBackPow_ = 0.0f;
		updateFunc_ = &Player::UpdateMove;
		state_ = State::Move;
	}
}

void Player::Gravity()
{
	// if (state_ != State::Fall) return;

	gravityTotalTime_ += SceneManager::GetInstance().GetDeltaTime();
	transform_->pos.y += -0.5 * GRAVITY * gravityTotalTime_ * gravityTotalTime_;
}

void Player::CollisionStage()
{
	//	CollisionManagerを無視してまでこう実装する意義を感じません。Stageの大きさを変更することになった場合、どう対応するおつもりですか？もう実際Stageは真円ではなくなりました。

	// ステージ情報
	VECTOR StageCenterPos = { 0.0f,0.0f,100.0f };
	float StageRadius = 2000.0f;

	// 判定
	float Distance = sqrtf(pow((StageCenterPos.x - transform_->pos.x), 2) + pow((StageCenterPos.z - transform_->pos.z), 2));
	if (StageRadius < Distance) {
		updateFunc_ = &Player::UpdateFall;
		state_ = State::Fall;
	}
}

void Player::Rotation()
{
	if (!IsInputMove()) return;

	Quaternion cameraRot = SceneManager::GetInstance().GetCamera()->GetQuaRotOutX();
	VECTOR cRot = cameraRot.PosAxis(moveDir_);

	float deltaTime = SceneManager::GetInstance().GetDeltaTime();
	float comSec = ROT_COM_SEC / deltaTime;

	Quaternion goalQuaRot = Quaternion::LookRotation(cRot);
	transform_->quaRot = Quaternion::Slerp(transform_->quaRot, goalQuaRot, comSec);
}