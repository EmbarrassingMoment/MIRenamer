// Copyright 2025 kurorekish. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MaterialInstanceRenamerBlueprintLibrary.generated.h"

class UMaterialInstanceConstant;

/**
 * Blueprint-exposed enum representing the result of a rename operation.
 */
UENUM(BlueprintType)
enum class EMaterialInstanceRenameResult : uint8
{
    Renamed       UMETA(DisplayName = "Successfully Renamed"),
    Skipped       UMETA(DisplayName = "Skipped (Already Correct)"),
    Failed        UMETA(DisplayName = "Failed"),
    InvalidPattern UMETA(DisplayName = "Invalid Naming Pattern")
};

/**
 * Blueprint-exposed struct for detailed rename results.
 */
USTRUCT(BlueprintType)
struct FMaterialInstanceRenameResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Rename Result")
    FString OldName;

    UPROPERTY(BlueprintReadOnly, Category = "Rename Result")
    FString NewName;

    UPROPERTY(BlueprintReadOnly, Category = "Rename Result")
    EMaterialInstanceRenameResult Result = EMaterialInstanceRenameResult::Failed;

    UPROPERTY(BlueprintReadOnly, Category = "Rename Result")
    FString ErrorMessage;
};

/**
 * Blueprint Function Library for renaming Material Instances.
 */
UCLASS()
class MATERIALINSTANCERENAMER_API UMaterialInstanceRenamerBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Renames a Material Instance asset according to the plugin's naming convention.
     *
     * @param MaterialInstance - The Material Instance asset to rename
     * @param OutNewName - The new name after renaming (or current name if skipped)
     * @param OutErrorMessage - Error description if the operation failed
     * @return true if the operation succeeded (renamed or skipped), false if it failed
     */
    UFUNCTION(BlueprintCallable, Category = "Material Instance Renamer", meta = (DisplayName = "Rename Material Instance"))
    static bool RenameMaterialInstance(
        UMaterialInstanceConstant* MaterialInstance,
        FString& OutNewName,
        FString& OutErrorMessage
    );

    /**
     * Renames a Material Instance asset by its path.
     *
     * @param AssetPath - The path to the asset (e.g., "/Game/Materials/M_Wood_Inst")
     * @param OutNewName - The new name after renaming (or current name if skipped)
     * @param OutErrorMessage - Error description if the operation failed
     * @return true if the operation succeeded (renamed or skipped), false if it failed
     */
    UFUNCTION(BlueprintCallable, Category = "Material Instance Renamer", meta = (DisplayName = "Rename Material Instance by Path"))
    static bool RenameMaterialInstanceByPath(
        const FString& AssetPath,
        FString& OutNewName,
        FString& OutErrorMessage
    );

    /**
     * Batch renames multiple Material Instance assets.
     *
     * @param AssetPaths - Array of asset paths to rename
     * @param OutRenamedCount - Number of assets successfully renamed
     * @param OutSkippedCount - Number of assets skipped (already correct)
     * @param OutFailedCount - Number of assets failed
     * @param OutErrorMessages - List of error messages encountered
     */
    UFUNCTION(BlueprintCallable, Category = "Material Instance Renamer", meta = (DisplayName = "Batch Rename Material Instances"))
    static void BatchRenameMaterialInstances(
        const TArray<FString>& AssetPaths,
        int32& OutRenamedCount,
        int32& OutSkippedCount,
        int32& OutFailedCount,
        TArray<FString>& OutErrorMessages
    );

    /**
     * Previews what the new name of a Material Instance would be without renaming it.
     *
     * @param MaterialInstance - The Material Instance asset to preview
     * @param bWouldBeRenamed - Outputs false if the asset already follows the naming convention or if the pattern is invalid
     * @return The previewed new name (or the current name if it wouldn't be renamed)
     */
    UFUNCTION(BlueprintPure, Category = "Material Instance Renamer", meta = (DisplayName = "Preview Rename Result"))
    static FString GetPreviewRenameName(
        UMaterialInstanceConstant* MaterialInstance,
        bool& bWouldBeRenamed
    );
};
