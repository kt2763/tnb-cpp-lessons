#pragma once

class Player
{
public:
    Player(int hp);
    void Damage(int amount);
    int GetHP() const; // 後ろのconstはconstメンバ関数, メンバ変数の変更を禁止する
private:
    int hp_;
};
