// Copyright 2025 kurorekish. All Rights Reserved.

#include "FAssetRenameUtil.h"
#include "MaterialInstanceRenamer.h" 
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Materials/MaterialInstanceConstant.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/Package.h"
#include "Misc/Paths.h"
#include "Misc/MessageDialog.h"
#include "Logging/LogMacros.h"


#define LOCTEXT_NAMESPACE "FMaterialInstanceRenamerModule" // Use the same namespace for LOCTEXT

// Renames the material instance asset based on rules.
void FAssetRenameUtil::RenameMaterialInstance(const FAssetData& SelectedAsset, bool bIsBatch /*= false*/)
{
	const FString RecommendedPrefix = TEXT("MI_");
	FString OldAssetName = SelectedAsset.AssetName.ToString();

	// Skip if the asset already has the correct prefix
	if (ShouldSkipRename(OldAssetName, RecommendedPrefix, bIsBatch))
	{
		return;
	}

	// Extract the base name
	FString BaseName = ExtractBaseName(OldAssetName, bIsBatch);
	if (BaseName.IsEmpty())
	{
		return; // Skip if the pattern is invalid
	}

	// Construct the new name
	FString NewAssetName = RecommendedPrefix + BaseName;

	// Perform the asset rename
	RenameAsset(SelectedAsset, NewAssetName);
}

bool FAssetRenameUtil::ShouldSkipRename(const FString& OldAssetName, const FString& RecommendedPrefix, bool bIsBatch)
{
	if (OldAssetName.StartsWith(RecommendedPrefix) && !OldAssetName.StartsWith(TEXT("MI_M_")))
	{
		if (!bIsBatch)
		{
			FText MessageTitle = LOCTEXT("RenameSkippedTitle", "Rename Skipped");
			FText MessageContent = FText::Format(
				LOCTEXT("RenameSkippedAlreadyPrefixed", "Asset '{0}' is already prefixed with '{1}' and was excluded from renaming."),
				FText::FromString(OldAssetName),
				FText::FromString(RecommendedPrefix)
			);
			FMessageDialog::Open(EAppMsgType::Ok, MessageContent, &MessageTitle);
		}
		return true;
	}
	return false;
}

FString FAssetRenameUtil::ExtractBaseName(const FString& OldAssetName, bool bIsBatch)
{
	FString BaseName;

	if (OldAssetName.StartsWith(TEXT("MI_M_")) && OldAssetName.EndsWith(TEXT("_Inst")))
	{
		BaseName = OldAssetName.Mid(5, OldAssetName.Len() - 10); // Remove "MI_M_" and "_Inst"
	}
	else if (OldAssetName.StartsWith(TEXT("MI_M_")))
	{
		BaseName = OldAssetName.Mid(5); // Remove "MI_M_"
	}
	else if (OldAssetName.StartsWith(TEXT("M_")) && OldAssetName.EndsWith(TEXT("_Inst")))
	{
		BaseName = OldAssetName.Mid(2, OldAssetName.Len() - 7); // Remove "M_" and "_Inst"
	}
	else if (OldAssetName.EndsWith(TEXT("_Inst")))
	{
		BaseName = OldAssetName.Left(OldAssetName.Len() - 5); // Remove "_Inst"
	}
	else
	{
		if (!bIsBatch)
		{
			FText MessageTitle = LOCTEXT("RenameSkippedTitle", "Rename Skipped");
			FText MessageContent = FText::Format(
				LOCTEXT("RenameSkippedInvalidPattern", "Asset '{0}' does not match the expected naming pattern and was excluded from renaming."),
				FText::FromString(OldAssetName)
			);
			FMessageDialog::Open(EAppMsgType::Ok, MessageContent, &MessageTitle);
		}
		UE_LOG(LogTemp, Warning, TEXT("Asset '%s' does not match the expected naming pattern."), *OldAssetName);
		return FString(); // Return empty string to indicate invalid
	}

	return BaseName;
}

// Renames the specified asset to the new name. Returns true on success.
bool FAssetRenameUtil::RenameAsset(const FAssetData& AssetToRename, const FString& NewName)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	IAssetTools& AssetTools = AssetToolsModule.Get();

	FString CurrentPackagePath = AssetToRename.PackagePath.ToString();
	FString BaseAssetName = FPaths::Combine(CurrentPackagePath, NewName);

	FString UniquePackageName;
	FString UniqueAssetName;

	AssetTools.CreateUniqueAssetName(BaseAssetName, TEXT(""), UniquePackageName, UniqueAssetName);

	UObject* AssetObject = AssetToRename.GetAsset();
	if (!AssetObject)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load asset '%s' for renaming."), *AssetToRename.GetObjectPathString());
		return false;
	}

	TArray<FAssetRenameData> AssetsToRenameData;
	AssetsToRenameData.Emplace(AssetObject, FPaths::GetPath(UniquePackageName), UniqueAssetName);

	bool bSuccess = AssetTools.RenameAssets(AssetsToRenameData);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully renamed '%s' to '%s'"), *AssetToRename.AssetName.ToString(), *UniqueAssetName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to rename asset '%s' to '%s' using AssetTools."), *AssetToRename.AssetName.ToString(), *UniqueAssetName);
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