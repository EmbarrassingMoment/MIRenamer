// Copyright 2025 kurorekish. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FAssetData;
class IAssetRegistry; // Forward declaration

class FAssetRenameUtil
{
public:
	// Generates a unique asset name. (Implementation not shown in original, kept declaration)
	static FString GenerateUniqueAssetName(const FString& PackagePath, const FString& BaseName, IAssetRegistry& AssetRegistry);

	// Renames the material instance asset based on rules.
	static void RenameMaterialInstance(const FAssetData& SelectedAsset, bool bIsBatch = false);

	static bool ShouldSkipRename(const FString& OldAssetName, const FString& RecommendedPrefix, bool bIsBatch);

	static FString ExtractBaseName(const FString& OldAssetName, bool bIsBatch);

	// Renames the specified asset to the new name.
	static bool RenameAsset(const FAssetData& AssetToRename, const FString& NewName);

	// Marks the package as dirty if the asset data is valid. (Implementation might be simple, kept declaration)
	static void MarkPackageDirtyIfValid(const FAssetData& NewAssetData);
};