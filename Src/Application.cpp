#include <DxLib.h>
#include "Manager/InputManager.h"
#include "Manager/ResourceManager.h"
#include "../Tool/HitboxTool.h"
#include "Application.h"

Application* Application::instance_ = nullptr;

const std::string Application::PATH_IMAGE = "Data/Image/";
const std::string Application::PATH_MODEL = "Data/Model/";
const std::string Application::PATH_SHADER = "Data/Shader/";
const std::string Application::PATH_SOUND = "Data/Sound/";
const std::string Application::PATH_JSON = "Data/JSON/";

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

	SetWindowText("HitboxTool");

	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, INIT_COLORBIT);
	ChangeWindowMode(true);

	SetUseDirect3DVersion(DX_DIRECT3D_11);
	isInitFail_ = false;
	if (DxLib_Init() == -1)
	{
		isInitFail_ = true;
		return;
	}

	//AddMenuItem_Name(NULL, "MENU");
	//AddMenuItem_Name("MENU", "QUIT");
	//SetUseMenuFlag(TRUE);

	SetUseDirectInputFlag(true);
	InputManager::CreateInstance();
	ResourceManager::CreateInstance();

	HitboxTool::CreateInstance();

	//SceneManager::CreateInstance();

	std::random_device rd;
	gen_ = std::mt19937(rd());

}

void Application::Run(void)
{
	auto& inputManager = InputManager::GetInstance();
	//auto& sceneManager = SceneManager::GetInstance();
	auto& hitboxTool = HitboxTool::GetInstance();

	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
	auto start = std::chrono::high_resolution_clock::now();

		inputManager.Update(false);

		hitboxTool.Update();

		ClsDrawScreen();
		hitboxTool.Draw();
		ScreenFlip();
		//sceneManager.Update();

		//sceneManager.Draw();
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		std::cout << "Frame time: " << duration.count() << " ms\n";
	}
}

void Application::Destroy(void)
{
	ResourceManager::GetInstance().Destroy();
	//SceneManager::GetInstance().Destroy();
	//CollisionManager::GetInstance().Destroy();
	InputManager::GetInstance().Destroy();
	//UIManager::GetInstance().Destroy();

	//Effkseer_End();

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
	: isInitFail_(false), isReleaseFail_(false)
{
}
