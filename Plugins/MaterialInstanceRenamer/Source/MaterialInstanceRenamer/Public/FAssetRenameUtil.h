// FAssetRenameUtil.h
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
	static void RenameMaterialInstance(const FAssetData& SelectedAsset, bool bIsBatch = false); // bIsBatch à¯êîÇí«â¡

	// Renames the specified asset to the new name.
	static bool RenameAsset(const FAssetData& AssetToRename, const FString& NewName);

	// Marks the package as dirty if the asset data is valid. (Implementation might be simple, kept declaration)
	static void MarkPackageDirtyIfValid(const FAssetData& NewAssetData);
};