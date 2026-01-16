# API Documentation

This document describes the internal API and extension methods for developers working with the **Material Instance Renamer** plugin.

## Core Classes

### FAssetRenameUtil

`FAssetRenameUtil` is the core utility class responsible for the renaming logic. It is a static class that encapsulates the rules for parsing old names and generating new names.

**Header:** `Source/MaterialInstanceRenamer/Public/FAssetRenameUtil.h`

#### Methods

-   `static ERenameResult RenameMaterialInstance(const FAssetData& SelectedAsset, FString& OutNewName)`
    -   Attempts to rename a single Material Instance asset.
    -   **Parameters:**
        -   `SelectedAsset`: The `FAssetData` of the asset to rename.
        -   `OutNewName`: Output parameter that receives the new name if the rename is successful.
    -   **Returns:** `ERenameResult` (Renamed, Skipped, Failed, or InvalidPattern).

-   `static bool ExtractBaseName(const FString& OldAssetName, FString& OutBaseName)`
    -   Extracts the base name from an asset name by stripping known prefixes and suffixes (e.g., `M_`, `_Inst`).
    -   **Parameters:**
        -   `OldAssetName`: The original name of the asset.
        -   `OutBaseName`: Output parameter for the stripped base name.
    -   **Returns:** `true` if a pattern was matched and the base name was extracted, `false` otherwise.

### UMaterialInstanceRenamerSettings

`UMaterialInstanceRenamerSettings` manages the configuration for the plugin, exposed via the Project Settings in the Editor.

**Header:** `Source/MaterialInstanceRenamer/Public/MaterialInstanceRenamerSettings.h`

#### Properties

-   `RenamePrefix` (`FString`): The prefix to apply to renamed Material Instances (default: "MI_").
-   `bAutoRenameOnCreate` (`bool`): If true, assets are automatically renamed upon creation.
-   `bShowNotificationOnAutoRename` (`bool`): If true, displays a toast notification when an auto-rename occurs.

## Commandlet

The plugin includes a commandlet `UMaterialInstanceRenamerCommandlet` for batch renaming assets via the command line. This is useful for automated pipelines.

**Header:** `Source/MaterialInstanceRenamer/Private/MaterialInstanceRenamerCommandlet.h`

### Usage

```bash
UnrealEditor-Cmd.exe <ProjectFile> -run=MaterialInstanceRenamer [Parameters]
```

### Parameters

-   `-Prefix=<String>`: Optional. Custom prefix to use for renaming. Defaults to project settings if not specified.
-   `-Path=<ContentPath>`: Optional. The content path to scan (defaults to `/Game`).
-   `-DryRun`: Optional. If present, simulates the operation without modifying assets.

## Module Extension

The main module class is `FMaterialInstanceRenamerModule`.

**Header:** `Source/MaterialInstanceRenamer/Public/MaterialInstanceRenamer.h`

It handles:
1.  **Startup/Shutdown**: Registers and unregisters the module.
2.  **Menu Integration**: Adds entries to the Tools menu and Content Browser context menu.
3.  **Event Handling**: Listens for `OnAssetAdded` events to trigger auto-renaming.

To extend the plugin's UI or behavior, you can modify `FMaterialInstanceRenamerModule::StartupModule` to register additional delegates or menu extensions.
