// Copyright 2025 kurorekish. All Rights Reserved.

#include "MaterialInstanceRenamer.h"
#include "FAssetRenameUtil.h" 
#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "ContentBrowserModule.h"
#include "ContentBrowserMenuContexts.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"
#include "Misc/MessageDialog.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/ScopedSlowTask.h"
#include "Styling/AppStyle.h"
#include "Algo/AnyOf.h" 

#define LOCTEXT_NAMESPACE "FMaterialInstanceRenamerModule"

//----------------------------------------------------------------------//
// Localization Manager
//----------------------------------------------------------------------//

class FLocalizationManager
{
public:
    static FText GetText(const FString& Key)
    {
        const FString CultureName = FInternationalization::Get().GetCurrentCulture()->GetTwoLetterISOLanguageName();
        const TMap<FString, FText>* LangMap = LocalizationTable.Find(CultureName);

        if (LangMap && LangMap->Contains(Key))
        {
            return (*LangMap)[Key];
        }

        // Fallback to English
        return LocalizationTable[TEXT("en")][Key];
    }

private:
    static TMap<FString, TMap<FString, FText>> CreateLocalizationTable()
    {
        TMap<FString, TMap<FString, FText>> Table;

        // English
        TMap<FString, FText>& EnMap = Table.Emplace(TEXT("en"));
        EnMap.Emplace(TEXT("RenameAll"), LOCTEXT("RenameAll_EN", "Rename All Material Instances"));
        EnMap.Emplace(TEXT("RenameAllTooltip"), LOCTEXT("RenameAllTooltip_EN", "Rename all Material Instances in the project to the recommended prefix."));
        EnMap.Emplace(TEXT("RenameSelected"), LOCTEXT("RenameSelected_EN", "Rename to Recommended Prefix"));
        EnMap.Emplace(TEXT("RenameSelectedTooltip"), LOCTEXT("RenameSelectedTooltip_EN", "Rename the selected Material Instance(s) to the recommended prefix (MI_)."));
        EnMap.Emplace(TEXT("ConfirmBatchRename"), LOCTEXT("ConfirmBatchRename_EN", "Do you want to rename all Material Instances in the project? (MI_)"));
        EnMap.Emplace(TEXT("NoAssetsFound"), LOCTEXT("NoAssetsFound_EN", "No Material Instances found to rename."));
        EnMap.Emplace(TEXT("RenamingProgress"), LOCTEXT("RenamingProgress_EN", "Renaming Material Instances..."));
        EnMap.Emplace(TEXT("NotAMaterialInstance"), LOCTEXT("NotAMaterialInstance_EN", "The selected asset is not a Material Instance."));
        EnMap.Emplace(TEXT("RenameComplete"), LOCTEXT("RenameComplete_EN", "Rename Complete"));
        EnMap.Emplace(TEXT("RenameSummary"), LOCTEXT("RenameSummary_EN", "Renamed: {0}\nSkipped: {1}\nFailed: {2}\nInvalid Pattern: {3}"));

        // Japanese
        TMap<FString, FText>& JaMap = Table.Emplace(TEXT("ja"));
        JaMap.Emplace(TEXT("RenameAll"), LOCTEXT("RenameAll_JP", "すべてのマテリアルインスタンスをリネーム"));
        JaMap.Emplace(TEXT("RenameAllTooltip"), LOCTEXT("RenameAllTooltip_JP", "プロジェクト内のすべてのマテリアルインスタンスを推奨プレフィックスにリネームします。"));
        JaMap.Emplace(TEXT("RenameSelected"), LOCTEXT("RenameSelected_JP", "推奨プレフィックスにリネーム"));
        JaMap.Emplace(TEXT("RenameSelectedTooltip"), LOCTEXT("RenameSelectedTooltip_JP", "選択したマテリアルインスタンスを推奨プレフィックス（MI_）にリネームします。"));
        JaMap.Emplace(TEXT("ConfirmBatchRename"), LOCTEXT("ConfirmBatchRename_JP", "プロジェクト内のすべてのマテリアルインスタンスをリネームしますか？（MI_）"));
        JaMap.Emplace(TEXT("NoAssetsFound"), LOCTEXT("NoAssetsFound_JP", "リネーム対象のマテリアルインスタンスが見つかりませんでした。"));
        JaMap.Emplace(TEXT("RenamingProgress"), LOCTEXT("RenamingProgress_JP", "マテリアルインスタンスをリネーム中..."));
        JaMap.Emplace(TEXT("NotAMaterialInstance"), LOCTEXT("NotAMaterialInstance_JP", "選択されたアセットはマテリアルインスタンスではありません。"));
        JaMap.Emplace(TEXT("RenameComplete"), LOCTEXT("RenameComplete_JP", "リネーム完了"));
        JaMap.Emplace(TEXT("RenameSummary"), LOCTEXT("RenameSummary_JP", "リネーム: {0}\nスキップ: {1}\n失敗: {2}\n不正なパターン: {3}"));

        return Table;
    }

    static const TMap<FString, TMap<FString, FText>> LocalizationTable;
};

const TMap<FString, TMap<FString, FText>> FLocalizationManager::LocalizationTable = FLocalizationManager::CreateLocalizationTable();


//----------------------------------------------------------------------//
// Context Menu Logic
//----------------------------------------------------------------------//
namespace MenuExtension_MaterialInstance
{
    static void OnExecuteAction(const FToolMenuContext& MenuContext)
    {
        const UContentBrowserAssetContextMenuContext* Context = UContentBrowserAssetContextMenuContext::FindContextWithAssets(MenuContext);
        if (!Context) return;

        TArray<FAssetData> MaterialInstances;
        for (const FAssetData& AssetData : Context->SelectedAssets)
        {
            if (AssetData.GetClass()->IsChildOf(UMaterialInstanceConstant::StaticClass()))
            {
                MaterialInstances.Add(AssetData);
            }
        }

        if (MaterialInstances.IsEmpty())
        {
            FMessageDialog::Open(EAppMsgType::Ok, FLocalizationManager::GetText("NotAMaterialInstance"));
            return;
        }

        FScopedSlowTask SlowTask(MaterialInstances.Num(), FLocalizationManager::GetText("RenamingProgress"));
        SlowTask.MakeDialog();

        int32 RenamedCount = 0, SkippedCount = 0, FailedCount = 0, InvalidPatternCount = 0;

        for (const FAssetData& AssetData : MaterialInstances)
        {
            FString AssetName = AssetData.AssetName.ToString();
            SlowTask.EnterProgressFrame(1, FText::Format(LOCTEXT("RenamingAsset", "Renaming {0}..."), FText::FromString(AssetName)));

            const ERenameResult Result = FAssetRenameUtil::RenameMaterialInstance(AssetData);
            switch (Result)
            {
                case ERenameResult::Renamed: RenamedCount++; break;
                case ERenameResult::Skipped: SkippedCount++; break;
                case ERenameResult::Failed: FailedCount++; break;
                case ERenameResult::InvalidPattern: InvalidPatternCount++; break;
            }
        }

        FText DialogTitle = FLocalizationManager::GetText("RenameComplete");
        FText DialogMessage = FText::Format(
            FLocalizationManager::GetText("RenameSummary"),
            FText::AsNumber(RenamedCount),
            FText::AsNumber(SkippedCount),
            FText::AsNumber(FailedCount),
            FText::AsNumber(InvalidPatternCount)
        );
        FMessageDialog::Open(EAppMsgType::Ok, DialogMessage, &DialogTitle);
    }
}

//----------------------------------------------------------------------//
// FMaterialInstanceRenamerModule Implementation
//----------------------------------------------------------------------//

void FMaterialInstanceRenamerModule::StartupModule()
{
    if (IsRunningCommandlet()) return;
    AddMaterialContextMenuEntry();
    AddToolMenuEntry();
}

void FMaterialInstanceRenamerModule::ShutdownModule()
{
}

void FMaterialInstanceRenamerModule::AddMaterialContextMenuEntry()
{
    if (!FSlateApplication::IsInitialized()) return;

    FToolMenuOwnerScoped OwnerScoped(this);
    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.MaterialInterface");
    FToolMenuSection& Section = Menu->FindOrAddSection("GetAssetActions");

    Section.AddDynamicEntry(TEXT("MaterialInstanceRenamerEntry"), FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
    {
        const UContentBrowserAssetContextMenuContext* Context = InSection.FindContext<UContentBrowserAssetContextMenuContext>();
        if (Context)
        {
            bool bHasMaterialInstance = Algo::AnyOf(Context->SelectedAssets, [](const FAssetData& AssetData)
            {
                return AssetData.GetClass()->IsChildOf(UMaterialInstanceConstant::StaticClass());
            });

            if (bHasMaterialInstance)
            {
                InSection.AddMenuEntry(
                    "MaterialInstance_RenameToRecommendedPrefix",
                    FLocalizationManager::GetText("RenameSelected"),
                    FLocalizationManager::GetText("RenameSelectedTooltip"),
                    FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.MaterialInstanceConstant"),
                    FToolMenuExecuteAction::CreateStatic(&MenuExtension_MaterialInstance::OnExecuteAction)
                );
            }
        }
    }));
}

void FMaterialInstanceRenamerModule::AddToolMenuEntry()
{
    if (!FSlateApplication::IsInitialized()) return;

    FToolMenuOwnerScoped OwnerScoped(this);
    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
    FToolMenuSection& Section = Menu->FindOrAddSection("MaterialInstanceRenamer", LOCTEXT("MaterialInstanceRenamerSection", "MaterialInstanceRenamer"));

    Section.AddMenuEntry(
        "RenameAllMaterialInstances",
        FLocalizationManager::GetText("RenameAll"),
        FLocalizationManager::GetText("RenameAllTooltip"),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FMaterialInstanceRenamerModule::OnRenameAllMaterialInstancesClicked))
    );
}

void FMaterialInstanceRenamerModule::OnRenameAllMaterialInstancesClicked()
{
    if (FMessageDialog::Open(EAppMsgType::YesNo, FLocalizationManager::GetText("ConfirmBatchRename")) != EAppReturnType::Yes)
    {
        return;
    }

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    TArray<FString> ScanPaths;
    ScanPaths.Add(TEXT("/Game"));
    AssetRegistry.ScanPathsSynchronous(ScanPaths, true);

    TArray<FAssetData> MaterialInstanceAssets;
    FARFilter Filter;
    Filter.PackagePaths.Add("/Game");
    Filter.bRecursivePaths = true;
    Filter.ClassPaths.Add(UMaterialInstanceConstant::StaticClass()->GetClassPathName());
    AssetRegistry.GetAssets(Filter, MaterialInstanceAssets);

    if (MaterialInstanceAssets.IsEmpty())
    {
        FMessageDialog::Open(EAppMsgType::Ok, FLocalizationManager::GetText("NoAssetsFound"));
        return;
    }

    FScopedSlowTask SlowTask(MaterialInstanceAssets.Num(), FLocalizationManager::GetText("RenamingProgress"));
    SlowTask.MakeDialog(true);

    int32 RenamedCount = 0, SkippedCount = 0, FailedCount = 0, InvalidPatternCount = 0;

    for (const FAssetData& AssetData : MaterialInstanceAssets)
    {
        if (SlowTask.ShouldCancel()) break;

        FString AssetName = AssetData.AssetName.ToString();
        SlowTask.EnterProgressFrame(1, FText::Format(LOCTEXT("RenamingAsset", "Renaming {0}..."), FText::FromString(AssetName)));

        const ERenameResult Result = FAssetRenameUtil::RenameMaterialInstance(AssetData);
        switch (Result)
        {
            case ERenameResult::Renamed: RenamedCount++; break;
            case ERenameResult::Skipped: SkippedCount++; break;
            case ERenameResult::Failed: FailedCount++; break;
            case ERenameResult::InvalidPattern: InvalidPatternCount++; break;
        }
    }

    FText DialogTitle = FLocalizationManager::GetText("RenameComplete");
    FText DialogMessage = FText::Format(
        FLocalizationManager::GetText("RenameSummary"),
        FText::AsNumber(RenamedCount),
        FText::AsNumber(SkippedCount),
        FText::AsNumber(FailedCount),
        FText::AsNumber(InvalidPatternCount)
    );
    FMessageDialog::Open(EAppMsgType::Ok, DialogMessage, &DialogTitle);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMaterialInstanceRenamerModule, MaterialInstanceRenamer)