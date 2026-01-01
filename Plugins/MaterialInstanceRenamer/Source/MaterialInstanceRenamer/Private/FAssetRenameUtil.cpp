// Copyright 2026 kurorekish. All Rights Reserved.

#include "FAssetRenameUtil.h"
#include "MaterialInstanceRenamerSettings.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/Package.h"
#include "Misc/Paths.h"
#include "Logging/LogMacros.h"
#include "UObject/Object.h"


// Define a structure to hold renaming patterns
struct FRenamePattern
{
	FString Prefix;
	FString Suffix;
	int32 PrefixLen;
	int32 SuffixLen;

	FRenamePattern(const FString& InPrefix, const FString& InSuffix)
		: Prefix(InPrefix), Suffix(InSuffix), PrefixLen(InPrefix.Len()), SuffixLen(InSuffix.Len())
	{}
};

// Rule-based approach for extracting the base name
bool FAssetRenameUtil::ExtractBaseName(const FString& OldAssetName, FString& OutBaseName)
{
	const UMaterialInstanceRenamerSettings* Settings = GetDefault<UMaterialInstanceRenamerSettings>();
	const FString& CurrentPrefix = Settings->RenamePrefix;

	static TArray<FRenamePattern> Patterns;
	static FString LastPrefix;

	// Rebuild patterns only if the prefix settings have changed or this is the first run
	if (Patterns.IsEmpty() || LastPrefix != CurrentPrefix)
	{
		Patterns.Reset();
		Patterns.Reserve(6);
		LastPrefix = CurrentPrefix;

		// Add patterns for the current custom prefix first, as they are most specific.
		Patterns.Emplace(CurrentPrefix + TEXT("M_"), TEXT("_Inst"));
		Patterns.Emplace(CurrentPrefix + TEXT("M_"), TEXT(""));

		// Add patterns for the legacy "MI_" prefix to allow cleanup of old assets,
		// but only if the custom prefix is not the same.
		if (CurrentPrefix != TEXT("MI_"))
		{
			Patterns.Emplace(TEXT("MI_M_"), TEXT("_Inst"));
			Patterns.Emplace(TEXT("MI_M_"), TEXT(""));
		}

		// Add general patterns
		Patterns.Emplace(TEXT("M_"), TEXT("_Inst"));
		Patterns.Emplace(TEXT(""), TEXT("_Inst"));
	}

	for (const FRenamePattern& Pattern : Patterns)
	{
		bool bPrefixMatches = Pattern.Prefix.IsEmpty() || OldAssetName.StartsWith(Pattern.Prefix);
		bool bSuffixMatches = Pattern.Suffix.IsEmpty() || OldAssetName.EndsWith(Pattern.Suffix);

		if (bPrefixMatches && bSuffixMatches)
		{
			int32 End = OldAssetName.Len() - Pattern.SuffixLen;
			OutBaseName = OldAssetName.Mid(Pattern.PrefixLen, End - Pattern.PrefixLen);
			return true;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Asset '%s' does not match any expected naming pattern."), *OldAssetName);
	return false; // No pattern matched
}

// Main function to orchestrate the renaming process
ERenameResult FAssetRenameUtil::RenameMaterialInstance(const FAssetData& SelectedAsset, FString& OutNewName)
{
	const UMaterialInstanceRenamerSettings* Settings = GetDefault<UMaterialInstanceRenamerSettings>();
	const FString& RecommendedPrefix = Settings->RenamePrefix;
	FString OldAssetName = SelectedAsset.AssetName.ToString();

	// 1. Check if the asset should be skipped
	// The MI_M_ prefix is a special case for cleaning up material-prefixed instances.
	if (OldAssetName.StartsWith(RecommendedPrefix) && !OldAssetName.StartsWith(TEXT("MI_M_")))
	{
		UE_LOG(LogTemp, Log, TEXT("Skipped rename for '%s' as it already has the recommended prefix."), *OldAssetName);
		return ERenameResult::Skipped;
	}

	// 2. Extract the base name using the rule-based method
	FString BaseName;
	if (!ExtractBaseName(OldAssetName, BaseName))
	{
		return ERenameResult::InvalidPattern;
	}

	// 3. Construct the new name and perform the rename
	FString NewAssetName = RecommendedPrefix + BaseName;
	if (RenameAsset(SelectedAsset, NewAssetName, OutNewName))
	{
		return ERenameResult::Renamed;
	}

	return ERenameResult::Failed;
}

// Performs the actual asset rename using the AssetTools module.
bool FAssetRenameUtil::RenameAsset(const FAssetData& AssetToRename, const FString& NewName, FString& OutNewName)
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

	if (AssetTools.RenameAssets(AssetsToRenameData))
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully renamed '%s' to '%s'"), *AssetToRename.AssetName.ToString(), *UniqueAssetName);
		OutNewName = UniqueAssetName;
		return true;
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to rename asset '%s' to '%s' using AssetTools."), *AssetToRename.AssetName.ToString(), *UniqueAssetName);
	return false;
}
