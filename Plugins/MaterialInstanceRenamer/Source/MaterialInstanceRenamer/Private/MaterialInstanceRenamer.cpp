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
            switch (RenameResult)
            {
            case EAssetRenameResult::Failure:
                FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("RenameFailure", "Rename failed due to an unspecified error."));
                break;
            case EAssetRenameResult::Pending:
                FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("RenamePending", "Rename is pending."));
                break;
            default:
                FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("RenameUnknownError", "Rename failed with an unknown error."));
                break;
            }
        }
    }


    static void RenameMaterialInstance(const FAssetData& SelectedAsset)
    {
        FString OldName = SelectedAsset.AssetName.ToString();

        FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
        IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

        FAssetData AssetData = AssetRegistry.GetAssetByObjectPath(*SelectedAsset.ObjectPath.ToString());
        if (!AssetData.IsValid())
        {
            FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("InvalidAssetPath", "The asset path is invalid."));
            return;
        }

        if (OldName.StartsWith(TEXT("M_")) && OldName.Contains(TEXT("_Inst")))
        {
            int32 StartIndex = 2;
            int32 EndIndex = OldName.Find(TEXT("_Inst"));
            FString CoreName = OldName.Mid(StartIndex, EndIndex - StartIndex);
            FString NewName = TEXT("MI_") + CoreName;
            RenameAsset(SelectedAsset, NewName);
        }
        else if (!OldName.StartsWith(TEXT("MI_")))
        {
            EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("ConfirmRename", "名前を本当に変更していいですか？"));
            if (ReturnType == EAppReturnType::Yes)
            {
                FString NewName = TEXT("MI_") + OldName;
                RenameAsset(SelectedAsset, NewName);
            }
        }
        else
        {
            FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("NamePatternMismatch", "The name does not match the required pattern."));
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
                const TAttribute<FText> Label = LOCTEXT("Material_RenameToRecommendedPrefix", "Rename to Recommended Prefix");
                const TAttribute<FText> ToolTip = LOCTEXT("Material_RenameToRecommendedPrefixTooltip", "Rename the selected material instance to the Unreal Engine recommended prefix.");
                const FSlateIcon Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.MaterialInstanceActor");
                const FToolMenuExecuteAction Action = FToolMenuExecuteAction::CreateStatic(&MenuExtension_MaterialInstance::OnExecuteAction);

                InSection.AddMenuEntry("Material_RenameToRecommendedPrefix", Label, ToolTip, Icon, Action);
            }));
#endif
    }
}

void FMaterialInstanceRenamerModule::StartupModule()
{
    MenuExtension_MaterialInstance::AddMaterialContextMenuEntry();
    AddToolMenuEntry();
}

void FMaterialInstanceRenamerModule::AddToolMenuEntry()
{
    FToolMenuOwnerScoped OwnerScoped(UE_MODULE_NAME);
    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");

    FToolMenuSection& Section = Menu->FindOrAddSection("MaterialInstanceRenamer");
    Section.AddMenuEntry(
        "RenameAllMaterialInstances",
        LOCTEXT("RenameAllMaterialInstances", "Rename All Material Instances"),
        LOCTEXT("RenameAllMaterialInstancesTooltip", "Rename all material instances in the assets folder"),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FMaterialInstanceRenamerModule::OnRenameAllMaterialInstancesClicked))
    );
}

void FMaterialInstanceRenamerModule::OnRenameAllMaterialInstancesClicked()
{
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

    // Notify completion of renaming
    FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("RenameComplete", "Renaming of all material instances is complete."));
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
