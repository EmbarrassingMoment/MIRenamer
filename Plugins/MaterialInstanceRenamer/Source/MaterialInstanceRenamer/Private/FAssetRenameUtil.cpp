// Copyright 2025 kurorekish. All Rights Reserved.

#include "FAssetRenameUtil.h"
#include "MaterialInstanceRenamer.h"
#include "Modules/ModuleManager.h"
#include "ContentBrowserMenuContexts.h"
#include "ToolMenus.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Internationalization/Culture.h"
#include <AssetRegistry/AssetRegistryModule.h>

#define LOCTEXT_NAMESPACE "FMaterialInstanceRenamerModule"

namespace MenuExtension_MaterialInstance {

    /**
     * Renames the material instance asset to the recommended prefix.
     *
     * @param SelectedAsset The material instance asset to rename.
     */
    void RenameMaterialInstance(const FAssetData& SelectedAsset)
    {
        FAssetRenameUtil::RenameMaterialInstance(SelectedAsset);
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
        if (Key == "ConfirmBatchRename")
            return LOCTEXT("ConfirmBatchRename_JP", "すべてのマテリアルインスタンスの名前を変更しますか？");
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
        if (Key == "ConfirmBatchRename")
            return LOCTEXT("ConfirmBatchRename_EN", "Do you want to rename all material instances?");
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
    if (FMessageDialog::Open(EAppMsgType::YesNo, GetLocalizedText("ConfirmBatchRename")) != EAppReturnType::Yes)
    {
        return;
    }

    bIsBatchRename = true;

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    TArray<FAssetData> MaterialInstanceAssets;
    FARFilter Filter;
    Filter.PackagePaths.Add("/Game");
    Filter.bRecursivePaths = true;
    Filter.ClassPaths.Add(UMaterialInstanceConstant::StaticClass()->GetClassPathName());
    AssetRegistry.GetAssets(Filter, MaterialInstanceAssets);

    int32 TotalAssets = MaterialInstanceAssets.Num();
    if (TotalAssets == 0)
    {
        FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("NoAssetsFound", "マテリアルインスタンスが見つかりませんでした。"));
        bIsBatchRename = false;
        return;
    }
    FScopedSlowTask SlowTask(TotalAssets, LOCTEXT("RenamingMaterialInstances", "マテリアルインスタンスの名前を変更中..."));

    int32 RenamedCount = 0;
    for (const FAssetData& AssetData : MaterialInstanceAssets)
    {
        SlowTask.EnterProgressFrame(1, FText::Format(LOCTEXT("RenamingAsset", "{0} をリネーム中..."), FText::FromName(AssetData.AssetName)));

        FAssetRenameUtil::RenameMaterialInstance(AssetData, true);
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