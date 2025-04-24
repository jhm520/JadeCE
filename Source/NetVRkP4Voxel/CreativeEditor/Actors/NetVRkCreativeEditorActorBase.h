// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetVRkCreativeEditorUtility/Actor/CreativeEditorActorBase.h"
#include "NetVRkSaveGameUtility/Interfaces/SceneSaveGameActorInterface.h"
#include "SelectionSystem/Interfaces/SelectableInterface.h"
#include "NetVRkCreativeEditorActorBase.generated.h"

/**
 * 
 */
UCLASS()
class NETVRKP4VOXEL_API ANetVRkCreativeEditorActorBase : public ACreativeEditorActorBase, public ISceneSaveGameActorInterface, public ISelectableInterface
{
	GENERATED_BODY()

#pragma region Framework

	ANetVRkCreativeEditorActorBase();
	
#pragma endregion

#pragma region Save Game Utility
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scene Save Game Actor")
	USceneSaveGameActorComponent* SceneSaveGameActorComponent;
	
public:
	virtual USceneSaveGameActorComponent* GetSceneSaveGameActorComponent_Implementation() const override;

	virtual bool PopulateSceneActorSaveInfo_Implementation(FSceneActorSaveInfo& OutSceneSaveInfo) override;

	virtual void SetupActorFromSaveInfo_Implementation(const FSceneActorSaveInfo& OutSceneSaveInfo) override;

#pragma endregion

#pragma region Selectable

public:
	virtual USelectableComponent* GetSelectableComponent_Implementation() const override {return SelectableComponent;}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection")
	USelectableComponent* SelectableComponent;

#pragma endregion
	
};
