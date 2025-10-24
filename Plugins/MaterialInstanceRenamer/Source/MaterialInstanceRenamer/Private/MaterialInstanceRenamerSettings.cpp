// Copyright 2025 kurorekish. All Rights Reserved.

#include "MaterialInstanceRenamerSettings.h"

UMaterialInstanceRenamerSettings::UMaterialInstanceRenamerSettings()
{
	RenamePrefix = TEXT("MI_");
	bAutoRenameOnCreate = false;
	bShowNotificationOnAutoRename = false;
}