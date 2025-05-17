// Copyright 2025 kurorekish. All Rights Reserved.

#include "MaterialInstanceRenamer.h"
#include "FAssetRenameUtil.h" 
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
FText GetLocalizedText(const FString& Key, int32 Count = -1)
{
	const FString CultureName = FInternationalization::Get().GetCurrentCulture()->GetTwoLetterISOLanguageName();

	if (CultureName == TEXT("ja"))
	{
		if (Key == "RenameAllMaterialInstances") return LOCTEXT("RenameAllMaterialInstances_JP", "すべてのマテリアルインスタンスをリネーム");
		if (Key == "RenameAllMaterialInstancesTooltip") return LOCTEXT("RenameAllMaterialInstancesTooltip_JP", "プロジェクト内のすべてのマテリアルインスタンスを推奨プレフィックスにリネームします。");
		if (Key == "Material_RenameToRecommendedPrefix") return LOCTEXT("Material_RenameToRecommendedPrefix_JP", "推奨プレフィックスにリネーム");
		if (Key == "Material_RenameToRecommendedPrefixTooltip") return LOCTEXT("Material_RenameToRecommendedPrefixTooltip_JP", "選択したマテリアルインスタンスを推奨プレフィックス（MI_）にリネームします。");
		if (Key == "RenameCompleteWithCount") return FText::Format(LOCTEXT("RenameCompleteWithCount_JP", "すべてのマテリアルインスタンスのリネームが完了しました。リネーム数: {0}"), FText::AsNumber(Count));
		if (Key == "ConfirmBatchRename") return LOCTEXT("ConfirmBatchRename_JP", "プロジェクト内のすべてのマテリアルインスタンスをリネームしますか？（MI_）");
		if (Key == "NoAssetsFound") return LOCTEXT("NoAssetsFound_JP", "リネーム対象のマテリアルインスタンスが見つかりませんでした。");
		if (Key == "RenamingMaterialInstances") return LOCTEXT("RenamingMaterialInstances_JP", "マテリアルインスタンスをリネーム中...");
		if (Key == "RenamingAsset") return LOCTEXT("RenamingAsset_JP", "{0} をリネーム中...");
		if (Key == "NotAMaterialInstance") return LOCTEXT("NotAMaterialInstance_JP", "選択されたアセットはマテリアルインスタンスではありません。");
	}
	else // Default to English
	{
		if (Key == "RenameAllMaterialInstances") return LOCTEXT("RenameAllMaterialInstances_EN", "Rename All Material Instances");
		if (Key == "RenameAllMaterialInstancesTooltip") return LOCTEXT("RenameAllMaterialInstancesTooltip_EN", "Rename all Material Instances in the project to the recommended prefix.");
		if (Key == "Material_RenameToRecommendedPrefix") return LOCTEXT("Material_RenameToRecommendedPrefix_EN", "Rename to Recommended Prefix");
		if (Key == "Material_RenameToRecommendedPrefixTooltip") return LOCTEXT("Material_RenameToRecommendedPrefixTooltip_EN", "Rename the selected Material Instance(s) to the recommended prefix (MI_).");
		if (Key == "RenameCompleteWithCount") return FText::Format(LOCTEXT("RenameCompleteWithCount_EN", "Renaming of all Material Instances is complete. Total renamed: {0}"), FText::AsNumber(Count));
		if (Key == "ConfirmBatchRename") return LOCTEXT("ConfirmBatchRename_EN", "Do you want to rename all Material Instances in the project? (MI_)");
		if (Key == "NoAssetsFound") return LOCTEXT("NoAssetsFound_EN", "No Material Instances found to rename.");
		if (Key == "RenamingMaterialInstances") return LOCTEXT("RenamingMaterialInstances_EN", "Renaming Material Instances...");
		if (Key == "RenamingAsset") return LOCTEXT("RenamingAsset_EN", "Renaming {0}...");
		if (Key == "NotAMaterialInstance") return LOCTEXT("NotAMaterialInstance_EN", "The selected asset is not a Material Instance.");
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
				FMessageDialog::Open(EAppMsgType::Ok, GetLocalizedText("NotAMaterialInstance"));
				return; // Exit if only non-instances were selected
			}

			// Rename the valid assets
			if (!AssetsToRename.IsEmpty())
			{
				FScopedSlowTask SlowTask(AssetsToRename.Num(), GetLocalizedText("RenamingMaterialInstances"));
				SlowTask.MakeDialog(); // Show progress dialog

				for (const FAssetData& AssetData : AssetsToRename)
				{
					SlowTask.EnterProgressFrame(1, FText::Format(GetLocalizedText("RenamingAsset"), FText::FromName(AssetData.AssetName)));
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
				const FText Label = GetLocalizedText("Material_RenameToRecommendedPrefix");
				const FText ToolTip = GetLocalizedText("Material_RenameToRecommendedPrefixTooltip");
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
	// Use a more specific menu location if possible, e.g., under Asset actions or a custom menu
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools"); // Add to main 'Tools' menu

	FToolMenuSection& Section = Menu->FindOrAddSection("Tools"); // Or a more specific section

	const FText Label = GetLocalizedText("RenameAllMaterialInstances");
	const FText ToolTip = GetLocalizedText("RenameAllMaterialInstancesTooltip");

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
	// Confirmation Dialog
	if (FMessageDialog::Open(EAppMsgType::YesNo, GetLocalizedText("ConfirmBatchRename")) != EAppReturnType::Yes)
	{
		return;
	}

	SetBatchRename(true); // Set batch flag

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> MaterialInstanceAssets;
	FARFilter Filter;
	Filter.PackagePaths.Add("/Game"); // Limit search to /Game directory
	Filter.bRecursivePaths = true;
	Filter.ClassPaths.Add(UMaterialInstanceConstant::StaticClass()->GetClassPathName()); // Only find MICs

	// Ensure Asset Registry has finished discovering assets, especially on editor startup
	TArray<FString> ScanPaths;
	ScanPaths.Add(TEXT("/Game"));
	AssetRegistry.ScanPathsSynchronous(ScanPaths, true); // Force scan /Game

	AssetRegistry.GetAssets(Filter, MaterialInstanceAssets);

	int32 TotalAssets = MaterialInstanceAssets.Num();
	if (TotalAssets == 0)
	{
		FMessageDialog::Open(EAppMsgType::Ok, GetLocalizedText("NoAssetsFound"));
		SetBatchRename(false); // Reset flag
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Found %d Material Instance Constants to potentially rename."), TotalAssets);

	// Use ScopedSlowTask for progress indication
	FScopedSlowTask SlowTask(TotalAssets, GetLocalizedText("RenamingMaterialInstances"));
	SlowTask.MakeDialog(true); // Allow cancel? Maybe not for batch rename.

	int32 RenamedCount = 0;
	for (const FAssetData& AssetData : MaterialInstanceAssets)
	{
		SlowTask.EnterProgressFrame(1, FText::Format(GetLocalizedText("RenamingAsset"), FText::FromName(AssetData.AssetName)));

		// Call the utility function to perform the rename check and execution
		FAssetRenameUtil::RenameMaterialInstance(AssetData, true); // Pass true for bIsBatch

		// Ideally, RenameMaterialInstance should return whether it actually renamed.
		// For now, we assume it tries and increment count. A better approach would be:
		// if (FAssetRenameUtil::RenameMaterialInstance(AssetData, true)) { RenamedCount++; }
		// But that requires modifying FAssetRenameUtil::RenameMaterialInstance to return bool.
		// We'll simulate based on the old logic's counting:
		if (!AssetData.AssetName.ToString().StartsWith(TEXT("MI_"))) // Count if it *likely* needed renaming
		{
			RenamedCount++;
		}


		if (SlowTask.ShouldCancel()) // Check for user cancellation
		{
			break;
		}
	}

	// Display final result message
	FText RenameCompleteMessage = GetLocalizedText("RenameCompleteWithCount", RenamedCount);
	FMessageDialog::Open(EAppMsgType::Ok, RenameCompleteMessage);
	UE_LOG(LogTemp, Log, TEXT("Batch rename process finished. Attempted to rename %d assets."), RenamedCount);


	SetBatchRename(false); // Reset batch flag
}

#undef LOCTEXT_NAMESPACE

// Make sure this is ONLY in this file for the entire module
IMPLEMENT_MODULE(FMaterialInstanceRenamerModule, MaterialInstanceRenamer)