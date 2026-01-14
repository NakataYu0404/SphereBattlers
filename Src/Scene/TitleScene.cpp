#include <memory>
#include <string>
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Common/Easing.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Object/Common/AnimationController.h"
#include "../Object/Stage/Stage.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
{
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	//タイトル画像
	titleBackImg_ = LoadGraph((Application::PATH_IMAGE + "TitleBack.png").c_str());
	titleLogoImg_[0] = LoadGraph((Application::PATH_IMAGE + "Title1.png").c_str());
	titleLogoImg_[1] = LoadGraph((Application::PATH_IMAGE + "Title2.png").c_str());
	titleLogoImg_[2] = LoadGraph((Application::PATH_IMAGE + "Title3.png").c_str());

	for (int i = 0; i < TITLE_LOGO_NUM; i++)
	{
		titleLogoPos_[i] = { Application::SCREEN_SIZE_X / 2 , -TITLE_LOGO_SIZE_Y };
		titleLogoShiftTime_[i] = 0;
		titleLogoBackScale_[i] = 1.0f;
	}

	titleLogoPos_[2] = { Application::SCREEN_SIZE_X / 2 , TITLE_LOGO_DEFAULT_POS_Y };
	titleLogoBackScale_[2] = 0.0f;
}

void TitleScene::Update(void)
{
	auto& input = InputManager::GetInstance();

	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE) || input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}

	//タイトルロゴの下部の動き
	if (titleLogoShiftTime_[0] < TITLE_LOGO_UNDER_SHIFT_TIME)
	{
		titleLogoShiftTime_[0]++;
		titleLogoPos_[0].y = SineOut(titleLogoShiftTime_[0]
			, TITLE_LOGO_UNDER_SHIFT_TIME
			, -TITLE_LOGO_SIZE_Y
			, TITLE_LOGO_DEFAULT_POS_Y);
	}
	//タイトルロゴの上部の動き
	if (titleLogoShiftTime_[1] < TITLE_LOGO_TOP_SHIFT_TIME)
	{
		titleLogoShiftTime_[1]++;
		titleLogoPos_[1].y = QuintIn(titleLogoShiftTime_[1]
			, TITLE_LOGO_TOP_SHIFT_TIME
			, -TITLE_LOGO_SIZE_Y
			, TITLE_LOGO_DEFAULT_POS_Y);
	}
	//タイトルロゴ背景の動き(タイトルロゴの上部の動きが終わったら)
	else if (titleLogoShiftTime_[2] < TITLE_LOGO_BACK_SHIFT_TIME)
	{
		titleLogoShiftTime_[2]++;
		titleLogoBackScale_[2] = QuintIn(titleLogoShiftTime_[2]
			, TITLE_LOGO_BACK_SHIFT_TIME
			, 0.0f
			, 1.0f);
	}
}

//void TitleScene::Draw(void)
//{
//}

void TitleScene::Draw3D(void)
{
}

void TitleScene::Draw2D(void)
{
	//タイトル背景
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 1.0, 0.0, titleBackImg_, true);
}

void TitleScene::DrawUI(void)
{
	//フォント作成
	int font;
	font = CreateFontToHandle(NULL, 50, 5);

	//タイトルロゴ
	for (int i = TITLE_LOGO_NUM - 1; i >= 0; i--)
	{
		DrawRotaGraph(titleLogoPos_[i].x, titleLogoPos_[i].y, titleLogoBackScale_[i], 0.0, titleLogoImg_[i], true);
	}

	//push key
	DrawStringToHandle(Application::SCREEN_SIZE_X / 2 - 180
		, Application::SCREEN_SIZE_Y / 2 + 200
		, GetJoypadNum() > 0 ? "Push A Button!!" : "Push Space Key!!"
		, (GetNowCount() / (1000 / 2)) % 2 == 0 ? 0xffffff : 0xbbbbbb
		, font);

	//フォント削除
	DeleteFontToHandle(font);

}
