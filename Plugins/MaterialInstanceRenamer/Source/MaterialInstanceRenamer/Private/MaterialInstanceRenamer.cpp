// Copyright 2025 kurorekish. All Rights Reserved.

#include "MaterialInstanceRenamer.h"
#include "Modules/ModuleManager.h"
#include "Materials/MaterialInstance.h"
#include "ContentBrowserMenuContexts.h"
#include "ToolMenus.h"
#include "AssetRegistry/AssetData.h"
#include "AssetToolsModule.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Materials/MaterialInstanceConstant.h"
#include <AssetRegistry/AssetRegistryModule.h>
#include "Internationalization/Culture.h"

#define LOCTEXT_NAMESPACE "FMaterialInstanceRenamerModule"

const FName PrefixM = "M_";
const FName PrefixMI = "MI_";
const FName SuffixInst = "_Inst";

namespace MenuExtension_MaterialInstance {

    /**
     * Generates a unique asset name by appending a suffix if necessary.
     *
     * @param OldPackagePath The original package path.
     * @param BaseName The base name for the new asset.
     * @param AssetRegistry The asset registry to check for existing assets.
     * @return A unique asset name.
     */
    static FString GenerateUniqueAssetName(const FString& OldPackagePath, const FString& BaseName, IAssetRegistry& AssetRegistry)
    {
        FString FinalNewName = BaseName;
        FString NewAssetPath = OldPackagePath / FinalNewName + TEXT(".") + FinalNewName;
        int32 Suffix = 1;
        while (AssetRegistry.GetAssetByObjectPath(*NewAssetPath).IsValid())
        {
            FinalNewName = BaseName + FString::Printf(TEXT("%d"), Suffix++);
            NewAssetPath = OldPackagePath / FinalNewName + TEXT(".") + FinalNewName;
        }
        return FinalNewName;
    }

    /**
     * Marks the package as dirty if the asset data is valid.
     *
     * @param NewAssetData The asset data to check.
     */
    static void MarkPackageDirtyIfValid(const FAssetData& NewAssetData)
    {
        if (NewAssetData.IsValid())
        {
            UPackage* Package = NewAssetData.GetAsset()->GetOutermost();
            if (Package)
            {
                Package->MarkPackageDirty();
                UE_LOG(LogTemp, Log, TEXT("Package marked as dirty: %s"), *Package->GetName());
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to retrieve renamed asset."));
        }
    }

    /**
     * Handles the result of the rename operation.
     *
     * @param RenameResult The result of the rename operation.
     * @param OldPackagePath The original package path.
     * @param FinalNewName The final new name of the asset.
     * @param AssetRegistry The asset registry to check for the renamed asset.
     */
    static void HandleRenameResult(EAssetRenameResult RenameResult, const FString& OldPackagePath, const FString& FinalNewName, IAssetRegistry& AssetRegistry)
    {
        if (RenameResult == EAssetRenameResult::Success)
        {
            FString FinalNewAssetPath = OldPackagePath / FinalNewName + TEXT(".") + FinalNewName;
            FAssetData NewAssetData = AssetRegistry.GetAssetByObjectPath(*FinalNewAssetPath);
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
            FMessageDialog::Open(EAppMsgType::Ok, ErrorMessage);
        }
    }

    /**
     * Renames the specified asset to the new name.
     *
     * @param SelectedAsset The asset to rename.
     * @param NewName The new name for the asset.
     */
    static void RenameAsset(const FAssetData& SelectedAsset, const FString& NewName)
    {
        FString OldPackagePath = SelectedAsset.PackagePath.ToString();

        FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
        FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
        IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

        FString FinalNewName = GenerateUniqueAssetName(OldPackagePath, NewName, AssetRegistry);

        UE_LOG(LogTemp, Log, TEXT("Renaming to: %s"), *FinalNewName);

        FAssetRenameData RenameData(SelectedAsset.GetAsset(), OldPackagePath, FinalNewName);
        EAssetRenameResult RenameResult = AssetToolsModule.Get().RenameAssetsWithDialog({ RenameData }, true);

        HandleRenameResult(RenameResult, OldPackagePath, FinalNewName, AssetRegistry);
    }

    /**
     * Renames the material instance asset to the recommended prefix.
     *
     * @param SelectedAsset The material instance asset to rename.
     */
    void RenameMaterialInstance(const FAssetData& SelectedAsset)
    {
        FName OldName = SelectedAsset.AssetName;

        if (OldName.ToString().StartsWith(PrefixM.ToString()) && OldName.ToString().Contains(SuffixInst.ToString()))
        {
            FString CoreName = OldName.ToString().Mid(PrefixM.ToString().Len(), OldName.ToString().Find(SuffixInst.ToString()) - PrefixM.ToString().Len());
            FName NewName = FName(*PrefixMI.ToString() + CoreName);
            RenameAsset(SelectedAsset, NewName.ToString());
        }
        else if (!OldName.ToString().StartsWith(PrefixMI.ToString()))
        {
            if (FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("ConfirmRename", "名前を本当に変更していいですか？")) == EAppReturnType::Yes)
            {
                FName NewName = FName(*PrefixMI.ToString() + OldName.ToString());
                RenameAsset(SelectedAsset, NewName.ToString());
            }
        }
        else
        {
            if (!FMaterialInstanceRenamerModule::Get().IsBatchRename())
            {
                FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("NamePatternMismatch", "The name does not match the required pattern."));
            }
        }
    }
    /**
     * Executes the rename action from the context menu.
     *
     * @param MenuContext The context of the tool menu.
     */
    static void OnExecuteAction(const FToolMenuContext& MenuContext)
    {
        if (const UContentBrowserAssetContextMenuContext* Context = MenuContext.FindContext<UContentBrowserAssetContextMenuContext>())
        {
            for (const FAssetData& SelectedAsset : Context->SelectedAssets)
            {
                if (UObject* Asset = SelectedAsset.GetAsset())
                {
                    if (Asset->IsA<UMaterialInstance>())
                    {
                        RenameMaterialInstance(SelectedAsset);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Log, TEXT("Selected asset is not a Material Instance"));
                        FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("NotAMaterialInstance", "The selected asset is not a Material Instance."));
                    }
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Context not found"));
        }
    }

    /**
     * Adds the material context menu entry for renaming.
     */
    static void AddMaterialContextMenuEntry()
    {
#if WITH_EDITOR
        FToolMenuOwnerScoped OwnerScoped(UE_MODULE_NAME);

        UToolMenu* Menu = UE::ContentBrowser::ExtendToolMenu_AssetContextMenu(UMaterialInterface::StaticClass());

        FToolMenuSection& Section = Menu->FindOrAddSection("GetAssetActions");
        Section.AddDynamicEntry(NAME_None, FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
            {
                FText Label;
                FText ToolTip;

                if (FInternationalization::Get().GetCurrentCulture()->GetTwoLetterISOLanguageName() == TEXT("ja"))
                {
                    Label = LOCTEXT("Material_RenameToRecommendedPrefix_JP", "推奨プレフィックスに名前を変更");
                    ToolTip = LOCTEXT("Material_RenameToRecommendedPrefixTooltip_JP", "選択したマテリアルインスタンスの名前をUnreal Engine推奨のプレフィックスに変更します。");
                }
                else
                {
                    Label = LOCTEXT("Material_RenameToRecommendedPrefix_EN", "Rename to Recommended Prefix");
                    ToolTip = LOCTEXT("Material_RenameToRecommendedPrefixTooltip_EN", "Rename the selected material instance to the Unreal Engine recommended prefix.");
                }

                const FSlateIcon Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.MaterialInstanceActor");
                const FToolMenuExecuteAction Action = FToolMenuExecuteAction::CreateStatic(&MenuExtension_MaterialInstance::OnExecuteAction);

                InSection.AddMenuEntry("Material_RenameToRecommendedPrefix", Label, ToolTip, Icon, Action);
            }));
#endif
    }
}

/**
 * Retrieves localized text based on the current culture.
 *
 * @param Key The key for the localized text.
 * @param Count Optional count for formatting.
 * @return The localized text.
 */
FText GetLocalizedText(const FString& Key, int32 Count = -1)
{
    if (FInternationalization::Get().GetCurrentCulture()->GetTwoLetterISOLanguageName() == TEXT("ja"))
    {
        if (Key == "RenameAllMaterialInstances")
            return LOCTEXT("RenameAllMaterialInstances_JP", "すべてのマテリアルインスタンスの名前を変更");
        if (Key == "RenameAllMaterialInstancesTooltip")
            return LOCTEXT("RenameAllMaterialInstancesTooltip_JP", "アセットフォルダ内のすべてのマテリアルインスタンスの名前を変更します");
        if (Key == "Material_RenameToRecommendedPrefix")
            return LOCTEXT("Material_RenameToRecommendedPrefix_JP", "推奨プレフィックスに名前を変更");
        if (Key == "Material_RenameToRecommendedPrefixTooltip")
            return LOCTEXT("Material_RenameToRecommendedPrefixTooltip_JP", "選択したマテリアルインスタンスの名前をUnreal Engine推奨のプレフィックスに変更します。");
        if (Key == "RenameCompleteWithCount")
            return FText::Format(LOCTEXT("RenameCompleteWithCount_JP", "すべてのマテリアルインスタンスの名前変更が完了しました。合計リネーム数: {0}"), Count);
    }
    else
    {
        if (Key == "RenameAllMaterialInstances")
            return LOCTEXT("RenameAllMaterialInstances_EN", "Rename All Material Instances");
        if (Key == "RenameAllMaterialInstancesTooltip")
            return LOCTEXT("RenameAllMaterialInstancesTooltip_EN", "Rename all material instances in the assets folder");
        if (Key == "Material_RenameToRecommendedPrefix")
            return LOCTEXT("Material_RenameToRecommendedPrefix_EN", "Rename to Recommended Prefix");
        if (Key == "Material_RenameToRecommendedPrefixTooltip")
            return LOCTEXT("Material_RenameToRecommendedPrefixTooltip_EN", "Rename the selected material instance to the Unreal Engine recommended prefix.");
        if (Key == "RenameCompleteWithCount")
            return FText::Format(LOCTEXT("RenameCompleteWithCount_EN", "Renaming of all material instances is complete. Total renamed: {0}"), Count);
    }
    return FText::GetEmpty();
}

/**
 * Adds a tool menu entry for renaming all material instances.
 */
void FMaterialInstanceRenamerModule::AddToolMenuEntry()
{
    UE_LOG(LogTemp, Log, TEXT("Adding tool menu entry for renaming all material instances"));

    FToolMenuOwnerScoped OwnerScoped(UE_MODULE_NAME);
    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");

    FToolMenuSection& Section = Menu->FindOrAddSection("Tools");

    const FText Label = GetLocalizedText("RenameAllMaterialInstances");
    const FText ToolTip = GetLocalizedText("RenameAllMaterialInstancesTooltip");

    Section.AddMenuEntry(
        "RenameAllMaterialInstances",
        Label,
        ToolTip,
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FMaterialInstanceRenamerModule::OnRenameAllMaterialInstancesClicked))
    );

    UE_LOG(LogTemp, Log, TEXT("Tool menu entry added successfully"));
}



void FMaterialInstanceRenamerModule::StartupModule()
{
    MenuExtension_MaterialInstance::AddMaterialContextMenuEntry();
    AddToolMenuEntry();
}


void FMaterialInstanceRenamerModule::OnRenameAllMaterialInstancesClicked()
{
    bIsBatchRename = true;

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    TArray<FAssetData> MaterialInstanceAssets;
    FARFilter Filter;
    Filter.PackagePaths.Add("/Game");
    Filter.bRecursivePaths = true;
    Filter.ClassPaths.Add(UMaterialInstanceConstant::StaticClass()->GetClassPathName());
    AssetRegistry.GetAssets(Filter, MaterialInstanceAssets);

    int32 RenamedCount = 0;
    for (const FAssetData& AssetData : MaterialInstanceAssets)
    {
        MenuExtension_MaterialInstance::RenameMaterialInstance(AssetData);
        RenamedCount++;
    }

    FText RenameCompleteMessage = GetLocalizedText("RenameCompleteWithCount", RenamedCount);
    FMessageDialog::Open(EAppMsgType::Ok, RenameCompleteMessage);

    bIsBatchRename = false;
}

void FMaterialInstanceRenamerModule::ShutdownModule()
{
    if (UToolMenus::IsToolMenuUIEnabled())
    {
        UToolMenus::UnregisterOwner(this);
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMaterialInstanceRenamerModule, MaterialInstanceRenamer)
