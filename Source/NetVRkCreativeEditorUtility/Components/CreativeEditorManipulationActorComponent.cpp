// Fill out your copyright notice in the Description page of Project Settings.


#include "CreativeEditorManipulationActorComponent.h"

#include "CreativeEditorPawnComponent.h"
#include "GameplayTagContainer.h"
#include "ManipulationComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorManipulationActorInterface.h"
#include "NetVRkCreativeEditorUtility/Libraries/NetVRkCreativeEditorBlueprintLibrary.h"

#pragma region Framework

// Sets default values for this component's properties
UCreativeEditorManipulationActorComponent::UCreativeEditorManipulationActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	// ...
}


// Called when the game starts
void UCreativeEditorManipulationActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	AActor* ManipulatedActor = GetManipulatedActor();

	if (!ManipulatedActor)
	{
		return;
	}

	//ManipulatedActor->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);

	ManipulatedActor->OnDestroyed.AddDynamic(this, &UCreativeEditorManipulationActorComponent::OnManipulatedActorDestroyed);

	APlayerController* ManipulatorController = GetManipulatorController();

	if (!ManipulatorController || !ManipulatorController->IsLocalPlayerController())
	{
		GetOwner()->SetActorHiddenInGame(true);
	}
	else
	{
		GetOwner()->SetActorHiddenInGame(false);
	}

	APawn* ManipulatorPawn = GetManipulatorPawn();

	if (!ManipulatorPawn)
	{
		return;
	}

	UCreativeEditorPawnComponent* PawnComponent = UNetVRkCreativeEditorBlueprintLibrary::GetCreativeEditorPawnComponent(ManipulatorPawn);

	if (!PawnComponent)
	{
		return;
	}

	PawnComponent->AddManipulationGizmoActor(GetOwner());

	const FGameplayTag& ManipulationMode = PawnComponent->GetManipulationModeTag();

	SetManipulationMode(ManipulationMode);
}

void UCreativeEditorManipulationActorComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	APawn* ManipulatorPawn = GetManipulatorPawn();

	if (!ManipulatorPawn)
	{
		Super::EndPlay(EndPlayReason);
		return;
	}
	
	UCreativeEditorPawnComponent* PawnComponent = UNetVRkCreativeEditorBlueprintLibrary::GetCreativeEditorPawnComponent(ManipulatorPawn);

	if (!PawnComponent)
	{
		Super::EndPlay(EndPlayReason);
		return;
	}

	PawnComponent->RemoveManipulationGizmoActor(GetOwner());
	
	Super::EndPlay(EndPlayReason);
}


void UCreativeEditorManipulationActorComponent::OnManipulatedActorDestroyed(AActor* DestroyedActor)
{
	GetOwner()->Destroy();
}

// Called every frame
void UCreativeEditorManipulationActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

#pragma endregion

#pragma region Creative Editor

AActor* UCreativeEditorManipulationActorComponent::GetManipulatedActor() const
{
	return GetOwner()->GetOwner();
}

APawn* UCreativeEditorManipulationActorComponent::GetManipulatorPawn() const
{
	APlayerController* ManipulatorController = GetManipulatorController();

	if (!ManipulatorController)
	{
		return nullptr;
	}
	
	return ManipulatorController->GetPawn();
}

APlayerController* UCreativeEditorManipulationActorComponent::GetManipulatorController() const
{
	if (!GetManipulatedActor())
	{
		return nullptr;
	}
	
	APlayerController* ManipulatorController = Cast<APlayerController>(GetManipulatedActor()->GetOwner());

	if (!ManipulatorController)
	{
		return nullptr;
	}
	
	return ManipulatorController;
}
#pragma endregion

#pragma region Manipulation

void UCreativeEditorManipulationActorComponent::SetManipulationMode(FGameplayTag InManipulationModeTag)
{
	const bool bImplements = GetOwner()->Implements<UCreativeEditorManipulationActorInterface>();

	if (!bImplements)
	{
		return;
	}

	ICreativeEditorManipulationActorInterface::Execute_OnManipulationModeChanged(GetOwner(), GetManipulatorController(), InManipulationModeTag);
}

void UCreativeEditorManipulationActorComponent::SetManipulationSnapEnabled(bool bInSnapEnabled, FGameplayTagContainer InManipulationTags)
{
	TArray<UManipulationComponent*> ManipulationComponents;

	GetOwner()->GetComponents<UManipulationComponent>(ManipulationComponents);

	for (UManipulationComponent* ManipulationComponent : ManipulationComponents)
	{
		if (InManipulationTags.IsEmpty() || ManipulationComponent->GetManipulationTags().HasAny(InManipulationTags))
		{
			ManipulationComponent->SetSnapEnabled(bInSnapEnabled);
		}
	}
}

void UCreativeEditorManipulationActorComponent::SetManipulationSnapIncrement(float InSnapIncrement, FGameplayTagContainer InManipulationTags)
{
	TArray<UManipulationComponent*> ManipulationComponents;

	GetOwner()->GetComponents<UManipulationComponent>(ManipulationComponents);

	for (UManipulationComponent* ManipulationComponent : ManipulationComponents)
	{
		if (InManipulationTags.IsEmpty() || ManipulationComponent->GetManipulationTags().HasAny(InManipulationTags))
		{
			ManipulationComponent->SetSnapIncrement(InSnapIncrement);
		}
	}
}

void UCreativeEditorManipulationActorComponent::SetManipulationAbsolute(bool bInAbsolute, FGameplayTagContainer InManipulationTags)
{
	TArray<UManipulationComponent*> ManipulationComponents;

	GetOwner()->GetComponents<UManipulationComponent>(ManipulationComponents);

	for (UManipulationComponent* ManipulationComponent : ManipulationComponents)
	{
		if (InManipulationTags.IsEmpty() || ManipulationComponent->GetManipulationTags().HasAny(InManipulationTags))
		{
			ManipulationComponent->SetManipulationAbsolute(bInAbsolute);
		}
	}
}

void UCreativeEditorManipulationActorComponent::SetManipulationDepth(float InDepth, FGameplayTagContainer InManipulationTags)
{
	TArray<UManipulationComponent*> ManipulationComponents;

	GetOwner()->GetComponents<UManipulationComponent>(ManipulationComponents);

	for (UManipulationComponent* ManipulationComponent : ManipulationComponents)
	{
		if (InManipulationTags.IsEmpty() || ManipulationComponent->GetManipulationTags().HasAny(InManipulationTags))
		{
			ManipulationComponent->SetManipulationDepth(InDepth);
		}
	}
}

UManipulationComponent* UCreativeEditorManipulationActorComponent::GetManipulationComponentByTag(const FGameplayTag InManipulationTag) const
{
	TArray<UManipulationComponent*> ManipulationComponents;

	GetOwner()->GetComponents<UManipulationComponent>(ManipulationComponents);

	for (UManipulationComponent* ManipulationComponent : ManipulationComponents)
	{
		if (InManipulationTag.IsValid() && ManipulationComponent->GetManipulationTags().HasTagExact(InManipulationTag))
		{
			return ManipulationComponent;
		}
	}
	
	return nullptr;
}

#pragma endregion
