// Copyright 2025 kurorekish. All Rights Reserved.

#include "MaterialInstanceRenamerBlueprintLibrary.h"
#include "FAssetRenameUtil.h"
#include "Materials/MaterialInstanceConstant.h"
#include "AssetRegistry/AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "MaterialInstanceRenamerBlueprintLibrary"

bool UMaterialInstanceRenamerBlueprintLibrary::RenameMaterialInstance(
    UMaterialInstanceConstant* MaterialInstance,
    FString& OutNewName,
    FString& OutErrorMessage
)
{
    if (!MaterialInstance)
    {
        OutErrorMessage = LOCTEXT("InvalidAsset", "Invalid Material Instance provided.").ToString();
        return false;
    }

    // Create FAssetData from the object
    FAssetData AssetData(MaterialInstance);

    // We cannot easily update the object reference in Blueprint if it gets renamed (and thus the object might be reloaded/replaced).
    // However, RenameAsset in Util usually handles the asset rename in place if possible, or we might need to handle it.
    // FAssetRenameUtil uses AssetTools::RenameAssets which updates references.

    ERenameResult Result = FAssetRenameUtil::RenameMaterialInstance(AssetData, OutNewName);

    switch (Result)
    {
    case ERenameResult::Renamed:
        return true;
    case ERenameResult::Skipped:
        OutErrorMessage = LOCTEXT("AlreadyCorrect", "Asset name is already correct.").ToString();
        return true; // Skipped is considered success in terms of "processed"
    case ERenameResult::InvalidPattern:
        OutErrorMessage = LOCTEXT("InvalidPattern", "Asset name does not match any expected naming pattern.").ToString();
        return false;
    case ERenameResult::Failed:
    default:
        OutErrorMessage = LOCTEXT("RenameFailed", "Failed to rename asset.").ToString();
        return false;
    }
}

bool UMaterialInstanceRenamerBlueprintLibrary::RenameMaterialInstanceByPath(
    const FString& AssetPath,
    FString& OutNewName,
    FString& OutErrorMessage
)
{
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(AssetPath));

    if (!AssetData.IsValid())
    {
        OutErrorMessage = FText::Format(LOCTEXT("AssetNotFound", "Asset not found at path: {0}"), FText::FromString(AssetPath)).ToString();
        return false;
    }

    // Ensure it is a Material Instance Constant
    if (!AssetData.GetClass()->IsChildOf(UMaterialInstanceConstant::StaticClass()))
    {
        OutErrorMessage = FText::Format(LOCTEXT("NotMaterialInstance", "Asset at path {0} is not a Material Instance Constant."), FText::FromString(AssetPath)).ToString();
        return false;
    }

    ERenameResult Result = FAssetRenameUtil::RenameMaterialInstance(AssetData, OutNewName);

    switch (Result)
    {
    case ERenameResult::Renamed:
        return true;
    case ERenameResult::Skipped:
        OutErrorMessage = LOCTEXT("AlreadyCorrect", "Asset name is already correct.").ToString();
        return true;
    case ERenameResult::InvalidPattern:
        OutErrorMessage = LOCTEXT("InvalidPattern", "Asset name does not match any expected naming pattern.").ToString();
        return false;
    case ERenameResult::Failed:
    default:
        OutErrorMessage = LOCTEXT("RenameFailed", "Failed to rename asset.").ToString();
        return false;
    }
}

void UMaterialInstanceRenamerBlueprintLibrary::BatchRenameMaterialInstances(
    const TArray<FString>& AssetPaths,
    int32& OutRenamedCount,
    int32& OutSkippedCount,
    int32& OutFailedCount,
    TArray<FString>& OutErrorMessages
)
{
    OutRenamedCount = 0;
    OutSkippedCount = 0;
    OutFailedCount = 0;
    OutErrorMessages.Empty();

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    for (const FString& Path : AssetPaths)
    {
        FString NewName;
        FString ErrorMessage;

        // We reuse the logic from RenameMaterialInstanceByPath logic but implemented inline to get specific results
        FAssetData AssetData = AssetRegistry.GetAssetByObjectPath(FSoftObjectPath(Path));

        if (!AssetData.IsValid())
        {
            OutFailedCount++;
            OutErrorMessages.Add(FText::Format(LOCTEXT("BatchAssetNotFound", "Asset not found: {0}"), FText::FromString(Path)).ToString());
            continue;
        }

        if (!AssetData.GetClass()->IsChildOf(UMaterialInstanceConstant::StaticClass()))
        {
            OutFailedCount++;
            OutErrorMessages.Add(FText::Format(LOCTEXT("BatchNotMI", "Not a Material Instance: {0}"), FText::FromString(Path)).ToString());
            continue;
        }

        ERenameResult Result = FAssetRenameUtil::RenameMaterialInstance(AssetData, NewName);

        switch (Result)
        {
        case ERenameResult::Renamed:
            OutRenamedCount++;
            break;
        case ERenameResult::Skipped:
            OutSkippedCount++;
            break;
        case ERenameResult::InvalidPattern:
            OutFailedCount++; // Considering Invalid Pattern as a failure in batch context if user expected rename
            OutErrorMessages.Add(FText::Format(LOCTEXT("BatchInvalidPattern", "Invalid pattern: {0}"), FText::FromString(Path)).ToString());
            break;
        case ERenameResult::Failed:
        default:
            OutFailedCount++;
            OutErrorMessages.Add(FText::Format(LOCTEXT("BatchFailed", "Failed to rename: {0}"), FText::FromString(Path)).ToString());
            break;
        }
    }
}

FString UMaterialInstanceRenamerBlueprintLibrary::GetPreviewRenameName(
    UMaterialInstanceConstant* MaterialInstance,
    bool& bWouldBeRenamed
)
{
    bWouldBeRenamed = false;
    if (!MaterialInstance)
    {
        return FString();
    }

    FAssetData AssetData(MaterialInstance);
    FString NewName;
    ERenameResult Result = FAssetRenameUtil::CalculateNewName(AssetData, NewName);

    if (Result == ERenameResult::Renamed)
    {
        bWouldBeRenamed = true;
        return NewName;
    }
    else if (Result == ERenameResult::Skipped)
    {
        return NewName; // CalculateNewName returns current name if skipped
    }

    // For InvalidPattern or Failed, we return the current name as fallback?
    // Requirement says "Returns what the new name would be". If no new name, current name is best answer.
    return MaterialInstance->GetName();
}

#undef LOCTEXT_NAMESPACE
