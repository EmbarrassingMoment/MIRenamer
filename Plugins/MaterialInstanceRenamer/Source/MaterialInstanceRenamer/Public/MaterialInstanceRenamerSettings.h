// Copyright 2026 kurorekish. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MaterialInstanceRenamerSettings.generated.h"

/**
 * Settings for the Material Instance Renamer plugin.
 */
UCLASS(config = Editor, defaultconfig)
class MATERIALINSTANCERENAMER_API UMaterialInstanceRenamerSettings : public UObject
{
	GENERATED_BODY()

public:
	UMaterialInstanceRenamerSettings();

	/** The prefix to use when renaming Material Instance assets. */
	UPROPERTY(Config, EditAnywhere, Category = "Renaming", meta = (DisplayName = "Rename Prefix"))
	FString RenamePrefix;

	/** Enables/disables automatic renaming of Material Instance assets when they are created. */
	UPROPERTY(Config, EditAnywhere, Category = "Renaming", meta = (DisplayName = "Auto-Rename on Create"))
	bool bAutoRenameOnCreate;

	/** Show a notification when a Material Instance asset is automatically renamed. */
	UPROPERTY(Config, EditAnywhere, Category = "Renaming", meta = (DisplayName = "Show Notification on Auto-Rename"))
	bool bShowNotificationOnAutoRename;
};