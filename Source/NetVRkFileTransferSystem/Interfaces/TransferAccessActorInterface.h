// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TransferAccessActorInterface.generated.h"

class UCreativeEditorActorComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTransferAccessActorInterface : public UInterface
{
	GENERATED_BODY()
};
class UNetVRkTransferAccessComponent;
/**
 * 
 */
class NETVRKFILETRANSFERSYSTEM_API ITransferAccessActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "File Transfer System")
	UNetVRkTransferAccessComponent* GetTransferAccessComponent();

	virtual void OnTransferCompleted(UNetVRkTransferAccessComponent* TransferAccessComponent, int64 TransferID, const TArray<uint8>& Data) = 0;
};
