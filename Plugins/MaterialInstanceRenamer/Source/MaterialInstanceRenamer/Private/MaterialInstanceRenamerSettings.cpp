// Copyright 2026 kurorekish. All Rights Reserved.

#include "MaterialInstanceRenamerSettings.h"

UMaterialInstanceRenamerSettings::UMaterialInstanceRenamerSettings()
{
	RenamePrefix = TEXT("MI_");
	bAutoRenameOnCreate = false;
	bShowNotificationOnAutoRename = false;
}