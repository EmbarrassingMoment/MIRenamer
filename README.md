# Material Instance Renamer (English / 日本語)

## Overview (English)

This Unreal Engine plugin provides functionality to easily rename Material Instance assets within the Content Browser according to Unreal Engine's recommended naming conventions. It supports both individual renaming and batch renaming of all Material Instances within the project.

## Features

* **Context Menu Renaming:**
    * You can rename one or more Material Instance assets by right-clicking them in the Content Browser and selecting "Rename to Recommended Prefix".
    * An error message will be displayed if none of the selected assets are Material Instances.
    * If multiple assets are processed, a summary dialog is shown with a detailed breakdown of the results.
* **Batch Renaming:**
    * You can rename all Material Instances under the `/Game` folder in your project at once by selecting "Rename All Material Instances" .from the "Tools" > "MaterialInstanceRenamer" section in the editor's top menu bar.
    * A confirmation dialog is displayed before processing.
    * A progress dialog is shown during the operation.
    * After completion, a summary dialog shows a detailed breakdown of the results (Renamed, Skipped, Failed, Invalid Pattern).
* **Naming Convention:**
    * The plugin renames assets to use the `MI_` prefix (e.g., `MI_AssetName`).
    * It extracts a clean base name by matching the following patterns in order:
        * `MI_M_AssetName_Inst` → `MI_AssetName`
        * `MI_M_AssetName` → `MI_AssetName`
        * `M_AssetName_Inst` → `MI_AssetName`
        * `AssetName_Inst` → `MI_AssetName`
    * Assets that already start with `MI_` are skipped (unless they match a more specific pattern like `MI_M_...`).
    * Assets that do not match any of the recognized patterns are left unchanged and are counted under "Invalid Pattern" in the summary report.
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

### Context Menu Rename

1.  In the Content Browser, select one or more Material Instance assets you want to rename.
2.  Right-click the selected asset(s) and choose "Rename to Recommended Prefix" from the context menu.
3.  The asset name(s) will be changed to include the `MI_` prefix. If multiple assets were processed, a summary dialog will be shown.

### Batch Rename

1.  Go to the "Tools" > "MaterialInstanceRenamer" in the Unreal Engine editor's top menu bar.
2.  Select "Rename All Material Instances".
3.  A confirmation dialog will appear. Click "Yes" to proceed.
4.  All Material Instances within the project will be scanned and renamed according to the naming convention.
5.  A dialog box indicating the results will be displayed upon completion.

## Compatibility

*   **Engine Versions:** 5.4, 5.5, 5.6
*   **Platforms:** Windows (Win64)

## Changelog

### v1.0.1 (2025-07-13)
* **UI Improvement:** Moved the batch rename action into its own "MaterialInstanceRenamer" section under the "Tools" menu for better organization.
* **Compatibility:** Added support for Unreal Engine 5.5.

### v1.0.2 (2025-07-23)
* **Refactor:** Replaced the custom unique name generation logic with the standard `IAssetTools::CreateUniqueAssetName` engine function for better stability and maintainability.
* **Code Cleanup:** Performed other minor refactorings to improve overall code quality.

## Marketplace

You can find this plugin on the Unreal Engine Marketplace:

*   [View on Marketplace](https://www.fab.com/listings/0523dde6-2cc5-4129-9d83-78584910d5a5)

## Author

*   **Created by:** Kurorekishi
*   **GitHub:** [https://github.com/EmbarrassingMoment](https://github.com/EmbarrassingMoment)
*   **Support/Issues:** [https://github.com/EmbarrassingMoment/MIRenamer/issues](https://github.com/EmbarrassingMoment/MIRenamer/issues)

## License

This plugin is distributed under the [Unreal Engine EULA](https://www.unrealengine.com/eula).

---
---


## 概要

このUnreal Engineプラグインは、コンテンツブラウザ内のマテリアルインスタンス (Material Instance) アセットの名前を、Unreal Engineの推奨命名規則に従って簡単に変更するための機能を提供します。個別のリネームと、プロジェクト内の全マテリアルインスタンスの一括リネームに対応しています。

## 機能

* **コンテキストメニューからのリネーム:**
    * コンテンツブラウザで1つまたは複数のマテリアルインスタンスアセットを右クリックし、「推奨プレフィックスに名前を変更」を選択することで、リネームできます。
    * 選択されたアセットにマテリアルインスタンスが含まれていない場合は、エラーメッセージが表示されます。
    * 複数のアセットを処理した場合、結果の内訳を示す概要ダイアログが表示されます。
* **一括リネーム:**
    * エディタ上部のメニューバー「ツール」内の「MaterialInstanceRenamer」セクションから「すべてのマテリアルインスタンスの名前を変更」を選択することで、プロジェクト内の `/Game` フォルダ以下にある全てのマテリアルインスタンスを一括でリネームできます。
    * 処理前に確認ダイアログが表示されます。
    * 処理中は進捗ダイアログが表示されます。
    * 処理完了後、結果（リネーム、スキップ、失敗、不正なパターン）の内訳を示す概要ダイアログが表示されます。
* **命名規則:**
    * アセット名を`MI_`プレフィックスを使用するようにリネームします (例: `MI_アセット名`)。
    * 以下のパターンを順番に照合し、クリーンなベース名を抽出します:
        * `MI_M_アセット名_Inst` → `MI_アセット名`
        * `MI_M_アセット名` → `MI_アセット名`
        * `M_アセット名_Inst` → `MI_アセット名`
        * `アセット名_Inst` → `MI_アセット名`
    * 既に`MI_`で始まるアセットはスキップされます（ただし、`MI_M_...`のような、より具体的なパターンに一致する場合を除く）。
    * いずれのパターンにも一致しないアセットは変更されず、概要レポートで「不正なパターン」としてカウントされます。
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

### コンテキストメニューからのリネーム

1.  コンテンツブラウザで、名前を変更したい1つまたは複数のマテリアルインスタンスアセットを選択します。
2.  選択したアセットを右クリックし、表示されるコンテキストメニューから「推奨プレフィックスに名前を変更」を選択します。
3.  アセット名が `MI_` プレフィックス付きの名前に変更されます。複数のアセットを処理した場合は、概要ダイアログが表示されます。

### 一括リネーム

1.  Unreal Engineエディタ上部のメニューバーから「ツール」>「MaterialInstanceRenamer」を開きます。
2.  「すべてのマテリアルインスタンスの名前を変更」を選択します。
3.  確認ダイアログが表示されるので、「はい」を選択します。
4.  プロジェクト内のすべてのマテリアルインスタンスがスキャンされ、命名規則に従ってリネーム処理が実行されます。
5.  処理完了後、結果を示すダイアログが表示されます。

## 互換性

*   **エンジンバージョン:** 5.4, 5.5, 5.6
*   **プラットフォーム:** Windows (Win64)

## 変更履歴 (Changelog)

### v1.0.1 (2025-07-13)
* **UIの改善:** 一括リネーム機能を、見つけやすくするために「ツール」メニュー配下に専用の「MaterialInstanceRenamer」セクションを設けて移動しました。
* **互換性:** Unreal Engine 5.5に対応しました。

### v1.0.2 (2025-07-23)
* **リファクタリング:** アセットのユニーク名を生成するロジックを、独自実装からエンジン標準のIAssetTools::CreateUniqueAssetName関数に置き換えました。これにより、処理の安定性とメンテナンス性が向上しました。
* **コード整理:** その他、全体的なコード品質を向上させるための軽微なリファクタリングを実施しました。

## マーケットプレイス

このプラグインはUnreal Engine マーケットプレイスで公開されています:

*   [マーケットプレイスで表示](https://www.fab.com/listings/0523dde6-2cc5-4129-9d83-78584910d5a5)

## 作者

*   **制作:** Kurorekishi
*   **GitHub:** [https://github.com/EmbarrassingMoment](https://github.com/EmbarrassingMoment)
*   **サポート/不具合報告:** [https://github.com/EmbarrassingMoment/MIRenamer/issues](https://github.com/EmbarrassingMoment/MIRenamer/issues)

## ライセンス

本プラグインは [Unreal Engine EULA](https://www.unrealengine.com/eula) に準拠します。
