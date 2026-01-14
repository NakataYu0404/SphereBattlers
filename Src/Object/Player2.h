#pragma once
#include "Player.h"
class Player2 :
    public Player
{
public:
    using Player::Player;
    virtual void InitModel()override;
protected:
    virtual const bool& IsInputMove()override;
    virtual void CheckMoveDirection()override;
};

