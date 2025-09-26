// Copyright 2025 kurorekish. All Rights Reserved.

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
};