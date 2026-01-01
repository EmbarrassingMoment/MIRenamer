// Copyright 2026 kurorekish. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMaterialInstanceRenamerModule : public IModuleInterface
{
public:
	static FMaterialInstanceRenamerModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FMaterialInstanceRenamerModule>("MaterialInstanceRenamer");
	}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
    void RegisterSettings();
    void UnregisterSettings();
	void OnRenameAllMaterialInstancesClicked();
	void AddToolMenuEntry();
	void AddMaterialContextMenuEntry();

	void OnAssetAdded(const FAssetData& AssetData);

	void RegisterMenus();

	bool bIsRenamingAsset = false;
};