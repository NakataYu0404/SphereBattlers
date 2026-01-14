#include <DxLib.h>
#include "../../Manager/SceneManager.h"
#include "EffectController.h"

EffectController::EffectController()
{
	playType_ = -1;
	isLoop_ = false;

	isStop_ = false;
	endLoopSpeed_ = 0.0f;
	stepEndLoopStart_ = 0.0f;
	stepEndLoopEnd_ = 0.0f;
}

EffectController::~EffectController(void)
{
}

void EffectController::Add(int type, const int ResId)
{

	Effect efc;

	efc.ResId = ResId;

	efc.Type = type;
	
	if (effects_.count(type) == 0)
	{
		// 追加
		effects_.emplace(type, efc);
	}
	else
	{
		// 入れ替え
		effects_[type].ResId = efc.ResId;
		effects_[type].Type = efc.Type;
	}

}

void EffectController::Play(int type, bool isLoop, int startFrame, int endFrame, float speed)
{
	if (effects_.size() <= 0)
	{
		return;
	}

	if (playType_ != type || (playType_ == type && IsEffekseer3DEffectPlaying(effects_[type].PlayId)))
	{
		// 再生＆プレイＩＤを管理
		//int playId = PlayEffekseer3DEffect(effect.resId);
		//effect.playIds.emplace_back(playId);

		// 管理マネージャ取得
		Effekseer::ManagerRef mng = GetEffekseer3DManager();
		
		// エフェクトパラメータークラス取得
		Effekseer::EffectRef ref = GetEffekseerEffect(effects_[type].ResId);
		Effekseer::Vector3D pos3D;

		pos3D.X = 0.0f;
		pos3D.Y = 0.0f;
		pos3D.Z = 0.0f;
		effects_[type].PlayId = mng->Play(ref, pos3D, startFrame);

		effects_[type].startFrame = startFrame;
		effects_[type].nowTime = (float)startFrame;
		effects_[type].endFrame = endFrame;

		playType_ = type;
		effects_[type].IsLoop = isLoop;

		speed_ = speed;
		mng->SetSpeed(effects_[type].PlayId, speed);

		TransUpdate(type);
	}
}

void EffectController::TransUpdate(int type, VECTOR pos, VECTOR scl, VECTOR rot)
{
	
	effects_[type].pos = pos;
	effects_[type].scl = scl;
	effects_[type].rot = rot;

	// 大きさ、回転、座標の設定
	SetScalePlayingEffekseer3DEffect(effects_[type].PlayId, scl.x, scl.y, scl.z);
	SetRotationPlayingEffekseer3DEffect(effects_[type].PlayId, rot.x, rot.y, rot.z);
	SetPosPlayingEffekseer3DEffect(effects_[type].PlayId, pos.x, pos.y, pos.z);
}

void EffectController::Update(void)
{

	for (int i = 0; i < effects_.size(); i++)
	{
		auto efc = effects_[i];	
		
		if (efc.endFrame != -1 && efc.nowTime >= efc.endFrame && !efc.IsLoop)
		{
			// 停止処理
			Stop(efc.Type);
		}
		else if (efc.endFrame != -1 && efc.nowTime >= efc.endFrame && efc.IsLoop)
		{
			//	ループ処理
			Stop(efc.Type);
			Play(efc.Type, efc.IsLoop, efc.startFrame, efc.endFrame,speed_);
			TransUpdate(efc.Type, efc.pos, efc.scl, efc.rot);
		}
		else if (efc.Type == playType_ && efc.IsLoop)
		{
			//	最初の再生処理
			Play(efc.Type, efc.IsLoop, efc.startFrame, efc.endFrame, speed_);
			TransUpdate(efc.Type, efc.pos, efc.scl, efc.rot);
		}
		
		//	エフェクト毎の現在フレーム増加
		effects_[i].nowTime += speed_;
	}
}

void EffectController::Draw(void)
{
	for (int i = 0; i < effects_.size(); i++)
	{
		auto efc = effects_[i];

		if (efc.PlayId != -1)
		{
			DrawEffekseer3D_Begin();
			DrawEffekseer3D_Draw(efc.PlayId);
			DrawEffekseer3D_End();
		}
	}
}

void EffectController::Stop(int type, bool without)
{
	Effekseer::ManagerRef mng = GetEffekseer3DManager();

	if (!without)
	{
		mng->StopEffect(effects_[type].PlayId);

		//StopEffekseer3DEffect(effects_[type].PlayId);
	}
	else
	{
		for (int i = 0; i < effects_.size(); i++)
		{
			if (effects_[i].Type != type)
			{
				mng->StopEffect(effects_[i].PlayId);
				//StopEffekseer3DEffect(effects_[i].PlayId);
			}
		}
	}
}

int EffectController::GetPlayType(void) const
{
	return playType_;
}

bool EffectController::IsEnd(int type)
{
	if (effects_.size() <= 0)
	{
		return false;
	}

	for (int i = 0; i < effects_.size(); i++)
	{
		auto& efc = effects_[i];

		if (efc.Type != type || efc.IsLoop)
		{
			continue;
		}

		if (efc.nowTime >= efc.endFrame)
		{
			return true;
		}
	}
	return false;
}

//	制作者：中田
//	他プロジェクトに持ち出し禁止！
