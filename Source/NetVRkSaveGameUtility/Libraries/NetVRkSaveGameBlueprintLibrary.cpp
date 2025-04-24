// Fill out your copyright notice in the Description page of Project Settings.


#include "NetVRkSaveGameBlueprintLibrary.h"

#include "NetVRkSaveGameUtility/Components/SceneSaveGameActorComponent.h"
#include "NetVRkSaveGameUtility/Components/SceneSaveGameOwnerComponent.h"
#include "NetVRkSaveGameUtility/Interfaces/SceneSaveGameActorInterface.h"
#include "NetVRkSaveGameUtility/Interfaces/SceneSaveGameOwnerInterface.h"

void UNetVRkSaveGameBlueprintLibrary::SaveSceneToSlot(UObject* WorldContextObject, APlayerController* Player, const FString& SlotName)
{
	if (!WorldContextObject)
	{
		return;
	}
	
	if (!Player)
	{
		return;
	}
	
	const bool bImplements = Player->Implements<USceneSaveGameOwnerInterface>();
	
	if (!bImplements)
	{
		return;
	}
	
	USceneSaveGameOwnerComponent* ControllerComponent = ISceneSaveGameOwnerInterface::Execute_GetSceneSaveGameOwnerComponent(Player);
	
	if (!ControllerComponent)
	{
		return;
	}
	
	ControllerComponent->SaveSceneToSlot(SlotName);
	
}

void UNetVRkSaveGameBlueprintLibrary::LoadSceneFromSlot(UObject* WorldContextObject, APlayerController* Player, const FString& SlotName)
{
	if (!WorldContextObject)
	{
		return;
	}
	
	if (!Player)
	{
		return;
	}
	
	const bool bImplements = Player->Implements<USceneSaveGameOwnerInterface>();
	
	if (!bImplements)
	{
		return;
	}
	
	USceneSaveGameOwnerComponent* ControllerComponent = ISceneSaveGameOwnerInterface::Execute_GetSceneSaveGameOwnerComponent(Player);
	
	if (!ControllerComponent)
	{
		return;
	}
	
	ControllerComponent->LoadSceneFromSlot(SlotName);
}

bool UNetVRkSaveGameBlueprintLibrary::PopulateSceneActorSaveInfo(AActor* SceneActor, FSceneActorSaveInfo& OutSceneSaveInfo)
{
	USceneSaveGameActorComponent* SceneSaveGameActorComponent = ISceneSaveGameActorInterface::Execute_GetSceneSaveGameActorComponent(SceneActor);

	if (!SceneSaveGameActorComponent)
	{
		//still true since its valid to not have a SceneSaveGameActorComponent
		return true;
	}

	SceneSaveGameActorComponent->PopulateSceneActorSaveInfo(OutSceneSaveInfo);
	
	return true;
}

bool UNetVRkSaveGameBlueprintLibrary::DoesOwnSceneSaveActor(AActor* SceneOwner, AActor* SceneSaveActor)
{
	if (!SceneOwner)
	{
		return false;
	}
	
	const bool bOwnerImplements = SceneOwner->Implements<USceneSaveGameOwnerInterface>();
	
	if (!bOwnerImplements)
	{
		return false;
	}

	USceneSaveGameOwnerComponent* SaveGameOwnerComponent = ISceneSaveGameOwnerInterface::Execute_GetSceneSaveGameOwnerComponent(SceneOwner);

	if (!SaveGameOwnerComponent)
	{
		return false;
	}

	if (!SceneSaveActor)
	{
		return false;
	}
	
	const bool bActorImplements = SceneSaveActor->Implements<USceneSaveGameActorInterface>();
	
	if (!bActorImplements)
	{
		return false;
	}

	USceneSaveGameActorComponent* SaveGameActorComponent = ISceneSaveGameActorInterface::Execute_GetSceneSaveGameActorComponent(SceneSaveActor);

	if (!SaveGameActorComponent)
	{
		return false;
	}

	return SaveGameOwnerComponent->SceneSaveGameOwnerName == SaveGameActorComponent->SceneActorSaveGameOwnerName;
}

bool UNetVRkSaveGameBlueprintLibrary::DoesSceneActorRegenerate(AActor* SceneSaveActor)
{
	if (!SceneSaveActor)
	{
		return false;
	}

	const bool bActorImplements = SceneSaveActor->Implements<USceneSaveGameActorInterface>();
	
	if (!bActorImplements)
	{
		return false;
	}

	USceneSaveGameActorComponent* SaveGameActorComponent = ISceneSaveGameActorInterface::Execute_GetSceneSaveGameActorComponent(SceneSaveActor);

	if (!SaveGameActorComponent)
	{
		return false;
	}

	return SaveGameActorComponent->bRegenerateActorOnLoadGame;
}

FName UNetVRkSaveGameBlueprintLibrary::GetSceneActorSaveGameSlotName(AActor* SceneSaveActor)
{
	if (!SceneSaveActor)
	{
		return NAME_None;
	}

	const bool bActorImplements = SceneSaveActor->Implements<USceneSaveGameActorInterface>();
	
	if (!bActorImplements)
	{
		return NAME_None;
	}

	USceneSaveGameActorComponent* SaveGameActorComponent = ISceneSaveGameActorInterface::Execute_GetSceneSaveGameActorComponent(SceneSaveActor);

	if (!SaveGameActorComponent)
	{
		return NAME_None;
	}

	return SaveGameActorComponent->SceneActorSaveGameSlotName;
}

void UNetVRkSaveGameBlueprintLibrary::SetupSceneActorFromSaveInfo(AActor* SceneSaveActor,
	const FSceneActorSaveInfo& ActorSaveInfo, AActor* SaveOwner)
{
	if (!SceneSaveActor)
	{
		return;
	}

	const bool bActorImplements = SceneSaveActor->Implements<USceneSaveGameActorInterface>();
	
	if (!bActorImplements)
	{
		return;
	}

	USceneSaveGameActorComponent* SaveGameActorComponent = ISceneSaveGameActorInterface::Execute_GetSceneSaveGameActorComponent(SceneSaveActor);

	if (!SaveGameActorComponent)
	{
		return;
	}

	SaveGameActorComponent->SetupSceneActorFromSaveInfo(ActorSaveInfo, SaveOwner);
}
