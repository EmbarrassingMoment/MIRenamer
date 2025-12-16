# Changelog

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
