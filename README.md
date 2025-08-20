# Material Instance Renamer (English / 日本語)

## Overview (English)

This Unreal Engine plugin provides functionality to easily rename Material Instance assets within the Content Browser according to Unreal Engine's recommended naming conventions. It supports both individual renaming and batch renaming of all Material Instances within the project.

## Features

* **Context Menu Renaming:**
    * You can rename individual Material Instance assets by right-clicking them in the Content Browser and selecting "Rename to Recommended Prefix".
    * An error message will be displayed if the selected asset is not a Material Instance.
    * A confirmation dialog appears if the name does not conform to the expected pattern (excluding patterns like `M_Hoge_Inst`).
* **Batch Renaming:**
    * You can rename all Material Instances under the `/Game` folder in your project at once by selecting "Rename All Material Instances" .from the "Tools" > "MaterialInstanceRenamer" section in the editor's top menu bar.
    * A confirmation dialog is displayed before processing.
    * A progress dialog is shown during the operation.
    * After completion, a summary dialog shows the total number of renamed assets.
* **Naming Convention:**
    * The plugin renames assets to use the `MI_` prefix (e.g., `MI_AssetName`).
    * It handles several common naming patterns to extract a clean base name:
        * `M_AssetName_Inst` → `MI_AssetName`
        * `AssetName_Inst` → `MI_AssetName`
        * `MI_M_AssetName_Inst` → `MI_AssetName`
        * `MI_M_AssetName` → `MI_AssetName`
    * Assets already correctly prefixed with `MI_` are skipped.
    * Assets that do not match one of the recognized patterns are also skipped to prevent incorrect renames.
* **Automatic Handling of Duplicate Names:**
    * If the new name already exists after renaming, a numerical suffix (e.g., `MI_Hoge1`, `MI_Hoge2`) is automatically appended to prevent naming conflicts.
* **Localization:**
    * Menu items and messages are displayed in either English or Japanese, depending on the editor's language setting.

## How to Use

### Installation

1.  Copy this plugin folder into your Unreal Engine project's `Plugins` directory.
    * Example: `[YourProjectDirectory]/Plugins/MaterialInstanceRenamer`
2.  Launch (or restart) the Unreal Engine editor.
3.  The plugin should be automatically enabled. (If necessary, verify it's enabled by searching for `MaterialInstanceRenamer` under "Edit" > "Plugins" in the menu.)

### Individual Rename

1.  In the Content Browser, select the Material Instance asset you want to rename.
2.  Right-click the asset and choose "Rename to Recommended Prefix" from the context menu.
3.  If prompted, click "Yes" in the confirmation dialog.
4.  The asset name will be changed to include the `MI_` prefix.

### Batch Rename

1.  Go to the "Tools" > "MaterialInstanceRenamer" in the Unreal Engine editor's top menu bar.
2.  Select "Rename All Material Instances".
3.  A confirmation dialog will appear. Click "Yes" to proceed.
4.  All Material Instances within the project will be scanned and renamed according to the naming convention.
5.  A dialog box indicating the results will be displayed upon completion.

## Case Study

In a project containing Material Instance assets named `M_Player_Inst` and `Weapon_Inst`, running the batch rename feature converted them to `MI_Player` and `MI_Weapon`.

**Before → After**

- `M_Player_Inst` → `MI_Player`
- `Weapon_Inst` → `MI_Weapon`

Automatically applying the naming convention cleaned up the asset list and saved time by removing repetitive manual renaming.

## Changelog

### v1.0.1 (2025-07-13)
* **UI Improvement:** Moved the batch rename action into its own "MaterialInstanceRenamer" section under the "Tools" menu for better organization.
* **Compatibility:** Added support for Unreal Engine 5.6.

### v1.0.2 (2025-07-23)
* **Refactor:** Replaced the custom unique name generation logic with the standard `IAssetTools::CreateUniqueAssetName` engine function for better stability and maintainability.
* **Code Cleanup:** Performed other minor refactorings to improve overall code quality.

## License

This plugin is distributed under the [Unreal Engine EULA](https://www.unrealengine.com/eula).

---
---


## 概要

このUnreal Engineプラグインは、コンテンツブラウザ内のマテリアルインスタンス (Material Instance) アセットの名前を、Unreal Engineの推奨命名規則に従って簡単に変更するための機能を提供します。個別のリネームと、プロジェクト内の全マテリアルインスタンスの一括リネームに対応しています。

## 機能

* **コンテキストメニューからのリネーム:**
    * コンテンツブラウザでマテリアルインスタンスアセットを右クリックし、「推奨プレフィックスに名前を変更」を選択することで、個別にリネームできます。
    * リネーム対象のアセットがマテリアルインスタンスでない場合は、エラーメッセージが表示されます。
    * 命名規則に沿っていない場合に確認ダイアログを表示します（`M_Hoge_Inst` のようなパターンを除く）。
* **一括リネーム:**
    * エディタ上部のメニューバー「ツール」内の「MaterialInstanceRenamer」セクションから「すべてのマテリアルインスタンスの名前を変更」を選択することで、プロジェクト内の `/Game` フォルダ以下にある全てのマテリアルインスタンスを一括でリネームできます。
    * 処理前に確認ダイアログが表示されます。
    * 処理中は進捗ダイアログが表示されます。
    * 処理完了後、リネームしたアセットの総数を表示します。
* **命名規則:**
    * アセット名を`MI_`プレフィックスを使用するようにリネームします (例: `MI_アセット名`)。
    * 以下の一般的な命名パターンを処理し、クリーンなベース名を抽出します:
        * `M_アセット名_Inst` → `MI_アセット名`
        * `アセット名_Inst` → `MI_アセット名`
        * `MI_M_アセット名_Inst` → `MI_アセット名`
        * `MI_M_アセット名` → `MI_アセット名`
    * 既に`MI_`プレフィックスが正しく付いているアセットはスキップされます。
    * 認識されたパターンのいずれにも一致しないアセットも、不正なリネームを防ぐためにスキップされます。
* **重複名の自動処理:**
    * リネーム後の名前が既に存在する場合、自動的に連番（例: `MI_Hoge1`, `MI_Hoge2`）を付与して名前の重複を防ぎます。
* **多言語対応:**
    * エディタの言語設定に応じて、メニュー項目やメッセージが日本語または英語で表示されます。

## 使い方

### インストール

1.  このプラグインフォルダを、お使いのUnreal Engineプロジェクトの `Plugins` フォルダ内にコピーします。
    * 例: `[YourProjectDirectory]/Plugins/MaterialInstanceRenamer`
2.  Unreal Engineエディタを起動（または再起動）します。
3.  プラグインが自動的に有効化されます。（必要であれば、メニューの「編集」>「プラグイン」から`MaterialInstanceRenamer`を検索し、有効になっているか確認してください。）

### 個別リネーム

1.  コンテンツブラウザで、名前を変更したいマテリアルインスタンスアセットを選択します。
2.  アセットを右クリックし、表示されるコンテキストメニューから「推奨プレフィックスに名前を変更」を選択します。
3.  必要に応じて表示される確認ダイアログで「はい」を選択します。
4.  アセット名が `MI_` プレフィックス付きの名前に変更されます。

### 一括リネーム

1.  Unreal Engineエディタ上部のメニューバーから「ツール」>「MaterialInstanceRenamer」を開きます。
2.  「すべてのマテリアルインスタンスの名前を変更」を選択します。
3.  確認ダイアログが表示されるので、「はい」を選択します。
4.  プロジェクト内のすべてのマテリアルインスタンスがスキャンされ、命名規則に従ってリネーム処理が実行されます。
5.  処理完了後、結果を示すダイアログが表示されます。

## ケーススタディ

サンプルプロジェクトでは、`M_Player_Inst` と `Weapon_Inst` というマテリアルインスタンスが存在していました。一括リネーム機能を実行すると、それぞれ `MI_Player` と `MI_Weapon` に変換されました。

**Before → After**

- `M_Player_Inst` → `MI_Player`
- `Weapon_Inst` → `MI_Weapon`

命名規則に沿った名前へ自動的に変換されることで、アセット一覧が整理され、繰り返しの手動リネーム作業を省けるため作業効率が向上します。

## 変更履歴 (Changelog)

### v1.0.1 (2025-07-13)
* **UIの改善:** 一括リネーム機能を、見つけやすくするために「ツール」メニュー配下に専用の「MaterialInstanceRenamer」セクションを設けて移動しました。
* **互換性:** Unreal Engine 5.6に対応しました。

### v1.0.2 (2025-07-23)
* **リファクタリング:** アセットのユニーク名を生成するロジックを、独自実装からエンジン標準のIAssetTools::CreateUniqueAssetName関数に置き換えました。これにより、処理の安定性とメンテナンス性が向上しました。
* **コード整理:** その他、全体的なコード品質を向上させるための軽微なリファクタリングを実施しました。

## ライセンス

本プラグインは [Unreal Engine EULA](https://www.unrealengine.com/eula) に準拠します。

## 作者

* kurorekish
