// Copyright 2025 kurorekish. All Rights Reserved.

#include "FAssetRenameUtil.h"
#include "MaterialInstanceRenamer.h" // Include module header to access Get() for flags if needed (like IsBatchRename)
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h" // Required for IAssetTools
#include "Materials/MaterialInstanceConstant.h" // Required for UMaterialInstanceConstant
#include "UObject/UObjectGlobals.h" // Required for FindObject / LoadObject
#include "UObject/Package.h" // Required for UPackage, MarkPackageDirty
#include "Misc/Paths.h" // Required for FPaths
#include "Misc/MessageDialog.h" // For FMessageDialog (optional, for error reporting)
#include "Logging/LogMacros.h" // For UE_LOG

// Define a unique log category if desired
// DEFINE_LOG_CATEGORY_STATIC(LogMaterialInstanceRenamerUtil, Log, All);

#define LOCTEXT_NAMESPACE "FMaterialInstanceRenamerModule" // Use the same namespace for LOCTEXT

// Placeholder implementation - needs refinement based on actual requirements
FString FAssetRenameUtil::GenerateUniqueAssetName(const FString& PackagePath, const FString& BaseName, IAssetRegistry& AssetRegistry)
{
	FString CandidateName;
	int32 Counter = 0;
	bool bFoundUniqueName = false;

	// Simple approach: append _Number until unique
	while (!bFoundUniqueName)
	{
		if (Counter == 0)
		{
			CandidateName = BaseName;
		}
		else
		{
			CandidateName = FString::Printf(TEXT("%s_%d"), *BaseName, Counter);
		}

		FString CandidatePackagePath = PackagePath + TEXT("/") + CandidateName;
		FAssetData ExistingAsset = AssetRegistry.GetAssetByObjectPath(FSoftObjectPath(CandidatePackagePath));

		if (!ExistingAsset.IsValid())
		{
			bFoundUniqueName = true;
		}
		else
		{
			Counter++;
		}
	}
	return CandidateName;
}


// Renames the material instance asset based on rules.
void FAssetRenameUtil::RenameMaterialInstance(const FAssetData& SelectedAsset, bool bIsBatch /*= false*/)
{
	const FString RecommendedPrefix = TEXT("MI_");
	FString OldAssetName = SelectedAsset.AssetName.ToString();

	// Check if the asset already has the recommended prefix
	if (OldAssetName.StartsWith(RecommendedPrefix))
	{
		// UE_LOG(LogMaterialInstanceRenamerUtil, Verbose, TEXT("Asset '%s' already has the recommended prefix."), *OldAssetName);
		return; // No rename needed
	}

	// Construct the new base name (remove existing common prefixes if necessary)
	// This logic might need adjustment depending on how aggressively you want to rename.
	// Example: If it's M_MyMaterial_Inst, should it become MI_MyMaterial_Inst or MI_Inst?
	// Current logic: MI_ + OriginalAssetName
	FString BaseName = OldAssetName;
	FString NewAssetName = RecommendedPrefix + BaseName;

	// Call the generic RenameAsset function
	RenameAsset(SelectedAsset, NewAssetName);
}


// Renames the specified asset to the new name. Returns true on success.
bool FAssetRenameUtil::RenameAsset(const FAssetData& AssetToRename, const FString& NewName)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	IAssetTools& AssetTools = AssetToolsModule.Get();

	TArray<FAssetRenameData> AssetsToRenameData;
	FString PackagePath = AssetToRename.PackagePath.ToString();
	FString NewPackagePath = PackagePath; // Assuming rename within the same folder
	FString NewAssetPath = FPaths::Combine(NewPackagePath, NewName);

	// Check if the target name already exists (basic check)
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	FAssetData ExistingAsset = AssetRegistry.GetAssetByObjectPath(FSoftObjectPath(NewAssetPath));

	if (ExistingAsset.IsValid() && ExistingAsset.PackageName != AssetToRename.PackageName)
	{
		// Optionally handle name collision (e.g., generate unique name or skip/warn)
		// For simplicity, we log and skip here. Could use GenerateUniqueAssetName.
		UE_LOG(LogTemp, Warning, TEXT("Rename skipped for '%s': Target name '%s' already exists."), *AssetToRename.GetObjectPathString(), *NewName);
		// In batch mode, we might not want message dialogs
		if (!FMaterialInstanceRenamerModule::Get().IsBatchRename())
		{
			FText ErrorTitle = LOCTEXT("RenameFailedTitle", "Rename Failed");
			FText ErrorMsg = FText::Format(LOCTEXT("RenameFailedNameCollision", "Could not rename '{0}' to '{1}' because an asset with that name already exists in this folder."), FText::FromName(AssetToRename.AssetName), FText::FromString(NewName));
			FMessageDialog::Open(EAppMsgType::Ok, ErrorMsg, &ErrorTitle);
		}
		return false;
	}

	// Prepare rename data
	UObject* AssetObject = AssetToRename.GetAsset(); // Load the asset
	if (!AssetObject)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load asset '%s' for renaming."), *AssetToRename.GetObjectPathString());
		return false;
	}

	AssetsToRenameData.Emplace(AssetObject, NewPackagePath, NewName);

	// Execute the rename
	bool bSuccess = AssetTools.RenameAssets(AssetsToRenameData);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully renamed '%s' to '%s'"), *AssetToRename.AssetName.ToString(), *NewName);
		// Optionally mark package dirty (though RenameAssets often handles this)
		// MarkPackageDirtyIfValid(AssetRegistry.GetAssetByObjectPath(FSoftObjectPath(NewAssetPath)));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to rename asset '%s' to '%s' using AssetTools."), *AssetToRename.AssetName.ToString(), *NewName);
		// Optionally show error message only if not in batch mode
		if (!FMaterialInstanceRenamerModule::Get().IsBatchRename())
		{
            FText ErrorTitle = LOCTEXT("RenameFailedTitle", "Rename Failed");
			FText ErrorMsg = FText::Format(LOCTEXT("RenameFailedGeneric", "An error occurred while renaming '{0}'."), FText::FromName(AssetToRename.AssetName));
			FMessageDialog::Open(EAppMsgType::Ok, ErrorMsg, &ErrorTitle);
		}
	}

	return bSuccess;
}


// Marks the package as dirty if the asset data is valid.
void FAssetRenameUtil::MarkPackageDirtyIfValid(const FAssetData& NewAssetData)
{
	if (NewAssetData.IsValid())
	{
		UPackage* Package = FindPackage(nullptr, *NewAssetData.PackageName.ToString());
		if (Package)
		{
			Package->MarkPackageDirty();
		}
	}
}


#undef LOCTEXT_NAMESPACE