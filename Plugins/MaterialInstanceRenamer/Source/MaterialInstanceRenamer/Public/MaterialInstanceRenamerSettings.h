// Copyright 2025 kurorekish. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MaterialInstanceRenamerSettings.generated.h"

UCLASS(Config = Editor, DefaultConfig)
class MATERIALINSTANCERENAMER_API UMaterialInstanceRenamerSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Renaming", meta = (DisplayName = "Auto Rename On Create"))
	bool bAutoRenameOnCreate = false;
};
