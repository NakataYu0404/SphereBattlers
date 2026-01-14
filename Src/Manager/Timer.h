#pragma once

#include<DxLib.h>
#include"../Common/Vector2.h"

class Timer
{
public:

	//定数
	//--------------------

	//カウンター初期化
	static constexpr DATEDATA COUNTER_ZERO = { 0,0,0,0,0,0 };

	//制限時間
	static constexpr DATEDATA TIME_LIMIT = { 0,0,0,0,0,60 };

	//一秒のミリ秒変換
	static constexpr int SEC_2_MILLISEC = 1000;

	//画像関係
	//--------------------

	//タイマーフレームの大きさ
	static constexpr int FRAME_SIZE_X = 200;
	static constexpr int FRAME_SIZE_Y = 80;

	//フレームの座標をずらす
	static constexpr int FRAME_SHIFT_POS_Y = 10;

	//数字画像サイズ
	static constexpr int NUMBER_SIZE_X = 50;
	static constexpr int NUMBER_SIZE_Y = 80;

	//数字画像の分割数
	static constexpr int NUMBER_X_DIV = 10;
	static constexpr int NUMBER_Y_DIV = 1;
	static constexpr int NUMBER_ALL_DIV = NUMBER_X_DIV * NUMBER_Y_DIV;

	//関数
	//--------------------

	void Init(void);			//初期化処理
	void Update(void);			//更新処理
	void Draw(void);			//描画処理
	void CountDownDraw(void);	//カウントダウンの描画処理
	void Release(void);			//解放処理

	//タイマーのリセット
	void ResetTimer(void);

	//制限時間超過判定を返す
	bool IsTimeOver(void);

	/// <summary>
	/// カウントダウン
	/// </summary>
	/// <param name="_countTime">カウントする秒数</param>
	/// <returns>カウントしきったらtrue</returns>
	bool CountDown(const float _countTime);

	//カウントダウンタイマーのリセット
	void ResetCountDownTimer(void);

	//シングルトン化
	//--------------------------
	//外部から静的にインスタンスの生成をする
	static void CreateInstance(void);

	//外部でインスタンスを利用可能とするために、
	//インスタンスを返す関数を作る
	static Timer& GetInstance(void);

	//インスタンスの破棄
	void Destroy(void);

private:
	//シングルトン化
	//---------------------------

	//コンストラクタ
	Timer(void);
	//デストラクタ
	~Timer(void);

	//コピーコンストラクタを利用できないようにする
	Timer(const Timer& ins);

	//メンバー変数
	//--------------------------
	//静的なインスタンス格納領域
	static Timer* instance_;

	//経過時間	
	DATEDATA counter_;

	//現在時刻
	int nowCount_;

	//タイマーフレームの画像
	int timerFrameImg_;

	//タイマーフレームの座標
	Vector2 framePos_;

	//数字の画像
	int numberImg_[10];

	//カウントダウンカウンタ
	float countDownCounter_;

	//カウントダウン時間保存
	float countDownTime_;
};