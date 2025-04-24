// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetVRkSaveGameUtility/SaveGames/NetVRkSceneSaveGame.h"
#include "SceneSaveGameActorComponent.generated.h"

class USaveGame;
class UTransferComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETVRKSAVEGAMEUTILITY_API USceneSaveGameActorComponent : public UActorComponent
{
	GENERATED_BODY()
#pragma region Framework
public:	
	// Sets default values for this component's properties
	USceneSaveGameActorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion

#pragma region Save Game

	//whether to regenerate the actor on load game
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	bool bRegenerateActorOnLoadGame = true;

	//the additional name of the sub save game slot
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	FName SceneActorSaveGameSlotName = NAME_None;

	//the class of the sub save game that the actor will save to
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	TSubclassOf<USaveGame> SceneActorSaveGameClass;
	
	bool PopulateSceneActorSaveInfo(FSceneActorSaveInfo& OutSceneSaveInfo);

	void SetupSceneActorFromSaveInfo(const FSceneActorSaveInfo& ActorSaveInfo, AActor* SaveOwner);

	void SetCurrentSceneSaveInfo(const FSceneActorSaveInfo& NewSceneSaveInfo, AActor* SaveOwner);

	void OnCurrentSceneSaveInfoChanged();

	UPROPERTY(ReplicatedUsing="OnRep_CurrentSceneSaveInfo")
	FSceneActorSaveInfo CurrentSceneSaveInfo;

	UFUNCTION()
	void OnRep_CurrentSceneSaveInfo();

#pragma endregion

	
#pragma region Save Game | Owner

	//the additional name of the sub save game slot
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	FName SceneActorSaveGameOwnerName = NAME_None;

	UFUNCTION()
	void RegisterWithSaveGameOwner();

	UFUNCTION()
	void UnregisterWithSaveGameOwner();
#pragma endregion

#pragma region File Transfer System

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "File Transfer System")
	UTransferComponent* TransferComponent;
	
#pragma endregion


};
