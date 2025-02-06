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
			UE_LOG(LogTemp, Error, TEXT("Invalid asset path: %s"), *AssetPath);
			return;
		}


		TArray<FName> Referencers;
		AssetRegistry.GetReferencers(AssetData.PackageName, Referencers);


		for (const FName& Referencer : Referencers)
		{
			UE_LOG(LogTemp, Log, TEXT("Referencer: %s"), *Referencer.ToString());
		}
	}

	static void OnExecuteAction(const FToolMenuContext& MenuContext)
	{
		UE_LOG(LogTemp, Log, TEXT("OnExecuteAction called"));

		if (const UContentBrowserAssetContextMenuContext* Context = MenuContext.FindContext<UContentBrowserAssetContextMenuContext>())
		{
			UE_LOG(LogTemp, Log, TEXT("Context found"));

			for (const FAssetData& SelectedAsset : Context->SelectedAssets)
			{
				UObject* Asset = SelectedAsset.GetAsset();
				if (Asset && Asset->IsA<UMaterialInstance>())
				{
					FString OldName = SelectedAsset.AssetName.ToString();
					FString OldPackagePath = SelectedAsset.PackagePath.ToString();

					CheckAssetReferences(SelectedAsset.ObjectPath.ToString());

					if (OldName.StartsWith(TEXT("M_")) && OldName.Contains(TEXT("_Inst")))
					{
						int32 StartIndex = 2;
						int32 EndIndex = OldName.Find(TEXT("_Inst"));
						FString CoreName = OldName.Mid(StartIndex, EndIndex - StartIndex);
						FString NewName = TEXT("MI_") + CoreName;
						UE_LOG(LogTemp, Log, TEXT("Renaming to: %s"), *NewName);

						FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
						FString NewPackagePath = OldPackagePath;

						FText OutErrorMessage;
						FAssetRenameData RenameData(SelectedAsset.GetAsset(), NewPackagePath, NewName);
						EAssetRenameResult RenameResult = AssetToolsModule.Get().RenameAssetsWithDialog({ RenameData }, true);
						bool bSuccess = (RenameResult == EAssetRenameResult::Success);
						if (!bSuccess)
						{
							UE_LOG(LogTemp, Error, TEXT("Failed to rename asset. RenameResult: %d"), static_cast<int32>(RenameResult));
							switch (RenameResult)
							{
							case EAssetRenameResult::Failure:
								UE_LOG(LogTemp, Error, TEXT("Rename failed due to an unspecified error."));
								break;
							case EAssetRenameResult::Pending:
								UE_LOG(LogTemp, Error, TEXT("Pending."));
								break;
							default:
								UE_LOG(LogTemp, Error, TEXT("Rename failed with an unknown error."));
								break;
							}
						}
						else
						{
							UE_LOG(LogTemp, Log, TEXT("Asset renamed to: %s"), *NewName);
						}
					}
					else
					{
						UE_LOG(LogTemp, Log, TEXT("Name does not match pattern"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Selected asset is not a Material Instance"));
					FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("NotAMaterialInstance", "The selected asset is not a Material Instance."));
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