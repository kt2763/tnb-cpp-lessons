// スレッドプール
// 効率的なスレッド管理と作業の分散

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <chrono>

// 基本的なスレッドプール
class BasicThreadPool
{
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    bool stop_;
    
public:
    explicit BasicThreadPool(size_t numThreads) : stop_(false)
    {
        for (size_t i = 0; i < numThreads; ++i)
        {
            workers_.emplace_back([this, i] {
                std::cout << "ワーカースレッド " << i << " 開始" << std::endl;
                
                while (true)
                {
                    std::function<void()> task;
                    
                    {
                        std::unique_lock<std::mutex> lock(queueMutex_);
                        condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                        
                        if (stop_ && tasks_.empty())
                            return;
                        
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    
                    task();
                }
            });
        }
    }
    
    ~BasicThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            stop_ = true;
        }
        
        condition_.notify_all();
        
        for (std::thread& worker : workers_)
        {
            worker.join();
        }
    }
    
    template<class F>
    void enqueue(F&& f)
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            if (stop_)
                throw std::runtime_error("スレッドプールが停止されています");
            
            tasks_.emplace(std::forward<F>(f));
        }
        condition_.notify_one();
    }
};

// 高度なスレッドプール（戻り値サポート）
class AdvancedThreadPool
{
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    bool stop_;
    
public:
    explicit AdvancedThreadPool(size_t numThreads) : stop_(false)
    {
        for (size_t i = 0; i < numThreads; ++i)
        {
            workers_.emplace_back([this] {
                while (true)
                {
                    std::function<void()> task;
                    
                    {
                        std::unique_lock<std::mutex> lock(queueMutex_);
                        condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                        
                        if (stop_ && tasks_.empty())
                            return;
                        
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    
                    task();
                }
            });
        }
    }
    
    ~AdvancedThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            stop_ = true;
        }
        
        condition_.notify_all();
        
        for (std::thread& worker : workers_)
        {
            worker.join();
        }
    }
    
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using return_type = typename std::result_of<F(Args...)>::type;
        
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<return_type> result = task->get_future();
        
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            if (stop_)
                throw std::runtime_error("スレッドプールが停止されています");
            
            tasks_.emplace([task](){ (*task)(); });
        }
        
        condition_.notify_one();
        return result;
    }
    
    size_t getWorkerCount() const { return workers_.size(); }
};

// ゲーム専用のタスクプール
class GameTaskPool
{
public:
    enum class TaskPriority
    {
        Low = 0,
        Normal = 1,
        High = 2,
        Critical = 3
    };
    
    struct Task
    {
        std::function<void()> function;
        TaskPriority priority;
        std::string name;
        std::chrono::steady_clock::time_point submitTime;
        
        Task(std::function<void()> f, TaskPriority p, const std::string& n)
            : function(f), priority(p), name(n), submitTime(std::chrono::steady_clock::now()) {}
        
        bool operator<(const Task& other) const
        {
            return priority < other.priority;  // 優先度キューでは逆順
        }
    };
    
private:
    std::vector<std::thread> workers_;
    std::priority_queue<Task> tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_;
    std::atomic<int> activeTasks_;
    
public:
    explicit GameTaskPool(size_t numThreads) : stop_(false), activeTasks_(0)
    {
        for (size_t i = 0; i < numThreads; ++i)
        {
            workers_.emplace_back([this, i] {
                std::cout << "ゲームワーカー " << i << " 開始" << std::endl;
                
                while (!stop_.load())
                {
                    Task task("", TaskPriority::Low, "");
                    
                    {
                        std::unique_lock<std::mutex> lock(queueMutex_);
                        condition_.wait(lock, [this] { return stop_.load() || !tasks_.empty(); });
                        
                        if (stop_.load() && tasks_.empty())
                            return;
                        
                        if (!tasks_.empty())
                        {
                            task = tasks_.top();
                            tasks_.pop();
                        }
                        else
                        {
                            continue;
                        }
                    }
                    
                    activeTasks_.fetch_add(1);
                    
                    auto startTime = std::chrono::steady_clock::now();
                    std::cout << "実行開始: " << task.name << " (優先度: " 
                              << static_cast<int>(task.priority) << ")" << std::endl;
                    
                    task.function();
                    
                    auto endTime = std::chrono::steady_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                    auto waitTime = std::chrono::duration_cast<std::chrono::milliseconds>(startTime - task.submitTime);
                    
                    std::cout << "実行完了: " << task.name 
                              << " (実行時間: " << duration.count() << "ms"
                              << ", 待機時間: " << waitTime.count() << "ms)" << std::endl;
                    
                    activeTasks_.fetch_sub(1);
                }
            });
        }
    }
    
    ~GameTaskPool()
    {
        stop_.store(true);
        condition_.notify_all();
        
        for (std::thread& worker : workers_)
        {
            worker.join();
        }
    }
    
    void submitTask(std::function<void()> f, TaskPriority priority, const std::string& name)
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            if (stop_.load())
                return;
            
            tasks_.emplace(f, priority, name);
        }
        condition_.notify_one();
    }
    
    int getActiveTasks() const { return activeTasks_.load(); }
    
    size_t getPendingTasks() const
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        return tasks_.size();
    }
};

// ワークスティーリングプール（より高度な実装の簡易版）
class WorkStealingPool
{
private:
    struct WorkerData
    {
        std::queue<std::function<void()>> localQueue;
        std::mutex localMutex;
    };
    
    std::vector<std::thread> workers_;
    std::vector<std::unique_ptr<WorkerData>> workerData_;
    std::atomic<bool> stop_;
    std::atomic<int> workersWaiting_;
    std::condition_variable globalCondition_;
    std::mutex globalMutex_;
    
public:
    explicit WorkStealingPool(size_t numThreads) : stop_(false), workersWaiting_(0)
    {
        workerData_.resize(numThreads);
        
        for (size_t i = 0; i < numThreads; ++i)
        {
            workerData_[i] = std::make_unique<WorkerData>();
            
            workers_.emplace_back([this, i] {
                auto& localData = *workerData_[i];
                
                while (!stop_.load())
                {
                    std::function<void()> task;
                    bool foundTask = false;
                    
                    // ローカルキューから作業を探す
                    {
                        std::lock_guard<std::mutex> lock(localData.localMutex);
                        if (!localData.localQueue.empty())
                        {
                            task = localData.localQueue.front();
                            localData.localQueue.pop();
                            foundTask = true;
                        }
                    }
                    
                    // ローカルにない場合、他のワーカーから盗む
                    if (!foundTask)
                    {
                        for (size_t j = 0; j < workerData_.size(); ++j)
                        {
                            if (j == i) continue;
                            
                            auto& otherData = *workerData_[j];
                            std::lock_guard<std::mutex> lock(otherData.localMutex);
                            
                            if (!otherData.localQueue.empty())
                            {
                                task = otherData.localQueue.front();
                                otherData.localQueue.pop();
                                foundTask = true;
                                std::cout << "ワーカー " << i << " がワーカー " << j << " から作業を盗取" << std::endl;
                                break;
                            }
                        }
                    }
                    
                    if (foundTask)
                    {
                        task();
                    }
                    else
                    {
                        // 作業がない場合は少し待機
                        std::unique_lock<std::mutex> lock(globalMutex_);
                        workersWaiting_.fetch_add(1);
                        globalCondition_.wait_for(lock, std::chrono::milliseconds(100));
                        workersWaiting_.fetch_sub(1);
                    }
                }
            });
        }
    }
    
    ~WorkStealingPool()
    {
        stop_.store(true);
        globalCondition_.notify_all();
        
        for (std::thread& worker : workers_)
        {
            worker.join();
        }
    }
    
    void submit(std::function<void()> task, size_t preferredWorker = 0)
    {
        if (preferredWorker >= workerData_.size())
            preferredWorker = 0;
        
        auto& workerData = *workerData_[preferredWorker];
        
        {
            std::lock_guard<std::mutex> lock(workerData.localMutex);
            workerData.localQueue.push(task);
        }
        
        globalCondition_.notify_one();
    }
};

// 実演関数
void demonstrateBasicThreadPool()
{
    std::cout << "\n=== 基本的なスレッドプール ===" << std::endl;
    
    BasicThreadPool pool(4);
    
    // タスクを投入
    for (int i = 1; i <= 8; ++i)
    {
        pool.enqueue([i] {
            std::cout << "タスク " << i << " 実行開始" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            std::cout << "タスク " << i << " 実行完了" << std::endl;
        });
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void demonstrateAdvancedThreadPool()
{
    std::cout << "\n=== 高度なスレッドプール ===" << std::endl;
    
    AdvancedThreadPool pool(3);
    
    // 戻り値のあるタスク
    std::vector<std::future<int>> results;
    
    for (int i = 1; i <= 6; ++i)
    {
        results.emplace_back(pool.enqueue([i] {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            int result = i * i;
            std::cout << "計算完了: " << i << " * " << i << " = " << result << std::endl;
            return result;
        }));
    }
    
    // 結果を取得
    std::cout << "計算結果: ";
    for (auto& result : results)
    {
        std::cout << result.get() << " ";
    }
    std::cout << std::endl;
}

void demonstrateGameTaskPool()
{
    std::cout << "\n=== ゲーム専用タスクプール ===" << std::endl;
    
    GameTaskPool gamePool(3);
    
    // 様々な優先度のタスクを投入
    gamePool.submitTask(
        [] { 
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
            std::cout << "  物理演算処理完了" << std::endl;
        },
        GameTaskPool::TaskPriority::High,
        "物理演算"
    );
    
    gamePool.submitTask(
        [] { 
            std::this_thread::sleep_for(std::chrono::milliseconds(600));
            std::cout << "  AI思考処理完了" << std::endl;
        },
        GameTaskPool::TaskPriority::Normal,
        "AI思考"
    );
    
    gamePool.submitTask(
        [] { 
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            std::cout << "  緊急イベント処理完了" << std::endl;
        },
        GameTaskPool::TaskPriority::Critical,
        "緊急イベント"
    );
    
    gamePool.submitTask(
        [] { 
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            std::cout << "  リソース最適化完了" << std::endl;
        },
        GameTaskPool::TaskPriority::Low,
        "リソース最適化"
    );
    
    gamePool.submitTask(
        [] { 
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            std::cout << "  レンダリング処理完了" << std::endl;
        },
        GameTaskPool::TaskPriority::High,
        "レンダリング"
    );
    
    // 状態監視
    while (gamePool.getActiveTasks() > 0 || gamePool.getPendingTasks() > 0)
    {
        std::cout << "アクティブタスク: " << gamePool.getActiveTasks() 
                  << ", 待機タスク: " << gamePool.getPendingTasks() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main()
{
    std::cout << "=== スレッドプール ===" << std::endl;
    
    // 1. 基本的なスレッドプール
    demonstrateBasicThreadPool();
    
    // 2. 高度なスレッドプール
    demonstrateAdvancedThreadPool();
    
    // 3. ゲーム専用タスクプール
    demonstrateGameTaskPool();
    
    // 4. ワークスティーリング
    std::cout << "\n=== ワークスティーリング ===" << std::endl;
    WorkStealingPool stealingPool(4);
    
    // 不均等な作業負荷を投入
    for (int i = 0; i < 10; ++i)
    {
        stealingPool.submit([i] {
            int sleepTime = (i % 3 + 1) * 200;  // 不均等な処理時間
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
            std::cout << "ワークスティーリングタスク " << i << " 完了" << std::endl;
        }, i % 4);  // 異なるワーカーに振り分け
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    
    return 0;
}

// まとめ：
// - スレッドプールでスレッドの再利用を実現
// - 優先度付きタスクキューで重要度管理
// - 戻り値のあるタスクも効率的に処理
// - ワークスティーリングで負荷分散
// - ゲーム開発ではフレーム処理や背景タスクに活用