// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <functional>
#include "TransferComponent.h"
#include "NetVRkTransferAccessComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTransferCompletedDelegate, UNetVRkTransferAccessComponent*, TransferAccessComponent, uint64, TransferID, const TArray<uint8>&, Data);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETVRKFILETRANSFERSYSTEM_API UNetVRkTransferAccessComponent : public UActorComponent
{
	GENERATED_BODY()
#pragma region Framework
public:	
	// Sets default values for this component's properties
	UNetVRkTransferAccessComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion

#pragma region Transfer
	
private: // Network methods
	UFUNCTION(Server, Reliable)
	void Server_TransferCompleted(uint64 TransferId, AActor* TransferTargetActor);
	void Server_TransferCompleted_Implementation(uint64 TransferId, AActor* TransferTargetActor);

	UFUNCTION(Client, Reliable)
	void Client_TransferCompleted(uint64 TransferId);
	void Client_TransferCompleted_Implementation(uint64 TransferId);
public:
	UFUNCTION(Server, Reliable)
	void Server_TotalTransferCompleted(const TArray<uint8>& Data, AActor* TransferTargetActor);
	
public:
	uint64 BeginTransferToServer(TArray<uint8>& Data, AActor* TransferTargetActor = nullptr);

	UFUNCTION()
	uint64 BeginTransferToClient(TArray<uint8>& Data);

	// UPROPERTY(BlueprintAssignable, Category = "Transfer")
	// FOnTransferCompletedDelegate OnTransferCompletedDelegate;

	Transfer* GetTransfer(uint64 TransferId);

	bool GetTransferData(uint64 TransferId, TArray<uint8> &Data);

	void FreeCompletedTransfer(uint64 TransferId);
	
protected:
	UPROPERTY()
	UTransferComponent* TransferComponent;

#pragma endregion
		
};
