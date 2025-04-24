// Fill out your copyright notice in the Description page of Project Settings.


#include "ManipulationComponent_TranslatePlane.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BaseGizmos/GizmoMath.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NetVRkCreativeEditorUtility/NetVRkCreativeEditorGameplayTags.h"
#include "NetVRkCreativeEditorUtility/Libraries/NetVRkCreativeEditorBlueprintLibrary.h"

#pragma region Framework

// Sets default values for this component's properties
UManipulationComponent_TranslatePlane::UManipulationComponent_TranslatePlane()
{
	MoveDirectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("MoveDirectionCapsule"));
	MoveDirectionBox->SetupAttachment(ManipulationRootComponent);
	MoveDirectionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MoveDirectionBox->SetCollisionProfileName(FName("Clickable"));
	//MoveDirectionCapsule->SetUsingAbsoluteScale(true);


	MoveDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("MoveDirectionArrow"));
	MoveDirectionArrow->SetupAttachment(MoveDirectionBox);
	MoveDirectionArrow->SetHiddenInGame(false);
	//MoveDirectionArrow->SetUsingAbsoluteScale(true);


	MoveDirectionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MoveDirectionMesh"));
	MoveDirectionMesh->SetupAttachment(MoveDirectionBox);
	MoveDirectionMesh->SetHiddenInGame(false);
	MoveDirectionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//MoveDirectionMesh->SetUsingAbsoluteScale(true);

	SetIsReplicatedByDefault(false);
	
	ManipulationTags.AddTag(NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Translation);
}

void UManipulationComponent_TranslatePlane::BeginPlay()
{
	Super::BeginPlay();

	MoveDirectionBox->OnClicked.AddDynamic(this, &UManipulationComponent_TranslatePlane::OnManipulatorClicked);
	MoveDirectionBox->OnReleased.AddDynamic(this, &UManipulationComponent_TranslatePlane::OnManipulatorReleased);
	MoveDirectionBox->OnBeginCursorOver.AddDynamic(this, &UManipulationComponent_TranslatePlane::OnManipulatorBeginCursorOver);
	MoveDirectionBox->OnEndCursorOver.AddDynamic(this, &UManipulationComponent_TranslatePlane::OnManipulatorEndCursorOver);
	
	MoveDirectionMesh->SetMaterial(0, ManipulatorMaterial);
	MoveDirectionMesh->SetRenderCustomDepth(true);
	ManipulatorDynamicMaterialInstance = MoveDirectionMesh->CreateDynamicMaterialInstance(0, ManipulatorMaterial);

	ManipulatorDynamicMaterialInstance->SetVectorParameterValue(FName("Color"), FLinearColor(MoveDirectionArrow->ArrowColor));

	MoveDirectionArrow->SetVisibility(false);
	MoveDirectionArrow->SetHiddenInGame(true);
}

void UManipulationComponent_TranslatePlane::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UManipulationComponent_TranslatePlane::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);
}

#pragma endregion

#pragma region Mouse Events

void UManipulationComponent_TranslatePlane::OnManipulatorClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	Super::OnManipulatorClicked(TouchedComponent, ButtonPressed);
}

void UManipulationComponent_TranslatePlane::OnManipulatorReleased(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	Super::OnManipulatorReleased(TouchedComponent, ButtonPressed);
}

void UManipulationComponent_TranslatePlane::OnManipulatorBeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	CachedOriginalColor = MoveDirectionArrow->ArrowColor;
	MoveDirectionArrow->SetArrowColor(HighlightColor);

	Super::OnManipulatorBeginCursorOver(TouchedComponent);
}

void UManipulationComponent_TranslatePlane::OnManipulatorEndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	MoveDirectionArrow->SetArrowColor(CachedOriginalColor);

	Super::OnManipulatorEndCursorOver(TouchedComponent);
}

#pragma endregion

#pragma region Manipulation

void UManipulationComponent_TranslatePlane::SetManipulationEnabled(bool bInEnabled)
{
	Super::SetManipulationEnabled(bInEnabled);
	
	MoveDirectionBox->SetVisibility(bInEnabled);
	MoveDirectionMesh->SetVisibility(bInEnabled);
	MoveDirectionArrow->SetVisibility(bInEnabled);

	MoveDirectionBox->SetCollisionEnabled(bInEnabled ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	
}

void UManipulationComponent_TranslatePlane::TickManipulation(float DeltaTime)
{
	Super::TickManipulation(DeltaTime);
	
}

void UManipulationComponent_TranslatePlane::UpdateManipulationActorLocation(bool& bOutLocationWasChanged, FVector& OutNewLocation)
{
	APawn* ManipulatorPawn = GetManipulatorPawn();

	if (!ManipulatorPawn || !ManipulatorPawn->IsLocallyControlled())
	{
		return;
	}
	
	if (!MoveDirectionArrow)
	{
		return;
	}

	const FVector& ArrowOrigin = MoveDirectionArrow->GetComponentLocation();
	const FVector& ArrowDirection = MoveDirectionArrow->GetForwardVector();

	FVector CurrentMouseWorldLocation;
	FVector CurrentMouseWorldDirection;
	
	const bool bGotMouseLocations = GetManipulatorMouseScreenWorldLocation(CurrentMouseWorldLocation, CurrentMouseWorldDirection);
	bool bIntersectsOut;
	FVector CurrentPlaneIntersectionPointOut;

	GizmoMath::RayPlaneIntersectionPoint(CachedManipulatorWorldLocation_ManipulationStart, TranslationNormalDirection, CurrentMouseWorldLocation, CurrentMouseWorldDirection, bIntersectsOut, CurrentPlaneIntersectionPointOut);

	FVector StartingPlaneIntersectionPointOut;

	GizmoMath::RayPlaneIntersectionPoint(CachedManipulatorWorldLocation_ManipulationStart, TranslationNormalDirection, CachedMouseWorldLocation_ManipulationStart, CachedMouseWorldDirection_ManipulationStart, bIntersectsOut, StartingPlaneIntersectionPointOut);

	const FVector& MoveDist = CurrentPlaneIntersectionPointOut - StartingPlaneIntersectionPointOut;

	float MoveDistSize = MoveDist.Size();

	if (bSnapEnabled)
	{
		MoveDistSize = FMath::RoundToInt(MoveDistSize / SnapDistance) * SnapDistance;
	}
	
	const FVector& MoveDirection = MoveDist.GetSafeNormal();
	
	const FVector& NewLocation = CachedManipulatorWorldLocation_ManipulationStart + (MoveDirection * MoveDistSize);

	bOutLocationWasChanged = NewLocation != GetOwner()->GetActorLocation();
	
	if (bOutLocationWasChanged)
	{
		OutNewLocation = NewLocation;
		//move the manipulator
		GetOwner()->SetActorLocation(NewLocation);

		//TODO: Make this into a gameplay ability with a target actor
		if (!GetOwner()->HasAuthority())
		{
			Server_UpdateManipulationActorLocation(NewLocation);
		}
	}
}


void UManipulationComponent_TranslatePlane::OnIsManipulatingChanged()
{
	Super::OnIsManipulatingChanged();

	if (bIsManipulating)
	{
		OnStartedManipulating();
	}
	else
	{
		OnStoppedManipulating();
	}
}

void UManipulationComponent_TranslatePlane::OnStartedManipulating()
{
	TranslationNormalDirection = MoveDirectionBox->GetUpVector();
}

void UManipulationComponent_TranslatePlane::OnStoppedManipulating()
{
}

void UManipulationComponent_TranslatePlane::TriggerStartManipulatingAbility()
{
	if (!GetManipulatorPawn() || !GetManipulatorPawn()->IsLocallyControlled())
	{
		return;
	}
	
	UAbilitySystemComponent* ManipulatorAbilitySystemComponent = GetManipulatorAbilitySystemComponent();

	if (ManipulatorAbilitySystemComponent && GameplayAbility_OnStartedManipulating)
	{
		const FGameplayTag& OnStartedManipulatingTag = GameplayAbility_OnStartedManipulating.GetDefaultObject()->AbilityTags.GetByIndex(0);
	
		FGameplayEventData EventData;
		EventData.EventTag = OnStartedManipulatingTag;
		EventData.Instigator = GetManipulatorPawn();
		EventData.EventMagnitude = 0.25;
		EventData.Target = GetManipulatedActor();
		
		const FGameplayTag& StartFollowTag = GameplayAbility_OnStartedManipulating.GetDefaultObject()->AbilityTags.GetByIndex(1);
		EventData.TargetTags.AddTag(StartFollowTag);
		EventData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(GetOwner());

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetManipulatorPawn(), OnStartedManipulatingTag, EventData);
	}
}

void UManipulationComponent_TranslatePlane::TriggerStopManipulatingAbility()
{
	if (!GetManipulatorPawn() || !GetManipulatorPawn()->IsLocallyControlled())
	{
		return;
	}
	
	UAbilitySystemComponent* ManipulatorAbilitySystemComponent = GetManipulatorAbilitySystemComponent();

	if (ManipulatorAbilitySystemComponent && GameplayAbility_OnStoppedManipulating)
	{
		ManipulatorAbilitySystemComponent->CancelAbility(GameplayAbility_OnStoppedManipulating.GetDefaultObject());
	}
}

void UManipulationComponent_TranslatePlane::TriggerTickManipulatingAbility()
{
	if (!GetManipulatorPawn() || !GetManipulatorPawn()->IsLocallyControlled())
	{
		return;
	}
	
	UAbilitySystemComponent* ManipulatorAbilitySystemComponent = GetManipulatorAbilitySystemComponent();

	if (ManipulatorAbilitySystemComponent && GameplayAbility_TickManipulating)
	{
		const FGameplayTag& TickManipulatingAbilityTag = GameplayAbility_TickManipulating.GetDefaultObject()->AbilityTags.GetByIndex(0);
	
		FGameplayEventData EventData;
		EventData.EventTag = TickManipulatingAbilityTag;
		EventData.Instigator = GetManipulatorPawn();
		EventData.EventMagnitude = 0.0f;
		EventData.Target = GetManipulatedActor();

		FGameplayAbilityTargetingLocationInfo SourceLocation;
		SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
		SourceLocation.SourceActor = GetManipulatedActor();

		FGameplayAbilityTargetingLocationInfo TargetLocation;
		TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
		TargetLocation.SourceActor = GetOwner();
	
		EventData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromLocations(SourceLocation, TargetLocation);
		//EventData.TargetTags.AddTag();

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetManipulatorPawn(), TickManipulatingAbilityTag, EventData);
		//ManipulatorAbilitySystemComponent->TryActivateAbilityByClass(GameplayAbility_OnStartedManipulating);
	}
}

#pragma endregion