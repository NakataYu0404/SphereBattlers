#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"
#include"../Common/Vector2.h"
class SceneManager;
class AnimationController;


class TitleScene : public SceneBase
{

public:

	//	コンストラクタ
	TitleScene(void);

	//	デストラクタ
	~TitleScene(void);

	void Init(void) override;
	void Update(void) override;
	//void Draw(void) override;

private:
	//タイトルロゴの大きさ
	static constexpr int TITLE_LOGO_SIZE_X = 800;
	static constexpr int TITLE_LOGO_SIZE_Y = 400;

	//タイトルロゴの合計数
	static constexpr int TITLE_LOGO_NUM = 3;

	//タイトルロゴの基本位置
	static constexpr int TITLE_LOGO_DEFAULT_POS_Y = 300;

	//タイトルロゴの移動時間
	static constexpr float TITLE_LOGO_UNDER_SHIFT_TIME = 60.0f * 2.0f;
	static constexpr float TITLE_LOGO_TOP_SHIFT_TIME = 60.0f * 3.0f;
	static constexpr float TITLE_LOGO_BACK_SHIFT_TIME = 60.0f * 1.0f;

	void Draw3D(void) override;
	void Draw2D(void) override;
	void DrawUI(void) override;

	//タイトル背景
	int titleBackImg_;
	
	//タイトルロゴ
	int titleLogoImg_[TITLE_LOGO_NUM];

	//タイトルロゴの座標
	Vector2 titleLogoPos_[TITLE_LOGO_NUM];

	//タイトルロゴの移動時間
	int titleLogoShiftTime_[TITLE_LOGO_NUM];

	//タイトルロゴの背景の大きさ用
	float titleLogoBackScale_[TITLE_LOGO_NUM];
};
