// ミューテックスと同期
// 共有リソースへの安全なアクセスを実現

#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <lock_guard>
#include <unique_lock>
#include <vector>
#include <string>
#include <chrono>
#include <queue>

// 基本的なミューテックスの使用
class Counter
{
private:
    int count_;
    std::mutex mutex_;
    
public:
    Counter() : count_(0) {}
    
    void increment()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ++count_;
        std::cout << "カウント: " << count_ << " (スレッド: " << std::this_thread::get_id() << ")" << std::endl;
    }
    
    void decrement()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        --count_;
        std::cout << "カウント: " << count_ << " (スレッド: " << std::this_thread::get_id() << ")" << std::endl;
    }
    
    int getValue() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return count_;
    }
};

// ゲームでの例：プレイヤースコア管理
class GameScore
{
private:
    int totalScore_;
    int playerCount_;
    mutable std::mutex scoreMutex_;  // constメソッドでも使用するためmutable
    
public:
    GameScore() : totalScore_(0), playerCount_(0) {}
    
    void addScore(int playerId, int points)
    {
        std::lock_guard<std::mutex> lock(scoreMutex_);
        totalScore_ += points;
        std::cout << "プレイヤー " << playerId << " が " << points 
                  << " 点獲得！ 総スコア: " << totalScore_ << std::endl;
    }
    
    void addPlayer()
    {
        std::lock_guard<std::mutex> lock(scoreMutex_);
        ++playerCount_;
        std::cout << "プレイヤー参加！ 現在の参加者数: " << playerCount_ << std::endl;
    }
    
    void removePlayer()
    {
        std::lock_guard<std::mutex> lock(scoreMutex_);
        if (playerCount_ > 0)
        {
            --playerCount_;
            std::cout << "プレイヤー退出。 現在の参加者数: " << playerCount_ << std::endl;
        }
    }
    
    int getTotalScore() const
    {
        std::lock_guard<std::mutex> lock(scoreMutex_);
        return totalScore_;
    }
    
    int getPlayerCount() const
    {
        std::lock_guard<std::mutex> lock(scoreMutex_);
        return playerCount_;
    }
};

// リーダーライターロック（shared_mutex）
class GameConfig
{
private:
    std::string serverName_;
    int maxPlayers_;
    float difficulty_;
    mutable std::shared_mutex configMutex_;
    
public:
    GameConfig(const std::string& name, int maxPlayers, float difficulty)
        : serverName_(name), maxPlayers_(maxPlayers), difficulty_(difficulty) {}
    
    // 読み取り専用操作（複数スレッドが同時実行可能）
    std::string getServerName() const
    {
        std::shared_lock<std::shared_mutex> lock(configMutex_);
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 読み取り処理のシミュレート
        return serverName_;
    }
    
    int getMaxPlayers() const
    {
        std::shared_lock<std::shared_mutex> lock(configMutex_);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return maxPlayers_;
    }
    
    float getDifficulty() const
    {
        std::shared_lock<std::shared_mutex> lock(configMutex_);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return difficulty_;
    }
    
    // 書き込み操作（排他的アクセス）
    void updateServerName(const std::string& newName)
    {
        std::unique_lock<std::shared_mutex> lock(configMutex_);
        std::cout << "サーバー名を更新中..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        serverName_ = newName;
        std::cout << "サーバー名を " << newName << " に更新しました" << std::endl;
    }
    
    void updateDifficulty(float newDifficulty)
    {
        std::unique_lock<std::shared_mutex> lock(configMutex_);
        std::cout << "難易度を更新中..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        difficulty_ = newDifficulty;
        std::cout << "難易度を " << newDifficulty << " に更新しました" << std::endl;
    }
};

// デッドロック回避の例
class BankAccount
{
private:
    std::string accountName_;
    int balance_;
    mutable std::mutex mutex_;
    
public:
    BankAccount(const std::string& name, int initialBalance)
        : accountName_(name), balance_(initialBalance) {}
    
    std::string getName() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return accountName_;
    }
    
    int getBalance() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return balance_;
    }
    
    void deposit(int amount)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        balance_ += amount;
        std::cout << accountName_ << " に " << amount << " 入金。残高: " << balance_ << std::endl;
    }
    
    bool withdraw(int amount)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (balance_ >= amount)
        {
            balance_ -= amount;
            std::cout << accountName_ << " から " << amount << " 出金。残高: " << balance_ << std::endl;
            return true;
        }
        return false;
    }
    
    // デッドロック回避のため、アカウントのアドレスでロック順序を決定
    static void transfer(BankAccount& from, BankAccount& to, int amount)
    {
        if (&from == &to) return;  // 同じアカウントへの送金は無効
        
        // アドレスの小さい方から順にロック（デッドロック回避）
        BankAccount* first = &from;
        BankAccount* second = &to;
        if (&from > &to)
        {
            first = &to;
            second = &from;
        }
        
        std::lock_guard<std::mutex> lock1(first->mutex_);
        std::lock_guard<std::mutex> lock2(second->mutex_);
        
        if (from.balance_ >= amount)
        {
            from.balance_ -= amount;
            to.balance_ += amount;
            std::cout << from.accountName_ << " から " << to.accountName_ 
                      << " へ " << amount << " 送金完了" << std::endl;
        }
        else
        {
            std::cout << "送金失敗：残高不足" << std::endl;
        }
    }
};

// より高度な同期：unique_lockの柔軟性
class ResourceManager
{
private:
    std::vector<std::string> resources_;
    std::mutex resourceMutex_;
    
public:
    void addResource(const std::string& resource)
    {
        std::unique_lock<std::mutex> lock(resourceMutex_);
        
        // 条件チェック
        if (resources_.size() >= 10)
        {
            std::cout << "リソース上限に達しています" << std::endl;
            return;
        }
        
        // 重い処理のためロックを一時的に解除
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 重い処理をシミュレート
        lock.lock();
        
        // リソースを追加
        resources_.push_back(resource);
        std::cout << "リソース追加: " << resource << " (総数: " << resources_.size() << ")" << std::endl;
    }
    
    std::vector<std::string> getAllResources()
    {
        std::unique_lock<std::mutex> lock(resourceMutex_);
        return resources_;  // コピーを返す
    }
};

// スレッドセーフなキュー
template<typename T>
class ThreadSafeQueue
{
private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    
public:
    void push(const T& item)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(item);
        std::cout << "キューにアイテム追加 (サイズ: " << queue_.size() << ")" << std::endl;
    }
    
    bool pop(T& item)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty())
        {
            return false;
        }
        
        item = queue_.front();
        queue_.pop();
        std::cout << "キューからアイテム取得 (残りサイズ: " << queue_.size() << ")" << std::endl;
        return true;
    }
    
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
    
    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
};

// 実演関数
void demonstrateBasicMutex()
{
    std::cout << "\n=== 基本的なミューテックス ===" << std::endl;
    
    Counter counter;
    std::vector<std::thread> threads;
    
    // 複数スレッドで同時にカウンターを操作
    for (int i = 0; i < 5; ++i)
    {
        threads.emplace_back([&counter, i]() {
            for (int j = 0; j < 3; ++j)
            {
                if (j % 2 == 0)
                    counter.increment();
                else
                    counter.decrement();
                
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }
    
    for (auto& t : threads)
    {
        t.join();
    }
    
    std::cout << "最終カウント: " << counter.getValue() << std::endl;
}

void demonstrateSharedMutex()
{
    std::cout << "\n=== 共有ミューテックス ===" << std::endl;
    
    GameConfig config("TestServer", 20, 1.0f);
    std::vector<std::thread> threads;
    
    // 読み取りスレッド（複数同時実行可能）
    for (int i = 0; i < 3; ++i)
    {
        threads.emplace_back([&config, i]() {
            for (int j = 0; j < 5; ++j)
            {
                std::cout << "読み取りスレッド " << i << ": " 
                          << config.getServerName() << ", 難易度: " 
                          << config.getDifficulty() << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });
    }
    
    // 書き込みスレッド（排他的）
    threads.emplace_back([&config]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        config.updateServerName("UpdatedServer");
        config.updateDifficulty(1.5f);
    });
    
    for (auto& t : threads)
    {
        t.join();
    }
}

int main()
{
    std::cout << "=== ミューテックスと同期 ===" << std::endl;
    
    // 1. 基本的なミューテックス
    demonstrateBasicMutex();
    
    // 2. ゲームスコア管理
    std::cout << "\n=== ゲームスコア管理 ===" << std::endl;
    GameScore gameScore;
    std::vector<std::thread> scoreThreads;
    
    for (int i = 1; i <= 4; ++i)
    {
        scoreThreads.emplace_back([&gameScore, i]() {
            gameScore.addPlayer();
            
            for (int j = 0; j < 3; ++j)
            {
                gameScore.addScore(i, (i * 10) + j);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
            
            gameScore.removePlayer();
        });
    }
    
    for (auto& t : scoreThreads)
    {
        t.join();
    }
    
    std::cout << "最終スコア: " << gameScore.getTotalScore() << std::endl;
    
    // 3. 共有ミューテックス
    demonstrateSharedMutex();
    
    // 4. スレッドセーフキュー
    std::cout << "\n=== スレッドセーフキュー ===" << std::endl;
    ThreadSafeQueue<std::string> taskQueue;
    
    // 生産者スレッド
    std::thread producer([&taskQueue]() {
        for (int i = 0; i < 10; ++i)
        {
            taskQueue.push("タスク" + std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
    
    // 消費者スレッド
    std::thread consumer([&taskQueue]() {
        std::string task;
        int processed = 0;
        
        while (processed < 10)
        {
            if (taskQueue.pop(task))
            {
                std::cout << "処理中: " << task << std::endl;
                processed++;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    });
    
    producer.join();
    consumer.join();
    
    return 0;
}

// まとめ：
// - std::mutexで排他制御を実現
// - lock_guardで自動的なロック管理
// - shared_mutexで読み書き分離
// - unique_lockで柔軟なロック制御
// - デッドロック回避のためのロック順序管理