#pragma once
#include <memory>
#include <vector>
#include "../Object/Common/Transform.h"
#include "SceneBase.h"

class SceneManager;
class CollisionManager;
class AnimationController;

class Stage;
class Magma;

class Player;

using namespace std;

class GameScene : public SceneBase
{

public:

	//	ゲームシーン内のシーン
	enum class InSceneType
	{
		//	ゲーム開始前シーン
		READY,

		//	ゲーム中シーン
		INGAME,

		//	ゲーム終了演出シーン
		GAMEOVER,
	};

	enum class WinType
	{
		PLAYER_1,
		PLAYER_2,
		PLAYER_3,
		PLAYER_4,
		DRAW,
	};

	static constexpr int PLAYERNUM_MAX = 4;

	//	コンストラクタ
	GameScene(void);

	//	デストラクタ
	~GameScene(void);

	void Init(void) override;
	void Update(void) override;
	//void Draw(void) override;

private:

	void UpdateReady(void);
	void UpdateInGame(void);
	void UpdateOver(void);

	void Draw3D(void) override;
	void Draw2D(void) override;
	void DrawUI(void) override;

	void GoGameOver(WinType type);

	//	今、ゲームシーン内のどこか
	InSceneType inTypeGame_;

	//	誰が勝ったのか
	WinType winType_;

	std::shared_ptr<Stage> stage_;
	std::shared_ptr<Magma> magma_;

	std::vector<shared_ptr<Player>> players_;

	//	シングルトン参照
	CollisionManager& colMng_;

	//プレイヤーの準備フラグ
	bool player1ReadyFlag_;
	bool player2ReadyFlag_;
	bool player3ReadyFlag_;
	bool player4ReadyFlag_;

	int imgAlready_;
	int imgReady_;
	int imgWin_;
	int imgPlayer1_;
	int imgPlayer2_;
	int imgPlayer3_;
	int imgPlayer4_;

	int sndDecide_;
	int sndGameStart_;
	int sndGameEnd_;
	int sndBGM_;

	////	折角用意したCollisionManagerを無視してゲームシーンに書かないで下さい。
	//	void Collision();
};
