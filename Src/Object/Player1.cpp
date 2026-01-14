#include "Player1.h"
#include "../Manager/InputManager.h"
#include "../Utility/AsoUtility.h"

void Player1::InitModel()
{
    //  Playerでも記述されていませんでしたか？
    //  あと、Dataフォルダがあるのに同じ階層に勝手にPlayerDataとかいうわけわからんフォルダを作らないで下さい。邪魔です。Data/Model/Playerに移動させました。
    //  あと、ResourceManagerを使って下さい。
    //  あと、ファイル名にスペースを用いないでください。
	transform_->modelId = MV1LoadModel("PlayerData/Model/X bot.mv1");
	transform_->quaRot = Quaternion();
	transform_->quaRotLocal = Quaternion::Euler({ 0.0f,AsoUtility::Deg2RadF(180.0f),0.0f });
}

const bool& Player1::IsInputMove()
{
	// 仮(パッド対応)
	if (input_.IsNew(KEY_INPUT_W)) return true;
	if (input_.IsNew(KEY_INPUT_A)) return true;
	if (input_.IsNew(KEY_INPUT_S)) return true;
	if (input_.IsNew(KEY_INPUT_D)) return true;
	return false;
}

void Player1::CheckMoveDirection()
{
    VECTOR oldMoveDir = moveDir_;

    if (input_.IsNew(KEY_INPUT_W)) {
        moveDir_ = { 0.0f,0.0f,1.0f };
    }
    if (input_.IsNew(KEY_INPUT_A)) {
        moveDir_ = { -1.0f,0.0f,0.0f };
    }
    if (input_.IsNew(KEY_INPUT_S)) {
        moveDir_ = { 0.0f,0.0f,-1.0f };
    }
    if (input_.IsNew(KEY_INPUT_D)) {
        moveDir_ = { 1.0f,0.0f,0.0f };
    }

    // 移動方向を切り替えたときに角度の数値で減速！
    if (IsInputMove()) {
        // 角度を計算
        float angle = acosf(VDot(VNorm(oldMoveDir), VNorm(moveDir_)));
        float angleFactor = (1.0f - (angle / DX_PI));
        moveAcc_ *= angleFactor;
    }
}
