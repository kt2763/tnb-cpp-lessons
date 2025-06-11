/*
 * C++モダンコース - セクション9: アルゴリズムとイテレータ
 * レクチャー5: 集合アルゴリズム（set_union, set_intersection, set_difference）
 * 
 * このレクチャーでは、ソート済みの集合に対する演算アルゴリズムを学びます
 * ゲーム開発でのスキル組み合わせ、アイテム管理、プレイヤーグループ処理を実践します
 */

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <iomanip>

// ゲームスキルのデータ構造
struct GameSkill
{
    int skill_id;
    std::string name;
    std::string category;
    int level;
    
    GameSkill(int id, const std::string& n, const std::string& cat, int lv)
        : skill_id(id), name(n), category(cat), level(lv) {}
    
    void display() const
    {
        std::cout << "[" << std::setw(3) << skill_id << "] "
                  << std::setw(15) << name 
                  << " (" << std::setw(8) << category << ") "
                  << "Lv." << level << std::endl;
    }
    
    bool operator<(const GameSkill& other) const
    {
        return skill_id < other.skill_id;
    }
    
    bool operator==(const GameSkill& other) const
    {
        return skill_id == other.skill_id;
    }
};

// プレイヤーのデータ構造
struct Player
{
    std::string name;
    std::vector<int> skill_ids;
    std::vector<int> item_ids;
    
    Player(const std::string& n, const std::vector<int>& skills, const std::vector<int>& items)
        : name(n), skill_ids(skills), item_ids(items) 
    {
        // ソートして集合演算を可能にする
        std::sort(skill_ids.begin(), skill_ids.end());
        std::sort(item_ids.begin(), item_ids.end());
    }
};

void demonstrateSetUnion()
{
    std::cout << "\n=== set_union（和集合）の実例 ===" << std::endl;
    
    // 2つの職業のスキルセット
    std::vector<int> warrior_skills = {101, 102, 105, 108, 110, 115};      // 戦士スキル
    std::vector<int> paladin_skills = {101, 103, 105, 109, 111, 112, 115}; // パラディンスキル
    
    std::cout << "戦士のスキル: ";
    for (int skill : warrior_skills)
    {
        std::cout << skill << " ";
    }
    std::cout << std::endl;
    
    std::cout << "パラディンのスキル: ";
    for (int skill : paladin_skills)
    {
        std::cout << skill << " ";
    }
    std::cout << std::endl;
    
    // 1. 和集合（全スキルの統合）
    std::vector<int> all_combat_skills;
    std::set_union(warrior_skills.begin(), warrior_skills.end(),
                   paladin_skills.begin(), paladin_skills.end(),
                   std::back_inserter(all_combat_skills));
    
    std::cout << "\n戦闘系全スキル（和集合）: ";
    for (int skill : all_combat_skills)
    {
        std::cout << skill << " ";
    }
    std::cout << std::endl;
    std::cout << "総スキル数: " << all_combat_skills.size() << std::endl;
}

void demonstrateSetIntersection()
{
    std::cout << "\n=== set_intersection（積集合）の実例 ===" << std::endl;
    
    // プレイヤーのアイテム所持状況
    std::vector<int> player1_items = {1001, 1003, 1005, 1007, 1010, 1012, 1015};
    std::vector<int> player2_items = {1002, 1003, 1006, 1007, 1009, 1012, 1014, 1015};
    
    std::cout << "プレイヤー1のアイテム: ";
    for (int item : player1_items)
    {
        std::cout << item << " ";
    }
    std::cout << std::endl;
    
    std::cout << "プレイヤー2のアイテム: ";
    for (int item : player2_items)
    {
        std::cout << item << " ";
    }
    std::cout << std::endl;
    
    // 1. 積集合（共通アイテム）
    std::vector<int> common_items;
    std::set_intersection(player1_items.begin(), player1_items.end(),
                         player2_items.begin(), player2_items.end(),
                         std::back_inserter(common_items));
    
    std::cout << "\n共通所持アイテム（積集合）: ";
    for (int item : common_items)
    {
        std::cout << item << " ";
    }
    std::cout << std::endl;
    std::cout << "共通アイテム数: " << common_items.size() << std::endl;
    
    // 2. トレード可能性の判定
    if (!common_items.empty())
    {
        std::cout << "トレード提案: 共通アイテムがあるため交換可能です" << std::endl;
    }
}

void demonstrateSetDifference()
{
    std::cout << "\n=== set_difference（差集合）の実例 ===" << std::endl;
    
    // ギルドメンバーのスキル習得状況
    std::vector<int> guild_required_skills = {201, 202, 203, 204, 205, 206, 207, 208};
    std::vector<int> player_current_skills = {201, 203, 205, 207, 209, 210};
    
    std::cout << "ギルド必須スキル: ";
    for (int skill : guild_required_skills)
    {
        std::cout << skill << " ";
    }
    std::cout << std::endl;
    
    std::cout << "プレイヤー現在スキル: ";
    for (int skill : player_current_skills)
    {
        std::cout << skill << " ";
    }
    std::cout << std::endl;
    
    // 1. 差集合（不足スキル）
    std::vector<int> missing_skills;
    std::set_difference(guild_required_skills.begin(), guild_required_skills.end(),
                       player_current_skills.begin(), player_current_skills.end(),
                       std::back_inserter(missing_skills));
    
    std::cout << "\n習得必要スキル（差集合）: ";
    for (int skill : missing_skills)
    {
        std::cout << skill << " ";
    }
    std::cout << std::endl;
    std::cout << "不足スキル数: " << missing_skills.size() << std::endl;
    
    // 2. 余剰スキル（ギルド要件外のスキル）
    std::vector<int> extra_skills;
    std::set_difference(player_current_skills.begin(), player_current_skills.end(),
                       guild_required_skills.begin(), guild_required_skills.end(),
                       std::back_inserter(extra_skills));
    
    std::cout << "\nギルド要件外スキル: ";
    for (int skill : extra_skills)
    {
        std::cout << skill << " ";
    }
    std::cout << std::endl;
}

void demonstrateSetSymmetricDifference()
{
    std::cout << "\n=== set_symmetric_difference（対称差集合）の実例 ===" << std::endl;
    
    // 2つのチームのメンバーID
    std::vector<int> team_alpha = {1, 3, 5, 7, 9, 11, 13};
    std::vector<int> team_beta = {2, 3, 6, 7, 10, 11, 14};
    
    std::cout << "チームアルファ: ";
    for (int member : team_alpha)
    {
        std::cout << member << " ";
    }
    std::cout << std::endl;
    
    std::cout << "チームベータ: ";
    for (int member : team_beta)
    {
        std::cout << member << " ";
    }
    std::cout << std::endl;
    
    // 1. 対称差集合（どちらか一方にのみ所属）
    std::vector<int> unique_members;
    std::set_symmetric_difference(team_alpha.begin(), team_alpha.end(),
                                 team_beta.begin(), team_beta.end(),
                                 std::back_inserter(unique_members));
    
    std::cout << "\n片方のチームのみ所属（対称差集合）: ";
    for (int member : unique_members)
    {
        std::cout << member << " ";
    }
    std::cout << std::endl;
    std::cout << "一方のみ所属メンバー数: " << unique_members.size() << std::endl;
}

void demonstrateSetIncludes()
{
    std::cout << "\n=== includes（包含関係チェック）の実例 ===" << std::endl;
    
    // マスタースキルセットと専門スキルセット
    std::vector<int> master_skill_set = {301, 302, 303, 304, 305, 306, 307, 308, 309, 310};
    std::vector<int> fire_magic_skills = {302, 304, 306, 308};
    std::vector<int> ice_magic_skills = {301, 303, 305, 307};
    std::vector<int> forbidden_skills = {311, 312, 313}; // マスターセットにない
    
    std::cout << "マスタースキルセット: ";
    for (int skill : master_skill_set)
    {
        std::cout << skill << " ";
    }
    std::cout << std::endl;
    
    // 1. 各専門スキルがマスターセットに含まれるかチェック
    std::cout << "\n包含関係チェック:" << std::endl;
    
    bool fire_included = std::includes(master_skill_set.begin(), master_skill_set.end(),
                                      fire_magic_skills.begin(), fire_magic_skills.end());
    std::cout << "火魔法スキルがマスターセットに含まれる: " << (fire_included ? "はい" : "いいえ") << std::endl;
    
    bool ice_included = std::includes(master_skill_set.begin(), master_skill_set.end(),
                                     ice_magic_skills.begin(), ice_magic_skills.end());
    std::cout << "氷魔法スキルがマスターセットに含まれる: " << (ice_included ? "はい" : "いいえ") << std::endl;
    
    bool forbidden_included = std::includes(master_skill_set.begin(), master_skill_set.end(),
                                           forbidden_skills.begin(), forbidden_skills.end());
    std::cout << "禁断スキルがマスターセットに含まれる: " << (forbidden_included ? "はい" : "いいえ") << std::endl;
}

void demonstrateGameGuildSystem()
{
    std::cout << "\n=== ゲームギルドシステムでの活用 ===" << std::endl;
    
    // ギルドの構成
    struct Guild
    {
        std::string name;
        std::vector<int> member_ids;
        std::vector<int> available_skills;
        std::vector<int> required_items;
        
        Guild(const std::string& n, const std::vector<int>& members, 
              const std::vector<int>& skills, const std::vector<int>& items)
            : name(n), member_ids(members), available_skills(skills), required_items(items)
        {
            std::sort(member_ids.begin(), member_ids.end());
            std::sort(available_skills.begin(), available_skills.end());
            std::sort(required_items.begin(), required_items.end());
        }
    };
    
    Guild dragons("ドラゴンナイツ", {101, 102, 103, 105, 107, 110}, 
                  {401, 402, 403, 405, 407, 410}, {501, 502, 503, 505});
    
    Guild phoenixes("フェニックス", {102, 104, 106, 108, 109, 110}, 
                   {402, 404, 406, 408, 409, 410}, {502, 504, 506, 507});
    
    std::cout << "ギルド「" << dragons.name << "」メンバー: ";
    for (int id : dragons.member_ids)
    {
        std::cout << id << " ";
    }
    std::cout << std::endl;
    
    std::cout << "ギルド「" << phoenixes.name << "」メンバー: ";
    for (int id : phoenixes.member_ids)
    {
        std::cout << id << " ";
    }
    std::cout << std::endl;
    
    // 1. 両ギルドに所属するプレイヤー
    std::vector<int> dual_members;
    std::set_intersection(dragons.member_ids.begin(), dragons.member_ids.end(),
                         phoenixes.member_ids.begin(), phoenixes.member_ids.end(),
                         std::back_inserter(dual_members));
    
    std::cout << "\n両ギルドに所属: ";
    for (int id : dual_members)
    {
        std::cout << id << " ";
    }
    std::cout << std::endl;
    
    // 2. 全メンバーの統合
    std::vector<int> all_members;
    std::set_union(dragons.member_ids.begin(), dragons.member_ids.end(),
                   phoenixes.member_ids.begin(), phoenixes.member_ids.end(),
                   std::back_inserter(all_members));
    
    std::cout << "全メンバー（重複なし）: ";
    for (int id : all_members)
    {
        std::cout << id << " ";
    }
    std::cout << " (総人数: " << all_members.size() << "人)" << std::endl;
    
    // 3. 利用可能スキルの統合
    std::vector<int> combined_skills;
    std::set_union(dragons.available_skills.begin(), dragons.available_skills.end(),
                   phoenixes.available_skills.begin(), phoenixes.available_skills.end(),
                   std::back_inserter(combined_skills));
    
    std::cout << "統合スキル数: " << combined_skills.size() << std::endl;
    
    // 4. 共通で必要なアイテム
    std::vector<int> common_items;
    std::set_intersection(dragons.required_items.begin(), dragons.required_items.end(),
                         phoenixes.required_items.begin(), phoenixes.required_items.end(),
                         std::back_inserter(common_items));
    
    std::cout << "両ギルド共通必要アイテム: ";
    for (int item : common_items)
    {
        std::cout << item << " ";
    }
    std::cout << std::endl;
}

void demonstrateRaidPreparation()
{
    std::cout << "\n=== レイド準備システムでの活用 ===" << std::endl;
    
    // レイドに必要なスキルと装備
    std::vector<int> raid_required_skills = {601, 602, 603, 604, 605, 606, 607, 608};
    std::vector<int> raid_required_items = {701, 702, 703, 704, 705};
    
    // パーティメンバーの所持状況
    std::vector<Player> party_members = 
    {
        Player("タンク", {601, 602, 603, 609}, {701, 702, 706}),
        Player("ヒーラー", {604, 605, 610, 611}, {703, 704, 707}),
        Player("DPS1", {606, 607, 612, 613}, {702, 705, 708}),
        Player("DPS2", {607, 608, 614, 615}, {701, 704, 709})
    };
    
    std::cout << "レイド必須スキル: ";
    for (int skill : raid_required_skills)
    {
        std::cout << skill << " ";
    }
    std::cout << std::endl;
    
    std::cout << "レイド必須アイテム: ";
    for (int item : raid_required_items)
    {
        std::cout << item << " ";
    }
    std::cout << std::endl;
    
    // 1. パーティ全体のスキル統合
    std::vector<int> party_all_skills;
    for (const auto& member : party_members)
    {
        std::vector<int> temp_skills;
        std::set_union(party_all_skills.begin(), party_all_skills.end(),
                      member.skill_ids.begin(), member.skill_ids.end(),
                      std::back_inserter(temp_skills));
        party_all_skills = temp_skills;
    }
    
    // 2. 不足スキルの確認
    std::vector<int> missing_skills;
    std::set_difference(raid_required_skills.begin(), raid_required_skills.end(),
                       party_all_skills.begin(), party_all_skills.end(),
                       std::back_inserter(missing_skills));
    
    std::cout << "\nパーティ不足スキル: ";
    for (int skill : missing_skills)
    {
        std::cout << skill << " ";
    }
    std::cout << std::endl;
    
    // 3. パーティ全体のアイテム統合
    std::vector<int> party_all_items;
    for (const auto& member : party_members)
    {
        std::vector<int> temp_items;
        std::set_union(party_all_items.begin(), party_all_items.end(),
                      member.item_ids.begin(), member.item_ids.end(),
                      std::back_inserter(temp_items));
        party_all_items = temp_items;
    }
    
    // 4. 不足アイテムの確認
    std::vector<int> missing_items;
    std::set_difference(raid_required_items.begin(), raid_required_items.end(),
                       party_all_items.begin(), party_all_items.end(),
                       std::back_inserter(missing_items));
    
    std::cout << "パーティ不足アイテム: ";
    for (int item : missing_items)
    {
        std::cout << item << " ";
    }
    std::cout << std::endl;
    
    // 5. レイド準備状況
    bool skills_ready = missing_skills.empty();
    bool items_ready = missing_items.empty();
    
    std::cout << "\nレイド準備状況:" << std::endl;
    std::cout << "スキル準備: " << (skills_ready ? "完了" : "不完了") << std::endl;
    std::cout << "アイテム準備: " << (items_ready ? "完了" : "不完了") << std::endl;
    std::cout << "レイド挑戦可能: " << (skills_ready && items_ready ? "可能" : "要準備") << std::endl;
}

int main()
{
    std::cout << "C++アルゴリズムとイテレータ - レクチャー5: 集合アルゴリズム" << std::endl;
    std::cout << "======================================================" << std::endl;
    
    // 各集合アルゴリズムの実演
    demonstrateSetUnion();
    demonstrateSetIntersection();
    demonstrateSetDifference();
    demonstrateSetSymmetricDifference();
    demonstrateSetIncludes();
    demonstrateGameGuildSystem();
    demonstrateRaidPreparation();
    
    std::cout << "\n=== まとめ ===" << std::endl;
    std::cout << "1. set_union: 和集合（A ∪ B）" << std::endl;
    std::cout << "2. set_intersection: 積集合（A ∩ B）" << std::endl;
    std::cout << "3. set_difference: 差集合（A - B）" << std::endl;
    std::cout << "4. set_symmetric_difference: 対称差集合（A △ B）" << std::endl;
    std::cout << "5. includes: 包含関係チェック（A ⊆ B）" << std::endl;
    std::cout << "注意: すべてソート済みの範囲が前提条件" << std::endl;
    std::cout << "ゲーム開発では、スキル管理、アイテム比較、グループ操作で活用" << std::endl;
    
    return 0;
}