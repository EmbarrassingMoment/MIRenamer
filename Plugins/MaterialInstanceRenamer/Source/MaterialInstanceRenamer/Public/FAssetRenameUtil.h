// Copyright 2026 kurorekish. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FAssetData;

/**
 * Enum representing the result of a rename operation.
 */
enum class ERenameResult
{
	Renamed,        // The asset was successfully renamed.
	Skipped,        // The asset was skipped because it already followed the naming convention.
	Failed,         // The rename operation failed for an internal reason (e.g., file system error).
	InvalidPattern, // The asset name did not match any of the expected patterns for renaming.
};


/**
 * Utility class for renaming Material Instance assets.
 */
class FAssetRenameUtil
{
public:
	/**
	 * Renames a single material instance asset based on a set of predefined rules.
	 * This function is now responsible for checking if a rename is needed,
	 * extracting the base name, and performing the rename.
	 *
	 * @param SelectedAsset The asset data of the material instance to rename.
	 * @param OutNewName The new name of the asset if the rename was successful.
	 * @return An ERenameResult indicating the outcome of the operation.
	 */
	static ERenameResult RenameMaterialInstance(const FAssetData& SelectedAsset, FString& OutNewName);

	/**
	 * Extracts the base name from a material instance asset's name according to defined patterns.
	 *
	 * @param OldAssetName The current name of the asset.
	 * @param OutBaseName The extracted base name, if a pattern is matched.
	 * @return True if a pattern was matched and the base name was extracted, false otherwise.
	 */
	static bool ExtractBaseName(const FString& OldAssetName, FString& OutBaseName);

private:
	/**
	 * Performs the actual asset rename using the AssetTools module.
	 *
	 * @param AssetToRename The asset data of the asset to rename.
	 * @param NewName The desired new name for the asset.
	 * @param OutNewName The new name of the asset if the rename was successful.
	 * @return True if the rename was successful, false otherwise.
	 */
	static bool RenameAsset(const FAssetData& AssetToRename, const FString& NewName, FString& OutNewName);
};