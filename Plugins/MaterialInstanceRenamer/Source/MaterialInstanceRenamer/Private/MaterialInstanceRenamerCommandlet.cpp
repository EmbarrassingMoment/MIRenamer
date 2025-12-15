// Copyright 2025 kurorekish. All Rights Reserved.

#include "MaterialInstanceRenamerCommandlet.h"
#include "FAssetRenameUtil.h"
#include "MaterialInstanceRenamerSettings.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "UObject/UObjectGlobals.h"

UMaterialInstanceRenamerCommandlet::UMaterialInstanceRenamerCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UMaterialInstanceRenamerCommandlet::Main(const FString& Params)
{
	UE_LOG(LogTemp, Display, TEXT("Starting Material Instance Renamer Commandlet..."));

	// 1. Parse Command Line Arguments
	FString PathToScan = TEXT("/Game");
	FString CustomPrefix;
	bool bDryRun = false;

	// Parse -Path=
	FParse::Value(*Params, TEXT("Path="), PathToScan);

	// Parse -Prefix=
	if (FParse::Value(*Params, TEXT("Prefix="), CustomPrefix))
	{
		UE_LOG(LogTemp, Display, TEXT("Using custom prefix: %s"), *CustomPrefix);
	}

	// Parse -DryRun
	if (FParse::Param(*Params, TEXT("DryRun")))
	{
		bDryRun = true;
		UE_LOG(LogTemp, Display, TEXT("Dry Run mode enabled. No changes will be made."));
	}

	// Apply Custom Prefix if provided
	UMaterialInstanceRenamerSettings* Settings = GetMutableDefault<UMaterialInstanceRenamerSettings>();
	if (!CustomPrefix.IsEmpty())
	{
		Settings->RenamePrefix = CustomPrefix;
	}

	UE_LOG(LogTemp, Display, TEXT("Scanning path: %s"), *PathToScan);

	// 2. Asset Registry Initialization
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// Scan paths synchronously to ensure assets are found
	TArray<FString> ScanPaths;
	ScanPaths.Add(PathToScan);
	AssetRegistry.ScanPathsSynchronous(ScanPaths, true);

	// 3. Search for Material Instances
	TArray<FAssetData> MaterialInstanceAssets;
	FARFilter Filter;
	Filter.PackagePaths.Add(FName(*PathToScan));
	Filter.bRecursivePaths = true;
	Filter.ClassPaths.Add(UMaterialInstanceConstant::StaticClass()->GetClassPathName());

	AssetRegistry.GetAssets(Filter, MaterialInstanceAssets);

	UE_LOG(LogTemp, Display, TEXT("Found %d Material Instances."), MaterialInstanceAssets.Num());

	if (MaterialInstanceAssets.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Material Instances found in %s"), *PathToScan);
		return 0; // Success but nothing done
	}

	// 4. Rename Execution
	int32 RenamedCount = 0;
	int32 SkippedCount = 0;
	int32 FailedCount = 0;
	int32 InvalidPatternCount = 0;

	const FString& RecommendedPrefix = Settings->RenamePrefix;

	for (const FAssetData& AssetData : MaterialInstanceAssets)
	{
		FString AssetName = AssetData.AssetName.ToString();

		// Logic for Dry Run to simulate what RenameMaterialInstance does
		if (bDryRun)
		{
			// Replicate checks from FAssetRenameUtil::RenameMaterialInstance
			// 1. Check if skipped
			if (AssetName.StartsWith(RecommendedPrefix) && !AssetName.StartsWith(TEXT("MI_M_")))
			{
				UE_LOG(LogTemp, Display, TEXT("[DryRun] Skipped: %s (Already valid)"), *AssetName);
				SkippedCount++;
				continue;
			}

			// 2. Extract base name
			FString BaseName;
			// ExtractBaseName is now public so we can use it
			// Note: ExtractBaseName reads from GetDefault<UMaterialInstanceRenamerSettings>(), so modifying the CDO earlier works.
			if (FAssetRenameUtil::ExtractBaseName(AssetName, BaseName))
			{
				FString NewName = RecommendedPrefix + BaseName;
				UE_LOG(LogTemp, Display, TEXT("[DryRun] Would Rename: %s -> %s"), *AssetName, *NewName);
				RenamedCount++;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[DryRun] Invalid Pattern: %s"), *AssetName);
				InvalidPatternCount++;
			}
		}
		else
		{
			// Actual Execution
			FString NewName;
			ERenameResult Result = FAssetRenameUtil::RenameMaterialInstance(AssetData, NewName);

			switch (Result)
			{
			case ERenameResult::Renamed:
				UE_LOG(LogTemp, Display, TEXT("Renamed: %s -> %s"), *AssetName, *NewName);
				RenamedCount++;
				break;
			case ERenameResult::Skipped:
				// Verbose log for skipped? Or Display? Keeping it minimal to avoid spam if many skipped.
				// But user might want to know.
				// UE_LOG(LogTemp, Display, TEXT("Skipped: %s"), *AssetName);
				SkippedCount++;
				break;
			case ERenameResult::Failed:
				UE_LOG(LogTemp, Error, TEXT("Failed to rename: %s"), *AssetName);
				FailedCount++;
				break;
			case ERenameResult::InvalidPattern:
				UE_LOG(LogTemp, Warning, TEXT("Invalid Pattern: %s"), *AssetName);
				InvalidPatternCount++;
				break;
			}
		}
	}

	// 5. Result Summary
	UE_LOG(LogTemp, Display, TEXT("========================================"));
	UE_LOG(LogTemp, Display, TEXT("Material Instance Renamer Summary"));
	UE_LOG(LogTemp, Display, TEXT("========================================"));
	UE_LOG(LogTemp, Display, TEXT("Total Assets Processed: %d"), MaterialInstanceAssets.Num());
	UE_LOG(LogTemp, Display, TEXT("Renamed: %d"), RenamedCount);
	UE_LOG(LogTemp, Display, TEXT("Skipped: %d"), SkippedCount);
	UE_LOG(LogTemp, Display, TEXT("Failed: %d"), FailedCount);
	UE_LOG(LogTemp, Display, TEXT("Invalid Pattern: %d"), InvalidPatternCount);
	UE_LOG(LogTemp, Display, TEXT("========================================"));

	if (FailedCount > 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Commandlet finished with errors."));
		return 1; // Non-zero exit code for failure
	}

	UE_LOG(LogTemp, Display, TEXT("Commandlet finished successfully."));
	return 0;
}
