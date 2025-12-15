// Copyright 2025 kurorekish. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "MaterialInstanceRenamerCommandlet.generated.h"

/**
 * Commandlet for renaming Material Instance assets from the command line.
 *
 * Usage:
 *   UnrealEditor-Cmd.exe <ProjectFile> -run=MaterialInstanceRenamer -Prefix=MI_ -DryRun -Path=/Game/Assets
 *
 * Parameters:
 *   -Prefix=<String>  : Optional. Custom prefix to use for renaming. Defaults to project settings if not specified.
 *   -DryRun           : Optional. If present, simulates the renaming process without making changes.
 *   -Path=<Path>      : Optional. The content path to scan. Defaults to "/Game".
 */
UCLASS()
class UMaterialInstanceRenamerCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UMaterialInstanceRenamerCommandlet();

	//~ Begin UCommandlet Interface
	virtual int32 Main(const FString& Params) override;
	//~ End UCommandlet Interface
};
