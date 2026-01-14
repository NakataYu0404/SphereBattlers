#include <memory>
#include <string>
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/Timer.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Object/Common/AnimationController.h"
#include "../Object/Stage/Magma.h"
#include "../Object/Stage/Stage.h"
#include "GameScene.h"

//#include "../Object/Player.h"
//#include "../Object/Player1.h"
//#include "../Object/Player2.h"

#include "../Object/Player/PlayerNakata.h"

#include "../Object/Common/CollisionManager.h"
#include "../Object/Common/Sphere.h"

GameScene::GameScene(void) : colMng_(CollisionManager::GetInstance())
{
	//	プレイヤーを複数実装する場合、Playerのコンストラクタ等でプレイヤー番号を渡せば別クラスを作る必要がありません。機能も同じですし。
	//	プレイヤー毎の初期座標はPlayer1,2でなく結局Playerのコンストラクタで決めている所を見るに完全な冗長であると思います。
	////players_.push_back(make_shared<Player1>());
	////VECTOR initPos = { 300.0f,0.0f,0.0f };
	////players_.push_back(make_shared<Player2>(initPos));
}

GameScene::~GameScene(void)
{
	CollisionManager::GetInstance().ClearCollider();
	players_.clear();

}

void GameScene::Init(void)
{
	colMng_.Init();

	inTypeGame_ = InSceneType::READY;
	for (int i = 0; i < PLAYERNUM_MAX; i++)
	{
		players_.push_back(make_shared<Player>(i));
	}
	for (auto& p : players_)
	{
		p->Init();
		colMng_.Add(p);
	}

	for (auto& p : players_)
	{
		for (auto& a : players_)
		{
			p->SetPlayers(a);
		}
	}

	stage_ = std::make_shared<Stage>();
	stage_->Init();

	colMng_.Add(stage_);

	magma_ = std::make_shared<Magma>();
	magma_->Init();

	Timer::GetInstance().ResetCountDownTimer();

	player1ReadyFlag_ = false;
	player2ReadyFlag_ = false;
	player3ReadyFlag_ = false;
	player4ReadyFlag_ = false;

	imgAlready_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::IMG_ALREADY).handleId_;
	imgReady_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::IMG_READY).handleId_;
	imgWin_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::IMG_WIN).handleId_;
	imgPlayer1_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::IMG_PLAYER1).handleId_;
	imgPlayer2_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::IMG_PLAYER2).handleId_;
	imgPlayer3_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::IMG_PLAYER3).handleId_;
	imgPlayer4_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::IMG_PLAYER4).handleId_;

	sndDecide_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::SND_DECIDE).handleId_;
	sndGameStart_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::SND_GAME_START).handleId_;
	sndGameEnd_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::SND_GAME_END).handleId_;
	sndBGM_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::SND_BGM).handleId_;
}

void GameScene::Update(void)
{
	switch (inTypeGame_)
	{
	case GameScene::InSceneType::READY:
		UpdateReady();
		break;
	case GameScene::InSceneType::INGAME:
		UpdateInGame();
		break;
	case GameScene::InSceneType::GAMEOVER:
		UpdateOver();
		break;
	default:
		break;
	}

}

//void GameScene::Draw(void)
//{
//
//	//	3Dを描画
//	Draw3D();
//
//	//	2Dを描画
//	Draw2D();
//
//	//	UIを描画
//	DrawUI();
//
////	なぜここでCollision関数が呼ばれるのか流石に理解できません
////	Collision();
//}

void GameScene::Draw3D(void)
{
	//	3Dのもの描画
	stage_->Draw();
	magma_->Draw();

	for (auto& p : players_) 
	{
		p->Draw();
	}

}

void GameScene::Draw2D(void)
{
	//	3D上に描画したい2Dのもの描画

}

void GameScene::DrawUI(void)
{
	//	UI描画
	switch (inTypeGame_)
	{
	case GameScene::InSceneType::READY:
		DrawGraph(0, 0, imgReady_, true);
		if (player1ReadyFlag_)
		{
			DrawGraph(80, 287, imgAlready_, true);
		}
		if (player2ReadyFlag_)
		{
			DrawGraph(460, 287, imgAlready_, true);
		}
		if (player3ReadyFlag_)
		{
			DrawGraph(840, 287, imgAlready_, true);
		}
		if (player4ReadyFlag_)
		{
			DrawGraph(1220, 287, imgAlready_, true);
		}

		Timer::GetInstance().CountDownDraw();

		break;
	case GameScene::InSceneType::INGAME:
		Timer::GetInstance().Draw();
		break;
	case GameScene::InSceneType::GAMEOVER:
		
		DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 200, 1.0, 0.0, imgWin_, true);

		switch (winType_)
		{
			//	ここに、勝利Draw
		case GameScene::WinType::PLAYER_1:
			DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 + 150, 1.0, 0.0, imgPlayer1_, true);
			break;
	
		case GameScene::WinType::PLAYER_2:
			DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 + 150, 1.0, 0.0, imgPlayer2_, true);
			break;

		case GameScene::WinType::PLAYER_3:
			DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 + 150, 1.0, 0.0, imgPlayer3_, true);
			break;

		case GameScene::WinType::PLAYER_4:
			DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 + 150, 1.0, 0.0, imgPlayer4_, true);
			break;

		case GameScene::WinType::DRAW:
			break;
		}

		break;
	default:
		break;
	}


}

void GameScene::GoGameOver(WinType type)
{
	winType_ = type;
	inTypeGame_ = InSceneType::GAMEOVER;
}

void GameScene::UpdateReady(void)
{
	auto& input = InputManager::GetInstance();

	if (input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		player1ReadyFlag_ = true;
		PlaySoundMem(sndDecide_, DX_PLAYTYPE_BACK);
	}
	if (input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD2, InputManager::JOYPAD_BTN::DOWN))
	{
		player2ReadyFlag_ = true;
		PlaySoundMem(sndDecide_, DX_PLAYTYPE_BACK);
	}
	if (input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD3, InputManager::JOYPAD_BTN::DOWN))
	{
		player3ReadyFlag_ = true;
		PlaySoundMem(sndDecide_, DX_PLAYTYPE_BACK);
	}
	if (input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD4, InputManager::JOYPAD_BTN::DOWN))
	{
		player4ReadyFlag_ = true;
		PlaySoundMem(sndDecide_, DX_PLAYTYPE_BACK);
	}
	if (input.IsTrgDown(KEY_INPUT_SPACE))
	{
		player1ReadyFlag_ = true;
		player2ReadyFlag_ = true;
		player3ReadyFlag_ = true;
		player4ReadyFlag_ = true;
		PlaySoundMem(sndDecide_, DX_PLAYTYPE_BACK);
	}

	if(
		player1ReadyFlag_ && 
		player2ReadyFlag_ &&
		player3ReadyFlag_ && 
		player4ReadyFlag_
		)
	{
		//	TODO:ここにカウントダウン演出を追加して、カウントダウンが終わったらinTypeGameが変わるようにする
  		if (Timer::GetInstance().CountDown(5.0f))
		{
			inTypeGame_ = InSceneType::INGAME;
			Timer::GetInstance().ResetTimer();
			PlaySoundMem(sndGameStart_, DX_PLAYTYPE_BACK);
			PlaySoundMem(sndBGM_, DX_PLAYTYPE_BACK);
		}
	}
}

void GameScene::UpdateInGame(void)
{
	magma_->Update();

	for (auto& p : players_)
	{
		p->Update();
	}

	colMng_.Update();


	if (Timer::GetInstance().IsTimeOver())
	{
		GoGameOver(WinType::DRAW);
	}
	else
	{
		if (!players_[0]->GetAlive() && !players_[1]->GetAlive() && !players_[2]->GetAlive())
		{
			GoGameOver(WinType::PLAYER_4);
			PlaySoundMem(sndGameEnd_, DX_PLAYTYPE_BACK);
			StopSoundMem(sndBGM_);
		}
		if (!players_[0]->GetAlive() && !players_[2]->GetAlive() && !players_[3]->GetAlive())
		{
			GoGameOver(WinType::PLAYER_2);
			PlaySoundMem(sndGameEnd_, DX_PLAYTYPE_BACK);
			StopSoundMem(sndBGM_);
		}
		if (!players_[1]->GetAlive() && !players_[2]->GetAlive() && !players_[3]->GetAlive())
		{
			GoGameOver(WinType::PLAYER_1);
			PlaySoundMem(sndGameEnd_, DX_PLAYTYPE_BACK);
			StopSoundMem(sndBGM_);
		}
		if (!players_[0]->GetAlive() && !players_[1]->GetAlive() && !players_[3]->GetAlive())
		{
			GoGameOver(WinType::PLAYER_3);
			PlaySoundMem(sndGameEnd_, DX_PLAYTYPE_BACK);
			StopSoundMem(sndBGM_);
		}
	}
}

void GameScene::UpdateOver(void)
{
	auto& input = InputManager::GetInstance();
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE) || input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
}


//void GameScene::Collision()
//{
//	for (auto& p1 : players_) {
//		for (auto& p2 : players_) {
//			if (p1 == p2) continue;
//			// 判定
//			float distance = VSize(VSub(p1->GetTransform().lock()->pos, p2->GetTransform().lock()->pos));
//			float CollDistance = p1->GetSphere().lock()->GetRadius() + p2->GetSphere().lock()->GetRadius();
//			if (distance <= CollDistance) {
//				// 衝突判定
//				VECTOR p1Dir = p1->GetMoveDir();
//				VECTOR p2Dir = p2->GetMoveDir();
//				VECTOR dir = { 0.0f,0.0f,1.0f };
//				//p1->SetMoveDir(dir);
//				// p2->SetMoveDir(p1Dir);
//				// p1->SwitchMoveDir();
//				if (p1->GetMoveAcc() >= p2->GetMoveAcc()) {
//					p1->ProcessKnockBack(VScale(p1Dir, -1), 1.1f);
//				}
//				else{
//					p2->ProcessKnockBack(VScale(p2Dir, -1), 1.1f);
//				}
//				DrawFormatString(0, 64, 0xffffff, "当たった");
//				return;
//			}
//		}
//	}
//}