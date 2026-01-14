#pragma once
#include <DxLib.h>
#include <vector>

class Collider
{

public:

	//	対象種別
	enum class Category
	{
		PLAYER1,
		PLAYER2,
		PLAYER3,
		PLAYER4,
		STAGE,
	};

	//	衝突種別
	enum class TYPE
	{
		MODEL,
		CAPSULE,
		SPHERE,
	};

	//	衝突情報
	struct Collision_Date
	{
		bool isHit;
		Category tag;
		TYPE type;
		int modelId;
		VECTOR movedPos;
		DxLib::VECTOR Normal;
	};

	//	コンストラクタ
	Collider(Category tag, TYPE type, int modelId);

	//	デストラクタ
	~Collider(void);

	//	対象種別
	Category category_;

	//	衝突種別
	TYPE type_;

	//	モデルのハンドルID
	int modelId_;

	Collision_Date hitInfo_;

};
