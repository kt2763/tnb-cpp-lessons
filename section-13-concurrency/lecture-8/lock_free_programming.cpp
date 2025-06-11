// ロックフリープログラミング
// 高性能な並行データ構造の実装

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <memory>
#include <chrono>
#include <cassert>

// ロックフリーな単方向リンクリスト
template<typename T>
class LockFreeLinkedList
{
private:
    struct Node
    {
        std::atomic<T> data;
        std::atomic<Node*> next;
        
        Node(const T& value) : data(value), next(nullptr) {}
    };
    
    std::atomic<Node*> head_;
    
public:
    LockFreeLinkedList() : head_(nullptr) {}
    
    ~LockFreeLinkedList()
    {
        while (Node* const old_head = head_.load())
        {
            head_.store(old_head->next);
            delete old_head;
        }
    }
    
    void push_front(const T& data)
    {
        Node* new_node = new Node(data);
        new_node->next = head_.load();
        
        // Compare-and-swapでアトミックに挿入
        while (!head_.compare_exchange_weak(new_node->next, new_node))
        {
            // 失敗した場合、new_node->nextは最新のheadに更新される
        }
        
        std::cout << "追加: " << data << " (スレッド: " << std::this_thread::get_id() << ")" << std::endl;
    }
    
    bool pop_front(T& result)
    {
        Node* old_head = head_.load();
        
        // 空でない限り、headを次のノードに進める
        while (old_head && !head_.compare_exchange_weak(old_head, old_head->next))
        {
            // CASが失敗した場合、old_headは最新のheadに更新される
        }
        
        if (old_head)
        {
            result = old_head->data.load();
            std::cout << "削除: " << result << " (スレッド: " << std::this_thread::get_id() << ")" << std::endl;
            delete old_head;
            return true;
        }
        
        return false;  // リストが空
    }
    
    void print_all() const
    {
        Node* current = head_.load();
        std::cout << "リスト内容: ";
        while (current)
        {
            std::cout << current->data.load() << " ";
            current = current->next.load();
        }
        std::cout << std::endl;
    }
};

// ロックフリーキュー（Michael & Scott algorithm）
template<typename T>
class LockFreeQueue
{
private:
    struct Node
    {
        std::atomic<T*> data;
        std::atomic<Node*> next;
        
        Node() : data(nullptr), next(nullptr) {}
    };
    
    std::atomic<Node*> head_;
    std::atomic<Node*> tail_;
    
public:
    LockFreeQueue()
    {
        Node* dummy = new Node;
        head_.store(dummy);
        tail_.store(dummy);
    }
    
    ~LockFreeQueue()
    {
        while (Node* const old_head = head_.load())
        {
            head_.store(old_head->next);
            delete old_head;
        }
    }
    
    void enqueue(const T& item)
    {
        Node* new_node = new Node;
        T* data = new T(std::move(item));
        new_node->data.store(data);
        
        Node* last = tail_.load();
        
        while (true)
        {
            Node* next = last->next.load();
            
            if (last == tail_.load())  // tailが変更されていないか確認
            {
                if (next == nullptr)
                {
                    // tailの次にnew_nodeを設定
                    if (last->next.compare_exchange_weak(next, new_node))
                    {
                        break;  // 成功
                    }
                }
                else
                {
                    // tailを進める
                    tail_.compare_exchange_weak(last, next);
                }
            }
            
            last = tail_.load();
        }
        
        // tailを新しいノードに更新
        tail_.compare_exchange_weak(last, new_node);
        
        std::cout << "エンキュー: " << item << " (スレッド: " << std::this_thread::get_id() << ")" << std::endl;
    }
    
    bool dequeue(T& result)
    {
        Node* first = head_.load();
        
        while (true)
        {
            Node* last = tail_.load();
            Node* next = first->next.load();
            
            if (first == head_.load())  // headが変更されていないか確認
            {
                if (first == last)
                {
                    if (next == nullptr)
                    {
                        return false;  // キューが空
                    }
                    
                    // tailを進める
                    tail_.compare_exchange_weak(last, next);
                }
                else if (next != nullptr)
                {
                    // データを読み取り
                    T* data = next->data.load();
                    
                    // headを進める
                    if (head_.compare_exchange_weak(first, next))
                    {
                        result = *data;
                        delete data;
                        delete first;
                        std::cout << "デキュー: " << result << " (スレッド: " << std::this_thread::get_id() << ")" << std::endl;
                        return true;
                    }
                }
            }
            
            first = head_.load();
        }
    }
    
    bool empty() const
    {
        Node* first = head_.load();
        Node* last = tail_.load();
        return (first == last) && (first->next.load() == nullptr);
    }
};

// ロックフリーなハッシュテーブル（簡易版）
template<typename K, typename V, size_t Size = 16>
class LockFreeHashMap
{
private:
    struct Entry
    {
        std::atomic<K> key;
        std::atomic<V> value;
        std::atomic<bool> valid;
        
        Entry() : valid(false) {}
    };
    
    Entry table_[Size];
    
    size_t hash(const K& key) const
    {
        return std::hash<K>{}(key) % Size;
    }
    
public:
    bool insert(const K& key, const V& value)
    {
        size_t index = hash(key);
        size_t original_index = index;
        
        do
        {
            bool expected = false;
            if (table_[index].valid.compare_exchange_weak(expected, true))
            {
                // スロットを確保できた
                table_[index].key.store(key);
                table_[index].value.store(value);
                std::cout << "挿入: [" << key << "] = " << value 
                          << " (インデックス: " << index << ")" << std::endl;
                return true;
            }
            
            // スロットが使用中の場合、キーが同じかチェック
            if (table_[index].key.load() == key)
            {
                // 既存のキーを更新
                table_[index].value.store(value);
                std::cout << "更新: [" << key << "] = " << value 
                          << " (インデックス: " << index << ")" << std::endl;
                return true;
            }
            
            // 線形探査
            index = (index + 1) % Size;
        } while (index != original_index);
        
        return false;  // テーブルが満杯
    }
    
    bool find(const K& key, V& value) const
    {
        size_t index = hash(key);
        size_t original_index = index;
        
        do
        {
            if (!table_[index].valid.load())
            {
                return false;  // 空のスロット = キーが存在しない
            }
            
            if (table_[index].key.load() == key)
            {
                value = table_[index].value.load();
                return true;
            }
            
            index = (index + 1) % Size;
        } while (index != original_index);
        
        return false;
    }
    
    void print_all() const
    {
        std::cout << "ハッシュテーブル内容:" << std::endl;
        for (size_t i = 0; i < Size; ++i)
        {
            if (table_[i].valid.load())
            {
                std::cout << "  [" << i << "] " << table_[i].key.load() 
                          << " -> " << table_[i].value.load() << std::endl;
            }
        }
    }
};

// ロックフリーなリングバッファ
template<typename T, size_t Size>
class LockFreeRingBuffer
{
private:
    struct alignas(64) Entry  // キャッシュライン境界にアライン
    {
        std::atomic<T> data;
        std::atomic<bool> valid;
        
        Entry() : valid(false) {}
    };
    
    Entry buffer_[Size];
    alignas(64) std::atomic<size_t> head_;  // キャッシュライン分離
    alignas(64) std::atomic<size_t> tail_;
    
public:
    LockFreeRingBuffer() : head_(0), tail_(0) {}
    
    bool push(const T& item)
    {
        size_t current_tail = tail_.load(std::memory_order_relaxed);
        size_t next_tail = (current_tail + 1) % Size;
        
        // バッファが満杯でないかチェック
        if (next_tail == head_.load(std::memory_order_acquire))
        {
            return false;  // 満杯
        }
        
        // データを書き込み
        buffer_[current_tail].data.store(item, std::memory_order_relaxed);
        buffer_[current_tail].valid.store(true, std::memory_order_release);
        
        // tailを進める
        tail_.store(next_tail, std::memory_order_release);
        
        std::cout << "リングバッファに追加: " << item << std::endl;
        return true;
    }
    
    bool pop(T& item)
    {
        size_t current_head = head_.load(std::memory_order_relaxed);
        
        // バッファが空でないかチェック
        if (current_head == tail_.load(std::memory_order_acquire))
        {
            return false;  // 空
        }
        
        // データが有効か確認
        if (!buffer_[current_head].valid.load(std::memory_order_acquire))
        {
            return false;
        }
        
        // データを読み取り
        item = buffer_[current_head].data.load(std::memory_order_relaxed);
        buffer_[current_head].valid.store(false, std::memory_order_release);
        
        // headを進める
        head_.store((current_head + 1) % Size, std::memory_order_release);
        
        std::cout << "リングバッファから取得: " << item << std::endl;
        return true;
    }
    
    size_t size() const
    {
        size_t head = head_.load(std::memory_order_acquire);
        size_t tail = tail_.load(std::memory_order_acquire);
        
        if (tail >= head)
        {
            return tail - head;
        }
        else
        {
            return Size - head + tail;
        }
    }
    
    bool empty() const
    {
        return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
    }
};

// 実演関数
void demonstrateLockFreeList()
{
    std::cout << "\n=== ロックフリーリンクリスト ===" << std::endl;
    
    LockFreeLinkedList<int> list;
    std::vector<std::thread> threads;
    
    // 複数スレッドでデータを追加
    for (int i = 0; i < 4; ++i)
    {
        threads.emplace_back([&list, i]() {
            for (int j = 0; j < 5; ++j)
            {
                list.push_front(i * 10 + j);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });
    }
    
    // データを削除するスレッド
    threads.emplace_back([&list]() {
        int value;
        for (int i = 0; i < 10; ++i)
        {
            if (list.pop_front(value))
            {
                // 削除成功
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
    
    for (auto& t : threads)
    {
        t.join();
    }
    
    list.print_all();
}

void demonstrateLockFreeQueue()
{
    std::cout << "\n=== ロックフリーキュー ===" << std::endl;
    
    LockFreeQueue<std::string> queue;
    std::vector<std::thread> threads;
    
    // 生産者スレッド
    for (int i = 0; i < 2; ++i)
    {
        threads.emplace_back([&queue, i]() {
            for (int j = 0; j < 5; ++j)
            {
                queue.enqueue("Item-" + std::to_string(i) + "-" + std::to_string(j));
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }
    
    // 消費者スレッド
    threads.emplace_back([&queue]() {
        std::string item;
        int count = 0;
        
        while (count < 10)
        {
            if (queue.dequeue(item))
            {
                count++;
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    });
    
    for (auto& t : threads)
    {
        t.join();
    }
}

void demonstrateLockFreeHashMap()
{
    std::cout << "\n=== ロックフリーハッシュマップ ===" << std::endl;
    
    LockFreeHashMap<int, std::string> hashmap;
    std::vector<std::thread> threads;
    
    // 複数スレッドでデータを挿入
    for (int i = 0; i < 3; ++i)
    {
        threads.emplace_back([&hashmap, i]() {
            for (int j = 0; j < 4; ++j)
            {
                int key = i * 10 + j;
                std::string value = "Value" + std::to_string(key);
                hashmap.insert(key, value);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });
    }
    
    for (auto& t : threads)
    {
        t.join();
    }
    
    hashmap.print_all();
    
    // 検索テスト
    std::string result;
    if (hashmap.find(15, result))
    {
        std::cout << "検索成功: [15] = " << result << std::endl;
    }
    else
    {
        std::cout << "キー 15 が見つかりません" << std::endl;
    }
}

void demonstrateLockFreeRingBuffer()
{
    std::cout << "\n=== ロックフリーリングバッファ ===" << std::endl;
    
    LockFreeRingBuffer<int, 8> buffer;
    std::vector<std::thread> threads;
    
    // 生産者
    threads.emplace_back([&buffer]() {
        for (int i = 1; i <= 12; ++i)
        {
            while (!buffer.push(i))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(80));
        }
    });
    
    // 消費者
    threads.emplace_back([&buffer]() {
        int value;
        int count = 0;
        
        while (count < 12)
        {
            if (buffer.pop(value))
            {
                count++;
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(120));
        }
    });
    
    for (auto& t : threads)
    {
        t.join();
    }
}

int main()
{
    std::cout << "=== ロックフリープログラミング ===" << std::endl;
    
    // 1. ロックフリーリンクリスト
    demonstrateLockFreeList();
    
    // 2. ロックフリーキュー
    demonstrateLockFreeQueue();
    
    // 3. ロックフリーハッシュマップ
    demonstrateLockFreeHashMap();
    
    // 4. ロックフリーリングバッファ
    demonstrateLockFreeRingBuffer();
    
    return 0;
}

// まとめ：
// - ロックフリーデータ構造は高い並行性を実現
// - Compare-and-swap（CAS）操作が基本
// - メモリオーダリングで正しい同期を保証
// - ABA問題やメモリ管理に注意が必要
// - 高性能が要求される場面で有効