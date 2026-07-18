# C++講義シリーズ

YouTubeで公開しているC++講義シリーズのサンプルコード集です。
各レッスンのフォルダに、動画内で使用したソースコードを収録しています。

📺 **再生リスト**: [C++講義シリーズ](https://www.youtube.com/playlist?list=PLu3oZbxew9L49DTxuG5rT__lh-kkF7K11)

## レッスン一覧

| #   | テーマ                                     | フォルダ                                                  | 動画                                    |
| --- | ------------------------------------------ | --------------------------------------------------------- | --------------------------------------- |
| 01  | 変数とデータ型                             | [01-variable-and-data-types](01-variable-and-data-types/) | [YouTube](https://youtu.be/CgeKFytwVEs) |
| 02  | 演算子                                     | [02-operator](02-operator/)                               | [YouTube](https://youtu.be/wIIU0bBtwCs) |
| 03  | 標準入出力 cin/cout                        | [03-cout-cin](03-cout-cin/)                               | [YouTube](https://youtu.be/HdA71gAfvXM) |
| 04  | 条件分岐 if文, switch文                    | [04-if-switch](04-if-switch/)                             | [YouTube](https://youtu.be/_GEx29eVE9U) |
| 05  | 繰り返し for文, while文                    | [05-for-while](05-for-while/)                             | [YouTube](https://youtu.be/K2PoGwbfl14) |
| 06  | 関数                                       | [06-function](06-function/)                               | [YouTube](https://youtu.be/e6tGRYKYcX4) |
| 07  | 参照とポインタ                             | [07-ref-pointer](07-ref-pointer/)                         | [YouTube](https://youtu.be/eQU9morFkNs) |
| 08  | 配列と std::vector                         | [08-array-vector](08-array-vector/)                       | [YouTube](https://youtu.be/3Quy_dlPHlk) |
| 09  | char と string                             | [09-char-string](09-char-string/)                         | [YouTube](https://youtu.be/El-z4nB0Qks) |
| 10  | 関数オーバーロードとデフォルト引数         | [10-func-overload](10-func-overload/)                     | [YouTube](https://youtu.be/byw-HBC5I1M) |
| 11  | inline と constexpr                        | [11-inline-constexpr](11-inline-constexpr/)               | [YouTube](https://youtu.be/fKy_4DKxvVI) |
| 12  | ヘッダファイルと実装ファイルの分け方       | [12-hpp-cpp](12-hpp-cpp/)                                 | [YouTube](https://youtu.be/7emFkbA2VTQ) |
| 13  | クラスの基本                               | [13-class](13-class/)                                     | [YouTube](https://youtu.be/SEVVeh1tp1o) |
| 14  | 名前空間とモジュール                       | [14-namespace](14-namespace/)                             | [YouTube](https://youtu.be/ctCF5NzpxOk) |
| 15  | new/delete と RAII                         | [15-raii](15-raii/)                                       | [YouTube](https://youtu.be/pBiqU5QeBD0) |
| 16  | pragma once とヘッダガード                 | [16-header-guard](16-header-guard/)                       | [YouTube](https://youtu.be/arKM6wudtcE) |
| 17  | コンストラクタ、デストラクタ、初期化リスト | [17-constructor-destructor](17-constructor-destructor/)   | [YouTube](https://youtu.be/MMhqzRubE0g) |
| 18  | 演算子オーバーロード                       | [18-operator-overload](18-operator-overload/)             | [YouTube](https://youtu.be/FlMDh82PTr8) |
| 19  | 継承と派生クラス                           | [19-inheritance](19-inheritance/)                         | [YouTube](https://youtu.be/aX2jtAeTCRg) |
| 20  | 抽象クラスとインターフェース               | [20-abstract](20-abstract/)                               | [YouTube](https://youtu.be/er0Vx8nLvIk) |
| 21  | スマートポインタ                           | [21-smart-pointer](21-smart-pointer/)                     | [YouTube](https://youtu.be/0dRail2pjPs) |
| 22  | ラムダ式                                   | [22-lambda](22-lambda/)                                   | [YouTube](https://youtu.be/2rZLQWd9B0k) |
| 23  | テンプレート                               | [23-template](23-template/)                               | [YouTube](https://youtu.be/b-bQu5FYvcg) |
| 24  | ranges, 範囲for, views                     | [24-ranges](24-ranges/)                                   | [YouTube](https://youtu.be/gj0M2T4WTDs) |
| 25  | stack, queue, deque                        | [25-stack-queue-deque](25-stack-queue-deque/)             | [YouTube](https://youtu.be/8STeOloWeZo) |
| 26  | ムーブセマンティクス                       | [26-move-semantics](26-move-semantics/)                   | [YouTube](https://youtu.be/l5BIOUWyV9M) |
| 27  | 例外処理                                   | [27-try-catch](27-try-catch/)                             | [YouTube](https://youtu.be/59C28RDufRA) |

## 使い方

各フォルダの `.cpp` ファイルは単体でコンパイルできます。

```sh
g++ -std=c++20 -Wall -Wextra lesson1_1.cpp -o lesson1_1
./lesson1_1
```

詳しくは各レッスンフォルダの README と動画をご覧ください。
