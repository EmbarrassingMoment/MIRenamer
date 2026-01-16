# API ドキュメント

このドキュメントでは、**Material Instance Renamer** プラグインを扱う開発者向けに、内部 API と拡張方法について説明します。

## コアクラス (Core Classes)

### FAssetRenameUtil

`FAssetRenameUtil` は、リネームロジックを担当する主要なユーティリティクラスです。これは静的クラスであり、古い名前の解析と新しい名前の生成に関するルールをカプセル化しています。

**ヘッダ:** `Source/MaterialInstanceRenamer/Public/FAssetRenameUtil.h`

#### メソッド (Methods)

-   `static ERenameResult RenameMaterialInstance(const FAssetData& SelectedAsset, FString& OutNewName)`
    -   単一のマテリアルインスタンスアセットのリネームを試みます。
    -   **パラメータ:**
        -   `SelectedAsset`: リネームするアセットの `FAssetData`。
        -   `OutNewName`: リネームが成功した場合に新しい名前を受け取る出力パラメータ。
    -   **戻り値:** `ERenameResult` (Renamed, Skipped, Failed, または InvalidPattern)。

-   `static bool ExtractBaseName(const FString& OldAssetName, FString& OutBaseName)`
    -   既知のプレフィックスやサフィックス（例: `M_`, `_Inst`）を取り除き、アセット名からベース名を抽出します。
    -   **パラメータ:**
        -   `OldAssetName`: アセットの元の名前。
        -   `OutBaseName`: 抽出されたベース名を受け取る出力パラメータ。
    -   **戻り値:** パターンが一致し、ベース名が抽出された場合は `true`、そうでない場合は `false`。

### UMaterialInstanceRenamerSettings

`UMaterialInstanceRenamerSettings` はプラグインの設定を管理し、エディタのプロジェクト設定 (Project Settings) を通じて公開されます。

**ヘッダ:** `Source/MaterialInstanceRenamer/Public/MaterialInstanceRenamerSettings.h`

#### プロパティ (Properties)

-   `RenamePrefix` (`FString`): リネームされたマテリアルインスタンスに適用するプレフィックス（デフォルト: "MI_"）。
-   `bAutoRenameOnCreate` (`bool`): true の場合、アセット作成時に自動的にリネームされます。
-   `bShowNotificationOnAutoRename` (`bool`): true の場合、自動リネーム発生時にトースト通知を表示します。

## コマンドレット (Commandlet)

このプラグインには、コマンドライン経由でアセットを一括リネームするためのコマンドレット `UMaterialInstanceRenamerCommandlet` が含まれています。これは自動化パイプラインに役立ちます。

**ヘッダ:** `Source/MaterialInstanceRenamer/Private/MaterialInstanceRenamerCommandlet.h`

### 使用法 (Usage)

```bash
UnrealEditor-Cmd.exe <ProjectFile> -run=MaterialInstanceRenamer [Parameters]
```

### パラメータ (Parameters)

-   `-Prefix=<String>`: オプション。リネームに使用するカスタムプレフィックス。指定しない場合はプロジェクト設定がデフォルトとなります。
-   `-Path=<ContentPath>`: オプション。スキャンするコンテンツパス（デフォルトは `/Game`）。
-   `-DryRun`: オプション。指定した場合、アセットを変更せずに操作をシミュレートします。

## モジュール拡張 (Module Extension)

メインのモジュールクラスは `FMaterialInstanceRenamerModule` です。

**ヘッダ:** `Source/MaterialInstanceRenamer/Public/MaterialInstanceRenamer.h`

以下の処理を行います:
1.  **起動/終了 (Startup/Shutdown)**: モジュールの登録と登録解除。
2.  **メニュー統合 (Menu Integration)**: ツールメニューとコンテンツブラウザのコンテキストメニューへのエントリ追加。
3.  **イベント処理 (Event Handling)**: `OnAssetAdded` イベントをリッスンして自動リネームをトリガー。

プラグインの UI や動作を拡張するには、`FMaterialInstanceRenamerModule::StartupModule` を変更して、追加のデリゲートやメニュー拡張を登録することができます。
