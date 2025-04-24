// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetVRkSaveGameUtility/SaveGames/NetVRkSceneSaveGame.h"
#include "SceneSaveGameOwnerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETVRKSAVEGAMEUTILITY_API USceneSaveGameOwnerComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region Framework
public:
	// Sets default values for this component's properties
	USceneSaveGameOwnerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion

#pragma region Controller
	
	UFUNCTION(BlueprintPure, Category = "Controller")
	APawn* GetPawn();
	
#pragma endregion
	
#pragma region Save Game

	//the additional name of the sub save game slot
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	FName SceneSaveGameOwnerName = NAME_None;
	
	void SaveSceneToSlot(const FString& SlotName);

	void LoadSceneFromSlot(const FString& SlotName);
	
	UPROPERTY(EditDefaultsOnly, Category = "Creative Editor")
	TSubclassOf<UNetVRkSceneSaveGame> SceneSaveGameClass;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> PlacedSceneActors;

	UFUNCTION()
	void RegisterPlacedSaveGameSceneActor(AActor* PlacedActor);

	UFUNCTION()
	void UnregisterPlacedSaveGameSceneActor(AActor* PlacedActor);

	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	void SetupScene(const FSceneSaveInfo& SceneSaveInfo);

	void DestroyRegeneratingSceneActors(const FSceneSaveInfo& SceneSaveInfo);
	
	void SetupNonRegeneratingSceneActors(const FSceneSaveInfo& SceneSaveInfo);

	UFUNCTION(Server, Reliable)
	void Server_SetupScene(const FSceneSaveInfo& SceneSaveInfo);
	
	void SpawnSceneActor(const FSceneActorSaveInfo& ActorSaveInfo);
	
	AActor* SpawnRegeneratedSceneActor(const FSceneActorSaveInfo& ActorSaveInfo);
	
	UFUNCTION(Server, Reliable)
	void Server_SetupSceneActor(const FSceneActorSaveInfo& ActorSaveInfo);
	
#pragma endregion

#pragma region Save Game | Ownership

	UFUNCTION()
	void RegisterOwnedSceneActorsByName();

#pragma endregion

};
