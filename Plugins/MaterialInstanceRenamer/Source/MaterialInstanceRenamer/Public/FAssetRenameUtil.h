// Copyright 2025 kurorekish. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FAssetData;
class IAssetRegistry; // Forward declaration
enum class ERenameResult { Renamed, Skipped, Failed };

class FAssetRenameUtil
{
public:

	// Renames the material instance asset based on rules. Returns true if renamed, false otherwise.
	static bool RenameMaterialInstance(const FAssetData& SelectedAsset, bool bIsBatch = false);

	static bool ShouldSkipRename(const FString& OldAssetName, const FString& RecommendedPrefix, bool bIsBatch);

	static FString ExtractBaseName(const FString& OldAssetName, bool bIsBatch);

	// Renames the specified asset to the new name.
	static bool RenameAsset(const FAssetData& AssetToRename, const FString& NewName);

};