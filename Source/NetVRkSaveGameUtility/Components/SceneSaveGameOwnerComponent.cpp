// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneSaveGameOwnerComponent.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NetVRkSaveGameUtility/Interfaces/SceneSaveGameActorInterface.h"
#include "NetVRkSaveGameUtility/Libraries/NetVRkSaveGameBlueprintLibrary.h"

#pragma region Framework

// Sets default values for this component's properties
USceneSaveGameOwnerComponent::USceneSaveGameOwnerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	// ...
}


// Called when the game starts
void USceneSaveGameOwnerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	RegisterOwnedSceneActorsByName();
}


// Called every frame
void USceneSaveGameOwnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

#pragma endregion

#pragma region Controller

APawn* USceneSaveGameOwnerComponent::GetPawn()
{
	AController* Controller = Cast<AController>(GetOwner());

	if (!Controller)
	{
		return nullptr;
	}

	return Controller->GetPawn();
}

#pragma endregion

#pragma region Scene Save Game

void USceneSaveGameOwnerComponent::SaveSceneToSlot(const FString& SlotName)
{
	FSceneSaveInfo SaveSceneInfo;
	
	UNetVRkSceneSaveGame* SaveGameObject = UNetVRkSceneSaveGame::CreateSaveGame(this, GetOwner(), SlotName, SceneSaveGameClass, PlacedSceneActors,SaveSceneInfo);

	if (!SaveGameObject)
	{
		return;
	}
	
	UGameplayStatics::SaveGameToSlot(SaveGameObject, SlotName, 0);

	for (const FSceneActorSaveInfo& SceneActorSaveInfo : SaveSceneInfo.ActorSaves)
	{
		if (!SceneActorSaveInfo.SceneActorSaveGame)
		{
			continue;
		}

		const FString& SceneActorSaveGameSlotNameStr = SlotName + "_" + SceneActorSaveInfo.SceneActorSaveGameSlotName.ToString();

		const FName& SceneActorSaveGameSlotName = FName(*SceneActorSaveGameSlotNameStr);
		
		UGameplayStatics::SaveGameToSlot(SceneActorSaveInfo.SceneActorSaveGame, SceneActorSaveGameSlotNameStr, 0);
	}
}

void USceneSaveGameOwnerComponent::LoadSceneFromSlot(const FString& SlotName)
{
	UNetVRkSceneSaveGame* LoadedSaveGameObject = Cast<UNetVRkSceneSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));

	if (!LoadedSaveGameObject)
	{
		return;
	}

	FSceneSaveInfo SceneSaveInfo;

	const bool bGotSceneSave = LoadedSaveGameObject->GetSceneSaveInfo(SceneSaveInfo);

	if (bGotSceneSave)
	{
		SetupScene(SceneSaveInfo);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Failed to load scene")));
	}
}

void USceneSaveGameOwnerComponent::RegisterPlacedSaveGameSceneActor(AActor* PlacedActor)
{
	PlacedSceneActors.AddUnique(PlacedActor);
}

void USceneSaveGameOwnerComponent::UnregisterPlacedSaveGameSceneActor(AActor* PlacedActor)
{
	PlacedSceneActors.Remove(PlacedActor);
}

void USceneSaveGameOwnerComponent::SetupScene(const FSceneSaveInfo& SceneSaveInfo)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_SetupScene(SceneSaveInfo);
		
		SetupNonRegeneratingSceneActors(SceneSaveInfo);

		return;
	}

	DestroyRegeneratingSceneActors(SceneSaveInfo);
	
	for (const FSceneActorSaveInfo& ActorSaveInfo : SceneSaveInfo.ActorSaves)
	{
		SpawnSceneActor(ActorSaveInfo);
	}
	
	SetupNonRegeneratingSceneActors(SceneSaveInfo);
}

void USceneSaveGameOwnerComponent::DestroyRegeneratingSceneActors(const FSceneSaveInfo& SceneSaveInfo)
{
	TArray<TObjectPtr<AActor>> PlacedSceneActorsCopy = PlacedSceneActors;

	for (AActor* PlacedActor : PlacedSceneActorsCopy)
	{
		if (!PlacedActor)
		{
			continue;
		}

		const bool bActorRegenerates = UNetVRkSaveGameBlueprintLibrary::DoesSceneActorRegenerate(PlacedActor);

		if (!bActorRegenerates)
		{
			continue;
		}
		
		PlacedActor->Destroy();
	}
}

void USceneSaveGameOwnerComponent::SetupNonRegeneratingSceneActors(const FSceneSaveInfo& SceneSaveInfo)
{
	TArray<TObjectPtr<AActor>> PlacedSceneActorsCopy = PlacedSceneActors;

	for (AActor* PlacedActor : PlacedSceneActorsCopy)
	{
		if (!PlacedActor)
		{
			continue;
		}

		const bool bRegenerates = UNetVRkSaveGameBlueprintLibrary::DoesSceneActorRegenerate(PlacedActor);

		if (bRegenerates)
		{
			continue;
		}

		const FName& SaveSlotName = UNetVRkSaveGameBlueprintLibrary::GetSceneActorSaveGameSlotName(PlacedActor);

		if (SaveSlotName == NAME_None)
		{
			continue;
		}

		//fix up all the actors that were not regenerated
		for (const FSceneActorSaveInfo& ActorSaveInfo : SceneSaveInfo.ActorSaves)
		{
			if (SaveSlotName == ActorSaveInfo.SceneActorSaveGameSlotName)
			{
				UNetVRkSaveGameBlueprintLibrary::SetupSceneActorFromSaveInfo(PlacedActor, ActorSaveInfo, GetOwner());
			}
		}
	}
}

void USceneSaveGameOwnerComponent::Server_SetupScene_Implementation(
	const FSceneSaveInfo& SceneSaveInfo)
{
	SetupScene(SceneSaveInfo);
}

void USceneSaveGameOwnerComponent::SpawnSceneActor(const FSceneActorSaveInfo& ActorSaveInfo)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_SetupSceneActor(ActorSaveInfo);
		return;
	}

	const bool bActorRegenerates = UNetVRkSaveGameBlueprintLibrary::DoesSceneActorRegenerate(ActorSaveInfo.ActorClass->GetDefaultObject<AActor>());

	if (bActorRegenerates)
	{
		SpawnRegeneratedSceneActor(ActorSaveInfo);
	}

	
}

AActor* USceneSaveGameOwnerComponent::SpawnRegeneratedSceneActor(const FSceneActorSaveInfo& ActorSaveInfo)
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetPawn();
	
	AActor* SpawnedActor = World->SpawnActorDeferred<AActor>(ActorSaveInfo.ActorClass, ActorSaveInfo.ActorTransform, SpawnParams.Owner, SpawnParams.Instigator, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	if (!SpawnedActor)
	{
		return nullptr;
	}

	SpawnedActor->SetReplicates(true);
	SpawnedActor->SetAutonomousProxy(true);

	SpawnedActor->FinishSpawning(ActorSaveInfo.ActorTransform);
	
	UNetVRkSaveGameBlueprintLibrary::SetupSceneActorFromSaveInfo(SpawnedActor, ActorSaveInfo, GetOwner());

	return SpawnedActor;
}

void USceneSaveGameOwnerComponent::RegisterOwnedSceneActorsByName()
{
	TArray<AActor*> SceneSaveGameActors;
	
	UGameplayStatics::GetAllActorsWithInterface(this, USceneSaveGameActorInterface::StaticClass(), SceneSaveGameActors);

	for (AActor* SceneActor : SceneSaveGameActors)
	{
		if (!SceneActor)
		{
			continue;
		}

		const bool bOwnsSaveActor = UNetVRkSaveGameBlueprintLibrary::DoesOwnSceneSaveActor(GetOwner(), SceneActor);

		if (!bOwnsSaveActor)
		{
			continue;
		}

		RegisterPlacedSaveGameSceneActor(SceneActor);
	}
}

void USceneSaveGameOwnerComponent::Server_SetupSceneActor_Implementation(const FSceneActorSaveInfo& ActorSaveInfo)
{
	SpawnSceneActor(ActorSaveInfo);
}

#pragma endregion