// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NetVRkCreativeEditorBlueprintLibrary.generated.h"

/**
 * 
 */

UCLASS()
class NETVRKCREATIVEEDITORUTILITY_API UNetVRkCreativeEditorBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintPure, Category = "Creative Editor")
	static bool GetCreativeEditorManipulationModeFromString(const FString& InCreativeModeString, FGameplayTag& OutCreativeModeTag);
	
	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	static bool ActivateCreativeMode(AActor* Creator, bool bActivate = true);

	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	static bool ToggleCreativeMode(AActor* Creator);

	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	static bool IsManipulating(AActor* Manipulator);

	//start manipulating with a specific actor and component specified
	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	static void StartManipulatingActor(AActor* Manipulator, AActor* ManipulatedActor, UManipulationComponent* ManipulationComponent);

	//start manipulating without any actor or component specified
	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	static void StartManipulating(AActor* Manipulator);

	//stop manipulating whatever is currently being manipulated
	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	static void StopManipulating(AActor* Manipulator);

	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	static void SetActorManipulationMode(AActor* Creator, AActor* ManipulatedActor, FGameplayTag ManipulationModeTag);

	UFUNCTION(BlueprintPure, Category = "Creative Editor")
	static UCreativeEditorPawnComponent* GetCreativeEditorPawnComponent(AActor* Creator);
	
	UFUNCTION(BlueprintPure, Category = "Creative Editor")
	static UCreativeEditorActorComponent* GetCreativeEditorActorComponent(AActor* Actor);
	
};
