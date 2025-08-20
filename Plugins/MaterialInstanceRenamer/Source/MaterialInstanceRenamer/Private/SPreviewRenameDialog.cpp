// Copyright 2025 kurorekish. All Rights Reserved.

#include "SPreviewRenameDialog.h"
#include "MaterialInstanceRenamer.h" // For GetLocalizedText
#include "Widgets/SBox/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Views/SHeaderRow.h"

void SPreviewRenameDialog::Construct(const FArguments& InArgs)
{
	PreviewItems = InArgs._PreviewItems;
	bUserConfirmed = false;

	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(600)
		.HeightOverride(400)
		[
			SNew(SVerticalBox)

			// Title
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FMaterialInstanceRenamerModule::GetLocalizedText("RenamePreviewTitle"))
				.Font(FCoreStyle::Get().GetFontStyle("EmbossedText"))
			]

			// List View
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SNew(SListView<TSharedPtr<FRenamePreviewItem>>)
				.ListItemsSource(&PreviewItems)
				.OnGenerateRow(this, &SPreviewRenameDialog::OnGenerateRow)
				.HeaderRow
				(
					SNew(SHeaderRow)
					+ SHeaderRow::Column("OldName").DefaultLabel(FMaterialInstanceRenamerModule::GetLocalizedText("CurrentNameHeader"))
					+ SHeaderRow::Column("NewName").DefaultLabel(FMaterialInstanceRenamerModule::GetLocalizedText("NewNameHeader"))
				)
			]

			// Buttons
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Right)
			.Padding(5)
			[
				SNew(SUniformGridPanel)
				.SlotPadding(FMargin(5))
				+ SUniformGridPanel::Slot(0, 0)
				[
					SNew(SButton)
					.Text(FMaterialInstanceRenamerModule::GetLocalizedText("ConfirmButton"))
					.OnClicked(this, &SPreviewRenameDialog::OnConfirm)
				]
				+ SUniformGridPanel::Slot(1, 0)
				[
					SNew(SButton)
					.Text(FMaterialInstanceRenamerModule::GetLocalizedText("CancelButton"))
					.OnClicked(this, &SPreviewRenameDialog::OnCancel)
				]
			]
		]
	];
}

bool SPreviewRenameDialog::ShowModal()
{
	TSharedRef<SWindow> Window = SNew(SWindow)
		.Title(FMaterialInstanceRenamerModule::GetLocalizedText("RenamePreviewTitle"))
		.ClientSize(FVector2D(600, 400))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
			AsShared()
		];

	ParentWindow = Window;
	GEditor->EditorAddModalWindow(Window);

	return bUserConfirmed;
}

TSharedRef<ITableRow> SPreviewRenameDialog::OnGenerateRow(TSharedPtr<FRenamePreviewItem> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FRenamePreviewItem>>, OwnerTable)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(5)
			[
				SNew(STextBlock).Text(FText::FromString(Item->OldName))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(5)
			[
				SNew(STextBlock).Text(FText::FromString(Item->NewName))
			]
		];
}

FReply SPreviewRenameDialog::OnConfirm()
{
	bUserConfirmed = true;
	if (ParentWindow.IsValid())
	{
		ParentWindow.Pin()->RequestDestroyWindow();
	}
	return FReply::Handled();
}

FReply SPreviewRenameDialog::OnCancel()
{
	bUserConfirmed = false;
	if (ParentWindow.IsValid())
	{
		ParentWindow.Pin()->RequestDestroyWindow();
	}
	return FReply::Handled();
}
