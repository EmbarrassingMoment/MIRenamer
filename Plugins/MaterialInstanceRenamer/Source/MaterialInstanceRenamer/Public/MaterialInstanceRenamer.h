// Copyright 2025 kurorekish. All Rights Reserved.

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

	// Retrieves localized text based on the current culture.
	static FText GetLocalizedText(const FString& Key, int32 Count = -1);

	bool IsBatchRename() const { return bIsBatchRename; }
	void SetBatchRename(bool bInBatchRename) { bIsBatchRename = bInBatchRename; }

private:
	void OnRenameAllMaterialInstancesClicked();
	void AddToolMenuEntry();
	void AddMaterialContextMenuEntry(); 

	bool bIsBatchRename = false;
};