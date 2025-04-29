// FAssetRenameUtil.h
#pragma once

#include "CoreMinimal.h"

struct FAssetData;
class IAssetRegistry;
enum class EAssetRenameResult : uint8;

class FAssetRenameUtil
{
public:
    // Generates a unique asset name.
    static FString GenerateUniqueAssetName(const FString& OldPackagePath, const FString& BaseName, IAssetRegistry& AssetRegistry);

    // Renames the material instance asset based on rules.
    static void RenameMaterialInstance(const FAssetData& SelectedAsset, bool bIsBatch = false); // bIsBatch à¯êîÇí«â¡

    // Renames the specified asset to the new name.
    static void RenameAsset(const FAssetData& SelectedAsset, const FString& NewName);

    // Handles the result of the rename operation.
    static void HandleRenameResult(EAssetRenameResult RenameResult, const FString& OldPackagePath, const FString& FinalNewName, IAssetRegistry& AssetRegistry);

    // Marks the package as dirty if the asset data is valid.
    static void MarkPackageDirtyIfValid(const FAssetData& NewAssetData);
};