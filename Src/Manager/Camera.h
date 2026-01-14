#pragma once
#include <DxLib.h>
#include "../Common/Quaternion.h"
#include "../Common/Vector2.h"
class Transform;
class CollisionManager;

class Camera
{

public:

	//	カメラスピード(度)
	static constexpr float SPEED = 1.0f;

	//	カメラクリップ：NEAR
	static constexpr float CAMERA_NEAR = 10.0f;

	//	カメラクリップ：NEAR
	static constexpr float CAMERA_FAR = 30000.0f;

	//	カメラの初期座標
	static constexpr VECTOR DEFAULT_CAMERA_POS = { 0.0f, 100.0f, -500.0f };

	//	追従位置からカメラ位置までの相対座標
	static constexpr VECTOR LOCAL_F2C_POS = { 0.0f, 0.0f, -650.0f };
	static constexpr VECTOR LOCAL_F2C_POS_LV2 = { 0.0f, 0.0f, -1000.0f };

	//	追従位置から注視点までの相対座標
	static constexpr VECTOR LOCAL_F2T_POS = { 0.0f, 200.0f, 200.0f };

	//	カメラのX回転上限度角
	static constexpr float LIMIT_X_UP_RAD = 80.0f * (DX_PI_F / 180.0f);
	static constexpr float LIMIT_X_DW_RAD = 89.0f * (DX_PI_F / 180.0f);

	static constexpr float MIDPOS_STRETCH_POW = 5000.0f;

	static constexpr float CAMERA_ROT_SPEED = 1.0f;

	//	カメラモード
	enum class MODE
	{
		NONE,
		FIXED_POINT,
		SELF_SHOT,
		FOLLOW,
	};

	Camera(void);
	~Camera(void);

	void Init(void);
	void Update(void);
	void SetBeforeDraw(void);
	void Draw(void);

	//	カメラ位置
	VECTOR GetPos(void) const;
	//	カメラの操作角度
	VECTOR GetAngles(void) const;
	//	カメラの注視点
	VECTOR GetTargetPos(void) const;

	//	カメラ角度
	Quaternion GetQuaRot(void) const;
	//	X回転を抜いたカメラ角度
	Quaternion GetQuaRotOutX(void) const;
	//	カメラの前方方向
	VECTOR GetForward(void) const;

	//	カメラモードの変更
	void ChangeMode(MODE mode);

	//	追従対象の設定
	void SetFollow(const Transform* follow);

	//	カメラの中心→視線方向に伸ばした先のPosを取得
	VECTOR GetMidRayDestantionPos(void);
private:

	//	カメラが追従対象とするTransform
	const Transform* followTransform_;

	//	カメラモード
	MODE mode_;

	//	カメラの位置
	VECTOR pos_;

	//	カメラ角度(rad)
	VECTOR angles_;

	//	X軸回転が無い角度
	Quaternion rotOutX_;

	//	カメラ角度
	Quaternion rot_;

	//	注視点
	VECTOR targetPos_;

	//	カメラの上方向
	VECTOR cameraUp_;

	Vector2 defaultMousePos_;

	//	カメラの中心からまっすぐ〇距離レイを伸ばした先のPosition
	VECTOR midRayDestinationPos_;

	//	カメラを初期位置に戻す
	void SetDefault(void);

	//	追従対象との位置同期を取る
	void SyncFollow(void);

	void ProcessRot(void);

	//	モード別更新ステップ
	void SetBeforeDrawFixedPoint(void);
	void SetBeforeDrawFollow(void);

	//	シングルトン参照
	CollisionManager& colMng_;

};

