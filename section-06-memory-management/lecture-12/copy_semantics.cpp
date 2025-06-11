// コピーセマンティクスの深堀り
// ゲーム開発での効率的なオブジェクトコピー管理

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace chrono;

// シャローコピーの問題を示すクラス
class BadGameCharacter
{
public:
    string name;
    int* stats;  // HP, MP, ATK, DEF
    size_t statCount;
    
    BadGameCharacter(const string& name) : name(name), statCount(4)
    {
        stats = new int[statCount];
        stats[0] = 100; // HP
        stats[1] = 50;  // MP
        stats[2] = 20;  // ATK
        stats[3] = 15;  // DEF
        cout << "キャラクター '" << name << "' を作成\n";
    }
    
    // デフォルトコピーコンストラクタ（シャローコピー）
    // BadGameCharacter(const BadGameCharacter& other) = default;
    
    ~BadGameCharacter()
    {
        delete[] stats;
        cout << "キャラクター '" << name << "' を削除\n";
    }
    
    void printStats() const
    {
        cout << name << ": HP=" << stats[0] << ", MP=" << stats[1] 
             << ", ATK=" << stats[2] << ", DEF=" << stats[3] << "\n";
    }
};

// 正しい深いコピーを実装したクラス
class GoodGameCharacter
{
public:
    string name;
    int* stats;
    size_t statCount;
    
    // コンストラクタ
    GoodGameCharacter(const string& name) : name(name), statCount(4)
    {
        stats = new int[statCount];
        stats[0] = 100; // HP
        stats[1] = 50;  // MP
        stats[2] = 20;  // ATK
        stats[3] = 15;  // DEF
        cout << "キャラクター '" << name << "' を作成\n";
    }
    
    // コピーコンストラクタ（深いコピー）
    GoodGameCharacter(const GoodGameCharacter& other)
        : name(other.name + "_コピー"), statCount(other.statCount)
    {
        stats = new int[statCount];
        for (size_t i = 0; i < statCount; ++i)
        {
            stats[i] = other.stats[i];
        }
        cout << "キャラクター '" << name << "' をコピー作成\n";
    }
    
    // コピー代入演算子（深いコピー）
    GoodGameCharacter& operator=(const GoodGameCharacter& other)
    {
        if (this != &other)
        {
            cout << "'" << name << "' に '" << other.name << "' をコピー代入\n";
            
            // 既存のメモリを解放
            delete[] stats;
            
            // 新しいデータをコピー
            name = other.name + "_コピー代入";
            statCount = other.statCount;
            stats = new int[statCount];
            for (size_t i = 0; i < statCount; ++i)
            {
                stats[i] = other.stats[i];
            }
        }
        return *this;
    }
    
    ~GoodGameCharacter()
    {
        delete[] stats;
        cout << "キャラクター '" << name << "' を削除\n";
    }
    
    void printStats() const
    {
        cout << name << ": HP=" << stats[0] << ", MP=" << stats[1] 
             << ", ATK=" << stats[2] << ", DEF=" << stats[3] << "\n";
    }
    
    void modifyStats(int hpBonus, int mpBonus, int atkBonus, int defBonus)
    {
        stats[0] += hpBonus;
        stats[1] += mpBonus;
        stats[2] += atkBonus;
        stats[3] += defBonus;
    }
};

// ゲームアセットクラス（深いコピーの実例）
class GameTexture
{
public:
    string filename;
    unsigned char* pixelData;
    size_t width, height;
    size_t dataSize;
    
    GameTexture(const string& filename, size_t width, size_t height)
        : filename(filename), width(width), height(height)
    {
        dataSize = width * height * 4; // RGBA
        pixelData = new unsigned char[dataSize];
        
        // ダミーデータで初期化
        for (size_t i = 0; i < dataSize; i += 4)
        {
            pixelData[i] = static_cast<unsigned char>(rand() % 256);     // R
            pixelData[i + 1] = static_cast<unsigned char>(rand() % 256); // G
            pixelData[i + 2] = static_cast<unsigned char>(rand() % 256); // B
            pixelData[i + 3] = 255; // A
        }
        
        cout << "テクスチャ '" << filename << "' をロード (" << width << "x" << height << ")\n";
    }
    
    // コピーコンストラクタ
    GameTexture(const GameTexture& other)
        : filename(other.filename + "_copy"), width(other.width), height(other.height), dataSize(other.dataSize)
    {
        pixelData = new unsigned char[dataSize];
        memcpy(pixelData, other.pixelData, dataSize);
        cout << "テクスチャ '" << filename << "' をコピー作成\n";
    }
    
    // コピー代入演算子
    GameTexture& operator=(const GameTexture& other)
    {
        if (this != &other)
        {
            cout << "テクスチャ '" << filename << "' に '" << other.filename << "' をコピー代入\n";
            
            delete[] pixelData;
            
            filename = other.filename + "_assigned";
            width = other.width;
            height = other.height;
            dataSize = other.dataSize;
            
            pixelData = new unsigned char[dataSize];
            memcpy(pixelData, other.pixelData, dataSize);
        }
        return *this;
    }
    
    ~GameTexture()
    {
        delete[] pixelData;
        cout << "テクスチャ '" << filename << "' をアンロード\n";
    }
    
    void modifyPixel(size_t x, size_t y, unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
    {
        if (x < width && y < height)
        {
            size_t index = (y * width + x) * 4;
            pixelData[index] = r;
            pixelData[index + 1] = g;
            pixelData[index + 2] = b;
            pixelData[index + 3] = a;
        }
    }
    
    void printInfo() const
    {
        cout << "  テクスチャ: " << filename << " (" << width << "x" << height << ", " << dataSize << " bytes)\n";
    }
};

// コピーエリションを回避するクラス
class CopyOnWriteTexture
{
private:
    struct TextureData
    {
        unsigned char* pixels;
        size_t width, height, dataSize;
        int refCount;
        
        TextureData(size_t w, size_t h) : width(w), height(h), refCount(1)
        {
            dataSize = width * height * 4;
            pixels = new unsigned char[dataSize];
            // ダミーデータで初期化
            for (size_t i = 0; i < dataSize; ++i)
            {
                pixels[i] = static_cast<unsigned char>(rand() % 256);
            }
            cout << "    TextureDataを作成 (" << width << "x" << height << ")\n";
        }
        
        ~TextureData()
        {
            delete[] pixels;
            cout << "    TextureDataを削除 (" << width << "x" << height << ")\n";
        }
    };
    
    shared_ptr<TextureData> data;
    string name;
    
public:
    CopyOnWriteTexture(const string& name, size_t width, size_t height)
        : name(name), data(make_shared<TextureData>(width, height))
    {
        cout << "COWテクスチャ '" << name << "' を作成\n";
    }
    
    // コピーコンストラクタ（シャローコピー）
    CopyOnWriteTexture(const CopyOnWriteTexture& other)
        : name(other.name + "_cow_copy"), data(other.data)
    {
        cout << "COWテクスチャ '" << name << "' をシャローコピー\n";
    }
    
    // コピー代入演算子
    CopyOnWriteTexture& operator=(const CopyOnWriteTexture& other)
    {
        if (this != &other)
        {
            cout << "COWテクスチャ '" << name << "' に '" << other.name << "' をシャロー代入\n";
            name = other.name + "_cow_assigned";
            data = other.data;
        }
        return *this;
    }
    
    void modifyPixel(size_t x, size_t y, unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
    {
        // 書き込み時にコピーを実行
        if (data.use_count() > 1)
        {
            cout << "    COW: 書き込み時にデータをコピー '" << name << "'\n";
            auto newData = make_shared<TextureData>(data->width, data->height);
            memcpy(newData->pixels, data->pixels, data->dataSize);
            data = newData;
        }
        
        if (x < data->width && y < data->height)
        {
            size_t index = (y * data->width + x) * 4;
            data->pixels[index] = r;
            data->pixels[index + 1] = g;
            data->pixels[index + 2] = b;
            data->pixels[index + 3] = a;
        }
    }
    
    void printInfo() const
    {
        cout << "  COWテクスチャ: " << name << " (" << data->width << "x" << data->height 
             << ", 参照カウント: " << data.use_count() << ")\n";
    }
};

// シャローコピーの問題をデモ
void shallowCopyProblemDemo()
{
    cout << "\n=== シャローコピーの問題 ===\n";
    
    cout << "注意: このデモは意図的にメモリエラーを発生させます\n";
    
    // ここでBadGameCharacterを使用するとダブルフリーが発生する
    cout << "シャローコピーではポインタだけがコピーされ、\n";
    cout << "同じメモリ領域を指すため、一方を削除すると\n";
    cout << "もう一方が無効なポインタを持つことになります。\n";
}

// 深いコピーのデモ
void deepCopyDemo()
{
    cout << "\n=== 深いコピーのデモ ===\n";
    
    {
        GoodGameCharacter hero("勇者");
        hero.printStats();
        
        // コピーコンストラクタで深いコピー
        GoodGameCharacter heroCopy(hero);
        heroCopy.printStats();
        
        // 元のキャラクターを強化
        hero.modifyStats(50, 25, 10, 5);
        cout << "\n元のキャラクターを強化後:\n";
        hero.printStats();
        heroCopy.printStats();
        
        // コピー代入演算子で深いコピー
        GoodGameCharacter mage("魔法使い");
        mage.modifyStats(-20, 30, 5, -5);
        cout << "\nコピー代入前:\n";
        mage.printStats();
        
        mage = hero;
        cout << "\nコピー代入後:\n";
        mage.printStats();
    }
    
    cout << "\nすべてのオブジェクトが正しく削除されました\n";
}

// テクスチャコピーのデモ
void textureCopyDemo()
{
    cout << "\n=== テクスチャコピーのデモ ===\n";
    
    {
        GameTexture originalTexture("player_texture.png", 256, 256);
        originalTexture.printInfo();
        
        // 大きなテクスチャのコピー（コストが高い）
        cout << "\n大きなテクスチャをコピー中...\n";
        GameTexture copiedTexture(originalTexture);
        copiedTexture.printInfo();
        
        // 元のテクスチャを変更
        originalTexture.modifyPixel(0, 0, 255, 0, 0);
        copiedTexture.modifyPixel(0, 0, 0, 255, 0);
        
        cout << "\n各テクスチャは独立して変更されました\n";
        
        // コピー代入のテスト
        GameTexture anotherTexture("enemy_texture.png", 128, 128);
        anotherTexture.printInfo();
        
        anotherTexture = originalTexture;
        anotherTexture.printInfo();
    }
    
    cout << "\nすべてのテクスチャが正しく削除されました\n";
}

// Copy-on-Writeのデモ
void copyOnWriteDemo()
{
    cout << "\n=== Copy-on-Writeのデモ ===\n";
    
    {
        CopyOnWriteTexture original("shared_texture.png", 512, 512);
        original.printInfo();
        
        // シャローコピー（高速）
        cout << "\nシャローコピーで複数のインスタンスを作成...\n";
        CopyOnWriteTexture copy1(original);
        CopyOnWriteTexture copy2(original);
        CopyOnWriteTexture copy3 = original;
        
        cout << "\nコピー後の状態:\n";
        original.printInfo();
        copy1.printInfo();
        copy2.printInfo();
        copy3.printInfo();
        
        // 一つだけを変更（ここで初めてコピーが発生）
        cout << "\ncopy1を変更中...\n";
        copy1.modifyPixel(0, 0, 255, 0, 0);
        
        cout << "\n変更後の状態:\n";
        original.printInfo();
        copy1.printInfo();
        copy2.printInfo();
        copy3.printInfo();
        
        // 別のインスタンスも変更
        cout << "\ncopy2も変更中...\n";
        copy2.modifyPixel(1, 1, 0, 255, 0);
        
        cout << "\n最終状態:\n";
        original.printInfo();
        copy1.printInfo();
        copy2.printInfo();
        copy3.printInfo();
    }
    
    cout << "\nすべてのCOWテクスチャが正しく削除されました\n";
}

// パフォーマンス比較
void performanceComparison()
{
    cout << "\n=== パフォーマンス比較 ===\n";
    
    const int iterations = 1000;
    
    // 深いコピーのパフォーマンス
    {
        auto start = high_resolution_clock::now();
        
        vector<GameTexture> textures;
        GameTexture original("perf_test.png", 128, 128);
        
        for (int i = 0; i < iterations; ++i)
        {
            textures.push_back(original);
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "深いコピー (" << iterations << "回): " << duration << " マイクロ秒\n";
    }
    
    // COWのパフォーマンス
    {
        auto start = high_resolution_clock::now();
        
        vector<CopyOnWriteTexture> textures;
        CopyOnWriteTexture original("cow_perf_test.png", 128, 128);
        
        for (int i = 0; i < iterations; ++i)
        {
            textures.push_back(original);
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "Copy-on-Write (" << iterations << "回): " << duration << " マイクロ秒\n";
    }
}

int main()
{
    cout << "=== コピーセマンティクスの深堀り ===\n";
    
    // シャローコピーの問題
    shallowCopyProblemDemo();
    
    // 深いコピーのデモ
    deepCopyDemo();
    
    // テクスチャコピーのデモ
    textureCopyDemo();
    
    // Copy-on-Writeのデモ
    copyOnWriteDemo();
    
    // パフォーマンス比較
    performanceComparison();
    
    cout << "\n=== コピーセマンティクスの種類 ===\n";
    cout << "1. シャローコピー: ポインタや参照だけをコピー（危険）\n";
    cout << "2. 深いコピー: オブジェクトの内容を完全に複製（安全、但し遅い）\n";
    cout << "3. Copy-on-Write: 書き込み時までシャロー、必要時に深いコピー\n";
    
    cout << "\n=== ゲーム開発での活用例 ===\n";
    cout << "1. テクスチャやメッシュの共有とコピー\n";
    cout << "2. ゲームオブジェクトのテンプレートからのインスタンス生成\n";
    cout << "3. セーブデータのバックアップと復元\n";
    cout << "4. アニメーションフレームの中間状態保存\n";
    cout << "5. マルチプレイでのゲーム状態の同期\n";
    
    cout << "\n=== ベストプラクティス ===\n";
    cout << "1. Rule of Three/Five: コピーコンストラクタ、コピー代入、デストラクタをセットで定義\n";
    cout << "2. 深いコピーはコストが高いことを認識し、必要最小限に\n";
    cout << "3. 大きなデータはshared_ptrやCOWで共有を検討\n";
    cout << "4. コピーの代わりにmoveセマンティクスを活用\n";
    cout << "5. const参照でパラメータを渡して不要なコピーを回避\n";
    
    return 0;
}

// コピーセマンティクスの重要なポイント：
// 1. メモリを動的に管理するクラスでは深いコピーが必要
// 2. コピーコンストラクタとコピー代入演算子はセットで実装
// 3. self-assignment（自己代入）のチェックを必ず実装
// 4. パフォーマンスを考慮してCOWや参照カウントを活用
// 5. ゲームでは特にメモリやパフォーマンスが重要なので慎重に設計