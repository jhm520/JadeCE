// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "NetVRkSceneSaveGame.generated.h"


USTRUCT(BlueprintType)
struct NETVRKSAVEGAMEUTILITY_API FSceneActorSaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene Actor Save Info")
	AActor* SaveOwnerActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene Actor Save Info")
	FName SceneActorSaveGameSlotName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene Actor Save Info")
	USaveGame* SceneActorSaveGame = nullptr;

	//the sculpt actor class we're using for the sculpt
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene Actor Save Info")
	TSubclassOf<AActor> ActorClass = AActor::StaticClass();

	// the transform of the sculpt actor, by default relative to the voxel actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene Actor Save Info")
	FTransform ActorTransform = FTransform::Identity;

	// parameters for the actor, to be interpreted by the actor CDO
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene Actor Save Info")
	FString JSONActorParameters = "";

	friend uint32 GetTypeHash(const FSceneActorSaveInfo& InActorSaveInfo)
	{
		uint32 Hash = GetTypeHash(InActorSaveInfo.ActorClass);
		Hash = HashCombineFast(Hash, GetTypeHash(InActorSaveInfo.ActorTransform));

		return Hash;
	}
};

USTRUCT(BlueprintType)
struct NETVRKSAVEGAMEUTILITY_API FSceneSaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel Sculpt Info")
	FName SceneName;

	//the sculpt actor class we're using for the sculpt
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel Sculpt Info")
	TArray<FSceneActorSaveInfo> ActorSaves;

	friend uint32 GetTypeHash(const FSceneSaveInfo& InSceneSaveInfo)
	{
		uint32 SceneHash = GetTypeHash(InSceneSaveInfo.SceneName);
		
		for (const FSceneActorSaveInfo& ActorSave : InSceneSaveInfo.ActorSaves)
		{
			uint32 ActorHash = GetTypeHash(ActorSave);
			
			SceneHash = HashCombineFast(SceneHash, ActorHash);
		}
		
		return SceneHash;
	}
};


/**
 * 
 */
UCLASS()
class NETVRKSAVEGAMEUTILITY_API UNetVRkSceneSaveGame : public USaveGame
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel Sculpt Info")
	FSceneSaveInfo SceneSaveInfo;

public:

	UFUNCTION(BlueprintPure, Category = "Creative Editor")
	bool GetSceneSaveInfo(FSceneSaveInfo& OutSceneSaveInfo);
	
	static UNetVRkSceneSaveGame* CreateSaveGame(UObject* WorldContextObject, AActor* Owner, const FString& SlotName, const TSubclassOf<UNetVRkSceneSaveGame>& SceneSaveGameClass, const TArray<TObjectPtr<AActor>>& SceneActors, FSceneSaveInfo& OutSceneSaveInfo);

	static bool CreateSceneSaveInfo(const FName& SceneName, const TArray<TObjectPtr<AActor>>& SceneActors, FSceneSaveInfo& OutSceneSaveInfo);
};
