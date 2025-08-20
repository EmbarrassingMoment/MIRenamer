// Copyright 2025 kurorekish. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SWindow.h"
#include "Widgets/Views/SListView.h"
#include "AssetRegistry/AssetData.h"

/**
 * A struct to hold the data for a single row in the rename preview list.
 */
struct FRenamePreviewItem
{
	FAssetData Asset;
	FString OldName;
	FString NewName;

	FRenamePreviewItem(const FAssetData& InAsset, const FString& InNewName)
		: Asset(InAsset), OldName(InAsset.AssetName.ToString()), NewName(InNewName) {}

	// Factory function for creating a shared pointer
	static TSharedRef<FRenamePreviewItem> Make(const FAssetData& Asset, const FString& NewName)
	{
		return MakeShareable(new FRenamePreviewItem(Asset, NewName));
	}
};

/**
 * A Slate dialog for previewing asset renames.
 */
class SPreviewRenameDialog : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPreviewRenameDialog)
		: _PreviewItems()
		{}
		SLATE_ARGUMENT(TArray<TSharedPtr<FRenamePreviewItem>>, PreviewItems)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// Shows the dialog modally and returns true if the user confirmed, false otherwise.
	bool ShowModal();

private:
	// Generates a row widget for the list view.
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FRenamePreviewItem> Item, const TSharedRef<STableViewBase>& OwnerTable);

	// Handles the "Confirm" button click.
	FReply OnConfirm();

	// Handles the "Cancel" button click.
	FReply OnCancel();

	// Pointer to the parent window.
	TWeakPtr<SWindow> ParentWindow;

	// The list of items to display.
	TArray<TSharedPtr<FRenamePreviewItem>> PreviewItems;

	// Flag to indicate if the user confirmed the dialog.
	bool bUserConfirmed = false;
};
