// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "CreativeEditorManipulationActorInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCreativeEditorManipulationActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NETVRKCREATIVEEDITORUTILITY_API ICreativeEditorManipulationActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Creative Editor")
	UCreativeEditorManipulationActorComponent* GetManipulationActorComponent() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Creative Editor")
	void OnManipulationModeChanged(APlayerController* ManipulationController, FGameplayTag ManipulationModeTag);
};
