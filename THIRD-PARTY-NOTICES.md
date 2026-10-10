# Third-Party Notices

WOLSwitch 本体は MIT License ([LICENSE](LICENSE)) で提供する。
配布物 (Windows 版・macOS 版) には、以下の第三者ソフトウェアを同梱している。

## Qt

- 名称: Qt 6.10.3 (Qt Core / Qt GUI / Qt Widgets / Qt Network と、それらのプラグイン)
- 著作権: Copyright (C) The Qt Company Ltd. and other contributors.
- ライセンス: GNU Lesser General Public License version 3 (LGPL-3.0)
  - 本文: [licenses/LGPL-3.0.txt](licenses/LGPL-3.0.txt)
  - LGPL-3.0 が参照する GNU General Public License version 3: [licenses/GPL-3.0.txt](licenses/GPL-3.0.txt)
- 入手元: https://www.qt.io/
- ソースコード: https://download.qt.io/archive/qt/6.10/6.10.3/

WOLSwitch は Qt を改変せず、動的リンクで利用している。
同梱の Qt ライブラリ (Windows 版の `Qt6*.dll` と各プラグイン、macOS 版の
`WOLSwitch.app/Contents/Frameworks` と `PlugIns` 以下) は、LGPL-3.0 の条件に従い、
利用者が互換性のある別のビルドに差し替えることができる。
WOLSwitch 本体のソースコードは https://github.com/akagaya/WOLSwitch で公開している。

Qt 自体が含む第三者コンポーネントとそのライセンスは、Qt のドキュメントを参照。
https://doc.qt.io/qt-6/licenses-used-in-qt.html

## Microsoft Visual C++ Redistributable (Windows 版のみ)

Windows 版には、実行に必要な Microsoft Visual C++ 再頒布可能パッケージのインストーラ
(`vc_redist.x64.exe`) を同梱している。これは Microsoft Corporation の著作物であり、
Microsoft のライセンス条項に基づいて再頒布している。
