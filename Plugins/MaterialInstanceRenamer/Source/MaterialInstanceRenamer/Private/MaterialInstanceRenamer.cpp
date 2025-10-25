// Copyright 2025 kurorekish. All Rights Reserved.

#include "MaterialInstanceRenamer.h"
#include "FAssetRenameUtil.h"
#include "MaterialInstanceRenamerSettings.h"
#include "ISettingsModule.h"
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
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

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

        if (LangMap)
        {
            const FText* FoundText = LangMap->Find(Key);
            if (FoundText)
            {
                return *FoundText;
            }
        }

        // Fallback to English
        const TMap<FString, FText>* EnLangMap = LocalizationTable.Find(TEXT("en"));
        if (EnLangMap)
        {
            const FText* FoundText = EnLangMap->Find(Key);
            if (FoundText)
            {
                return *FoundText;
            }
        }

        return FText::FromString(FString::Printf(TEXT("LOC_KEY_NOT_FOUND: %s"), *Key));
    }

private:
    static TMap<FString, TMap<FString, FText>> CreateLocalizationTable()
    {
        TMap<FString, TMap<FString, FText>> Table;

        // English
        TMap<FString, FText>& EnMap = Table.Emplace(TEXT("en"));
        EnMap.Emplace(TEXT("RenameAll"), LOCTEXT("RenameAll_EN", "Rename All Material Instances"));
        EnMap.Emplace(TEXT("RenameAllTooltip"), LOCTEXT("RenameAllTooltip_EN", "Rename all Material Instances in the project to the recommended prefix."));
        EnMap.Emplace(TEXT("RenameSelected"), LOCTEXT("RenameSelected_EN", "Rename with Prefix ({0})"));
        EnMap.Emplace(TEXT("RenameSelectedTooltip"), LOCTEXT("RenameSelectedTooltip_EN", "Rename the selected Material Instance(s) to the recommended prefix ({0})."));
        EnMap.Emplace(TEXT("ConfirmBatchRename"), LOCTEXT("ConfirmBatchRename_EN", "Do you want to rename all Material Instances in the project? ({0})"));
        EnMap.Emplace(TEXT("NoAssetsFound"), LOCTEXT("NoAssetsFound_EN", "No Material Instances found to rename."));
        EnMap.Emplace(TEXT("RenamingProgress"), LOCTEXT("RenamingProgress_EN", "Renaming Material Instances..."));
        EnMap.Emplace(TEXT("NotAMaterialInstance"), LOCTEXT("NotAMaterialInstance_EN", "The selected asset is not a Material Instance."));
        EnMap.Emplace(TEXT("RenameComplete"), LOCTEXT("RenameComplete_EN", "Rename Complete"));
        EnMap.Emplace(TEXT("RenameSummary"), LOCTEXT("RenameSummary_EN", "Renamed: {0}\nSkipped: {1}\nFailed: {2}\nInvalid Pattern: {3}"));
        EnMap.Emplace(TEXT("AutoRenameOnCreate"), LOCTEXT("AutoRenameOnCreate_EN", "Auto-Rename on Create"));
        EnMap.Emplace(TEXT("AutoRenameOnCreateTooltip"), LOCTEXT("AutoRenameOnCreateTooltip_EN", "Toggle auto-renaming of material instances on creation"));
        EnMap.Emplace(TEXT("ShowNotificationOnAutoRename"), LOCTEXT("ShowNotificationOnAutoRename_EN", "Show Notification on Auto-Rename"));
        EnMap.Emplace(TEXT("AutoRenameNotification"), LOCTEXT("AutoRenameNotification_EN", "Renamed {0} to {1}"));
		EnMap.Emplace(TEXT("MenuSection"), LOCTEXT("MenuSection_EN", "MaterialInstanceRenamer"));
		EnMap.Emplace(TEXT("MenuSubMenu"), LOCTEXT("MenuSubMenu_EN", "MaterialInstanceRenamer"));
		EnMap.Emplace(TEXT("MenuSubMenuTooltip"), LOCTEXT("MenuSubMenuTooltip_EN", "Material Instance Renamer Tools"));
		EnMap.Emplace(TEXT("MenuGeneralSection"), LOCTEXT("MenuGeneralSection_EN", "General"));

        // Japanese
        TMap<FString, FText>& JaMap = Table.Emplace(TEXT("ja"));
        JaMap.Emplace(TEXT("RenameAll"), LOCTEXT("RenameAll_JP", "すべてのマテリアルインスタンスをリネーム"));
        JaMap.Emplace(TEXT("RenameAllTooltip"), LOCTEXT("RenameAllTooltip_JP", "プロジェクト内のすべてのマテリアルインスタンスを推奨プレフィックスにリネームします。"));
        JaMap.Emplace(TEXT("RenameSelected"), LOCTEXT("RenameSelected_JP", "プレフィックス({0})でリネーム"));
        JaMap.Emplace(TEXT("RenameSelectedTooltip"), LOCTEXT("RenameSelectedTooltip_JP", "選択したマテリアルインスタンスを推奨プレフィックス（{0}）にリネームします。"));
        JaMap.Emplace(TEXT("ConfirmBatchRename"), LOCTEXT("ConfirmBatchRename_JP", "プロジェクト内のすべてのマテリアルインスタンスをリネームしますか？（{0}）"));
        JaMap.Emplace(TEXT("NoAssetsFound"), LOCTEXT("NoAssetsFound_JP", "リネーム対象のマテリアルインスタンスが見つかりませんでした。"));
        JaMap.Emplace(TEXT("RenamingProgress"), LOCTEXT("RenamingProgress_JP", "マテリアルインスタンスをリネーム中..."));
        JaMap.Emplace(TEXT("NotAMaterialInstance"), LOCTEXT("NotAMaterialInstance_JP", "選択されたアセットはマテリアルインスタンスではありません。"));
        JaMap.Emplace(TEXT("RenameComplete"), LOCTEXT("RenameComplete_JP", "リネーム完了"));
        JaMap.Emplace(TEXT("RenameSummary"), LOCTEXT("RenameSummary_JP", "リネーム: {0}\nスキップ: {1}\n失敗: {2}\n不正なパターン: {3}"));
        JaMap.Emplace(TEXT("AutoRenameOnCreate"), LOCTEXT("AutoRenameOnCreate_JP", "作成時に自動リネーム"));
        JaMap.Emplace(TEXT("AutoRenameOnCreateTooltip"), LOCTEXT("AutoRenameOnCreateTooltip_JP", "作成時のマテリアルインスタンスの自動リネームを切り替えます"));
        JaMap.Emplace(TEXT("ShowNotificationOnAutoRename"), LOCTEXT("ShowNotificationOnAutoRename_JP", "自動リネーム時に通知を表示"));
        JaMap.Emplace(TEXT("AutoRenameNotification"), LOCTEXT("AutoRenameNotification_JP", "{0} を {1} にリネームしました"));
        JaMap.Emplace(TEXT("MenuSection"), LOCTEXT("MenuSection_JP", "MaterialInstanceRenamer"));
        JaMap.Emplace(TEXT("MenuSubMenu"), LOCTEXT("MenuSubMenu_JP", "MaterialInstanceRenamer"));
		JaMap.Emplace(TEXT("MenuSubMenuTooltip"), LOCTEXT("MenuSubMenuTooltip_JP", "Material Instance Renamer ツール"));
		JaMap.Emplace(TEXT("MenuGeneralSection"), LOCTEXT("MenuGeneralSection_JP", "一般"));

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

        if (MaterialInstances.Num() > 1)
        {
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
}

//----------------------------------------------------------------------//
// FMaterialInstanceRenamerModule Implementation
//----------------------------------------------------------------------//

void FMaterialInstanceRenamerModule::StartupModule()
{
    RegisterSettings();
    if (IsRunningCommandlet()) return;
    AddMaterialContextMenuEntry();
    AddToolMenuEntry();

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
    AssetRegistryModule.Get().OnAssetAdded().AddRaw(this, &FMaterialInstanceRenamerModule::OnAssetAdded);
}

void FMaterialInstanceRenamerModule::ShutdownModule()
{
    UnregisterSettings();

    if (FModuleManager::Get().IsModuleLoaded(AssetRegistryConstants::ModuleName))
    {
        FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
        AssetRegistryModule.Get().OnAssetAdded().RemoveAll(this);
    }
}

void FMaterialInstanceRenamerModule::RegisterSettings()
{
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
    if (SettingsModule)
    {
        SettingsModule->RegisterSettings("Project", "Plugins", "MaterialInstanceRenamer",
            LOCTEXT("RenamerSettingsName", "Material Instance Renamer"),
            LOCTEXT("RenamerSettingsDescription", "Configure the settings for the Material Instance Renamer plugin."),
            GetMutableDefault<UMaterialInstanceRenamerSettings>()
        );
    }
}

void FMaterialInstanceRenamerModule::UnregisterSettings()
{
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
    if (SettingsModule)
    {
        SettingsModule->UnregisterSettings("Project", "Plugins", "MaterialInstanceRenamer");
    }
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
                const UMaterialInstanceRenamerSettings* Settings = GetDefault<UMaterialInstanceRenamerSettings>();
                const FText Label = FText::Format(
                    FLocalizationManager::GetText("RenameSelected"),
                    FText::FromString(Settings->RenamePrefix)
                );
                const FText Tooltip = FText::Format(
                    FLocalizationManager::GetText("RenameSelectedTooltip"),
                    FText::FromString(Settings->RenamePrefix)
                );

                InSection.AddMenuEntry(
                    "MaterialInstance_RenameToRecommendedPrefix",
                    Label,
                    Tooltip,
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
    FToolMenuSection& Section = Menu->FindOrAddSection("MaterialInstanceRenamer", FLocalizationManager::GetText("MenuSection"));

    Section.AddSubMenu(
        "MaterialInstanceRenamerSubMenu",
        FLocalizationManager::GetText("MenuSubMenu"),
        FLocalizationManager::GetText("MenuSubMenuTooltip"),
        FNewToolMenuDelegate::CreateLambda([this](UToolMenu* SubMenu)
        {
            FToolMenuSection& SubMenuSection = SubMenu->AddSection("General", FLocalizationManager::GetText("MenuGeneralSection"));
            SubMenuSection.AddMenuEntry(
                "RenameAllMaterialInstances",
                FLocalizationManager::GetText("RenameAll"),
                FLocalizationManager::GetText("RenameAllTooltip"),
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateRaw(this, &FMaterialInstanceRenamerModule::OnRenameAllMaterialInstancesClicked))
            );

            SubMenuSection.AddMenuEntry(
                "ToggleAutoRename",
                FLocalizationManager::GetText("AutoRenameOnCreate"),
                FLocalizationManager::GetText("AutoRenameOnCreateTooltip"),
                FSlateIcon(),
                FUIAction(
                    FExecuteAction::CreateLambda([]()
                    {
                        UMaterialInstanceRenamerSettings* Settings = GetMutableDefault<UMaterialInstanceRenamerSettings>();
                        Settings->bAutoRenameOnCreate = !Settings->bAutoRenameOnCreate;
                        Settings->SaveConfig();
                    }),
                    FCanExecuteAction(),
                    FIsActionChecked::CreateLambda([]()
                    {
                        return GetDefault<UMaterialInstanceRenamerSettings>()->bAutoRenameOnCreate;
                    })
                ),
                EUserInterfaceActionType::ToggleButton
            );
        })
    );
}

void FMaterialInstanceRenamerModule::OnRenameAllMaterialInstancesClicked()
{
    const UMaterialInstanceRenamerSettings* Settings = GetDefault<UMaterialInstanceRenamerSettings>();
    const FText ConfirmMessage = FText::Format(
        FLocalizationManager::GetText("ConfirmBatchRename"),
        FText::FromString(Settings->RenamePrefix)
    );

    if (FMessageDialog::Open(EAppMsgType::YesNo, ConfirmMessage) != EAppReturnType::Yes)
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

void FMaterialInstanceRenamerModule::OnAssetAdded(const FAssetData& AssetData)
{
	if (bIsRenamingAsset)
	{
		return;
	}

	const UMaterialInstanceRenamerSettings* Settings = GetDefault<UMaterialInstanceRenamerSettings>();
	if (!Settings->bAutoRenameOnCreate)
	{
		return;
	}

	if (!AssetData.GetClass()->IsChildOf(UMaterialInstanceConstant::StaticClass()))
	{
		return;
	}

	bIsRenamingAsset = true;
	const ERenameResult Result = FAssetRenameUtil::RenameMaterialInstance(AssetData);
	bIsRenamingAsset = false;

	if (Result == ERenameResult::Renamed && Settings->bShowNotificationOnAutoRename)
	{
		FText Message = FText::Format(
			FLocalizationManager::GetText("AutoRenameNotification"),
			FText::FromString(AssetData.AssetName.ToString()),
			FText::FromString(FAssetRenameUtil::GetNewAssetName(AssetData.AssetName.ToString(), Settings->RenamePrefix))
		);

		FNotificationInfo Info(Message);
		Info.ExpireDuration = 3.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMaterialInstanceRenamerModule, MaterialInstanceRenamer)