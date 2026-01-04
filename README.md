# Material Instance Renamer

[![Available on Fab](https://img.shields.io/badge/Available%20on-Fab-blue?style=for-the-badge)](https://www.fab.com/listings/0523dde6-2cc5-4129-9d83-78584910d5a5)
[![Unreal Engine](https://img.shields.io/badge/Unreal_Engine-5.4_%7C_5.5_%7C_5.6_%7C_5.7-555555?style=for-the-badge&logo=unrealengine&logoColor=white)](https://www.unrealengine.com)
[![Platform](https://img.shields.io/badge/Platform-Win64-blue?style=for-the-badge&logo=windows)](https://www.microsoft.com/windows)
[![GitHub release](https://img.shields.io/github/v/release/EmbarrassingMoment/MIRenamer?style=for-the-badge)](https://github.com/EmbarrassingMoment/MIRenamer/releases)

[English](./README.md) | [日本語 (Japanese)](./Readme.ja.md)

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

#### Option A: Install via Fab (Recommended)
This is the easiest and most reliable way to install the plugin.

1.  Visit the [Fab product page](https://www.fab.com/listings/0523dde6-2cc5-4129-9d83-78584910d5a5).
2.  Add the plugin to your library.
3.  Open the Epic Games Launcher and install the plugin to your Engine version.
4.  Launch your project and enable the plugin via "Edit" > "Plugins" if it is not already enabled.

#### Option B: Manual Installation (GitHub)
For developers who want to modify the source code or use a specific version.

1.  Clone or download this repository.
2.  Copy the `MaterialInstanceRenamer` folder into your project's `Plugins` directory (create the directory if it doesn't exist).
    *   Path: `[YourProjectDirectory]/Plugins/MaterialInstanceRenamer`
3.  Right-click your `.uproject` file and select "Generate Visual Studio project files".
4.  Build your project using Visual Studio (Development Editor configuration).
5.  Launch the editor.

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

### Command Line Interface (Commandlet)

You can run the renaming process from the command line using the `MaterialInstanceRenamer` commandlet. This is useful for automated builds or CI/CD pipelines.

**Command Format:**

```bash
UnrealEditor-Cmd.exe <ProjectFile> -run=MaterialInstanceRenamer [Arguments]
```

**Arguments:**

*   `-run=MaterialInstanceRenamer`: Required. Specifies the commandlet to run.
*   `-Path=<Path>`: (Optional) The content path to scan for Material Instances. Defaults to `/Game` if not specified.
*   `-Prefix=<String>`: (Optional) The prefix to use for renaming. If specified, this overrides the project settings.
*   `-DryRun`: (Optional) Simulation mode. If present, the commandlet will log what changes would be made without actually modifying any assets.

**Example:**

```bash
# Dry run scan of the /Game/Characters folder with a custom prefix "MI_Char_"
UnrealEditor-Cmd.exe "C:\Projects\MyGame\MyGame.uproject" -run=MaterialInstanceRenamer -Path=/Game/Characters -Prefix=MI_Char_ -DryRun
```

## Compatibility

*   **Engine Versions:** 5.4, 5.5, 5.6
*   **Platforms:** Windows (Win64)

## Known Issues

*   **Redirectors May Remain After Auto-Renaming:**
    * When the "Auto Rename on Asset Creation" feature is enabled, renaming a newly created Material Instance may leave a redirector in the original asset's place. While this redirector is harmless, it can clutter up your project.
    * **How to Fix:** To clean up these redirectors, right-click on the folder containing the assets (or a parent folder) in the Content Browser and select "Fix Up Redirectors in Folder" from the context menu. This will consolidate the redirectors and remove the unnecessary files.

## Changelog

Please see [CHANGELOG.md](./CHANGELOG.md) for the full history of changes.

## Marketplace

You can find this plugin on the Unreal Engine Marketplace:

*   [View on Fab](https://www.fab.com/listings/0523dde6-2cc5-4129-9d83-78584910d5a5)

## Author

*   **Created by:** Kurorekishi
*   **GitHub:** [https://github.com/EmbarrassingMoment](https://github.com/EmbarrassingMoment)
*   **Support/Issues:** [https://github.com/EmbarrassingMoment/MIRenamer/issues](https://github.com/EmbarrassingMoment/MIRenamer/issues)

## License

This plugin is distributed under the [Unreal Engine EULA](https://www.unrealengine.com/eula).
