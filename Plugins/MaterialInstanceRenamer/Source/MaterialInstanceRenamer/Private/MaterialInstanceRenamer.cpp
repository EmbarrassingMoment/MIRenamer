﻿// Copyright 2025 kurorekish. All Rights Reserved.

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

namespace MenuExtension_MaterialInstance {

    static void RenameAsset(const FAssetData& SelectedAsset, const FString& NewName)
    {
        FString OldPackagePath = SelectedAsset.PackagePath.ToString();

        FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
        FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
        IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

        FString FinalNewName = NewName;
        FString NewAssetPath = OldPackagePath / FinalNewName + TEXT(".") + FinalNewName;
        int32 Suffix = 1;
        while (AssetRegistry.GetAssetByObjectPath(*NewAssetPath).IsValid())
        {
            FinalNewName = NewName + FString::Printf(TEXT("%d"), Suffix++);
            NewAssetPath = OldPackagePath / FinalNewName + TEXT(".") + FinalNewName;
        }

        UE_LOG(LogTemp, Log, TEXT("Renaming to: %s"), *FinalNewName);

        FAssetRenameData RenameData(SelectedAsset.GetAsset(), OldPackagePath, FinalNewName);
        EAssetRenameResult RenameResult = AssetToolsModule.Get().RenameAssetsWithDialog({ RenameData }, true);
        if (RenameResult == EAssetRenameResult::Success)
        {
            FString FinalNewAssetPath = OldPackagePath / FinalNewName + TEXT(".") + FinalNewName;
            FAssetData NewAssetData = AssetRegistry.GetAssetByObjectPath(*FinalNewAssetPath);

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

    static void RenameMaterialInstance(const FAssetData& SelectedAsset)
    {
        FString OldName = SelectedAsset.AssetName.ToString();

        if (OldName.StartsWith(TEXT("M_")) && OldName.Contains(TEXT("_Inst")))
        {
            FString CoreName = OldName.Mid(2, OldName.Find(TEXT("_Inst")) - 2);
            FString NewName = TEXT("MI_") + CoreName;
            RenameAsset(SelectedAsset, NewName);
        }
        else if (!OldName.StartsWith(TEXT("MI_")))
        {
            if (FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("ConfirmRename", "名前を本当に変更していいですか？")) == EAppReturnType::Yes)
            {
                FString NewName = TEXT("MI_") + OldName;
                RenameAsset(SelectedAsset, NewName);
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

FText GetLocalizedText(const FString& Key)
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
    }
    return FText::GetEmpty();
}

void FMaterialInstanceRenamerModule::AddToolMenuEntry()
{
    FToolMenuOwnerScoped OwnerScoped(UE_MODULE_NAME);
    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");

    FToolMenuSection& Section = Menu->FindOrAddSection("MaterialInstanceRenamer");

    const FText Label = GetLocalizedText("RenameAllMaterialInstances");
    const FText ToolTip = GetLocalizedText("RenameAllMaterialInstancesTooltip");

    Section.AddMenuEntry(
        "RenameAllMaterialInstances",
        Label,
        ToolTip,
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FMaterialInstanceRenamerModule::OnRenameAllMaterialInstancesClicked))
    );
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

    for (const FAssetData& AssetData : MaterialInstanceAssets)
    {
        MenuExtension_MaterialInstance::RenameMaterialInstance(AssetData);
    }

    FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("RenameComplete", "Renaming of all material instances is complete."));

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