// Copyright 2026 kurorekish. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * Main module class for the Material Instance Renamer plugin.
 * Handles module startup, shutdown, settings registration, and menu extensions.
 */
class FMaterialInstanceRenamerModule : public IModuleInterface
{
public:
	/**
	 * Gets the singleton instance of the module.
	 *
	 * @return Reference to the FMaterialInstanceRenamerModule instance.
	 */
	static FMaterialInstanceRenamerModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FMaterialInstanceRenamerModule>("MaterialInstanceRenamer");
	}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/**
	 * Registers the plugin settings in the Project Settings window.
	 */
    void RegisterSettings();

	/**
	 * Unregisters the plugin settings.
	 */
    void UnregisterSettings();

	/**
	 * Callback function for the "Rename All Material Instances" menu action.
	 * Triggers a batch rename operation for all material instances in the project.
	 */
	void OnRenameAllMaterialInstancesClicked();

	/**
	 * Adds the "Material Instance Renamer" entry to the Level Editor's Tools menu.
	 */
	void AddToolMenuEntry();

	/**
	 * Adds a context menu entry to Material Instance assets in the Content Browser.
	 * Allows renaming selected material instances.
	 */
	void AddMaterialContextMenuEntry();

	/**
	 * Callback function called when a new asset is added to the registry.
	 * Checks if the asset is a Material Instance and performs auto-renaming if enabled.
	 *
	 * @param AssetData metadata for the newly added asset.
	 */
	void OnAssetAdded(const FAssetData& AssetData);

	/**
	 * Registers all menu extensions (Tools menu and Content Browser context menu).
	 */
	void RegisterMenus();

	/** Flag to prevent recursive renaming loops when modifying assets. */
	bool bIsRenamingAsset = false;
};