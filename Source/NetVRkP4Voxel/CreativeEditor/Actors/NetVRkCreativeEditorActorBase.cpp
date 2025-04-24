// Fill out your copyright notice in the Description page of Project Settings.


#include "NetVRkCreativeEditorActorBase.h"
#include "NetVRkSaveGameUtility/Components/SceneSaveGameActorComponent.h"
#include "SelectionSystem/Components/SelectableComponent.h"

#pragma region Framework

ANetVRkCreativeEditorActorBase::ANetVRkCreativeEditorActorBase()
{
	SceneSaveGameActorComponent = CreateDefaultSubobject<USceneSaveGameActorComponent>(TEXT("SceneSaveGameActorComponent"));

	SelectableComponent = CreateDefaultSubobject<USelectableComponent>(TEXT("SelectableComponent"));
}

#pragma endregion

#pragma region Save Game Utility

USceneSaveGameActorComponent* ANetVRkCreativeEditorActorBase::GetSceneSaveGameActorComponent_Implementation() const
{
	return SceneSaveGameActorComponent;
}

bool ANetVRkCreativeEditorActorBase::PopulateSceneActorSaveInfo_Implementation(FSceneActorSaveInfo& OutSceneSaveInfo)
{
	return true;
}

void ANetVRkCreativeEditorActorBase::SetupActorFromSaveInfo_Implementation(const FSceneActorSaveInfo& OutSceneSaveInfo)
{
	//Implement any setup here
}


#pragma endregion
