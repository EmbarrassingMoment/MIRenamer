
# Material Instance Renamer Plugin

## Overview

The Material Instance Renamer plugin is a tool for the Unreal Engine 5 editor that renames Material Instances to a format starting with the recommended prefix `MI_`.
**This plugin is intended for use by individual developers and small development teams.** It helps streamline asset management by organizing Material Instance names within a project and ensuring they comply with Unreal Engine's naming conventions.

## Features

This plugin provides the following two main features:

1.  **Individual Rename from Context Menu**:
    - By right-clicking on a Material Instance selected in the Content Browser and choosing "Rename to Recommended Prefix", you can rename the Material Instance to a recommended name starting with the `MI_` prefix.
    - If the original name starts with `M_` and includes `_Inst`, it will be replaced with `MI_` and renamed (e.g., `M_ExampleMaterial_Inst` -> `MI_ExampleMaterial`).
    - If the original name does not start with `MI_`, a confirmation dialog will be displayed, and after user confirmation, the `MI_` prefix will be added and renamed (e.g., `ExampleMaterialInstance` -> `MI_ExampleMaterialInstance`).
    - If the renamed name duplicates an existing asset, a suffix (number) will be automatically appended to generate a unique name.

2.  **Batch Rename from Tool Menu**:
    - By selecting "Tools" > "Rename All Material Instances" from the main menu, you can batch rename **all** Material Instances in the project to names starting with the `MI_` prefix.
    - During batch renaming, the same naming conventions and duplication avoidance processing as individual renaming are applied.
    - A completion message is displayed when the batch process is finished.

## Installation

1.  Copy the plugin folder `MaterialInstanceRenamer` to the `Plugins` folder of your Unreal Engine project.
    If the `Plugins` folder does not exist in your project, create a `Plugins` folder directly under your project folder, and copy the `MaterialInstanceRenamer` folder into it.

    Example: `<YourProject>/Plugins/MaterialInstanceRenamer`

2.  Restart the Unreal Engine editor.

3.  After restarting the editor, open the "Plugins" window (from the main menu, "Edit" > "Plugins"), and in the "Project" section, under the "Editor" category, check that the "Material Instance Renamer" plugin is enabled. If it is disabled, enable it by checking the checkbox, and restart the Unreal Engine editor again.

## Usage

### Individual Rename

1.  Select the **Material Instance** asset you want to rename in the Content Browser.
    If you select an asset other than a **Material Instance**, an error message will be displayed, and the renaming process will not be executed.

2.  Right-click on the selected asset, and from the context menu, select "**Rename to Recommended Prefix**".

3.  If a confirmation dialog is displayed, review the content and click "Yes" to execute the rename.
    The name will be renamed to a format starting with `MI_`.

### Batch Rename

1.  From the Unreal Engine editor main menu, select "**Tools**" > "**Rename All Material Instances**".

2.  The batch renaming process will start. All Material Instances in the project will be automatically renamed.
    Depending on the size of your project, this process may take some time.

3.  When the process is complete, a completion message dialog "Renaming of all material instances is complete." will be displayed. Click "OK" to close the dialog.

## Precautions

*   **Backup**: Since the renaming process modifies asset names and references, it is strongly recommended to create a backup of your project beforehand to prepare for unexpected situations.
*   **Naming Convention**: This plugin changes the prefix of Material Instances to `MI_` based on Unreal Engine's recommended naming conventions.
*   **Batch Rename Execution Time**: Batch renaming targets all Material Instances in the project, so it may take time for projects with a large number of assets. The editor may become temporarily unresponsive during processing, but please wait until the process is complete.

## Version Information

Version: 1.0 (Initial Release)

## Planned Features

*   **Multilingual Support**:  We are considering adding support for languages other than Japanese and English.

## Contribution

Bug reports and feature requests are accepted in the Issues section of the GitHub repository.

## Copyright Notice

Copyright 2025 kurorekish. All Rights Reserved.


<br>
<br>
<br>

# Material Instance Renamer プラグイン

## 概要

Material Instance Renamer プラグインは、Unreal Engine 5 エディタ内で、マテリアルインスタンスの名前を推奨されるプレフィックス `MI_` で始まる形式にリネームするためのツールです。
**本プラグインは、個人開発者や小規模開発チームでの利用を想定しており**、プロジェクト内のマテリアルインスタンスの名前を整理し、Unreal Engine の命名規則に準拠させることで、アセット管理の効率化に貢献します。

## 機能

このプラグインは以下の2つの主要な機能を提供します。

1. **コンテキストメニューからの個別リネーム**:
   - コンテンツブラウザで選択したマテリアルインスタンスを右クリックし、「Rename to Recommended Prefix」を選択することで、そのマテリアルインスタンスの名前を `MI_` プレフィックスで始まる推奨名に変更します。
   - 元の名前が `M_` で始まり `_Inst` を含む場合は、`MI_` に置き換えてリネームします (例: `M_ExampleMaterial_Inst` -> `MI_ExampleMaterial`)。
   - 元の名前が `MI_` で始まらない場合は、確認ダイアログを表示し、ユーザーの確認後に `MI_` プレフィックスを追加してリネームします (例: `ExampleMaterialInstance` -> `MI_ExampleMaterialInstance`)。
   - リネーム後の名前が既存のアセットと重複する場合は、サフィックス (数値) を自動的に付与してユニークな名前を生成します。

2. **ツールメニューからのバッチリネーム**:
   - メインメニューの「ツール」 > 「Rename All Material Instances」を選択することで、プロジェクト内の**全て**のマテリアルインスタンスの名前を `MI_` プレフィックスで始まる名称に一括で変更します。
   - バッチリネーム時も、個別リネームと同様の命名規則と重複回避処理が適用されます。
   - バッチ処理が完了すると、完了メッセージが表示されます。

## インストール方法

1.  このプラグインのフォルダ `MaterialInstanceRenamer` を、Unreal Engine プロジェクトの `Plugins` フォルダにコピーしてください。
    プロジェクトの `Plugins` フォルダが存在しない場合は、プロジェクトフォルダ直下に `Plugins` フォルダを作成し、その中に `MaterialInstanceRenamer` フォルダをコピーしてください。

    例: `<YourProject>/Plugins/MaterialInstanceRenamer`

2.  Unreal Engine エディタを再起動してください。

3.  エディタ起動後、「プラグイン」ウィンドウ (メインメニューの「編集」 > 「プラグイン」) を開き、「プロジェクト」セクションの「Editor」カテゴリ内で "Material Instance Renamer" プラグインが有効になっていることを確認してください。もし無効になっている場合は、チェックボックスをオンにして有効にし、Unreal Engine エディタを再度再起動してください。

## 使用方法

### 個別リネーム

1.  コンテンツブラウザでリネームしたい **マテリアルインスタンス** アセットを選択します。
    **マテリアルインスタンス** 以外のアセットを選択した場合、エラーメッセージが表示され、リネーム処理は実行されません。

2.  選択したアセットを右クリックし、コンテキストメニューから「**Rename to Recommended Prefix**」を選択します。

3.  確認ダイアログが表示された場合は、内容を確認し、「はい」をクリックしてリネームを実行します。
    名前が `MI_` で始まる形式にリネームされます。

### バッチリネーム

1.  Unreal Engine エディタのメインメニューから「**ツール**」 > 「**Rename All Material Instances**」を選択します。

2.  バッチリネーム処理が開始されます。プロジェクト内の全てのマテリアルインスタンスが自動的にリネームされます。
    プロジェクトの規模によっては処理に時間がかかる場合があります。

3.  処理が完了すると、「Renaming of all material instances is complete.」という完了メッセージダイアログが表示されます。「OK」をクリックしてダイアログを閉じます。

## 注意事項

*   **バックアップ**: リネーム処理はアセットの名前と参照を変更するため、予期せぬ事態に備えて、事前にプロジェクトのバックアップを作成することを強く推奨します。
*   **命名規則**: このプラグインは、Unreal Engine の推奨命名規則に基づいて、マテリアルインスタンスのプレフィックスを `MI_` に変更します。
*   **バッチリネームの実行時間**: バッチリネームはプロジェクト内の全てのマテリアルインスタンスを対象とするため、アセット数が多いプロジェクトでは処理に時間がかかる場合があります。処理中はエディタが一時的に応答しなくなる可能性がありますが、処理が完了するまでお待ちください。

## バージョン情報

バージョン: 1.0 (初期バージョン)

## 予定されている機能

*   **多言語対応**: 日本語と英語に加えて、他の言語への対応を検討しています。

## コントリビューション

バグ報告や機能要望は、GitHubのリポジトリのIssueにて受け付けています。

## 著作権表示

Copyright 2025 kurorekish. All Rights Reserved.
