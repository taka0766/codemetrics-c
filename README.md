# codemetrics-c

C++ で書かれたプログラムの実行行、コメント行、空白行をカウントするツールです。指定したパス以下のC言語 (.c) および C++ (.cpp) ソースファイルを再帰的に探索し、その結果をコンソールまたはCSVファイルに出力できます。

## 機能

  * 指定したパス以下の C言語 (.c) および C++ (.cpp) ソースファイルを再帰的に探索します。
  * 各ソースファイルについて、以下の行数をカウントします。
      * 実行行
      * コメント行 (//, /\* \*/)
      * 空白行
  * 探索対象のパスは、`-d` オプションで指定できます。デフォルトはプログラム実行時のカレントディレクトリです。
  * 出力は標準出力（コンソール）が基本動作です。
  * `-f` オプションで出力ファイル名を指定すると、結果をCSV形式でファイルに出力できます。
  * 不正な引数が指定された場合は、Usage（使い方）を表示します。
  * WindowsとLinuxの両環境で動作可能です。

## 使い方

### ビルド

#### Linux/macOS

```bash
g++ codemetrics-c.cpp -o codemetrics-c
```

#### Windows (MinGWなど)

```bash
g++ codemetrics-c.cpp -o codemetrics-c.exe
```

### 実行

```bash
./codemetrics-c [オプション]
```

または (Windowsの場合)

```bash
codemetrics-c.exe [オプション]
```

#### オプション

  * `-d <パス名>`: 探索するディレクトリを指定します。サブディレクトリも再帰的に探索します。
    ```bash
    ./codemetrics-c -d /path/to/your/code
    ```
  * `-f <出力ファイル名>`: 結果をCSV形式で指定したファイルに出力します。
    ```bash
    ./codemetrics-c -f output.csv
    ```
  * `-d <パス名> -f <出力ファイル名>`: 特定のディレクトリを探索し、結果をファイルに出力します。
    ```bash
    ./codemetrics-c -d /path/to/your/code -f report.csv
    ```
  * 引数なし: カレントディレクトリを探索し、結果をコンソールに出力します。
    ```bash
    ./codemetrics-c
    ```

#### 出力形式 (CSV)

出力ファイルまたはコンソール出力は、以下のCSV形式になります。

```csv
<ファイルパス>,<総行数>,<コメント行数>,<空白行数>
```

例:

```csv
./src/main.c,100,20,10
./include/header.h,50,15,5
./src/MyClass.cpp,150,30,15
```

## 環境変数 (Windows)

Windows環境でコンパイル時に `<windows.h>` が見つからないなどのエラーが発生する場合は、Windows SDKのインクルードパスがコンパイラに正しく設定されているか確認してください。環境変数 `WINSDK_INCLUDE_PATH` を設定することで、インクルードパスを指定できます。

例:

```bash
set WINSDK_INCLUDE_PATH="C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um"
```

(上記パスは一例です。実際のパスは環境によって異なります。)


## 作者

[taka0766](https://github.com/taka0766)
