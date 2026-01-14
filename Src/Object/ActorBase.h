#pragma once
#include <memory>
#include "Common/Transform.h"
class ResourceManager;
class SceneManager;
class Capsule;
class Sphere;

class ActorBase
{

public:

	//	コンストラクタ
	ActorBase(void);

	//	デストラクタ
	virtual ~ActorBase(void);

	virtual void Init(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

	virtual void SetParam(void);

	//	オブジェクトが衝突時に返す
	virtual void OnCollision(std::weak_ptr<Collider> collider);

	const std::weak_ptr<Transform> GetTransform(void) const;

	//	衝突用カプセルの取得
	const std::weak_ptr<Capsule> GetCapsule(void) const;
	const std::weak_ptr<Sphere> GetSphere(void) const;


protected:

	//	シングルトン参照
	ResourceManager& resMng_;
	SceneManager& scnMng_;

	//	モデル制御の基本情報
	std::shared_ptr<Transform> transform_;

	//	衝突判定用のカプセル
	std::shared_ptr<Capsule> capsule_;
	std::shared_ptr<Sphere> sphere_;
};
