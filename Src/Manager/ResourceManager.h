#pragma once
#include <map>
#include <string>
#include "Resource.h"

class ResourceManager
{

public:

	//	リソース名
	enum class SRC
	{
		//	画像 IMG_から始めること
		//	例：IMG_TITLE
		IMG_READY,
		IMG_ALREADY,
		IMG_WIN,
		IMG_PLAYER1,
		IMG_PLAYER2,
		IMG_PLAYER3,
		IMG_PLAYER4,

		//	モデル MDL_
		MDL_NONE,
		MDL_STAGE,
		MDL_ROCK,
		MDL_MAGMA,

		MDL_PLAYER_BALL1,
		MDL_PLAYER_BALL2,
		MDL_PLAYER_BALL3,
		MDL_PLAYER_BALL4,

		MDL_PLAYER_RED,
		MDL_PLAYER_GREEN,
		MDL_PLAYER_BLUE,
		MDL_PLAYER_YELLOW,

		//	エフェクト EFF_
		EFF_MAGMA_JUMP,
		EFF_MAGMA_BUBBLE,
		EFF_HITBODY,
		EFF_FIRE,

		//	サウンド SND_
		SND_WIN,
		SND_GAME_START,
		SND_GAME_END,
		SND_DECIDE,
		SND_ROLL_1,
		SND_ROLL_2,
		SND_ROLL_3,
		SND_ROLL_4,
		SND_HIT,
		SND_BGM,


	};

	//	明示的にインステンスを生成する
	static void CreateInstance(void);

	//	静的インスタンスの取得
	static ResourceManager& GetInstance(void);

	//	初期化
	void Init(void);

	//	解放(シーン切替時に一旦解放)
	void Release(void);

	//	リソースの完全破棄
	void Destroy(void);

	//	リソースのロード
	const Resource& Load(SRC src);

	//	リソースの複製ロード(モデル用)
	int LoadModelDuplicate(SRC src);

private:

	//	静的インスタンス
	static ResourceManager* instance_;

	//	リソース管理の対象
	std::map<SRC, Resource*> resourcesMap_;

	//	読み込み済みリソース
	std::map<SRC, Resource&> loadedMap_;

	Resource dummy_;

	//	デフォルトコンストラクタをprivateにして、
	//	外部から生成できない様にする
	ResourceManager(void);
	ResourceManager(const ResourceManager& manager) = default;
	~ResourceManager(void) = default;

	//	内部ロード
	Resource& _Load(SRC src);

};
