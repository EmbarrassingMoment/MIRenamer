# Material Instance Renamer

This plugin is available in English and Japanese.
* [English](./README.md)
* [日本語 (Japanese)](./Readme.ja.md)

## Overview

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

*   **Engine Versions:** 5.4, 5.5
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
