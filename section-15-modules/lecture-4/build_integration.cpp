// ビルドシステム統合
// モジュールを使ったプロジェクトのビルド設定と最適化

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <chrono>

// 注意：このファイルはビルドシステム統合の概念を説明するためのものです
// 実際のモジュールビルドには対応するコンパイラとビルドツールが必要です

// =======================================
// ビルドシステム統合の概念
// =======================================

namespace build_integration_concepts
{
    void explainBuildConcepts()
    {
        std::cout << "=== ビルドシステム統合の概念 ===" << std::endl;
        
        std::cout << "\n1. モジュールビルドの特徴" << std::endl;
        std::cout << "   - BMI（Binary Module Interface）ファイルの生成" << std::endl;
        std::cout << "   - 依存関係の自動解決" << std::endl;
        std::cout << "   - 並列ビルドの最適化" << std::endl;
        std::cout << "   - インクリメンタルビルドの改善" << std::endl;
        
        std::cout << "\n2. 従来のビルドとの違い" << std::endl;
        std::cout << "   従来: .h → .cpp → .o → .exe" << std::endl;
        std::cout << "   モジュール: .cppm → .bmi + .o → .exe" << std::endl;
        std::cout << "   - プリプロセッサ処理が削減" << std::endl;
        std::cout << "   - テンプレートの重複解決が削減" << std::endl;
        
        std::cout << "\n3. ビルドツール対応状況" << std::endl;
        std::cout << "   - CMake 3.20+: 実験的サポート" << std::endl;
        std::cout << "   - Ninja: モジュール対応" << std::endl;
        std::cout << "   - Visual Studio: 2019 16.8+でサポート" << std::endl;
        std::cout << "   - GCC: 11+で実験的サポート" << std::endl;
        std::cout << "   - Clang: 実験的サポート" << std::endl;
        
        std::cout << "\n4. ビルド時間の改善" << std::endl;
        std::cout << "   - 初回ビルド: やや遅くなる可能性" << std::endl;
        std::cout << "   - インクリメンタルビルド: 大幅改善" << std::endl;
        std::cout << "   - 並列ビルド: 依存関係最適化で改善" << std::endl;
    }
}

// =======================================
// CMakeサンプル設定
// =======================================

namespace cmake_configuration
{
    void generateCMakeFiles()
    {
        std::cout << "\n=== CMake設定サンプル ===" << std::endl;
        
        // CMakeLists.txt のサンプル
        std::string cmakeContent = R"(
# CMakeLists.txt - C++20モジュール対応
cmake_minimum_required(VERSION 3.20)
project(GameEngine VERSION 1.0.0 LANGUAGES CXX)

# C++20とモジュール機能を有効化
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 実験的モジュールサポートを有効化
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API 2182bf5c-ef0d-489a-91da-49dbc3090d2a)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)

# コンパイラ固有の設定
if(MSVC)
    add_compile_options(/experimental:module)
    add_compile_options(/std:c++20)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-fmodules-ts)
    add_compile_options(-std=c++20)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-fmodules)
    add_compile_options(-std=c++20)
endif()

# モジュールライブラリの定義
add_library(GameEngineModules)

# モジュールファイルの追加
target_sources(GameEngineModules
    PUBLIC
        FILE_SET CXX_MODULES FILES
            src/modules/game_engine.cppm
            src/modules/graphics.cppm
            src/modules/audio.cppm
            src/modules/input.cppm
)

# 実装ファイルの追加
target_sources(GameEngineModules
    PRIVATE
        src/graphics_impl.cpp
        src/audio_impl.cpp
        src/input_impl.cpp
)

# インクルードディレクトリ
target_include_directories(GameEngineModules
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src
)

# 外部ライブラリの依存関係
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

target_link_libraries(GameEngineModules
    PUBLIC
        OpenGL::GL
        ${SDL2_LIBRARIES}
)

# メインアプリケーション
add_executable(GameApp
    src/main.cpp
)

target_link_libraries(GameApp
    PRIVATE
        GameEngineModules
)

# インストール設定
install(TARGETS GameEngineModules GameApp
    EXPORT GameEngineTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    FILE_SET CXX_MODULES DESTINATION lib/modules
)

install(EXPORT GameEngineTargets
    FILE GameEngineTargets.cmake
    NAMESPACE GameEngine::
    DESTINATION lib/cmake/GameEngine
)

# パッケージ設定ファイル
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    GameEngineConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/GameEngineConfigVersion.cmake
    DESTINATION lib/cmake/GameEngine
)
)";
        
        std::cout << "CMakeLists.txt の内容:" << std::endl;
        std::cout << cmakeContent << std::endl;
        
        // プリセット設定のサンプル
        std::string presetContent = R"(
{
    "version": 3,
    "configurePresets": [
        {
            "name": "default",
            "displayName": "Default Config",
            "description": "Default build using Ninja generator",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release",
                "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
            }
        },
        {
            "name": "debug",
            "inherits": "default",
            "displayName": "Debug Config",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug"
            }
        },
        {
            "name": "modules-debug",
            "inherits": "debug",
            "displayName": "Modules Debug",
            "cacheVariables": {
                "CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API": "2182bf5c-ef0d-489a-91da-49dbc3090d2a",
                "CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP": "1"
            }
        }
    ],
    "buildPresets": [
        {
            "name": "default",
            "configurePreset": "default"
        },
        {
            "name": "debug",
            "configurePreset": "debug"
        }
    ]
}
)";
        
        std::cout << "\nCMakePresets.json の内容:" << std::endl;
        std::cout << presetContent << std::endl;
    }
}

// =======================================
// ビルド時間測定ツール
// =======================================

namespace build_timing
{
    struct BuildStep
    {
        std::string name;
        std::chrono::milliseconds duration;
        std::vector<std::string> inputs;
        std::vector<std::string> outputs;
        
        BuildStep(const std::string& n) : name(n), duration(0) {}
    };
    
    class BuildProfiler
    {
    private:
        std::vector<BuildStep> steps_;
        std::chrono::steady_clock::time_point startTime_;
        
    public:
        void startBuild()
        {
            std::cout << "\n=== ビルドプロファイリング開始 ===" << std::endl;
            startTime_ = std::chrono::steady_clock::now();
            steps_.clear();
        }
        
        void addStep(const std::string& name, 
                    const std::vector<std::string>& inputs,
                    const std::vector<std::string>& outputs,
                    std::chrono::milliseconds duration)
        {
            BuildStep step(name);
            step.duration = duration;
            step.inputs = inputs;
            step.outputs = outputs;
            steps_.push_back(step);
        }
        
        void finishBuild()
        {
            auto endTime = std::chrono::steady_clock::now();
            auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
                endTime - startTime_);
            
            std::cout << "\n=== ビルドプロファイル結果 ===" << std::endl;
            std::cout << "総ビルド時間: " << totalDuration.count() << "ms" << std::endl;
            
            std::cout << "\n各ステップの詳細:" << std::endl;
            for (const auto& step : steps_)
            {
                std::cout << "  " << step.name << ": " << step.duration.count() << "ms" << std::endl;
                
                if (!step.inputs.empty())
                {
                    std::cout << "    入力: ";
                    for (size_t i = 0; i < step.inputs.size(); ++i)
                    {
                        if (i > 0) std::cout << ", ";
                        std::cout << step.inputs[i];
                    }
                    std::cout << std::endl;
                }
                
                if (!step.outputs.empty())
                {
                    std::cout << "    出力: ";
                    for (size_t i = 0; i < step.outputs.size(); ++i)
                    {
                        if (i > 0) std::cout << ", ";
                        std::cout << step.outputs[i];
                    }
                    std::cout << std::endl;
                }
            }
            
            // 最も時間のかかるステップ
            auto maxElement = std::max_element(steps_.begin(), steps_.end(),
                [](const BuildStep& a, const BuildStep& b) {
                    return a.duration < b.duration;
                });
            
            if (maxElement != steps_.end())
            {
                std::cout << "\n最も時間のかかるステップ: " << maxElement->name 
                          << " (" << maxElement->duration.count() << "ms)" << std::endl;
            }
        }
        
        void simulateModuleBuild()
        {
            startBuild();
            
            // ステップ1: 基礎モジュールのコンパイル
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            addStep("math.cppm -> math.bmi",
                   {"src/modules/math.cppm"},
                   {"build/math.bmi", "build/math.o"},
                   std::chrono::milliseconds(150));
            
            // ステップ2: グラフィックスモジュール（mathに依存）
            std::this_thread::sleep_for(std::chrono::milliseconds(75));
            addStep("graphics.cppm -> graphics.bmi",
                   {"src/modules/graphics.cppm", "build/math.bmi"},
                   {"build/graphics.bmi", "build/graphics.o"},
                   std::chrono::milliseconds(200));
            
            // ステップ3: オーディオモジュール（独立）
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
            addStep("audio.cppm -> audio.bmi",
                   {"src/modules/audio.cppm"},
                   {"build/audio.bmi", "build/audio.o"},
                   std::chrono::milliseconds(120));
            
            // ステップ4: ゲームエンジンモジュール（全てに依存）
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            addStep("game_engine.cppm -> game_engine.bmi",
                   {"src/modules/game_engine.cppm", "build/math.bmi", 
                    "build/graphics.bmi", "build/audio.bmi"},
                   {"build/game_engine.bmi", "build/game_engine.o"},
                   std::chrono::milliseconds(250));
            
            // ステップ5: メインアプリケーション
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            addStep("main.cpp -> main.o",
                   {"src/main.cpp", "build/game_engine.bmi"},
                   {"build/main.o"},
                   std::chrono::milliseconds(80));
            
            // ステップ6: リンク
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
            addStep("Link executable",
                   {"build/math.o", "build/graphics.o", "build/audio.o", 
                    "build/game_engine.o", "build/main.o"},
                   {"build/GameApp"},
                   std::chrono::milliseconds(100));
            
            finishBuild();
        }
        
        void simulateTraditionalBuild()
        {
            std::cout << "\n=== 従来のビルドとの比較 ===" << std::endl;
            startBuild();
            
            // 従来のヘッダーベースビルド（各.cppファイルが大量のヘッダーを処理）
            std::this_thread::sleep_for(std::chrono::milliseconds(120));
            addStep("math.cpp (with headers)",
                   {"src/math.cpp", "include/math.h", "include/common.h", "include/types.h"},
                   {"build/math.o"},
                   std::chrono::milliseconds(300));  // ヘッダー処理で遅い
            
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
            addStep("graphics.cpp (with headers)",
                   {"src/graphics.cpp", "include/graphics.h", "include/math.h", 
                    "include/common.h", "include/types.h"},
                   {"build/graphics.o"},
                   std::chrono::milliseconds(400));  // より多くのヘッダー
            
            std::this_thread::sleep_for(std::chrono::milliseconds(130));
            addStep("audio.cpp (with headers)",
                   {"src/audio.cpp", "include/audio.h", "include/common.h", "include/types.h"},
                   {"build/audio.o"},
                   std::chrono::milliseconds(320));
            
            std::this_thread::sleep_for(std::chrono::milliseconds(180));
            addStep("game_engine.cpp (with headers)",
                   {"src/game_engine.cpp", "include/game_engine.h", "include/graphics.h",
                    "include/audio.h", "include/math.h", "include/common.h", "include/types.h"},
                   {"build/game_engine.o"},
                   std::chrono::milliseconds(500));  // 全ヘッダーを処理
            
            std::this_thread::sleep_for(std::chrono::milliseconds(110));
            addStep("main.cpp (with headers)",
                   {"src/main.cpp", "include/game_engine.h", "include/graphics.h",
                    "include/audio.h", "include/math.h", "include/common.h", "include/types.h"},
                   {"build/main.o"},
                   std::chrono::milliseconds(350));
            
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
            addStep("Link executable",
                   {"build/math.o", "build/graphics.o", "build/audio.o", 
                    "build/game_engine.o", "build/main.o"},
                   {"build/GameApp"},
                   std::chrono::milliseconds(100));
            
            finishBuild();
        }
    };
}

// =======================================
// 依存関係グラフ分析
// =======================================

namespace dependency_analysis
{
    struct ModuleDependency
    {
        std::string name;
        std::vector<std::string> dependencies;
        std::vector<std::string> dependents;
        int buildOrder;
        
        ModuleDependency(const std::string& n) : name(n), buildOrder(-1) {}
    };
    
    class DependencyAnalyzer
    {
    private:
        std::unordered_map<std::string, ModuleDependency> modules_;
        
    public:
        void addModule(const std::string& name, const std::vector<std::string>& dependencies = {})
        {
            modules_[name] = ModuleDependency(name);
            modules_[name].dependencies = dependencies;
            
            // 依存先に自分を登録
            for (const auto& dep : dependencies)
            {
                if (modules_.find(dep) == modules_.end())
                {
                    modules_[dep] = ModuleDependency(dep);
                }
                modules_[dep].dependents.push_back(name);
            }
        }
        
        std::vector<std::string> calculateBuildOrder()
        {
            std::vector<std::string> buildOrder;
            std::unordered_set<std::string> built;
            std::unordered_set<std::string> building;
            
            std::function<void(const std::string&)> visit = [&](const std::string& module) {
                if (built.find(module) != built.end())
                    return;
                
                if (building.find(module) != building.end())
                {
                    throw std::runtime_error("循環依存が検出されました: " + module);
                }
                
                building.insert(module);
                
                // 依存関係を先にビルド
                for (const auto& dep : modules_[module].dependencies)
                {
                    visit(dep);
                }
                
                building.erase(module);
                built.insert(module);
                buildOrder.push_back(module);
                modules_[module].buildOrder = buildOrder.size() - 1;
            };
            
            for (const auto& [name, module] : modules_)
            {
                visit(name);
            }
            
            return buildOrder;
        }
        
        void analyzeParallelism()
        {
            std::cout << "\n=== 並列ビルド分析 ===" << std::endl;
            
            auto buildOrder = calculateBuildOrder();
            
            std::cout << "ビルド順序:" << std::endl;
            for (size_t i = 0; i < buildOrder.size(); ++i)
            {
                std::cout << "  " << i << ": " << buildOrder[i] << std::endl;
            }
            
            // 並列化可能性の分析
            std::unordered_map<int, std::vector<std::string>> levels;
            for (const auto& [name, module] : modules_)
            {
                int level = 0;
                for (const auto& dep : module.dependencies)
                {
                    level = std::max(level, modules_[dep].buildOrder + 1);
                }
                levels[level].push_back(name);
            }
            
            std::cout << "\n並列ビルドレベル:" << std::endl;
            for (const auto& [level, moduleList] : levels)
            {
                std::cout << "  レベル " << level << ": ";
                for (size_t i = 0; i < moduleList.size(); ++i)
                {
                    if (i > 0) std::cout << ", ";
                    std::cout << moduleList[i];
                }
                std::cout << " (並列度: " << moduleList.size() << ")" << std::endl;
            }
            
            // 最大並列度
            size_t maxParallelism = 0;
            for (const auto& [level, moduleList] : levels)
            {
                maxParallelism = std::max(maxParallelism, moduleList.size());
            }
            
            std::cout << "\n最大並列度: " << maxParallelism << std::endl;
            std::cout << "並列化効率: " << (static_cast<double>(maxParallelism) / modules_.size()) * 100.0 << "%" << std::endl;
        }
        
        void detectCycles()
        {
            std::cout << "\n=== 循環依存検査 ===" << std::endl;
            
            try
            {
                calculateBuildOrder();
                std::cout << "循環依存は検出されませんでした" << std::endl;
            }
            catch (const std::exception& e)
            {
                std::cout << "エラー: " << e.what() << std::endl;
            }
        }
        
        void printDependencyGraph()
        {
            std::cout << "\n=== 依存関係グラフ ===" << std::endl;
            
            for (const auto& [name, module] : modules_)
            {
                std::cout << name << ":" << std::endl;
                
                if (!module.dependencies.empty())
                {
                    std::cout << "  依存: ";
                    for (size_t i = 0; i < module.dependencies.size(); ++i)
                    {
                        if (i > 0) std::cout << ", ";
                        std::cout << module.dependencies[i];
                    }
                    std::cout << std::endl;
                }
                
                if (!module.dependents.empty())
                {
                    std::cout << "  被依存: ";
                    for (size_t i = 0; i < module.dependents.size(); ++i)
                    {
                        if (i > 0) std::cout << ", ";
                        std::cout << module.dependents[i];
                    }
                    std::cout << std::endl;
                }
                
                std::cout << std::endl;
            }
        }
    };
}

// =======================================
// パッケージ管理
// =======================================

namespace package_management
{
    struct PackageInfo
    {
        std::string name;
        std::string version;
        std::vector<std::string> modules;
        std::vector<std::string> dependencies;
        
        PackageInfo(const std::string& n, const std::string& v) : name(n), version(v) {}
    };
    
    class PackageManager
    {
    private:
        std::unordered_map<std::string, PackageInfo> packages_;
        
    public:
        void addPackage(const std::string& name, const std::string& version,
                       const std::vector<std::string>& modules,
                       const std::vector<std::string>& dependencies = {})
        {
            PackageInfo package(name, version);
            package.modules = modules;
            package.dependencies = dependencies;
            packages_[name] = package;
        }
        
        void generatePackageConfig()
        {
            std::cout << "\n=== パッケージ設定生成 ===" << std::endl;
            
            for (const auto& [name, package] : packages_)
            {
                std::cout << "パッケージ: " << name << " v" << package.version << std::endl;
                
                // find_package用の設定ファイル
                std::string configContent = 
                    "# " + name + "Config.cmake\n"
                    "set(" + name + "_VERSION " + package.version + ")\n"
                    "\n"
                    "# モジュールの追加\n";
                
                for (const auto& module : package.modules)
                {
                    configContent += "add_library(" + name + "::" + module + " IMPORTED)\n";
                    configContent += "set_target_properties(" + name + "::" + module + "\n";
                    configContent += "    PROPERTIES\n";
                    configContent += "        IMPORTED_LOCATION \"${CMAKE_CURRENT_LIST_DIR}/../../../lib/lib" + module + ".a\"\n";
                    configContent += "        INTERFACE_MODULE_FILE \"${CMAKE_CURRENT_LIST_DIR}/../../../lib/modules/" + module + ".bmi\"\n";
                    configContent += ")\n\n";
                }
                
                // 依存関係
                if (!package.dependencies.empty())
                {
                    configContent += "# 依存関係\n";
                    for (const auto& dep : package.dependencies)
                    {
                        configContent += "find_dependency(" + dep + ")\n";
                    }
                    configContent += "\n";
                }
                
                // ターゲットエイリアス
                configContent += "# エイリアス\n";
                configContent += "add_library(" + name + "::" + name + " INTERFACE IMPORTED)\n";
                configContent += "target_link_libraries(" + name + "::" + name + " INTERFACE\n";
                for (const auto& module : package.modules)
                {
                    configContent += "    " + name + "::" + module + "\n";
                }
                configContent += ")\n";
                
                std::cout << "設定ファイル内容:" << std::endl;
                std::cout << configContent << std::endl;
            }
        }
        
        void demonstrateUsage()
        {
            std::cout << "\n=== パッケージ使用例 ===" << std::endl;
            
            std::string usageExample = R"(
# プロジェクトでのパッケージ使用例
cmake_minimum_required(VERSION 3.20)
project(MyGame)

set(CMAKE_CXX_STANDARD 20)

# モジュールパッケージの検索
find_package(GameEngine REQUIRED)
find_package(PhysicsEngine REQUIRED)

# アプリケーションの作成
add_executable(MyGame
    src/main.cpp
    src/game_logic.cpp
)

# モジュールライブラリのリンク
target_link_libraries(MyGame
    PRIVATE
        GameEngine::GameEngine
        PhysicsEngine::PhysicsEngine
)

# C++モジュールの使用例 (main.cpp)
// import game.engine;
// import physics.engine;
// 
// int main() {
//     auto engine = game::createEngine();
//     auto world = physics::createWorld();
//     // ...
// }
)";
            
            std::cout << usageExample << std::endl;
        }
    };
}

// =======================================
// 使用例とデモンストレーション
// =======================================

namespace usage_examples
{
    void demonstrateBuildIntegration()
    {
        std::cout << "\n=== ビルド統合のデモンストレーション ===" << std::endl;
        
        // CMake設定の生成
        cmake_configuration::generateCMakeFiles();
        
        // ビルドプロファイリング
        build_timing::BuildProfiler profiler;
        
        std::cout << "\n--- モジュールビルドの測定 ---" << std::endl;
        profiler.simulateModuleBuild();
        
        std::cout << "\n--- 従来ビルドとの比較 ---" << std::endl;
        profiler.simulateTraditionalBuild();
    }
    
    void demonstrateDependencyAnalysis()
    {
        std::cout << "\n=== 依存関係分析のデモンストレーション ===" << std::endl;
        
        dependency_analysis::DependencyAnalyzer analyzer;
        
        // ゲームエンジンモジュール群の依存関係を定義
        analyzer.addModule("math");
        analyzer.addModule("core", {"math"});
        analyzer.addModule("graphics", {"math", "core"});
        analyzer.addModule("audio", {"core"});
        analyzer.addModule("input", {"core"});
        analyzer.addModule("physics", {"math", "core"});
        analyzer.addModule("game_engine", {"graphics", "audio", "input", "physics"});
        analyzer.addModule("game_logic", {"game_engine"});
        
        analyzer.printDependencyGraph();
        analyzer.detectCycles();
        analyzer.analyzeParallelism();
        
        // 循環依存のテスト
        std::cout << "\n--- 循環依存のテスト ---" << std::endl;
        dependency_analysis::DependencyAnalyzer badAnalyzer;
        badAnalyzer.addModule("A", {"B"});
        badAnalyzer.addModule("B", {"C"});
        badAnalyzer.addModule("C", {"A"});  // 循環依存
        badAnalyzer.detectCycles();
    }
    
    void demonstratePackageManagement()
    {
        std::cout << "\n=== パッケージ管理のデモンストレーション ===" << std::endl;
        
        package_management::PackageManager manager;
        
        // パッケージの登録
        manager.addPackage("GameEngine", "1.0.0", 
                          {"graphics", "audio", "input", "core"});
        
        manager.addPackage("PhysicsEngine", "2.1.0",
                          {"collision", "dynamics", "joints"},
                          {"GameEngine"});
        
        manager.addPackage("NetworkEngine", "0.9.0",
                          {"client", "server", "protocol"},
                          {"GameEngine"});
        
        manager.generatePackageConfig();
        manager.demonstrateUsage();
    }
}

int main()
{
    std::cout << "=== ビルドシステム統合 ===" << std::endl;
    
    // 1. ビルド統合の概念
    build_integration_concepts::explainBuildConcepts();
    
    // 2. 実際のビルド統合
    usage_examples::demonstrateBuildIntegration();
    
    // 3. 依存関係分析
    usage_examples::demonstrateDependencyAnalysis();
    
    // 4. パッケージ管理
    usage_examples::demonstratePackageManagement();
    
    std::cout << "\n=== ビルドシステム統合の利点 ===" << std::endl;
    std::cout << "1. 自動依存関係解決でビルド順序を最適化" << std::endl;
    std::cout << "2. BMIファイルによる高速インクリメンタルビルド" << std::endl;
    std::cout << "3. 並列ビルドの効率化" << std::endl;
    std::cout << "4. パッケージ管理の標準化" << std::endl;
    std::cout << "5. 循環依存の早期検出" << std::endl;
    std::cout << "6. プリプロセッサ処理の削減によるビルド時間短縮" << std::endl;
    
    return 0;
}

// まとめ：
// - C++20モジュールには専用のビルドシステム対応が必要
// - CMake 3.20以降で実験的サポートが利用可能
// - BMIファイルによりインクリメンタルビルドが大幅改善
// - 依存関係の自動解決により並列ビルドが最適化
// - パッケージ管理システムとの統合でライブラリ配布が簡素化
// - 従来のヘッダーベースビルドから段階的に移行可能