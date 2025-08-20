// Copyright 2025 kurorekish. All Rights Reserved.

#include "MaterialInstanceRenamer.h"
#include "FAssetRenameUtil.h"
#include "SPreviewRenameDialog.h"
#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "ContentBrowserModule.h" // For ExtendToolMenu_AssetContextMenu
#include "ContentBrowserMenuContexts.h" // For UContentBrowserAssetContextMenuContext
#include "Materials/MaterialInterface.h" // For UMaterialInterface::StaticClass()
#include "Materials/MaterialInstanceConstant.h" // For Asset Registry Query
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h" // For FInternationalization
#include "Misc/MessageDialog.h" // For FMessageDialog
#include "Framework/MultiBox/MultiBoxBuilder.h" // For FUIAction
#include "AssetRegistry/AssetRegistryModule.h" // For Asset Registry
#include "Misc/ScopedSlowTask.h" // For FScopedSlowTask
#include "Styling/AppStyle.h" // For FAppStyle

#define LOCTEXT_NAMESPACE "FMaterialInstanceRenamerModule"

// ... (GetLocalizedText and MenuExtension_MaterialInstance namespaces are unchanged) ...
//----------------------------------------------------------------------//
// Localized Text Helper
//----------------------------------------------------------------------//

/**
 * Retrieves localized text based on the current culture.
 *
 * @param Key The key for the localized text.
 * @param Count Optional count for formatting.
 * @return The localized text.
 */
FText FMaterialInstanceRenamerModule::GetLocalizedText(const FString& Key, int32 Count)
{
	const FString CultureName = FInternationalization::Get().GetCurrentCulture()->GetTwoLetterISOLanguageName();

	if (CultureName == TEXT("ja"))
	{
		if (Key == "RenameAllMaterialInstances") return LOCTEXT("RenameAllMaterialInstances_JP", "すべてのマテリアルインスタンスをリネーム");
		if (Key == "RenameAllMaterialInstancesTooltip") return LOCTEXT("RenameAllMaterialInstancesTooltip_JP", "プロジェクト内のすべてのマテリアルインスタンスを推奨プレフィックスにリネームします。");
		if (Key == "Material_RenameToRecommendedPrefix") return LOCTEXT("Material_RenameToRecommendedPrefix_JP", "推奨プレフィックスにリネーム");
		if (Key == "Material_RenameToRecommendedPrefixTooltip") return LOCTEXT("Material_RenameToRecommendedPrefixTooltip_JP", "選択したマテリアルインスタンスを推奨プレフィックス（MI_）にリネームします。");
		if (Key == "RenameCompleteWithCount") return FText::Format(LOCTEXT("RenameCompleteWithCount_JP", "すべてのマテリアルインスタンスのリネームが完了しました。リネーム数: {0}"), FText::AsNumber(Count));
		if (Key == "NoAssetsFound") return LOCTEXT("NoAssetsFound_JP", "リネーム対象のマテリアルインスタンスが見つかりませんでした。");
		if (Key == "RenamingMaterialInstances") return LOCTEXT("RenamingMaterialInstances_JP", "マテリアルインスタンスをリネーム中...");
		if (Key == "RenamingAsset") return LOCTEXT("RenamingAsset_JP", "{0} をリネーム中...");
		if (Key == "NotAMaterialInstance") return LOCTEXT("NotAMaterialInstance_JP", "選択されたアセットはマテリアルインスタンスではありません。");
		if (Key == "NoAssetsToRename") return LOCTEXT("NoAssetsToRename_JP", "すべてのマテリアルインスタンスは既に命名規則に従っています。");
		if (Key == "RenamePreviewTitle") return LOCTEXT("RenamePreviewTitle_JP", "リネーム プレビュー");
		if (Key == "CurrentNameHeader") return LOCTEXT("CurrentNameHeader_JP", "現在の名前");
		if (Key == "NewNameHeader") return LOCTEXT("NewNameHeader_JP", "新しい名前");
		if (Key == "ConfirmButton") return LOCTEXT("ConfirmButton_JP", "確認");
		if (Key == "CancelButton") return LOCTEXT("CancelButton_JP", "キャンセル");
	}
	else // Default to English
	{
		if (Key == "RenameAllMaterialInstances") return LOCTEXT("RenameAllMaterialInstances_EN", "Rename All Material Instances");
		if (Key == "RenameAllMaterialInstancesTooltip") return LOCTEXT("RenameAllMaterialInstancesTooltip_EN", "Rename all Material Instances in the project to the recommended prefix.");
		if (Key == "Material_RenameToRecommendedPrefix") return LOCTEXT("Material_RenameToRecommendedPrefix_EN", "Rename to Recommended Prefix");
		if (Key == "Material_RenameToRecommendedPrefixTooltip") return LOCTEXT("Material_RenameToRecommendedPrefixTooltip_EN", "Rename the selected Material Instance(s) to the recommended prefix (MI_).");
		if (Key == "RenameCompleteWithCount") return FText::Format(LOCTEXT("RenameCompleteWithCount_EN", "Renaming of all Material Instances is complete. Total renamed: {0}"), FText::AsNumber(Count));
		if (Key == "NoAssetsFound") return LOCTEXT("NoAssetsFound_EN", "No Material Instances found to rename.");
		if (Key == "RenamingMaterialInstances") return LOCTEXT("RenamingMaterialInstances_EN", "Renaming Material Instances...");
		if (Key == "RenamingAsset") return LOCTEXT("RenamingAsset_EN", "Renaming {0}...");
		if (Key == "NotAMaterialInstance") return LOCTEXT("NotAMaterialInstance_EN", "The selected asset is not a Material Instance.");
		if (Key == "NoAssetsToRename") return LOCTEXT("NoAssetsToRename_EN", "All material instances already follow the naming convention.");
		if (Key == "RenamePreviewTitle") return LOCTEXT("RenamePreviewTitle_EN", "Rename Preview");
		if (Key == "CurrentNameHeader") return LOCTEXT("CurrentNameHeader_EN", "Current Name");
		if (Key == "NewNameHeader") return LOCTEXT("NewNameHeader_EN", "New Name");
		if (Key == "ConfirmButton") return LOCTEXT("ConfirmButton_EN", "Confirm");
		if (Key == "CancelButton") return LOCTEXT("CancelButton_EN", "Cancel");
	}
	return FText::Format(LOCTEXT("LocalizationNotFound", "Localization key '{0}' not found"), FText::FromString(Key));
}


//----------------------------------------------------------------------//
// Context Menu Logic (Internal Namespace)
//----------------------------------------------------------------------//
namespace MenuExtension_MaterialInstance
{
	/**
	 * Executes the rename action from the context menu.
	 * @param MenuContext The context of the tool menu.
	 */
	static void OnExecuteAction(const FToolMenuContext& MenuContext)
	{
		if (const UContentBrowserAssetContextMenuContext* Context = MenuContext.FindContext<UContentBrowserAssetContextMenuContext>())
		{
			bool bFoundNonMaterialInstance = false;
			TArray<FAssetData> AssetsToRename;

			for (const FAssetData& SelectedAsset : Context->SelectedAssets)
			{
				// Check if the asset is a Material Instance Constant
				if (SelectedAsset.GetClass() == UMaterialInstanceConstant::StaticClass())
				{
					AssetsToRename.Add(SelectedAsset);
				}
				else
				{
					// Attempt to load asset if class check fails (might be redirector or unloaded)
					UObject* Asset = SelectedAsset.GetAsset();
					if (Asset && Asset->IsA<UMaterialInstanceConstant>())
					{
						AssetsToRename.Add(SelectedAsset);
					}
					else
					{
						bFoundNonMaterialInstance = true;
						UE_LOG(LogTemp, Log, TEXT("Asset '%s' is not a Material Instance Constant."), *SelectedAsset.AssetName.ToString());
					}
				}
			}

			if (bFoundNonMaterialInstance && AssetsToRename.IsEmpty())
			{
				FMessageDialog::Open(EAppMsgType::Ok, FMaterialInstanceRenamerModule::GetLocalizedText("NotAMaterialInstance"));
				return; // Exit if only non-instances were selected
			}

			// Rename the valid assets
			if (!AssetsToRename.IsEmpty())
			{
				FScopedSlowTask SlowTask(AssetsToRename.Num(), FMaterialInstanceRenamerModule::GetLocalizedText("RenamingMaterialInstances"));
				SlowTask.MakeDialog(); // Show progress dialog

				for (const FAssetData& AssetData : AssetsToRename)
				{
					SlowTask.EnterProgressFrame(1, FText::Format(FMaterialInstanceRenamerModule::GetLocalizedText("RenamingAsset"), FText::FromName(AssetData.AssetName)));
					FAssetRenameUtil::RenameMaterialInstance(AssetData, false); // Call utility function (not batch)
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UContentBrowserAssetContextMenuContext not found in MenuContext for Material Instance rename."));
		}
	}
} // namespace MenuExtension_MaterialInstance


// ... (FMaterialInstanceRenamerModule implementation up to OnRenameAllMaterialInstancesClicked is unchanged) ...
//----------------------------------------------------------------------//
// FMaterialInstanceRenamerModule Implementation
//----------------------------------------------------------------------//

void FMaterialInstanceRenamerModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("MaterialInstanceRenamer Module Started"));
	if (IsRunningCommandlet()) return; // Don't register UI stuff in commandlets

	AddMaterialContextMenuEntry();
	AddToolMenuEntry();
}

void FMaterialInstanceRenamerModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("MaterialInstanceRenamer Module Shutdown"));
	// Unregister menus automatically via FToolMenuOwnerScoped in Add... functions
}

/**
 * Adds the material context menu entry for renaming.
 */
void FMaterialInstanceRenamerModule::AddMaterialContextMenuEntry()
{
	// Check if Slate is available (important for headless modes)
	if (!FSlateApplication::IsInitialized())
	{
		return;
	}

	FToolMenuOwnerScoped OwnerScoped(this); // Use 'this' as the owner

	// Target UMaterialInterface so it appears for both base Materials and Instances
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.MaterialInterface");

	FToolMenuSection& Section = Menu->FindOrAddSection("GetAssetActions"); // Common section for asset actions
	Section.AddDynamicEntry(TEXT("MaterialInstanceRenamerEntry"), FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
		{
			// Check if the context actually contains Material Instances
			const UContentBrowserAssetContextMenuContext* Context = InSection.FindContext<UContentBrowserAssetContextMenuContext>();
			bool bHasMaterialInstance = false;
			if (Context)
			{
				for (const FAssetData& AssetData : Context->SelectedAssets)
				{
					// Prefer checking class directly from AssetData if available
					if (AssetData.GetClass() == UMaterialInstanceConstant::StaticClass())
					{
						bHasMaterialInstance = true;
						break;
					}
					// Fallback check if class isn't loaded in AssetData
					UObject* Asset = AssetData.FastGetAsset(/*bLoad*/false);
					if (Asset && Asset->IsA<UMaterialInstanceConstant>())
					{
						bHasMaterialInstance = true;
						break;
					}
				}
			}

			// Only add the entry if at least one Material Instance is selected
			if (bHasMaterialInstance)
			{
				const FText Label = FMaterialInstanceRenamerModule::GetLocalizedText("Material_RenameToRecommendedPrefix");
				const FText ToolTip = FMaterialInstanceRenamerModule::GetLocalizedText("Material_RenameToRecommendedPrefixTooltip");
				const FSlateIcon Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.MaterialInstanceConstant"); // Use correct icon
				const FToolMenuExecuteAction Action = FToolMenuExecuteAction::CreateStatic(&MenuExtension_MaterialInstance::OnExecuteAction);

				InSection.AddMenuEntry(
					"MaterialInstance_RenameToRecommendedPrefix", // Unique name for the entry
					Label,
					ToolTip,
					Icon,
					Action
				);
			}
		}));
}


/**
 * Adds a tool menu entry for renaming all material instances.
 */
void FMaterialInstanceRenamerModule::AddToolMenuEntry()
{
	// Check if Slate is available
	if (!FSlateApplication::IsInitialized())
	{
		return;
	}

	FToolMenuOwnerScoped OwnerScoped(this);
	// Add to main 'Tools' menu
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");

	// Create a new section named "MaterialInstanceRenamer"
	FToolMenuSection& Section = Menu->FindOrAddSection("MaterialInstanceRenamer", LOCTEXT("MaterialInstanceRenamerSection", "MaterialInstanceRenamer"));

	const FText Label = FMaterialInstanceRenamerModule::GetLocalizedText("RenameAllMaterialInstances");
	const FText ToolTip = FMaterialInstanceRenamerModule::GetLocalizedText("RenameAllMaterialInstancesTooltip");

	Section.AddMenuEntry(
		"RenameAllMaterialInstances", // Entry name
		Label,
		ToolTip,
		FSlateIcon(), // Add an icon if desired: FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Renamer")
		FUIAction(FExecuteAction::CreateRaw(this, &FMaterialInstanceRenamerModule::OnRenameAllMaterialInstancesClicked)) // Use CreateRaw for member functions
	);

	UE_LOG(LogTemp, Log, TEXT("Added 'Rename All Material Instances' to Tools menu."));
}


/**
 * Handles the click event for the "Rename All Material Instances" menu item.
 */
void FMaterialInstanceRenamerModule::OnRenameAllMaterialInstancesClicked()
{
	SetBatchRename(true);

	// Find all Material Instance assets in the project
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> MaterialInstanceAssets;
	FARFilter Filter;
	Filter.PackagePaths.Add("/Game");
	Filter.bRecursivePaths = true;
	Filter.ClassPaths.Add(UMaterialInstanceConstant::StaticClass()->GetClassPathName());

	TArray<FString> ScanPaths;
	ScanPaths.Add(TEXT("/Game"));
	AssetRegistry.ScanPathsSynchronous(ScanPaths, true);
	AssetRegistry.GetAssets(Filter, MaterialInstanceAssets);

	if (MaterialInstanceAssets.Num() == 0)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FMaterialInstanceRenamerModule::GetLocalizedText("NoAssetsFound"));
		SetBatchRename(false);
		return;
	}

	// Generate a list of assets that need renaming
	TArray<TSharedPtr<FRenamePreviewItem>> ItemsToRename;
	for (const FAssetData& AssetData : MaterialInstanceAssets)
	{
		FString NewName = FAssetRenameUtil::GenerateNewAssetName(AssetData, true);
		if (!NewName.IsEmpty())
		{
			ItemsToRename.Add(FRenamePreviewItem::Make(AssetData, NewName));
		}
	}

	if (ItemsToRename.Num() == 0)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FMaterialInstanceRenamerModule::GetLocalizedText("NoAssetsToRename"));
		SetBatchRename(false);
		return;
	}

	// Show the preview dialog
	TSharedRef<SPreviewRenameDialog> PreviewDialog = SNew(SPreviewRenameDialog)
		.PreviewItems(ItemsToRename);

	if (!PreviewDialog->ShowModal())
	{
		// User cancelled
		SetBatchRename(false);
		return;
	}

	// User confirmed, proceed with renaming
	FScopedSlowTask SlowTask(ItemsToRename.Num(), FMaterialInstanceRenamerModule::GetLocalizedText("RenamingMaterialInstances"));
	SlowTask.MakeDialog(true);

	int32 RenamedCount = 0;
	for (const TSharedPtr<FRenamePreviewItem>& Item : ItemsToRename)
	{
		SlowTask.EnterProgressFrame(1, FText::Format(FMaterialInstanceRenamerModule::GetLocalizedText("RenamingAsset"), FText::FromString(Item->OldName)));

		if (FAssetRenameUtil::RenameAsset(Item->Asset, Item->NewName))
		{
			RenamedCount++;
		}

		if (SlowTask.ShouldCancel())
		{
			break;
		}
	}

	// Display final result
	FText RenameCompleteMessage = FMaterialInstanceRenamerModule::GetLocalizedText("RenameCompleteWithCount", RenamedCount);
	FMessageDialog::Open(EAppMsgType::Ok, RenameCompleteMessage);
	UE_LOG(LogTemp, Log, TEXT("Batch rename process finished. Renamed %d assets."), RenamedCount);

	SetBatchRename(false);
}

#undef LOCTEXT_NAMESPACE

// Make sure this is ONLY in this file for the entire module
IMPLEMENT_MODULE(FMaterialInstanceRenamerModule, MaterialInstanceRenamer)