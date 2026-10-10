# WOLSwitch

登録した MAC アドレス宛に Wake-on-LAN のマジックパケットを送信する GUI ツール。

## 必要なもの

- CMake 3.21 以上
- Qt 6 (Widgets / Network / Test)
- C++17 対応コンパイラ

macOS では `brew install qtbase cmake` で揃う。

## ビルド

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Homebrew 以外の Qt を使う場合は `-DCMAKE_PREFIX_PATH=<Qt のインストール先>` を指定する。

## 使い方

1. 一覧から対象を選ぶか、名前と MAC アドレスを入力する。
2. 送信元の IP アドレスを選ぶと、ブロードキャストアドレスが自動で入る (手動で書き換え可)。
3. 「送信」を押すと UDP ポート 7 にマジックパケットを送る。未登録の名前は一覧に追加される。

一覧の行を選んで「選択行を削除」(または Delete / Backspace キー) を押すと、確認のうえ一覧から削除する。

## 配布物

GitHub Actions (`.github/workflows/build.yml`) が push ごとに Windows 版 (x64) と macOS 版 (ユニバーサル) をビルドし、Artifacts に置く。Artifacts は最新の 1 セットだけを残し、過去の実行分は自動で削除する。

## 保存先

一覧は `名前,MACアドレス` 形式の `WOLSwitch.csv` に保存される。

| OS | 保存先 |
| --- | --- |
| macOS | `~/Documents/WOLSwitch.csv` |
| それ以外 | 実行ファイルと同じフォルダ |
