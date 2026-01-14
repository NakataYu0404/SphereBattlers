#pragma once
#include "ActorBase.h"
#include <memory>

class InputManager;

using namespace std;

class Player :
    public ActorBase
{

public:

    static constexpr float MOVE_SPEED = 2.0f;

    static constexpr float MOVE_ACC_MAX = 1.0f;

    static constexpr float GRAVITY = 9.81;

    static constexpr float ROT_COM_SEC = 1.0f;

    static constexpr VECTOR MODEL_CORRECTION_POS = { 0.0f,100.0f,0.0f };

    // ノックバック時切り替え可能
    static constexpr float CHANGEABLE_KNOCKBACK_POW = 0.35;

    // ノックバックの原則スピード
    static constexpr float KNOCKBACK_SPEED = 1.5;

    enum class State {
        //  ノックバックをStateで分ける必要が全く感じられません。
        Move,
        KnockBack,
        Fall,
    };

    Player();
    Player(const VECTOR& initPos);
    ~Player();

   virtual void Init()override;
   virtual void InitModel() = 0;
   virtual void Update()override;
   virtual void Draw()override;
   virtual void DebugDraw()final;

   virtual const VECTOR& GetMoveDir();
   virtual const float GetMoveAcc();

   virtual void SetMoveDir(const VECTOR& dir)final;

   virtual void ProcessKnockBack(const VECTOR& dir, float pow);

protected:

    InputManager& input_;

    State state_;

    // アップデート	// アップデート管理
    using UpdateFunc_t = void (Player::*)();
    UpdateFunc_t updateFunc_ = &Player::UpdateMove;
    virtual void UpdateMove()final;
    virtual void UpdateKnockBack()final;
    virtual void UpdateFall()final;

    VECTOR moveDir_ = { 0.0f,0.0f,0.0f };
    float speed_ = 0.0f;
    float oldSpeed_ = 0.0f;

    // 初速度
    float movePow_ = 2.0f;
    // 加速度
    float moveAcc_ = 0.0f;

    // キー押下判定(移動方向も決定する);
    virtual const bool& IsInputMove() = 0;
    virtual void CheckMoveDirection() = 0;

    // 移動
    virtual void ProcessMove()final;
    virtual void Move(const VECTOR& dir, float speed)final;
    float moveTotalTime_ = 0.0f;

    VECTOR velocity_ = {0.0f,0.0f,0.0f};

    // ノックバック
    virtual void KnockBack();
    VECTOR knockBackDir_ = { 0.0f,0.0f,1.0f };
    float KnockBackPow_ = 1.1f;
    float knockBackTotalTime_ = 0.0f;
    float knockBackComTime_ = 0.0f;

    // 落下
    virtual void Gravity()final;
    float gravityTotalTime_ = 0.0f;

    // 当たり判定
    virtual void CollisionStage()final;

    // 回転
    virtual void Rotation()final;
};