# Material Instance Renamer (English / 日本語)

## Overview (English)

This Unreal Engine plugin provides functionality to easily rename Material Instance assets within the Content Browser according to your project's naming conventions. It supports both individual renaming and batch renaming of all Material Instances within the project.

## Features

* **Customizable Prefix:**
    * You can change the default prefix `MI_` to any prefix you prefer (e.g., `P_`, `M_Inst_`) through the Project Settings menu.
* **Context Menu Renaming:**
    * You can rename one or more Material Instance assets by right-clicking them in the Content Browser and selecting the `Rename with Prefix (...)` option. The menu dynamically displays your current prefix.
    * An error message will be displayed if none of the selected assets are Material Instances.
    * If multiple assets are processed, a summary dialog is shown with a detailed breakdown of the results.
* **Batch Renaming:**
    * You can rename all Material Instances under the `/Game` folder in your project at once by selecting "Rename All Material Instances" from the "Tools" > "MaterialInstanceRenamer" section in the editor's top menu bar.
    * A confirmation dialog is displayed before processing.
    * A progress dialog is shown during the operation.
    * After completion, a summary dialog shows a detailed breakdown of the results (Renamed, Skipped, Failed, Invalid Pattern).
* **Auto Rename on Asset Creation:**
    * Automatically renames Material Instance assets the moment they are created in the Content Browser.
    * This feature is disabled by default to prevent unwanted renames.
    * It can be enabled via "Project Settings" > "Plugins" > "Material Instance Renamer" > "Auto-Rename on Create".
    * It can also be toggled on/off from the "Tools" > "MaterialInstanceRenamer" menu.
    * Optional notifications are displayed in the bottom-right corner of the editor upon renaming, which can be enabled/disabled in the project settings.
* **Naming Convention:**
    * The plugin renames assets to use a customizable prefix (default is `MI_`). For example, `M_Sample_Inst` becomes `MI_Sample`.
    * The logic intelligently handles various common naming schemes to extract a clean base name, such as `M_AssetName_Inst`, `AssetName_Inst`, and even `PREFIX_M_AssetName_Inst`.
    * Assets that already start with the configured prefix are skipped to avoid redundant renaming.
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

### Configuration

You can customize the renaming prefix:
1.  Go to "Edit" > "Project Settings..." in the menu bar.
2.  In the Project Settings window, find the "Plugins" section in the left-hand pane and click on "Material Instance Renamer".
3.  In the settings panel, you can change the `Rename Prefix` field to your desired prefix (e.g., `P_`).

### Context Menu Rename

1.  In the Content Browser, select one or more Material Instance assets you want to rename.
2.  Right-click the selected asset(s) and choose the `Rename with Prefix (...)` option from the context menu. The menu item will dynamically display your currently configured prefix.
3.  The asset name(s) will be changed to include your configured prefix. If multiple assets were processed, a summary dialog will be shown.

### Batch Rename

1.  Go to the "Tools" > "MaterialInstanceRenamer" in the Unreal Engine editor's top menu bar.
2.  Select "Rename All Material Instances".
3.  A confirmation dialog reflecting your custom prefix will appear. Click "Yes" to proceed.
4.  All Material Instances within the project will be scanned and renamed according to the naming convention.
5.  A dialog box indicating the results will be displayed upon completion.

## Compatibility

*   **Engine Versions:** 5.4, 5.5, 5.6
*   **Platforms:** Windows (Win64)

## Changelog

### v1.2.0 (2025-10-25)
* **New Feature:** Added an "Auto Rename on Asset Creation" feature. This allows for automatic renaming of Material Instances as soon as they are created. The feature is disabled by default but can be enabled in Project Settings or toggled via the "Tools" menu.
* **Bug Fix:** Fixed multiple crashes and compilation errors related to UI registration timing, localization, and deprecated API usage. The plugin is now more stable.

### v1.1.0 (2025-09-26)
* **New Feature:** Added the ability to customize the rename prefix via "Project Settings" > "Plugins" > "Material Instance Renamer". The default remains `MI_`.
* **Improvement:** The UI and renaming logic now dynamically update based on the configured prefix. The context menu label also displays the current prefix for better clarity.

### v1.0.2 (2025-07-23)
* **Refactor:** Replaced the custom unique name generation logic with the standard `IAssetTools::CreateUniqueAssetName` engine function for better stability and maintainability.
* **Code Cleanup:** Performed other minor refactorings to improve overall code quality.

### v1.0.1 (2025-07-13)
* **UI Improvement:** Moved the batch rename action into its own "MaterialInstanceRenamer" section under the "Tools" menu for better organization.
* **Compatibility:** Added support for Unreal Engine 5.5.

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

1.  このプラグインフォルダを、お使いのUnreal Engineプロジェクトの `Plugins` フォルダ内にコピーします。
    * 例: `[YourProjectDirectory]/Plugins/MaterialInstanceRenamer`
2.  Unreal Engineエディタを起動（または再起動）します。
3.  プラグインが自動的に有効化されます。（必要であれば、メニューの「編集」>「プラグイン」から`MaterialInstanceRenamer`を検索し、有効になっているか確認してください。）

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

## 互換性

*   **エンジンバージョン:** 5.4, 5.5, 5.6
*   **プラットフォーム:** Windows (Win64)

## 変更履歴 (Changelog)

### v1.2.0 (2025-10-25)
* **新機能:** 「アセット作成時の自動リネーム」機能を追加しました。これは、マテリアルインスタンスが作成されると同時に自動でリネームを行う機能です。デフォルトでは無効ですが、プロジェクト設定または「ツール」メニューから有効にできます。
* **バグ修正:** UIの登録タイミング、ローカライゼーション、非推奨APIの使用に関連する複数のクラッシュとコンパイルエラーを修正しました。プラグインの安定性が向上しました。

### v1.1.0 (2025-09-26)
* **新機能:** 「プロジェクト設定」>「プラグイン」>「Material Instance Renamer」から、リネームに使用するプレフィックスをカスタマイズできる機能を追加しました。デフォルトは`MI_`です。
* **改善:** 設定されたプレフィックスに応じて、UIとリネーム処理が動的に更新されるようになりました。また、明確化のためにコンテキストメニューのラベルにも現在のプレフィックスが表示されます。

### v1.0.2 (2025-07-23)
* **リファクタリング:** アセットのユニーク名を生成するロジックを、独自実装からエンジン標準のIAssetTools::CreateUniqueAssetName関数に置き換えました。これにより、処理の安定性とメンテナンス性が向上しました。
* **コード整理:** その他、全体的なコード品質を向上させるための軽微なリファクタリングを実施しました。

### v1.0.1 (2025-07-13)
* **UIの改善:** 一括リネーム機能を、見つけやすくするために「ツール」メニュー配下に専用の「MaterialInstanceRenamer」セクションを設けて移動しました。
* **互換性:** Unreal Engine 5.5に対応しました。

## マーケットプレイス

このプラグインはUnreal Engine マーケットプレイスで公開されています:

*   [マーケットプレイスで表示](https://www.fab.com/listings/0523dde6-2cc5-4129-9d83-78584910d5a5)

## 作者

*   **制作:** Kurorekishi
*   **GitHub:** [https://github.com/EmbarrassingMoment](https://github.com/EmbarrassingMoment)
*   **サポート/不具合報告:** [https://github.com/EmbarrassingMoment/MIRenamer/issues](https://github.com/EmbarrassingMoment/MIRenamer/issues)

## ライセンス

本プラグインは [Unreal Engine EULA](https://www.unrealengine.com/eula) に準拠します。