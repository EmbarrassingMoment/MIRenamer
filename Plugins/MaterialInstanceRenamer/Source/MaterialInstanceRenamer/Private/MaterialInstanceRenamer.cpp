// Copyright 2025 kurorekish. All Rights Reserved. (Or your updated copyright)

#include "FAssetRenameUtil.h" // Corresponding header file
#include "Materials/MaterialInstance.h" // For UMaterialInstance class check (though maybe better in header if RenameMaterialInstance needs it directly)
#include "AssetRegistry/AssetData.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/Paths.h"
#include "UObject/Package.h" // For UPackage
#include "Logging/LogMacros.h"
#include "Misc/MessageDialog.h"
#include "Internationalization/Internationalization.h"
#include "Templates/SharedPointer.h" // Required for FMessageDialog::Open

// Define LOCTEXT_NAMESPACE for this file
#define LOCTEXT_NAMESPACE "FAssetRenameUtil"

// --- Constants moved from the original module/namespace ---
static const FName PrefixM = "M_";
static const FName PrefixMI = "MI_";
static const FName SuffixInst = "_Inst";
// ---

FString FAssetRenameUtil::GenerateUniqueAssetName(const FString& OldPackagePath, const FString& BaseName, IAssetRegistry& AssetRegistry)
{
    FString FinalNewName = BaseName;
    // Correctly form the object path using PackagePath / AssetName.AssetName
    FString NewAssetObjectPath = OldPackagePath / FinalNewName + TEXT(".") + FinalNewName;
    int32 Suffix = 1;

    // Check if an asset with the potential new object path already exists
    while (AssetRegistry.GetAssetByObjectPath(FName(*NewAssetObjectPath)).IsValid())
    {
        FinalNewName = BaseName + FString::Printf(TEXT("_%d"), Suffix++); // Use underscore for clarity
        NewAssetObjectPath = OldPackagePath / FinalNewName + TEXT(".") + FinalNewName;
    }
    return FinalNewName;
}

void FAssetRenameUtil::MarkPackageDirtyIfValid(const FAssetData& NewAssetData)
{
    if (NewAssetData.IsValid())
    {
        UObject* AssetObject = NewAssetData.GetAsset(); // Get the UObject
        if (AssetObject)
        {
            UPackage* Package = AssetObject->GetOutermost();
            if (Package)
            {
                Package->MarkPackageDirty();
                UE_LOG(LogTemp, Log, TEXT("Package marked as dirty: %s"), *Package->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Could not get package for asset: %s"), *NewAssetData.GetObjectPathString());
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Could not get asset object after rename: %s"), *NewAssetData.GetObjectPathString());
        }
    }
    else
    {
        // This log was already here, it's valid.
        UE_LOG(LogTemp, Error, TEXT("Failed to retrieve renamed asset data (AssetData was invalid)."));
    }
}

void FAssetRenameUtil::HandleRenameResult(EAssetRenameResult RenameResult, const FString& OldPackagePath, const FString& FinalNewName, IAssetRegistry& AssetRegistry)
{
    if (RenameResult == EAssetRenameResult::Success)
    {
        // Need to construct the final object path to find the asset data
        FString FinalNewAssetObjectPath = OldPackagePath / FinalNewName + TEXT(".") + FinalNewName;
        // It might take a moment for the registry to update, consider adding a small delay or loop if immediate GetAsset fails
        FAssetData NewAssetData = AssetRegistry.GetAssetByObjectPath(FName(*FinalNewAssetObjectPath));

        if (!NewAssetData.IsValid())
        {
            // Attempt to scan path if not found immediately
            AssetRegistry.ScanPathsSynchronous({ OldPackagePath }, true);
            NewAssetData = AssetRegistry.GetAssetByObjectPath(FName(*FinalNewAssetObjectPath));
        }

        MarkPackageDirtyIfValid(NewAssetData);
    }
    else
    {
        FText ErrorMessage;
        switch (RenameResult)
        {
        case EAssetRenameResult::Failure:
            ErrorMessage = LOCTEXT("RenameFailure", "Rename failed due to an unspecified error.");
            break;
        case EAssetRenameResult::Pending:
            ErrorMessage = LOCTEXT("RenamePending", "Rename is pending.");
            break;
        default:
            ErrorMessage = LOCTEXT("RenameUnknownError", "Rename failed with an unknown error.");
            break;
        }
        // Use FMessageDialog::Open which returns immediately
        FMessageDialog::Open(EAppMsgType::Ok, ErrorMessage);
        UE_LOG(LogTemp, Error, TEXT("Asset rename failed: %s"), *ErrorMessage.ToString());
    }
}

void FAssetRenameUtil::RenameAsset(const FAssetData& SelectedAsset, const FString& NewName)
{
    // Load necessary modules (consider loading them once in the main module if performance is critical)
    FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    FString OldPackagePath = SelectedAsset.PackagePath.ToString();

    // Ensure AssetRegistry is up-to-date before generating unique name
    AssetRegistry.ScanPathsSynchronous({ OldPackagePath }, true);

    FString FinalNewName = GenerateUniqueAssetName(OldPackagePath, NewName, AssetRegistry);

    UE_LOG(LogTemp, Log, TEXT("Attempting rename of '%s' to '%s' in path '%s'"), *SelectedAsset.AssetName.ToString(), *FinalNewName, *OldPackagePath);

    // Prepare the rename data
    FAssetRenameData RenameData(SelectedAsset.GetAsset(), OldPackagePath, FinalNewName);

    // Perform the rename operation with a dialog (or without if preferred, adjust the last bool)
    // Using RenameAssetsWithDialog means the user gets a confirmation/progress dialog.
    // Setting the last parameter to 'false' would skip the dialog, useful for pure batch operations without UI.
    EAssetRenameResult RenameResult = AssetToolsModule.Get().RenameAssetsWithDialog({ RenameData }, true);

    // Handle the result
    HandleRenameResult(RenameResult, OldPackagePath, FinalNewName, AssetRegistry);
}

void FAssetRenameUtil::RenameMaterialInstance(const FAssetData& SelectedAsset, bool bIsBatch /*= false*/)
{
    // Basic check if the asset is indeed a Material Instance
    // Note: The actual UObject check might be better placed before calling this function,
    // or ensure SelectedAsset is pre-filtered. But we keep the logic similar to original.
    if (!SelectedAsset.GetClass()->IsChildOf(UMaterialInstance::StaticClass()))
    {
        // This case should ideally be handled by the caller (e.g., UI logic)
        UE_LOG(LogTemp, Warning, TEXT("Asset '%s' is not a Material Instance. Skipping rename logic."), *SelectedAsset.AssetName.ToString());
        // Maybe show a message only if it's not a batch operation?
        // if (!bIsBatch) { FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("NotAMaterialInstanceUtil", "Selected asset is not a Material Instance.")); }
        return;
    }

    FName OldName = SelectedAsset.AssetName;
    FString OldNameStr = OldName.ToString();

    // Rule 1: Check for M_..._Inst pattern
    if (OldNameStr.StartsWith(PrefixM.ToString()) && OldNameStr.EndsWith(SuffixInst.ToString()))
    {
        // Extract core name: Remove M_ and _Inst
        int32 CoreNameLen = OldNameStr.Len() - PrefixM.ToString().Len() - SuffixInst.ToString().Len();
        if (CoreNameLen > 0)
        {
            FString CoreName = OldNameStr.Mid(PrefixM.ToString().Len(), CoreNameLen);
            FName NewName = FName(*(PrefixMI.ToString() + CoreName));
            UE_LOG(LogTemp, Log, TEXT("Applying Rule 1: Renaming '%s' to '%s'"), *OldNameStr, *NewName.ToString());
            RenameAsset(SelectedAsset, NewName.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Asset '%s' matches M_..._Inst pattern but has no core name."), *OldNameStr);
        }
    }
    // Rule 2: Check if it does NOT start with MI_
    else if (!OldNameStr.StartsWith(PrefixMI.ToString()))
    {
        FName NewName = FName(*(PrefixMI.ToString() + OldNameStr));
        // In batch mode, rename directly. In single mode, ask for confirmation.
        if (bIsBatch)
        {
            UE_LOG(LogTemp, Log, TEXT("Applying Rule 2 (Batch): Renaming '%s' to '%s'"), *OldNameStr, *NewName.ToString());
            RenameAsset(SelectedAsset, NewName.ToString());
        }
        else
        {
            // Confirmation dialog for single rename
            FText ConfirmTitle = LOCTEXT("ConfirmRenameTitle", "Confirm Rename");
            FText ConfirmMsg = FText::Format(
                LOCTEXT("ConfirmRenameMsgFmt", "Asset '{0}' does not start with '{1}'.\nAdd the prefix to rename it to '{2}'?"),
                FText::FromName(OldName),
                FText::FromString(PrefixMI.ToString()),
                FText::FromName(NewName)
            );

            if (FMessageDialog::Open(EAppMsgType::YesNo, ConfirmMsg, &ConfirmTitle) == EAppReturnType::Yes)
            {
                UE_LOG(LogTemp, Log, TEXT("Applying Rule 2 (Confirmed): Renaming '%s' to '%s'"), *OldNameStr, *NewName.ToString());
                RenameAsset(SelectedAsset, NewName.ToString());
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("Rename of '%s' cancelled by user."), *OldNameStr);
            }
        }
    }
    // Rule 3: Already starts with MI_
    else
    {
        // Only show message if it's NOT a batch rename (batch should silently skip)
        if (!bIsBatch)
        {
            FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("AlreadyHasPrefix", "The asset already has the recommended 'MI_' prefix."));
        }
        UE_LOG(LogTemp, Log, TEXT("Asset '%s' already follows naming convention. Skipping."), *OldNameStr);
    }
}

// Undefine LOCTEXT_NAMESPACE at the end of the file
#undef LOCTEXT_NAMESPACE