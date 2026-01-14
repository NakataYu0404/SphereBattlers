#pragma once
#include <vector>
#include <map>
#include <memory>
//	#include <iostream>
#include <functional>
#include "Transform.h"
#include "../ActorBase.h"
#include "./Collider.h"

using namespace std;

class Transform;
class Collider;
class Capsule;
class Sphere;
class ActorBase;
class Player;

class Shape
{

};

class CollisionManager
{
public:

	static constexpr int MAX_COLLISION_TRY = 100;

	//	インスタンスの生成
	static void CreateInstance(void);

	//	インスタンスの取得
	static CollisionManager& GetInstance(void);

	void Init(void);
	void Update(void);
	void Draw(void);
	void Destroy(void);

	//	衝突判定に用いられるコライダ制御
	void AddCollider(std::weak_ptr<Collider> collider);
	void ClearCollider(void);

	void Add(std::shared_ptr<ActorBase> collider);

	//	カプセルとモデルの衝突判定
	Collider::Collision_Date Capsule2Model_Collider_PushBack(const std::weak_ptr<ActorBase> actor, const std::weak_ptr<Transform> transform);
	Collider::Collision_Date Sphere2Model_Collider_PushBack(const std::weak_ptr<ActorBase> actor, const std::weak_ptr<Transform> transform);
	DxLib::MV1_COLL_RESULT_POLY Line_IsCollision_Stage(const VECTOR LineTopPos, const VECTOR LineBotPos);

	//	元からあるカプセルコライダを使ったカプセルとカプセルの衝突判定
	bool Capsule2_Collider(const std::weak_ptr<Capsule> a, const std::weak_ptr<Capsule> b);

	//	入力したスフィア情報と、元からあるカプセルコライダとの当たり判定
	bool Sphere2Capsule_Collider(const std::weak_ptr<Sphere> a, const std::weak_ptr<Capsule> b);

private:

	//	静的インスタンス
	static CollisionManager* instance_;

	//	当たったかの判定
	bool isAttack = false;

	//	衝突したポリゴンの法線
	DxLib::VECTOR Normal_ = {};

	//	衝突判定に用いられるコライダ
	std::vector<std::weak_ptr<ActorBase>> actors_;
	std::vector<std::weak_ptr<Collider>> colliders_;

	//	衝突した場合の関数
	std::function<void(void)> onCollision_;

	//	衝突判定のための管理
	std::map<Collider::Category, std::vector<Collider::Category>> categoryMap_;

	//	制作者：中田
	//	他プロジェクトに持ち出し禁止！
};
