// Copyright 2025 kurorekish. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMaterialInstanceRenamerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


private:
	void OnRenameAllMaterialInstancesClicked();
	void AddToolMenuEntry();
};
