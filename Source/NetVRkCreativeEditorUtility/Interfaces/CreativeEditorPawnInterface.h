// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CreativeEditorPawnInterface.generated.h"

class UCreativeEditorPawnComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCreativeEditorPawnInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NETVRKCREATIVEEDITORUTILITY_API ICreativeEditorPawnInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "NvrkCreationEngine")
	UCreativeEditorPawnComponent* GetCreativeEditorPawnComponent() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "NvrkCreationEngine")
	void OnPreviewActorPlacement(AActor* PreviewedActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "NvrkCreationEngine")
	void OnEndedPreviewActorPlacement(AActor* PreviewedActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "NvrkCreationEngine")
	void OnConfirmedActorPlacement(AActor* Actor);
};
