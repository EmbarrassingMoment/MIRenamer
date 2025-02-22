// Copyright Epic Games, Inc. All Rights Reserved.

#include "MaterialInstanceRenamer.h"
#include "Modules/ModuleManager.h"
#include "Materials/MaterialInstance.h"
#include "ContentBrowserMenuContexts.h"
#include "ToolMenus.h"
#include "AssetRegistry/AssetData.h"
#include "AssetToolsModule.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include <AssetRegistry/AssetRegistryModule.h>

#define LOCTEXT_NAMESPACE "FMaterialInstanceRenamerModule"

namespace MenuExtension_MaterialInstance {
    static void CheckAssetReferences(const FString& AssetPath)
    {
        FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
        IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

        FAssetData AssetData = AssetRegistry.GetAssetByObjectPath(*AssetPath);
        if (!AssetData.IsValid())
        {
            FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("InvalidAssetPath", "The asset path is invalid."));
            return;
        }
    }

    static FString GenerateUniqueAssetName(const FString& BaseName, const FString& PackagePath, IAssetRegistry& AssetRegistry)
    {
        FString FinalNewName = BaseName;
        FString NewAssetPath = PackagePath / FinalNewName + TEXT(".") + FinalNewName;
        int32 Suffix = 1;
        while (AssetRegistry.GetAssetByObjectPath(*NewAssetPath).IsValid())
        {
            FinalNewName = BaseName + FString::Printf(TEXT("%d"), Suffix++);
            NewAssetPath = PackagePath / FinalNewName + TEXT(".") + FinalNewName;
        }
        return FinalNewName;
    }

    static void HandleRenameResult(EAssetRenameResult RenameResult)
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

    static bool ConfirmRename()
    {
        EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("ConfirmRename", "名前を本当に変更していいですか？"));
        return ReturnType == EAppReturnType::Yes;
    }

    static void RenameAsset(const FAssetData& SelectedAsset, const FString& NewName)
    {
        FString OldPackagePath = SelectedAsset.PackagePath.ToString();

        FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
        FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
        IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

        FString FinalNewName = GenerateUniqueAssetName(NewName, OldPackagePath, AssetRegistry);
        UE_LOG(LogTemp, Log, TEXT("Renaming to: %s"), *FinalNewName);

        FAssetRenameData RenameData(SelectedAsset.GetAsset(), OldPackagePath, FinalNewName);
        EAssetRenameResult RenameResult = AssetToolsModule.Get().RenameAssetsWithDialog({ RenameData }, true);
        if (RenameResult == EAssetRenameResult::Success)
        {
            FString NewAssetPath = OldPackagePath / FinalNewName + TEXT(".") + FinalNewName;
            FAssetData NewAssetData = AssetRegistry.GetAssetByObjectPath(*NewAssetPath);

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
            HandleRenameResult(RenameResult);
        }
    }

    static void RenameMaterialInstance(const FAssetData& SelectedAsset)
    {
        FString OldName = SelectedAsset.AssetName.ToString();

        CheckAssetReferences(SelectedAsset.ObjectPath.ToString());

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
            if (ConfirmRename())
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
    }
}

void FMaterialInstanceRenamerModule::StartupModule()
{
    MenuExtension_MaterialInstance::AddMaterialContextMenuEntry();
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