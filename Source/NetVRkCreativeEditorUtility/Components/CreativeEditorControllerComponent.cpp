// Fill out your copyright notice in the Description page of Project Settings.


#include "CreativeEditorControllerComponent.h"

#include "CreativeEditorActorComponent.h"
#include "CreativeEditorPawnComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorActorInterface.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorPawnInterface.h"
#include "NetVRkCreativeEditorUtility/Libraries/NetVRkCreativeEditorBlueprintLibrary.h"

#pragma region Framework

// Sets default values for this component's properties
UCreativeEditorControllerComponent::UCreativeEditorControllerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	// ...
}


// Called when the game starts
void UCreativeEditorControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCreativeEditorControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCreativeEditorControllerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCreativeEditorControllerComponent, CreativeEditorPawn);
}


#pragma endregion

#pragma region Controller

AController* UCreativeEditorControllerComponent::GetController()
{
	return Cast<AController>(GetOwner());
}

APlayerController* UCreativeEditorControllerComponent::GetPlayerController()
{
	return Cast<APlayerController>(GetOwner());
}

APawn* UCreativeEditorControllerComponent::GetPawn()
{
	AController* Controller = Cast<AController>(GetOwner());

	if (!Controller)
	{
		return nullptr;
	}

	return Controller->GetPawn();
}
	
#pragma endregion

#pragma region Creative Editor

bool UCreativeEditorControllerComponent::IsCreativeModeActive()
{
	return CreativeEditorPawn != nullptr;
}

bool UCreativeEditorControllerComponent::ToggleCreativeMode()
{
	if (!GetOwner()->HasAuthority())
	{
		Server_ToggleCreativeMode();
		return false;
	}
	
	if (IsCreativeModeActive())
	{
		return ActivateCreativeMode(false);
	}
	else
	{
		return ActivateCreativeMode(true);
	}
}

void UCreativeEditorControllerComponent::Server_ToggleCreativeMode_Implementation()
{
	ToggleCreativeMode();
}

bool UCreativeEditorControllerComponent::ActivateCreativeMode(bool bActivate)
{
	if (!GetOwner()->HasAuthority())
	{
		Server_ActivateCreativeMode(bActivate);
		return false;
	}

	if (bActivate)
	{
		 SpawnAndPosessCreativeEditorPawn();
	}
	else
	{
		DestroyCreativeEditorPawn();
	}
	
	return true;
}

void UCreativeEditorControllerComponent::Server_ActivateCreativeMode_Implementation(bool bActivate)
{
	ActivateCreativeMode(bActivate);
}

void UCreativeEditorControllerComponent::SpawnAndPosessCreativeEditorPawn()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	if (!CreativeEditorPawnClass)
	{
		return;
	}

	AController* CurrentController = GetController();

	if (!CurrentController)
	{
		return;
	}

	APawn* CurrentPawn = GetPawn();

	if (!CurrentPawn)
	{
		return;
	}
	
	CachedOriginalPawn = CurrentPawn;

	const FTransform& CurrentPawnTransform = CurrentPawn->GetActorTransform();

	CreativeEditorPawn = GetWorld()->SpawnActorDeferred<APawn>(CreativeEditorPawnClass, CurrentPawnTransform, GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	CreativeEditorPawn->SetReplicates(true);
	CreativeEditorPawn->SetAutonomousProxy(true);
	
	if (!CreativeEditorPawn)
	{
		return;
	}

	CreativeEditorPawn->FinishSpawning(CurrentPawnTransform, true);

	CurrentController->Possess(CreativeEditorPawn);

	OnRep_CreativeEditorPawn(nullptr);

	CachedCreativeEditorPawn = CreativeEditorPawn;
}

void UCreativeEditorControllerComponent::DestroyCreativeEditorPawn()
{
	AController* CurrentController = GetController();

	if (!CurrentController)
	{
		return;
	}

	if (!CreativeEditorPawn)
	{
		return;
	}

	if (!CachedOriginalPawn)
	{
		return;
	}

	// CurrentController->Possess(CachedOriginalPawn);

	//CachedCreativeEditorPawn->Destroy();

	APawn* OldCreativeEditorPawn = CreativeEditorPawn;

	CreativeEditorPawn = nullptr;
	OnRep_CreativeEditorPawn(OldCreativeEditorPawn);
}

void UCreativeEditorControllerComponent::OnRep_CreativeEditorPawn(APawn* OldCreativeEditorPawn)
{
	if (CreativeEditorPawn && !OldCreativeEditorPawn)
	{
		OnCreativeModeActivated(OldCreativeEditorPawn);
	}
	
	if (!CreativeEditorPawn && OldCreativeEditorPawn)
	{
		OnCreativeModeDeactivated(OldCreativeEditorPawn);
	}
}

void UCreativeEditorControllerComponent::OnCreativeModeActivated(APawn* OldCreativeEditorPawn)
{
	if (!CreativeEditorPawn)
	{
		return;
	}

	TArray<TObjectPtr<AActor>> PlacedActorsCopy = PlacedCreativeEditorActors;

	
	for (TObjectPtr<AActor> PlacedActor : PlacedActorsCopy)
	{
		UCreativeEditorActorComponent* CreativeEditorActorComponent = UNetVRkCreativeEditorBlueprintLibrary::GetCreativeEditorActorComponent(PlacedActor);

		if (!CreativeEditorActorComponent)
		{
			continue;
		}

		CreativeEditorActorComponent->OnCreativeModeActivated(CreativeEditorPawn);
	}
	
	UCreativeEditorPawnComponent* CreativeEditorPawnComponent = ICreativeEditorPawnInterface::Execute_GetCreativeEditorPawnComponent(CreativeEditorPawn);

	if (CreativeEditorPawnComponent)
	{
		CreativeEditorPawnComponent->OnCreativeModeActivated(Cast<APlayerController>(GetController()));
	}
}

void UCreativeEditorControllerComponent::OnCreativeModeDeactivated(APawn* OldCreativeEditorPawn)
{
	if (!OldCreativeEditorPawn)
	{
		return;
	}

	TArray<TObjectPtr<AActor>> PlacedActorsCopy = PlacedCreativeEditorActors;
	
	for (TObjectPtr<AActor> PlacedActor : PlacedActorsCopy)
	{
		UCreativeEditorActorComponent* CreativeEditorActorComponent = UNetVRkCreativeEditorBlueprintLibrary::GetCreativeEditorActorComponent(PlacedActor);

		if (!CreativeEditorActorComponent)
		{
			continue;
		}

		CreativeEditorActorComponent->OnCreativeModeDeactivated(OldCreativeEditorPawn);
	}
	
	UCreativeEditorPawnComponent* CreativeEditorPawnComponent = ICreativeEditorPawnInterface::Execute_GetCreativeEditorPawnComponent(OldCreativeEditorPawn);

	if (CreativeEditorPawnComponent)
	{
		CreativeEditorPawnComponent->OnCreativeModeDeactivated(Cast<APlayerController>(GetController()));
	}
}


void UCreativeEditorControllerComponent::RequestReposessOriginalPawn(FVector NewLocation)
{
	AController* CurrentController = GetController();

	if (!CurrentController)
	{
		return;
	}
	
	if (!GetOwner()->HasAuthority())
	{
		Server_RequestReposessOriginalPawn(NewLocation);
		return;
	}

	if (!CachedOriginalPawn || !CachedCreativeEditorPawn)
	{
		return;
	}

	CurrentController->Possess(CachedOriginalPawn);

	if (bMoveOriginalPawnToCreativeEditorPawnLocation)
	{
		CachedOriginalPawn->SetActorLocation(NewLocation);
	}

	CachedCreativeEditorPawn->Destroy();
	CachedCreativeEditorPawn = nullptr;

	CachedOriginalPawn = nullptr;
}

void UCreativeEditorControllerComponent::Server_RequestReposessOriginalPawn_Implementation(FVector NewLocation)
{
	RequestReposessOriginalPawn(NewLocation);
}

void UCreativeEditorControllerComponent::RegisterPlacedCreativeEditorActor(AActor* PlacedActor)
{
	PlacedCreativeEditorActors.AddUnique(PlacedActor);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Placed Actors: %d"), PlacedCreativeEditorActors.Num()));
}

void UCreativeEditorControllerComponent::UnregisterPlacedCreativeEditorActor(AActor* PlacedActor)
{
	PlacedCreativeEditorActors.Remove(PlacedActor);
}

#pragma endregion