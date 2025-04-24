// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NetVRkSaveGameBlueprintLibrary.generated.h"

/**
 * 
 */

UCLASS()
class NETVRKSAVEGAMEUTILITY_API UNetVRkSaveGameBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Creative Editor")
	static void SaveSceneToSlot(UObject* WorldContextObject, APlayerController* Player, const FString& SlotName);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Creative Editor")
	static void LoadSceneFromSlot(UObject* WorldContextObject, APlayerController* Player, const FString& SlotName);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Creative Editor")
	static bool PopulateSceneActorSaveInfo(AActor* SceneActor, FSceneActorSaveInfo& OutSceneSaveInfo);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Creative Editor")
	static bool DoesOwnSceneSaveActor(AActor* SceneOwner, AActor* SceneSaveActor);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Creative Editor")
	static bool DoesSceneActorRegenerate(AActor* SceneSaveActor);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Creative Editor")
	static FName GetSceneActorSaveGameSlotName(AActor* SceneSaveActor);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Creative Editor")
	static void SetupSceneActorFromSaveInfo(AActor* SceneSaveActor, const FSceneActorSaveInfo& ActorSaveInfo, AActor* SaveOwner);

};
