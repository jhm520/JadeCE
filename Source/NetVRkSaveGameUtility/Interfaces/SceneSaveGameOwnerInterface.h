// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SceneSaveGameOwnerInterface.generated.h"

class USceneSaveGameOwnerInterface;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USceneSaveGameOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NETVRKSAVEGAMEUTILITY_API ISceneSaveGameOwnerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Scene Save Game Controller")
	USceneSaveGameOwnerComponent* GetSceneSaveGameOwnerComponent();
};
