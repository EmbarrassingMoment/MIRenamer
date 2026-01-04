# Material Instance Renamer

[![Available on Fab](https://img.shields.io/badge/Available%20on-Fab-blue?style=for-the-badge)](https://www.fab.com/listings/0523dde6-2cc5-4129-9d83-78584910d5a5)
[![License: ISC](https://img.shields.io/badge/License-ISC-blue.svg?style=for-the-badge)](https://opensource.org/licenses/ISC)
[![Unreal Engine](https://img.shields.io/badge/Unreal_Engine-5.4_%7C_5.5_%7C_5.6_%7C_5.7-555555?style=for-the-badge&logo=unrealengine&logoColor=white)](https://www.unrealengine.com)
[![Platform](https://img.shields.io/badge/Platform-Win64-blue?style=for-the-badge&logo=windows)](https://www.microsoft.com/windows)
[![GitHub release](https://img.shields.io/github/v/release/EmbarrassingMoment/MIRenamer?style=for-the-badge)](https://github.com/EmbarrassingMoment/MIRenamer/releases)

[English](./README.md) | [日本語 (Japanese)](./Readme.ja.md)

## 概要

このUnreal Engineプラグインは、コンテンツブラウザ内のマテリアルインスタンス (Material Instance) アセットの名前を、プロジェクトの命名規則に従って簡単に変更するための機能を提供します。個別のリネームと、プロジェクト内の全マテリアルインスタンスの一括リネームに対応しています。

## 機能

* **プレフィックスのカスタマイズ:**
    * デフォルトのプレフィックス`MI_`を、プロジェクト設定メニューから好きなプレフィックス（例: `P_`, `M_Inst_`）に変更できます。
* **コンテキストメニューからのリネーム:**
    * コンテンツブラウザで1つまたは複数のマテリアルインスタンスアセットを右クリックし、「プレフィックス(...)でリネーム」を選択することで、リネームできます。メニューには現在のプレフィックスが動的に表示されます。
    * 選択されたアセットにマテリアルインスタンスが含まれていない場合は、エラーメッセージが表示されます。
    * 複数のアセットを処理した場合、結果の内訳を示す概要ダイアログが表示されます。
* **一括リネーム:**
    * エディタ上部のメニューバー「ツール」内の「MaterialInstanceRenamer」セクションから「すべてのマテリアルインスタンスの名前を変更」を選択することで、プロジェクト内の `/Game` フォルダ以下にある全てのマテリアルインスタンスを一括でリネームできます。
    * 処理前に確認ダイアログが表示されます。
    * 処理中は進捗ダイアログが表示されます。
    * 処理完了後、結果（リネーム、スキップ、失敗、不正なパターン）の内訳を示す概要ダイアログが表示されます。
* **アセット作成時の自動リネーム:**
    * コンテンツブラウザでマテリアルインスタンスアセットが作成された瞬間に、自動でリネームを実行します。
    * 意図しないリネームを防ぐため、この機能はデフォルトで無効になっています。
    * 「プロジェクト設定」 > 「プラグイン」 > 「Material Instance Renamer」 > 「Auto-Rename on Create」から有効にできます。
    * 「ツール」 > 「MaterialInstanceRenamer」メニューからもオン/オフを切り替え可能です。
    * リネーム実行時にエディタ右下にオプションの通知を表示します。この通知はプロジェクト設定で有効/無効を切り替えられます。
* **命名規則:**
    * アセット名を、カスタマイズ可能なプレフィックス（デフォルト: `MI_`）を使用するようにリネームします。 (例: `M_Sample_Inst` → `MI_Sample`)
    * `M_アセット名_Inst`や`アセット名_Inst`、さらには`PREFIX_M_アセット名_Inst`のような、様々な命名規則から適切なベース名をインテリジェントに抽出します。
    * 既に設定されたプレフィックスで始まるアセットは、不要なリネームを避けるためにスキップされます。
* **重複名の自動処理:**
    * リネーム後の名前が既に存在する場合、自動的に連番（例: `MI_Hoge1`, `MI_Hoge2`）を付与して名前の重複を防ぎます。
* **多言語対応:**
    * エディタの言語設定に応じて、メニュー項目やメッセージが日本語または英語で表示されます。

## 使い方

### インストール

#### Option A: Fabからインストール (推奨)
最も簡単で確実な方法です。

1. [Fabの商品ページ](https://www.fab.com/listings/0523dde6-2cc5-4129-9d83-78584910d5a5)にアクセスします。
2. ライブラリに追加します。
3. Epic Games Launcherを開き、使用するエンジンバージョンにプラグインをインストールします。
4. プロジェクトを開き、メニューの「編集」>「プラグイン」からプラグインを有効にします。

#### Option B: GitHubから手動インストール
ソースコードを変更したい場合や、開発者向けのインストール方法です。

1. このリポジトリをクローンまたはダウンロードします。
2. `MaterialInstanceRenamer` フォルダを、プロジェクトの `Plugins` ディレクトリにコピーします。
   * 例: `[YourProjectDirectory]/Plugins/MaterialInstanceRenamer`
3. `.uproject` ファイルを右クリックし、"Generate Visual Studio project files" を選択します。
4. Visual Studio でプロジェクトをビルドします (Development Editor 構成)。
5. エディタを起動します。

### 設定方法

リネームに使用するプレフィックスはカスタマイズできます。
1.  メニューバーから「編集」>「プロジェクト設定...」を開きます。
2.  プロジェクト設定ウィンドウの左ペインで「プラグイン」セクションを探し、「Material Instance Renamer」をクリックします。
3.  設定パネルに表示される `Rename Prefix` の項目を、任意のプレフィックス（例: `P_`）に変更します。

### コンテキストメニューからのリネーム

1.  コンテンツブラウザで、名前を変更したい1つまたは複数のマテリアルインスタンスアセットを選択します。
2.  選択したアセットを右クリックし、表示されるコンテキストメニューから「プレフィックス(...)でリネーム」を選択します。メニュー項目には、現在設定されているプレフィックスが動的に表示されます。
3.  アセット名が、設定したプレフィックス付きの名前に変更されます。複数のアセットを処理した場合は、概要ダイアログが表示されます。

### 一括リネーム

1.  Unreal Engineエディタ上部のメニューバーから「ツール」>「MaterialInstanceRenamer」を開きます。
2.  「すべてのマテリアルインスタンスの名前を変更」を選択します。
3.  設定したカスタムプレフィックスを反映した確認ダイアログが表示されるので、「はい」を選択します。
4.  プロジェクト内のすべてのマテリアルインスタンスがスキャンされ、命名規則に従ってリネーム処理が実行されます。
5.  処理完了後、結果を示すダイアログが表示されます。

### コマンドライン実行 (Commandlet)

`MaterialInstanceRenamer` コマンドレットを使用して、コマンドラインからリネーム処理を実行できます。これは自動ビルドやCI/CDパイプラインでの利用に便利です。

**コマンド形式:**

```bash
UnrealEditor-Cmd.exe <ProjectFile> -run=MaterialInstanceRenamer [Arguments]
```

**引数:**

*   `-run=MaterialInstanceRenamer`: 必須。実行するコマンドレットを指定します。
*   `-Path=<Path>`: (オプション) スキャンするコンテンツパスを指定します。指定しない場合のデフォルトは `/Game` です。
*   `-Prefix=<String>`: (オプション) リネームに使用するプレフィックスを指定します。指定した場合、プロジェクト設定よりも優先されます。
*   `-DryRun`: (オプション) シミュレーションモードです。指定すると、実際には変更を行わず、変更される内容をログに出力します。

**実行例:**

```bash
# /Game/Characters フォルダを対象に、プレフィックス "MI_Char_" を使用してドライラン（テスト実行）を行う例
UnrealEditor-Cmd.exe "C:\Projects\MyGame\MyGame.uproject" -run=MaterialInstanceRenamer -Path=/Game/Characters -Prefix=MI_Char_ -DryRun
```

## 互換性

*   **エンジンバージョン:** 5.4, 5.5
*   **プラットフォーム:** Windows (Win64)

## 変更履歴 (Changelog)

詳細な変更履歴については [CHANGELOG.ja.md](./CHANGELOG.ja.md) を参照してください。

## マーケットプレイス

このプラグインはUnreal Engine マーケットプレイス (Fab) で公開されています:

*   [Fabで表示](https://www.fab.com/listings/0523dde6-2cc5-4129-9d83-78584910d5a5)

## 作者

*   **制作:** Kurorekishi
*   **GitHub:** [https://github.com/EmbarrassingMoment](https://github.com/EmbarrassingMoment)
*   **サポート/不具合報告:** [https://github.com/EmbarrassingMoment/MIRenamer/issues](https://github.com/EmbarrassingMoment/MIRenamer/issues)

## ライセンス

本プラグインは [Unreal Engine EULA](https://www.unrealengine.com/eula) に準拠します。
