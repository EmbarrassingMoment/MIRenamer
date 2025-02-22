# MaterialInstanceRenamer Plugin

MaterialInstanceRenamer is a plugin for Unreal Engine 5 that provides functionality to automatically rename material instances based on recommended prefixes.

## Features

- Batch rename material instances
- Automatically generate new names based on naming patterns
- Confirmation dialog for renaming
- Notify users of the rename result

## Installation

1. Copy the plugin source code to your project's `Plugins` folder.
2. Open the Unreal Engine editor and enable the `MaterialInstanceRenamer` plugin from the plugin manager.
3. Restart your project.

## Usage

1. Right-click on a material instance in the Content Browser.
2. Select "Rename to Recommended Prefix" from the context menu.
3. A confirmation dialog will appear. Select "Yes" to proceed with the renaming.

## Renaming Rules

- If the name starts with `M_` and contains `_Inst`, it will be renamed with the `MI_` prefix.
- If the name does not start with `MI_`, the `MI_` prefix will be added.

## Developer Information

### Project Structure

- `MaterialInstanceRenamer.cpp`: Contains the main logic of the plugin.
- `MaterialInstanceRenamer.h`: Header file for the plugin.

### Dependencies

- `AssetRegistry`
- `AssetTools`
- `ContentBrowserMenuContexts`
- `ToolMenus`

## License

This plugin is provided under the copyright of Epic Games, Inc. Please refer to the license file for more details.

## Contributing

Bug reports and feature requests are welcome on the GitHub repository. Pull requests are also appreciated.

---

This Readme provides an overview and usage instructions for the MaterialInstanceRenamer plugin. For detailed code changes or additional context, please refer to the source code.
