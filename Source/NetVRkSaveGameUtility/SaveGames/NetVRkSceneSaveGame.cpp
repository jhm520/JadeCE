// Fill out your copyright notice in the Description page of Project Settings.


#include "NetVRkSceneSaveGame.h"

#include "Kismet/GameplayStatics.h"
#include "NetVRkSaveGameUtility/Components/SceneSaveGameActorComponent.h"
#include "NetVRkSaveGameUtility/Interfaces/SceneSaveGameActorInterface.h"
#include "NetVRkSaveGameUtility/Libraries/NetVRkSaveGameBlueprintLibrary.h"

bool UNetVRkSceneSaveGame::GetSceneSaveInfo(FSceneSaveInfo& OutSceneSaveInfo)
{
	if (SceneSaveInfo.SceneName.IsNone())
	{
		return false;
	}

	for (FSceneActorSaveInfo& ActorSaveInfo : SceneSaveInfo.ActorSaves)
	{
		if (ActorSaveInfo.SceneActorSaveGameSlotName.IsNone())
		{
			continue;
		}

		const FString& LoadSceneActorSaveSlot = SceneSaveInfo.SceneName.ToString() + "_" + ActorSaveInfo.SceneActorSaveGameSlotName.ToString();

		USaveGame* ActorSaveGame = UGameplayStatics::LoadGameFromSlot(LoadSceneActorSaveSlot, 0);

		if (!ActorSaveGame)
		{
			continue;
		}

		ActorSaveInfo.SceneActorSaveGame = ActorSaveGame;
	}

	OutSceneSaveInfo = SceneSaveInfo;
	
	return true;
}

UNetVRkSceneSaveGame* UNetVRkSceneSaveGame::CreateSaveGame(UObject* WorldContextObject, AActor* Owner, const FString& SlotName, const TSubclassOf<UNetVRkSceneSaveGame>& SceneSaveGameClass, const TArray<TObjectPtr<AActor>>& SceneActors, FSceneSaveInfo& OutSceneSaveInfo)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UNetVRkSceneSaveGame* SaveGameObject = Cast<UNetVRkSceneSaveGame>(UGameplayStatics::CreateSaveGameObject(SceneSaveGameClass));

	if (!SaveGameObject)
	{
		return nullptr;
	}
	
	const bool bGotSceneSaveInfo = CreateSceneSaveInfo(FName(SlotName), SceneActors, OutSceneSaveInfo);

	if (!bGotSceneSaveInfo)
	{
		return nullptr;
	}

	SaveGameObject->SceneSaveInfo = OutSceneSaveInfo;

	return SaveGameObject;
}

bool UNetVRkSceneSaveGame::CreateSceneSaveInfo(const FName& SceneName, const TArray<TObjectPtr<AActor>>& SceneActors, FSceneSaveInfo& OutSceneSaveInfo)
{
	for (AActor* SceneActor : SceneActors)
	{
		if (!SceneActor)
		{
			continue;
		}

		FSceneActorSaveInfo ActorSaveInfo;
		ActorSaveInfo.ActorClass = SceneActor->GetClass();
		ActorSaveInfo.ActorTransform = SceneActor->GetActorTransform();
		
		const bool bImplements = SceneActor->Implements<USceneSaveGameActorInterface>();

		if (bImplements)
		{
			UNetVRkSaveGameBlueprintLibrary::PopulateSceneActorSaveInfo(SceneActor, ActorSaveInfo);
		}
		
		OutSceneSaveInfo.ActorSaves.Add(ActorSaveInfo);
	}
	
	OutSceneSaveInfo.SceneName = SceneName;
	
	return true;
}
