// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CreativeEditorManipulableInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCreativeEditorManipulableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NETVRKCREATIVEEDITORUTILITY_API ICreativeEditorManipulableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Creative Editor")
	bool GetManipulationLocation(APlayerController* ManipulationController, UManipulationComponent* ManipulationComponent, FVector& OutLocation) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Creative Editor")
	bool GetManipulationClickDragType(APlayerController* ManipulationController, UManipulationComponent* ManipulationComponent, EManipulationClickDragType& OutClickDragType) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Creative Editor")
	void OnManipulationEnabled(APlayerController* ManipulationController, UManipulationComponent* ManipulationComponent, bool bIsEnabled);
};
