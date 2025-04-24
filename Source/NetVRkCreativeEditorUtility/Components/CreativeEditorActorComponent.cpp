// Fill out your copyright notice in the Description page of Project Settings.


#include "CreativeEditorActorComponent.h"

#include "CreativeEditorControllerComponent.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorActorInterface.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorControllerInterface.h"

#pragma region Framework

// Sets default values for this component's properties
UCreativeEditorActorComponent::UCreativeEditorActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	// ...
}


// Called when the game starts
void UCreativeEditorActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	RegisterWithController();
}

void UCreativeEditorActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterWithController();
	
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void UCreativeEditorActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCreativeEditorActorComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCreativeEditorActorComponent, bIsPreview);
	DOREPLIFETIME(UCreativeEditorActorComponent, bIsPlaced);
	DOREPLIFETIME(UCreativeEditorActorComponent, bIsManipulation);
	DOREPLIFETIME(UCreativeEditorActorComponent, ManipulationActor);

}
#pragma endregion

#pragma region Creative Editor

APawn* UCreativeEditorActorComponent::GetCreativeEditorPawn() const
{
	APlayerController* CreativeEditorController = GetCreativeEditorController();

	if (!CreativeEditorController)
	{
		return nullptr;
	}

	return CreativeEditorController->GetPawn();
}

APlayerController* UCreativeEditorActorComponent::GetCreativeEditorController() const
{
	return Cast<APlayerController>(GetOwner()->GetOwner());
}

void UCreativeEditorActorComponent::SetIsPreview(bool bInIsPreview)
{
	bIsPreview = bInIsPreview;
	OnRep_bIsPreview();
}

void UCreativeEditorActorComponent::SetIsPlaced(bool bInIsPlaced)
{
	bIsPlaced = bInIsPlaced;
	OnRep_bIsPlaced();
}

void UCreativeEditorActorComponent::SetIsManipulation(bool bInIsManipulation)
{
	bIsManipulation = bInIsManipulation;
	OnRep_bIsManipulation();
}

void UCreativeEditorActorComponent::OnRep_bIsPreview()
{
	const bool bActorImplements = GetOwner()->Implements<UCreativeEditorActorInterface>();

	if (bActorImplements)
	{
		if (bIsPreview)
		{
			ICreativeEditorActorInterface::Execute_OnPreviewActorPlacement(GetOwner());
		}
		else
		{
			ICreativeEditorActorInterface::Execute_OnEndedPreviewActorPlacement(GetOwner());
		}
	}
}

void UCreativeEditorActorComponent::OnRep_bIsPlaced()
{
	const bool bActorImplements = GetOwner()->Implements<UCreativeEditorActorInterface>();
	
	if (bActorImplements)
	{
		if (bIsPlaced)
		{
			ICreativeEditorActorInterface::Execute_OnConfirmedActorPlacement(GetOwner());
		}
	}
}

void UCreativeEditorActorComponent::OnRep_ManipulationActor()
{
	
}

void UCreativeEditorActorComponent::OnRep_bIsManipulation()
{
	if (bIsManipulation)
	{
		OnBeginManipulation();
	}
	else
	{
		OnEndManipulation();
	}
}

void UCreativeEditorActorComponent::OnBeginManipulation()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	
	const FTransform& CurrentActorTransform = GetOwner()->GetActorTransform();

	ManipulationActor = GetWorld()->SpawnActorDeferred<AActor>(ManuplationActorClass, CurrentActorTransform, GetOwner(), GetOwner()->GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!ManipulationActor)
	{
		return;
	}

	ManipulationActor->FinishSpawning(CurrentActorTransform);
	
}

void UCreativeEditorActorComponent::OnEndManipulation()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	
	if (ManipulationActor)
	{
		ManipulationActor->Destroy();
	}
}

#pragma endregion

#pragma region Creative Editor | Controller

void UCreativeEditorActorComponent::RegisterWithController()
{
	AActor* ControllerOwner = GetOwner()->GetOwner();
	
	if (!ControllerOwner)
	{
		return;
	}
	
	const bool bControllerOwnerImplements = ControllerOwner->Implements<UCreativeEditorControllerInterface>();

	if (!bControllerOwnerImplements)
	{
		return;
	}

	UCreativeEditorControllerComponent* CreativeEditorControllerComponent = ICreativeEditorControllerInterface::Execute_GetCreativeEditorControllerComponent(GetOwner()->GetOwner());

	if (!CreativeEditorControllerComponent)
	{
		return;
	}

	CreativeEditorControllerComponent->RegisterPlacedCreativeEditorActor(GetOwner());
}

void UCreativeEditorActorComponent::UnregisterWithController()
{
	AActor* ControllerOwner = GetOwner()->GetOwner();
	
	if (!ControllerOwner)
	{
		return;
	}
	
	const bool bControllerOwnerImplements = ControllerOwner->Implements<UCreativeEditorControllerInterface>();

	if (!bControllerOwnerImplements)
	{
		return;
	}

	UCreativeEditorControllerComponent* CreativeEditorControllerComponent = ICreativeEditorControllerInterface::Execute_GetCreativeEditorControllerComponent(GetOwner()->GetOwner());

	if (!CreativeEditorControllerComponent)
	{
		return;
	}

	CreativeEditorControllerComponent->UnregisterPlacedCreativeEditorActor(GetOwner());
}

void UCreativeEditorActorComponent::OnCreativeModeActivated(APawn* CreativeEditorPawn)
{
	const bool bIsCEActor = GetOwner()->Implements<UCreativeEditorActorInterface>();

	if (!bIsCEActor)
	{
		return;
	}

	ICreativeEditorActorInterface::Execute_OnCreativeModeActivated(GetOwner(), CreativeEditorPawn);
}

void UCreativeEditorActorComponent::OnCreativeModeDeactivated(APawn* OldCreativeEditorPawn)
{
	//if the actor is still in preview mode, and we're deactivating creative editor mode, destroy the actor
	if (GetIsPreview())
	{
		GetOwner()->Destroy();
		return;
	}
	
	const bool bIsCEActor = GetOwner()->Implements<UCreativeEditorActorInterface>();

	if (!bIsCEActor)
	{
		return;
	}

	ICreativeEditorActorInterface::Execute_OnCreativeModeDeactivated(GetOwner(), OldCreativeEditorPawn);
}

#pragma endregion
