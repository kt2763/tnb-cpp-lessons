#include "lesson12_3.hpp"

Player::Player(int hp) : hp_(hp) {}

void Player::Damage(int amount)
{
    hp_ -= amount;
}

int Player::GetHP() const
{
    return hp_;
}
