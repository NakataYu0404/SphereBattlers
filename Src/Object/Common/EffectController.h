#pragma once
#include <EffekseerForDXLib.h>
#include <string>
#include <map>
class SceneManager;

class EffectController
{
	
public:
	static constexpr VECTOR POS_DEFAULT = { 0.0f,0.0f,0.0f };
	static constexpr VECTOR SCL_DEFAULT = { 1.0f,1.0f,1.0f };
	static constexpr VECTOR ROT_DEFAULT = { 0.0f,0.0f,0.0f };

	//	アニメーションデータ
	struct Effect
	{
		int Type = -1;
		int ResId = -1;
		int PlayId = -1;
		bool IsLoop = false;
		int startFrame = 0;
		int endFrame = -1;
		float nowTime = 0;
		VECTOR pos = POS_DEFAULT;
		VECTOR scl = SCL_DEFAULT;
		VECTOR rot = ROT_DEFAULT;
	};

	//	コンストラクタ
	EffectController(void);

	//	デストラクタ
	~EffectController(void);

	//	アニメーション追加
	void Add(int type, const int ResId);

	//	アニメーション再生
	void Play(int type, bool isLoop = false, int startFrame = 0, int endFrame = -1, float speed = 1.0f);

	void TransUpdate(int type, VECTOR pos = POS_DEFAULT, VECTOR scl = SCL_DEFAULT, VECTOR rot = ROT_DEFAULT);

	void Update(void);

	void Draw(void);

	//	withoutが偽ならtypeのみ、真ならtype以外のeffectを全て止めます
	void Stop(int type,bool without = false);

	//	再生中のアニメーション
	int GetPlayType(void) const;

	bool IsEnd(int type);
private:

	//	種類別のアニメーションデータ
	std::map<int, Effect> effects_;

	int playType_;
	Effect playEffect_;

	//	アニメーションをループするかしないか
	bool isLoop_;

	//	アニメーションを止めたままにする
	bool isStop_;

	//	アニメーション終了後に繰り返すループステップ
	float stepEndLoopStart_;
	float stepEndLoopEnd_;
	float endLoopSpeed_;

	//	逆再生
	float switchLoopReverse_;

	float speed_;

	//	制作者：中田
	//	他プロジェクトに持ち出し禁止！
};

