// Fill out your copyright notice in the Description page of Project Settings.


#include "NetVRkTransferAccessComponent.h"

#include "NetVRkFileTransferSystem/Interfaces/TransferAccessActorInterface.h"


DEFINE_LOG_CATEGORY_STATIC(NetVRkTransferAccessComponentLog, Log, All);

#pragma region Framework
// Sets default values for this component's properties
UNetVRkTransferAccessComponent::UNetVRkTransferAccessComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TransferComponent = CreateDefaultSubobject<UTransferComponent>(TEXT("TransferComponent"));
	// ...
}


// Called when the game starts
void UNetVRkTransferAccessComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UNetVRkTransferAccessComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

#pragma endregion

#pragma region Transfer

void UNetVRkTransferAccessComponent::Server_TransferCompleted_Implementation(uint64 TransferId, AActor* TransferTargetActor)
{
	Transfer* Transfer = TransferComponent->ServerGetsCompletedTransfer(TransferId);

	if (!Transfer)
	{
		return;
	}
	
	uint8* Buffer = Transfer->Buffer;
	int BufferLength = Transfer->Length;

	TArray<uint8> Data = TArray<uint8>(Buffer, BufferLength);


	ITransferAccessActorInterface* TransferAccessActor = Cast<ITransferAccessActorInterface>(TransferTargetActor);

	if (TransferAccessActor)
	{
		TransferAccessActor->OnTransferCompleted(this, TransferId, Data);
	}

	TransferComponent->ServerFreesCompletedTransfer(TransferId);
	
}

void UNetVRkTransferAccessComponent::Client_TransferCompleted_Implementation(uint64 TransferId)
{
	Transfer* Transfer = TransferComponent->ClientGetsCompletedTransfer(TransferId);

	if (!Transfer)
	{
		return;
	}
	
	uint8* Buffer = Transfer->Buffer;
	int BufferLength = Transfer->Length;
	
	TransferComponent->ClientFreesCompletedTransfer(TransferId);
}

void UNetVRkTransferAccessComponent::Server_TotalTransferCompleted_Implementation(const TArray<uint8>& Data, AActor* TransferTargetActor)
{
	ITransferAccessActorInterface* TransferAccessActor = Cast<ITransferAccessActorInterface>(TransferTargetActor);

	if (TransferAccessActor)
	{
		TransferAccessActor->OnTransferCompleted(this, -1, Data);
	}
}

uint64 UNetVRkTransferAccessComponent::BeginTransferToServer(TArray<uint8>& Data, AActor* TransferTargetActor)
{
	int BufferLength = Data.Num();

	uint8* Buffer = Data.GetData();

	return TransferComponent->SendBufferToServer(Buffer, BufferLength, [this, TransferTargetActor] (uint64 completedTransferId)
		{
			UE_LOG(NetVRkTransferAccessComponentLog, Display, TEXT("We finished sending a buffer with id %llu from server to client"), completedTransferId);
			Server_TransferCompleted(completedTransferId, TransferTargetActor);
		});

}

uint64 UNetVRkTransferAccessComponent::BeginTransferToClient(TArray<uint8>& Data)
{
	
	int BufferLength = Data.Num();

	uint8* Buffer = Data.GetData();

	return TransferComponent->SendBufferToClient(Buffer, BufferLength,
		[this] (uint64 completedTransferId)
		{
			UE_LOG(NetVRkTransferAccessComponentLog, Display, TEXT("We finished sending a buffer with id %llu from server to client"), completedTransferId);
			Client_TransferCompleted(completedTransferId);
		}
	);
}

Transfer* UNetVRkTransferAccessComponent::GetTransfer(uint64 TransferId)
{
	if (GetOwner()->HasAuthority())
	{
		return TransferComponent->ServerGetsCompletedTransfer(TransferId);
	}
	else
	{
		return TransferComponent->ClientGetsCompletedTransfer(TransferId);
	}
}

bool UNetVRkTransferAccessComponent::GetTransferData(uint64 TransferId, TArray<uint8> &Data)
{
	Transfer* Transfer = GetTransfer(TransferId);

	if (!Transfer)
	{
		return false;
	}
	
	uint8* Buffer = Transfer->Buffer;
	int BufferLength = Transfer->Length;

	Data = TArray<uint8>(Buffer, BufferLength);

	return true;
}

void UNetVRkTransferAccessComponent::FreeCompletedTransfer(uint64 TransferId)
{
	if (GetOwner()->HasAuthority())
	{
		return TransferComponent->ServerFreesCompletedTransfer(TransferId);
	}
	else
	{
		return TransferComponent->ClientFreesCompletedTransfer(TransferId);
	}
}


#pragma endregion
