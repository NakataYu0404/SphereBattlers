#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "Manager/InputManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/Timer.h"
#include "Manager/SceneManager.h"
#include "Object/Common/CollisionManager.h"
#include "Object/Stage/Stage.h"
#include "Application.h"

Application* Application::instance_ = nullptr;

const std::string Application::PATH_IMAGE = "Data/Image/";
const std::string Application::PATH_MODEL = "Data/Model/";
const std::string Application::PATH_EFFECT = "Data/Effect/";
const std::string Application::PATH_SOUND = "Data/Sound/";
const std::string Application::PATH_SHADER = "Data/Shader/";

void Application::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Application();
	}
	instance_->Init();
}

Application& Application::GetInstance(void)
{
	return *instance_;
}

void Application::Init(void)
{
	/////aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

	//	アプリケーションの初期設定
	SetWindowText("Contest");

	//	ウィンドウサイズ
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);
	ChangeWindowMode(true);

	//	DxLibの初期化
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	isInitFail_ = false;
	if (DxLib_Init() == -1)
	{
		isInitFail_ = true;
		return;
	}

	//	Effekseerの初期化
	InitEffekseer();

	//	キー制御初期化
	SetUseDirectInputFlag(true);
	InputManager::CreateInstance();

	//	リソース管理初期化
	ResourceManager::CreateInstance();

	CollisionManager::CreateInstance();

	//タイマー管理初期化
	Timer::CreateInstance();

	//	シーン管理初期化
	SceneManager::CreateInstance();

	std::random_device rd;
	gen_ = std::mt19937(rd());

}

void Application::Run(void)
{

	auto& inputManager = InputManager::GetInstance();
	auto& sceneManager = SceneManager::GetInstance();
	auto& timer = Timer::GetInstance();

	//	ゲームループ
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{

		inputManager.Update();
		timer.Update();
		sceneManager.Update();

		sceneManager.Draw();

		ScreenFlip();

	}

}

void Application::Destroy(void)
{
	InputManager::GetInstance().Destroy();
	ResourceManager::GetInstance().Destroy();
	SceneManager::GetInstance().Destroy();
	Timer::GetInstance().Destroy();
	CollisionManager::GetInstance().Destroy();

	//	Effekseerを終了する。
	Effkseer_End();

	//	DxLib終了
	if (DxLib_End() == -1)
	{
		isReleaseFail_ = true;
	}

	delete instance_;

}

bool Application::IsInitFail(void) const
{
	return isInitFail_;
}

bool Application::IsReleaseFail(void) const
{
	return isReleaseFail_;
}

int Application::GetRandomNum(int max)
{
	std::uniform_int_distribution<> distr(0, max);

	return distr(gen_);
}

Application::Application(void)
{
	isInitFail_ = false;
	isReleaseFail_ = false;
}

void Application::InitEffekseer(void)
{
	if (Effekseer_Init(8000) == -1)
	{
		DxLib_End();
	}

	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
}
