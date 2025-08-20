// Copyright 2025 kurorekish. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FAssetData;

class FAssetRenameUtil
{
public:
	static const FString RecommendedPrefix;

	// Generates a new asset name based on renaming rules.
	// Returns the new name, or an empty string if no rename is required.
	static FString GenerateNewAssetName(const FAssetData& SelectedAsset, bool bIsBatch = false);

	// Renames the material instance asset based on rules. Returns true if renamed, false otherwise.
	static bool RenameMaterialInstance(const FAssetData& SelectedAsset, bool bIsBatch = false);

	static bool ShouldSkipRename(const FString& OldAssetName, const FString& RecommendedPrefix, bool bIsBatch);

	static FString ExtractBaseName(const FString& OldAssetName, bool bIsBatch);

	// Renames the specified asset to the new name.
	static bool RenameAsset(const FAssetData& AssetToRename, const FString& NewName);
};