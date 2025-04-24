// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetVRkFileTransferSystem/Interfaces/TransferAccessActorInterface.h"
#include "NetVRkPlayerController.generated.h"

/**
 * 
 */
class UNetVRkTransferAccessComponent;

UCLASS()
class NETVRKP4VOXEL_API ANetVRkPlayerController : public APlayerController, public ITransferAccessActorInterface
{
	GENERATED_BODY()

#pragma region Framework
public:
	ANetVRkPlayerController();


#pragma endregion

#pragma region File Transfer System
public:
	virtual UNetVRkTransferAccessComponent* GetTransferAccessComponent_Implementation() override {return TransferAccessComponent;}

	virtual void OnTransferCompleted(UNetVRkTransferAccessComponent* InTransferAccessComponent, int64 TransferID, const TArray<uint8>& Data) override {};

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "File Transfer System")
	UNetVRkTransferAccessComponent* TransferAccessComponent;

#pragma endregion

};
