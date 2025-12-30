#include "Player.hpp"  // ← ここで1回目
#include "Enemy.hpp"   // ← Enemy.hの中でまたPlayer.hが読まれる（2回目）

// この時点でPlayerクラスが2回定義されてエラー！
