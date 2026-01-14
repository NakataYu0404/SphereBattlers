
#include"../../Utility/AsoUtility.h"
#include"Capsule.h"
#include"Sphere.h"
#include"Collider.h"
#include "CollisionManager.h"

CollisionManager* CollisionManager::instance_ = nullptr;

void CollisionManager::CreateInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new CollisionManager();
	}
	instance_->Init();
}

CollisionManager& CollisionManager::GetInstance(void)
{
	return *instance_;
}

void CollisionManager::Init(void)
{

	std::vector<Collider::Category> tags;

	//	使用するタグを追加する
	tags.clear();
	tags.emplace_back(Collider::Category::PLAYER1);
	tags.emplace_back(Collider::Category::PLAYER2);
	tags.emplace_back(Collider::Category::PLAYER3);
	tags.emplace_back(Collider::Category::PLAYER4);
	tags.emplace_back(Collider::Category::STAGE);

	//	当たり判定のためのタグ管理
	categoryMap_.emplace(Collider::Category::PLAYER1, tags);
	categoryMap_.emplace(Collider::Category::PLAYER2, tags);
	categoryMap_.emplace(Collider::Category::PLAYER3, tags);
	categoryMap_.emplace(Collider::Category::PLAYER4, tags);


	//	tags.clear();
	//	tags.emplace_back(Collider::TAG::RAIDER);
	//	categoryMap_.emplace(Collider::TAG::SURVIVOR, tags);

}

void CollisionManager::Update(void)
{

	//	ループを抜けたときに削除するコライダの要素番号を保存する
	std::vector<int> deleteAct;
	deleteAct.clear();

	//	TODO:当たっていた時のOnCollisionが、
	//		 サバとレイダーを例とすると、actor=survivorナンバーとactor=raiderナンバーのときで二回当たってしまう
	//		 なので、当たったのは自分の要素番号でなのか相手の番号でなのか知れた方がいいかも

	//	範囲for文を使用していたが、「特定のIndexで要素を触りたい」ため、普通のfor文に
	for (int actNum = 0; actNum < actors_.size(); actNum++)
	{
		auto actor = actors_[actNum];

		//	取得したコライダー情報
		auto actorCollider = actor.lock()->GetTransform().lock()->collider_;
		auto actorType = actorCollider->type_;
		auto actorCategory = actorCollider->category_;

		//	現在のコライダが動くものでなければやりなおし
		if ((actorCategory != Collider::Category::PLAYER1 && actorCategory != Collider::Category::PLAYER2 && actorCategory != Collider::Category::PLAYER3 && actorCategory != Collider::Category::PLAYER4) || categoryMap_.count(actorCategory) == 0)
		{
			continue;
		}

		std::vector<int> deleteTar;
		deleteTar.clear();

		for (int tarNum = 0; tarNum < actors_.size(); tarNum++)
		{
			auto target = actors_[tarNum];

			//	同じもの同士では判定しない
			if (actor.lock() == target.lock())
			{
				continue;
			}

			//	それ以外のオブジェクト
			//	target:typ,CAPSULE, tag, enemy stage
			auto targetCollider = target.lock()->GetTransform().lock()->collider_;
			auto targetType = targetCollider->type_;
			auto targetCategory = targetCollider->category_;

			switch (actorType)
			{
			case Collider::TYPE::MODEL:
				break;
			case Collider::TYPE::CAPSULE:
				switch (targetType)
				{
				case Collider::TYPE::MODEL:		//	カプセル対モデル
				{

					auto info = Capsule2Model_Collider_PushBack(actor, target.lock()->GetTransform());

					if (info.isHit)
					{
						targetCollider->hitInfo_.isHit = true;
						targetCollider->hitInfo_.Normal = info.Normal;
						targetCollider->hitInfo_.movedPos = info.movedPos;
						actor.lock()->OnCollision(targetCollider);
					}
					else
					{
						targetCollider->hitInfo_.isHit = false;
						targetCollider->hitInfo_.Normal = { 0.0f, 0.0f, 0.0f };
					}

				}
				break;
				case Collider::TYPE::CAPSULE:	//	カプセル対カプセル
					if (Capsule2_Collider(
						actor.lock()->GetCapsule(),
						target.lock()->GetCapsule()))
					{
						actor.lock()->OnCollision(targetCollider);
						target.lock()->OnCollision(actorCollider);
					}
					break;
				case Collider::TYPE::SPHERE:	//	カプセル対スフィア
					if (Sphere2Capsule_Collider(
						target.lock()->GetSphere(),
						actor.lock()->GetCapsule()))
					{
						actor.lock()->OnCollision(targetCollider);
						target.lock()->OnCollision(actorCollider);
						deleteTar.push_back(tarNum);
					}
					break;
				}
				break;
			case Collider::TYPE::SPHERE:
				switch (targetType)
				{
				case Collider::TYPE::MODEL:

					auto info = Sphere2Model_Collider_PushBack(actor, target.lock()->GetTransform());

					if (info.isHit)
					{
						targetCollider->hitInfo_.isHit = true;
						targetCollider->hitInfo_.Normal = info.Normal;
						targetCollider->hitInfo_.movedPos = info.movedPos;
						actor.lock()->OnCollision(targetCollider);
					}
					else
					{
						targetCollider->hitInfo_.isHit = false;
						targetCollider->hitInfo_.Normal = { 0.0f, 0.0f, 0.0f };
					}

					break;
				case Collider::TYPE::CAPSULE:
					if (Sphere2Capsule_Collider(
						actor.lock()->GetSphere(),
						target.lock()->GetCapsule()))
					{
						actor.lock()->OnCollision(targetCollider);
						target.lock()->OnCollision(actorCollider);
						deleteAct.push_back(actNum);
					}
					break;
				case Collider::TYPE::SPHERE:
					if (HitCheck_Sphere_Sphere(actor.lock()->GetSphere().lock()->GetPos(), actor.lock()->GetSphere().lock()->GetRadius(), target.lock()->GetSphere().lock()->GetPos(), target.lock()->GetSphere().lock()->GetRadius()))
					{
						actor.lock()->OnCollision(targetCollider);
						target.lock()->OnCollision(actorCollider);
					}
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}

		}
		for (auto del : deleteTar)
		{
			actors_.erase(actors_.begin() + del);
		}
	}
	for (auto del : deleteAct)
	{
		actors_.erase(actors_.begin() + del);
	}

}

void CollisionManager::Draw(void)
{
	if (isAttack)
	{
		DrawString(10, 10, "当たった!!!", 0xff0000);
	}
}

void CollisionManager::Destroy(void)
{
	delete instance_;
}

void CollisionManager::Add(std::shared_ptr<ActorBase> collider)
{
	actors_.push_back(collider);
}

void CollisionManager::AddCollider(std::weak_ptr<Collider> collider)
{
	colliders_.push_back(collider);
}

void CollisionManager::ClearCollider(void)
{
	colliders_.clear();
	actors_.clear();
	categoryMap_.clear();
}

Collider::Collision_Date CollisionManager::Capsule2Model_Collider_PushBack(const std::weak_ptr<ActorBase> actor, const std::weak_ptr<Transform> transform)
{
	//	capsuleがキャラクターで、transformがステージのことだろう
	//	answerもおそらくstageよりのなんか

	Collider::Collision_Date answer;
	answer.isHit = false;

	answer.movedPos = actor.lock()->GetTransform().lock()->pos;


	//	カプセルを移動させる
	std::shared_ptr<Transform> trans = std::make_shared<Transform>();
	trans->pos = actor.lock()->GetTransform().lock()->pos;
	trans->Update();
	std::shared_ptr<Capsule> cap = std::make_shared<Capsule>(actor.lock()->GetCapsule(), trans);

	//	//	//	カプセルとの衝突判定
	//	for (const auto c : colliders_)
	//	{

		auto hits = MV1CollCheck_Capsule(
			transform.lock()->modelId, -1,
			cap->GetPosTop(), cap->GetPosDown(), cap->GetRadius());

		//	DxLib::MV1_COLL_RESULT_POLY_DIM


		//	衝突した複数のポリゴンと衝突回避するまで、
		//	プレイヤーの位置を移動させる
		for (int i = 0; i < hits.HitNum; i++)
		{

			auto hit = hits.Dim[i];
			//	DxLib::MV1_COLL_RESULT_POLY

			//	地面と異なり、衝突回避位置が不明なため、何度か移動させる
			//	この時、移動させる方向は、移動前座標に向いた方向であったり、
			//	衝突したポリゴンの法線方向だったりする
			for (int tryCnt = 0; tryCnt < MAX_COLLISION_TRY; tryCnt++)
			{

				//	再度、モデル全体と衝突検出するには、効率が悪過ぎるので、
				//	最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
				int pHit = HitCheck_Capsule_Triangle(
					cap->GetPosTop(), cap->GetPosDown(), cap->GetRadius(),
					hit.Position[0], hit.Position[1], hit.Position[2]);
				if (pHit)
				{
					//	法線の方向にちょっとだけ移動させる
					answer.movedPos = VAdd(answer.movedPos, VScale(hit.Normal, 2.0f));
					//	カプセルも一緒に移動させる
					trans->pos = answer.movedPos;
					trans->Update();

					answer.isHit = true;
					answer.Normal = hit.Normal;

					continue;
				}

				break;
			}
		}

		//	検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
	//	}

	return answer;
}

Collider::Collision_Date CollisionManager::Sphere2Model_Collider_PushBack(const std::weak_ptr<ActorBase> actor, const std::weak_ptr<Transform> transform)
{
	Collider::Collision_Date answer;
	answer.isHit = false;

	answer.movedPos = actor.lock()->GetTransform().lock()->pos;


	//	移動させる
	std::shared_ptr<Transform> trans = std::make_shared<Transform>();
	trans->pos = actor.lock()->GetTransform().lock()->pos;
	trans->Update();
	std::shared_ptr<Sphere> sph = std::make_shared<Sphere>(actor.lock()->GetSphere(), trans);

	//	//	//	衝突判定
	//	for (const auto c : colliders_)
	//	{

	auto hits = MV1CollCheck_Sphere(
		transform.lock()->modelId, -1,
		sph->GetPos(), sph->GetRadius());

	//	DxLib::MV1_COLL_RESULT_POLY_DIM


	//	衝突した複数のポリゴンと衝突回避するまで、
	//	プレイヤーの位置を移動させる
	for (int i = 0; i < hits.HitNum; i++)
	{

		auto hit = hits.Dim[i];
		//	DxLib::MV1_COLL_RESULT_POLY

		//	地面と異なり、衝突回避位置が不明なため、何度か移動させる
		//	この時、移動させる方向は、移動前座標に向いた方向であったり、
		//	衝突したポリゴンの法線方向だったりする
		for (int tryCnt = 0; tryCnt < MAX_COLLISION_TRY; tryCnt++)
		{

			//	再度、モデル全体と衝突検出するには、効率が悪過ぎるので、
			//	最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
			int pHit = HitCheck_Sphere_Triangle(
				sph->GetPos(), sph->GetRadius(),
				hit.Position[0], hit.Position[1], hit.Position[2]);
			if (pHit)
			{
				//	法線の方向にちょっとだけ移動させる
				answer.movedPos = VAdd(answer.movedPos, VScale(hit.Normal, 2.0f));
				//	カプセルも一緒に移動させる
				trans->pos = answer.movedPos;
				trans->Update();

				answer.isHit = true;
				answer.Normal = hit.Normal;

				continue;
			}

			break;
		}
	}

	//	検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);
	//	}

	return answer;
}


DxLib::MV1_COLL_RESULT_POLY CollisionManager::Line_IsCollision_Stage(const VECTOR LineTopPos, const VECTOR LineBotPos)
{
	MV1_COLL_RESULT_POLY a;

	if (this == nullptr)
	{
		a.HitFlag = false;
		return a;
	}

	//	ステージモデルと行うので、ステージを探すようにする
	for (auto actor : actors_)
	{
		//	取得したコライダー情報
		//	actor:typ,CAPSULE, tag, player
		auto actorCollider = actor.lock()->GetTransform().lock()->collider_;
		auto actorCategory = actorCollider->category_;

		//	ステージ以外であればループをやり直す
		if (actorCategory != Collider::Category::STAGE)
		{
			continue;
		}

		auto hit = MV1CollCheck_Line(actor.lock()->GetTransform().lock()->modelId, -1, LineTopPos, LineBotPos);

		return hit;
	}
}

bool CollisionManager::Capsule2_Collider(const std::weak_ptr<Capsule> a, const std::weak_ptr<Capsule> b)
{
	//	プレイヤーと敵の衝突判定
	if (HitCheck_Capsule_Capsule(a.lock()->GetPosTop(), a.lock()->GetPosDown(), a.lock()->GetRadius(),
									b.lock()->GetPosTop(), b.lock()->GetPosDown(), b.lock()->GetRadius()))
	{
		//	isAttack = true;
		return true;
	}
	else
	{
		isAttack = false;
		return false;
	}

}

bool CollisionManager::Sphere2Capsule_Collider(const std::weak_ptr<Sphere> a, const std::weak_ptr<Capsule> b)
{
	if (HitCheck_Sphere_Capsule(a.lock()->GetPos(), a.lock()->GetRadius(), b.lock()->GetPosTop(), b.lock()->GetPosDown(), b.lock()->GetRadius()))
	{
		return true;
	}

	return false;
}

//	制作者：中田
//	他プロジェクトに持ち出し禁止！
