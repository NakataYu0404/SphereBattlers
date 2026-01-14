#include <DxLib.h>
#include "../Application.h"
#include "Resource.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

void ResourceManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}
	instance_->Init();
	
}

ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

void ResourceManager::Init(void)
{

	using RES = Resource;
	using RES_T = RES::TYPE;
	static std::string PATH_IMG = Application::PATH_IMAGE;
	static std::string PATH_MDL = Application::PATH_MODEL;
	static std::string PATH_EFF = Application::PATH_EFFECT;
	static std::string PATH_SND = Application::PATH_SOUND;
	static std::string PATH_SHA = Application::PATH_SHADER;

	Resource* res;
	
	////res = new RES(RES_T::, PATH_IMG + "");
	////resourcesMap_.emplace(SRC::, res);

	//	画像
	res = new RES(RES_T::IMG, PATH_IMG + "magumasetumei.png");
	resourcesMap_.emplace(SRC::IMG_READY, res);
	res = new RES(RES_T::IMG, PATH_IMG + "ok.png");
	resourcesMap_.emplace(SRC::IMG_ALREADY, res);
	res = new RES(RES_T::IMG, PATH_IMG + "Win.png");
	resourcesMap_.emplace(SRC::IMG_WIN, res);
	res = new RES(RES_T::IMG, PATH_IMG + "Player1.png");
	resourcesMap_.emplace(SRC::IMG_PLAYER1, res);
	res = new RES(RES_T::IMG, PATH_IMG + "Player2.png");
	resourcesMap_.emplace(SRC::IMG_PLAYER2, res);
	res = new RES(RES_T::IMG, PATH_IMG + "Player3.png");
	resourcesMap_.emplace(SRC::IMG_PLAYER3, res);
	res = new RES(RES_T::IMG, PATH_IMG + "Player4.png");
	resourcesMap_.emplace(SRC::IMG_PLAYER4, res);

	//	モデル
	res = new RES(RES_T::MODEL, PATH_MDL + "-1");
	resourcesMap_.emplace(SRC::MDL_NONE, res);

	res = new RES(RES_T::MODEL, PATH_MDL + "Stage/stage2.mv1");
	resourcesMap_.emplace(SRC::MDL_STAGE, res);

	res = new RES(RES_T::MODEL, PATH_MDL + "Stage/rock.mv1");
	resourcesMap_.emplace(SRC::MDL_ROCK, res);

	res = new RES(RES_T::MODEL, PATH_MDL + "Stage/Magma/magma.mv1");
	resourcesMap_.emplace(SRC::MDL_MAGMA, res);

	res = new RES(RES_T::MODEL, PATH_MDL + "Player/ball.mv1");
	resourcesMap_.emplace(SRC::MDL_PLAYER_BALL1, res);

	res = new RES(RES_T::MODEL, PATH_MDL + "Player/ball2.mv1");
	resourcesMap_.emplace(SRC::MDL_PLAYER_BALL2, res);
	
	res = new RES(RES_T::MODEL, PATH_MDL + "Player/ball3.mv1");
	resourcesMap_.emplace(SRC::MDL_PLAYER_BALL3, res);
	
	res = new RES(RES_T::MODEL, PATH_MDL + "Player/ball4.mv1");
	resourcesMap_.emplace(SRC::MDL_PLAYER_BALL4, res);

	res = new RES(RES_T::MODEL, PATH_MDL + "Player/aka.mv1");
	resourcesMap_.emplace(SRC::MDL_PLAYER_RED, res);
	res = new RES(RES_T::MODEL, PATH_MDL + "Player/midori.mv1");
	resourcesMap_.emplace(SRC::MDL_PLAYER_GREEN, res);
	res = new RES(RES_T::MODEL, PATH_MDL + "Player/ao.mv1");
	resourcesMap_.emplace(SRC::MDL_PLAYER_BLUE, res);
	res = new RES(RES_T::MODEL, PATH_MDL + "Player/kiiro.mv1");
	resourcesMap_.emplace(SRC::MDL_PLAYER_YELLOW, res);

	//	エフェクト
	res = new RES(RES_T::EFFEKSEER, PATH_EFF + "Magma_Suekane/Magma01_Suekane.efkefc");
	resourcesMap_.emplace(SRC::EFF_MAGMA_JUMP, res);

	res = new RES(RES_T::EFFEKSEER, PATH_EFF + "MagmaBubble_Suekane/MagmaBubble_Suekane.efkefc");
	resourcesMap_.emplace(SRC::EFF_MAGMA_BUBBLE, res);

	res = new RES(RES_T::EFFEKSEER, PATH_EFF + "HitEffect/hit.efkefc");
	resourcesMap_.emplace(SRC::EFF_HITBODY, res);

	res = new RES(RES_T::EFFEKSEER, PATH_EFF + "Fire/MagicFire1.efkproj");
	resourcesMap_.emplace(SRC::EFF_FIRE, res);


	//	サウンド
	res = new RES(RES_T::SOUND, PATH_SND + "Win.mp3");
	resourcesMap_.emplace(SRC::SND_WIN, res);
	res = new RES(RES_T::SOUND, PATH_SND + "GameStart.mp3");
	resourcesMap_.emplace(SRC::SND_GAME_START, res);
	res = new RES(RES_T::SOUND, PATH_SND + "GameEnd.mp3");
	resourcesMap_.emplace(SRC::SND_GAME_END, res);
	res = new RES(RES_T::SOUND, PATH_SND + "DecideButton.mp3");
	resourcesMap_.emplace(SRC::SND_DECIDE, res);

	res = new RES(RES_T::SOUND, PATH_SND + "korogaru1.mp3");
	resourcesMap_.emplace(SRC::SND_ROLL_1, res);
	res = new RES(RES_T::SOUND, PATH_SND + "korogaru2.mp3");
	resourcesMap_.emplace(SRC::SND_ROLL_2, res);
	res = new RES(RES_T::SOUND, PATH_SND + "korogaru3.mp3");
	resourcesMap_.emplace(SRC::SND_ROLL_3, res);
	res = new RES(RES_T::SOUND, PATH_SND + "korogaru4.mp3");
	resourcesMap_.emplace(SRC::SND_ROLL_4, res);
	res = new RES(RES_T::SOUND, PATH_SND + "Hit.mp3");
	resourcesMap_.emplace(SRC::SND_HIT, res);
	res = new RES(RES_T::SOUND, PATH_SND + "BGM.mp3");
	resourcesMap_.emplace(SRC::SND_BGM, res);

	//	シェーダー



}

void ResourceManager::Release(void)
{
	for (auto& p : loadedMap_)
	{
		p.second.Release();
	}

	loadedMap_.clear();
}

void ResourceManager::Destroy(void)
{
	Release();
	for (auto& res : resourcesMap_)
	{
		res.second->Release();
		delete res.second;
	}
	resourcesMap_.clear();
	delete instance_;
}

const Resource& ResourceManager::Load(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return dummy_;
	}
	return res;
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return -1;
	}

	int duId = MV1DuplicateModel(res.handleId_);
	res.duplicateModelIds_.push_back(duId);

	return duId;
}

ResourceManager::ResourceManager(void)
{
}

Resource& ResourceManager::_Load(SRC src)
{

	//	ロード済みチェック
	const auto& lPair = loadedMap_.find(src);
	if (lPair != loadedMap_.end())
	{
		return *resourcesMap_.find(src)->second;
	}

	//	リソース登録チェック
	const auto& rPair = resourcesMap_.find(src);
	if (rPair == resourcesMap_.end())
	{
		//	登録されていない
		return dummy_;
	}

	//	ロード処理
	rPair->second->Load();

	//	念のためコピーコンストラクタ
	loadedMap_.emplace(src, *rPair->second);

	return *rPair->second;

}
