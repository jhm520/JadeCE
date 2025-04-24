// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NetVRkGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class NETVRKP4VOXEL_API UNetVRkGameInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;
	
};
