# Contributing

VeryFat Dungeonへようこそ！ご興味を持っていただき、ありがとうございます。

はじめに、encounter（dtkプログラマー）とkiwi（インスパイア）とvabold（誠に役に立った人）に感謝を申し上げますね！

以下には基本ルールです。

# 生成AI

（確かに）僕の個人的な優先により、このプロジェクトには生成AIはNGです。

# ソース設計

ヘダーはincludeフォルダーに収め、ソースコードはsrcフォルダーに含めている。以下の設計、ogws（Wii Sports）はベースです。

```text
└── src / include:
    ├── egg: 任天堂EADさんの内部ミドルウェア
    ├── MetroTRK: Metrowerks Target Resident Kernel (TRK) デバッガー
    ├── MSL: Metrowerks Standard Library
    ├── nw4r: NintendoWare for Revolution (NW4R) SDK
    ├── Pack: Revolution Pack Project (RP)
    ├── revolution: Revolution (RVL) SDK
    ├── runtime: CodeWarriorコンパイラーランタイム
    └── RVLFaceLib: Revolution Face Library (RFL、Miiライブラリー)
```
