#pragma once
#include <memory>
#include <DxLib.h>
class Transform;

class Sphere
{

public:

	//	デバッグ時の簡易カプセル表示の色
	static constexpr int COLOR = 0xffffff;

	//	コンストラクタ
	//	カプセルをつけるオブジェクトを指定する
	Sphere(std::weak_ptr<Transform> parent);
	Sphere(const std::weak_ptr<Sphere> base, const std::weak_ptr<Transform> parent);

	//	デストラクタ
	~Sphere(void);

	//	描画
	void Draw(void);

	//	親Transformからの相対位置を取得
	VECTOR GetLocalPos(void) const;

	void SetLocalPos(const VECTOR& pos);

	//	ワールド座標を取得
	VECTOR GetPos(void) const;

	//	半径
	float GetRadius(void) const;
	void SetRadius(float radius);
private:

	//	カプセルをくっつける相手
	std::weak_ptr<Transform> transformParent_;

	//	親Transformからの相対位置
	VECTOR localPos_;

	//	半径
	float radius_;

};
