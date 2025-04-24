// Fill out your copyright notice in the Description page of Project Settings.


#include "ManipulationComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "CreativeEditorPawnComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorManipulableInterface.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorPawnInterface.h"

// Sets default values for this component's properties
UManipulationComponent::UManipulationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ManipulationRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ManipulationRootComponent"));
	ManipulationRootComponent->SetupAttachment(this);

	//PrimaryComponentTick.TickInterval = 0.25f;
	SetIsReplicatedByDefault(true);

	SetUsingAbsoluteScale(false);

	// ...
}


// Called when the game starts
void UManipulationComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UManipulationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsManipulatorEnabled)
	{
		TickManipulatorEnabled(DeltaTime);	
	}

	if (bIsManipulating)
	{
		TickManipulation(DeltaTime);
	}
	// ...
}

#pragma region Manipulation

void UManipulationComponent::SetManipulationEnabled(bool bInEnabled)
{
	const bool bWasManipulatorEnabled = bIsManipulatorEnabled;

	
	bIsManipulatorEnabled = bInEnabled;

	if (!GetManipulatedActor())
	{
		return;
	}

	const bool bIsManipulable = GetManipulatedActor()->Implements<UCreativeEditorManipulableInterface>();

	if (!bIsManipulable)
	{
		return;
	}
	
	ICreativeEditorManipulableInterface::Execute_OnManipulationEnabled(GetManipulatedActor(), GetManipulatorController(), this, bInEnabled);

	if (bWasManipulatorEnabled == bIsManipulatorEnabled)
	{
		return;
	}
	
	if (bIsManipulatorEnabled)
	{
		TriggerManipulatorEnabledAbility();
	}
	else
	{
		TriggerManipulatorDisabledAbility();
	}
}

void UManipulationComponent::SetIsManipulating(bool bInIsManipulating)
{
	bIsManipulating = bInIsManipulating;

	OnIsManipulatingChanged();
}

void UManipulationComponent::OnIsManipulatingChanged()
{
	if (bIsManipulating)
	{
		TriggerStartManipulatingAbility();
	}
	else
	{
		TriggerStopManipulatingAbility();
	}
}

void UManipulationComponent::TriggerManipulatorEnabledAbility()
{
}

void UManipulationComponent::TriggerManipulatorDisabledAbility()
{
}

void UManipulationComponent::TickManipulation(float DeltaTime)
{
	APawn* Manipulator = GetManipulatorPawn();

	if (!Manipulator)
	{
		return;
	}
}

void UManipulationComponent::TickManipulatorEnabled(float DeltaTime)
{
	
}

void UManipulationComponent::TriggerTickManipulatingAbility()
{
	
}

void UManipulationComponent::TriggerStartManipulatingAbility()
{
	
}

void UManipulationComponent::TriggerStopManipulatingAbility()
{
	
}

APawn* UManipulationComponent::GetManipulatorPawn() const
{
	APlayerController* ManipulatorController = GetManipulatorController();

	if (!ManipulatorController)
	{
		return nullptr;
	}
	
	return ManipulatorController->GetPawn();
}

AActor* UManipulationComponent::GetManipulatedActor() const
{
	return GetOwner()->GetOwner();
}

APlayerController* UManipulationComponent::GetManipulatorController() const
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

UAbilitySystemComponent* UManipulationComponent::GetManipulatorAbilitySystemComponent() const
{
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(GetManipulatorPawn());

	if (!AbilitySystemInterface)
	{
		return nullptr;
	}

	return AbilitySystemInterface->GetAbilitySystemComponent();
}

void UManipulationComponent::SetSnapEnabled(bool bInSnapEnabled)
{
	
}

void UManipulationComponent::SetSnapIncrement(float InSnapIncrement)
{
	
}

void UManipulationComponent::SetManipulationAbsolute(bool bInAbsolute)
{
	
}

void UManipulationComponent::SetManipulationDepth(float InDepth)
{
	
}

void UManipulationComponent::Server_UpdateManipulationActorLocation_Implementation(FVector NewLocation)
{
	GetOwner()->SetActorLocation(NewLocation);
}

void UManipulationComponent::Server_UpdateManipulationActorQuat_Implementation(FQuat NewRotation)
{
	GetOwner()->SetActorRotation(NewRotation);
}

void UManipulationComponent::Server_UpdateManipulationActorScale_Implementation(FVector NewScale)
{
	GetOwner()->SetActorScale3D(NewScale);
}

void UManipulationComponent::Server_UpdateManipulationActorLocationAndRotation_Implementation(FVector NewLocation, FQuat NewRotation)
{
	GetOwner()->SetActorLocationAndRotation(NewLocation, NewRotation);
}

#pragma endregion