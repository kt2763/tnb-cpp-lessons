#pragma once
#include "Player.hpp"  // PlayerクラスのポインタをEnemy内で使う

class Enemy
{
    Player* target;  // プレイヤーへの参照
};
