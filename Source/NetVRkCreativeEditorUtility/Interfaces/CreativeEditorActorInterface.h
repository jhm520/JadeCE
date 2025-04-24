// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CreativeEditorActorInterface.generated.h"

class UCreativeEditorActorComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCreativeEditorActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NETVRKCREATIVEEDITORUTILITY_API ICreativeEditorActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Creative Editor")
	UCreativeEditorActorComponent* GetCreativeEditorActorComponent();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Creative Editor")
	void OnPreviewActorPlacement();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Creative Editor")
	void OnEndedPreviewActorPlacement();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Creative Editor")
	void OnConfirmedActorPlacement();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Creative Editor")
	void OnCreativeModeActivated(APawn* CreativeEditorPawn);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Creative Editor")
	void OnCreativeModeDeactivated(APawn* OldCreativeEditorPawn);
};
