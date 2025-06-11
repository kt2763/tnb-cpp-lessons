/*
 * テンプレート第1講: 関数テンプレートの基本
 * 
 * 関数テンプレートは、異なる型に対して同じ処理を行う関数を
 * 一つの定義で書けるようにする機能です。
 * ゲーム開発では、異なる数値型での計算や汎用的な処理でよく使用されます。
 */

#include <iostream>
#include <string>
#include <vector>

// 基本的な関数テンプレート - 最大値を求める
template<typename T>
T getMax(T a, T b)
{
    return (a > b) ? a : b;
}

// ゲーム例: プレイヤーのステータス比較
template<typename T>
T getHigherStat(T playerStat, T enemyStat)
{
    std::cout << "プレイヤー: " << playerStat << " vs 敵: " << enemyStat << std::endl;
    return (playerStat > enemyStat) ? playerStat : enemyStat;
}

// 複数の型パラメータを持つテンプレート
template<typename T, typename U>
void displayBattleInfo(T playerName, U playerLevel)
{
    std::cout << "勇者 " << playerName << " (レベル " << playerLevel << ") が戦闘準備完了！" << std::endl;
}

// ゲーム例: ダメージ計算テンプレート
template<typename DamageType>
DamageType calculateDamage(DamageType baseDamage, DamageType multiplier)
{
    return baseDamage * multiplier;
}

// 配列の要素を処理するテンプレート
template<typename T>
void printInventory(const std::vector<T>& items)
{
    std::cout << "=== インベントリ ===" << std::endl;
    for (size_t i = 0; i < items.size(); ++i)
    {
        std::cout << i + 1 << ". " << items[i] << std::endl;
    }
}

// より複雑な例: ゲームアイテムの比較
template<typename T>
bool isItemBetter(const T& newItem, const T& currentItem)
{
    // Tが比較演算子をサポートしていることを前提とする
    return newItem > currentItem;
}

int main()
{
    std::cout << "=== 関数テンプレートの基本デモ ===" << std::endl;
    
    // 整数での使用例
    std::cout << "\n1. 基本的な最大値計算:" << std::endl;
    int maxInt = getMax(10, 20);
    std::cout << "max(10, 20) = " << maxInt << std::endl;
    
    // 浮動小数点数での使用例
    double maxDouble = getMax(3.14, 2.71);
    std::cout << "max(3.14, 2.71) = " << maxDouble << std::endl;
    
    // 文字列での使用例
    std::string maxStr = getMax(std::string("apple"), std::string("banana"));
    std::cout << "max(\"apple\", \"banana\") = " << maxStr << std::endl;
    
    // ゲーム例: ステータス比較
    std::cout << "\n2. ゲームステータス比較:" << std::endl;
    int higherAttack = getHigherStat(85, 72);
    std::cout << "より高い攻撃力: " << higherAttack << std::endl;
    
    double higherSpeed = getHigherStat(12.5, 15.3);
    std::cout << "より高い速度: " << higherSpeed << std::endl;
    
    // 複数型パラメータの例
    std::cout << "\n3. 戦闘情報表示:" << std::endl;
    displayBattleInfo("アリス", 25);
    displayBattleInfo(std::string("ボブ"), 30);
    
    // ダメージ計算の例
    std::cout << "\n4. ダメージ計算:" << std::endl;
    int physicalDamage = calculateDamage(50, 2);
    std::cout << "物理ダメージ: " << physicalDamage << std::endl;
    
    double magicalDamage = calculateDamage(75.5, 1.8);
    std::cout << "魔法ダメージ: " << magicalDamage << std::endl;
    
    // インベントリ表示の例
    std::cout << "\n5. インベントリ表示:" << std::endl;
    std::vector<std::string> weapons = {"鉄の剣", "炎の杖", "氷の弓"};
    printInventory(weapons);
    
    std::vector<int> potionCounts = {5, 3, 8, 2};
    std::cout << "\nポーション数量:" << std::endl;
    printInventory(potionCounts);
    
    // アイテム比較の例
    std::cout << "\n6. アイテム比較:" << std::endl;
    int newSwordPower = 150;
    int currentSwordPower = 120;
    
    if (isItemBetter(newSwordPower, currentSwordPower))
    {
        std::cout << "新しい剣の方が強力です! (攻撃力: " << newSwordPower << ")" << std::endl;
    }
    else
    {
        std::cout << "現在の剣の方が良いです。" << std::endl;
    }
    
    return 0;
}

/*
 * 学習ポイント:
 * 1. template<typename T> で関数テンプレートを定義
 * 2. 型パラメータTは関数内で型として使用可能
 * 3. コンパイラが自動的に型を推定（型推定）
 * 4. 複数の型パラメータも使用可能
 * 5. テンプレートはヘッダファイルに定義するのが一般的
 * 6. ゲーム開発では数値計算、コンテナ操作で頻繁に使用
 */