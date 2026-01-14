#pragma once
#include "Player.h"
class Player1 :
    public Player
{
public:
    using Player::Player;

    //  ‚±‚±‚©‚ç‰½‚©‚ÉŒp³‚·‚é‚í‚¯‚Å‚à‚È‚³‚»‚¤‚È‚Ì‚ÉAvirtual‚ğg‚¤ˆÓ–¡‚ğŠ´‚¶‚Ü‚¹‚ñB
    virtual void InitModel()override;
protected:
    virtual const bool& IsInputMove()override;
    virtual void CheckMoveDirection()override;
};

