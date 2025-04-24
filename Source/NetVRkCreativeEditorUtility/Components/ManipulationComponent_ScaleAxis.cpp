// Fill out your copyright notice in the Description page of Project Settings.


#include "ManipulationComponent_ScaleAxis.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BaseGizmos/GizmoMath.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NetVRkCreativeEditorUtility/NetVRkCreativeEditorGameplayTags.h"
#include "NetVRkCreativeEditorUtility/Libraries/NetVRkCreativeEditorBlueprintLibrary.h"

#pragma region Framework

// Sets default values for this component's properties
UManipulationComponent_ScaleAxis::UManipulationComponent_ScaleAxis()
{
	MoveDirectionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("MoveDirectionCapsule"));
	MoveDirectionCapsule->SetupAttachment(ManipulationRootComponent);
	MoveDirectionCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MoveDirectionCapsule->SetCollisionProfileName(FName("Clickable"));
	//MoveDirectionCapsule->SetUsingAbsoluteScale(true);

	MoveDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("MoveDirectionArrow"));
	MoveDirectionArrow->SetupAttachment(MoveDirectionCapsule);
	MoveDirectionArrow->SetHiddenInGame(false);
	//MoveDirectionArrow->SetUsingAbsoluteScale(true);

	MoveDirectionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MoveDirectionMesh"));
	MoveDirectionMesh->SetupAttachment(MoveDirectionCapsule);
	MoveDirectionMesh->SetHiddenInGame(false);
	MoveDirectionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//MoveDirectionMesh->SetUsingAbsoluteScale(true);

	SetIsReplicatedByDefault(false);

	ManipulationTags.AddTag(NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Scale);

}

void UManipulationComponent_ScaleAxis::BeginPlay()
{
	Super::BeginPlay();

	MoveDirectionCapsule->OnClicked.AddDynamic(this, &UManipulationComponent_ScaleAxis::OnManipulatorClicked);
	MoveDirectionCapsule->OnReleased.AddDynamic(this, &UManipulationComponent_ScaleAxis::OnManipulatorReleased);
	MoveDirectionCapsule->OnBeginCursorOver.AddDynamic(this, &UManipulationComponent_ScaleAxis::OnManipulatorBeginCursorOver);
	MoveDirectionCapsule->OnEndCursorOver.AddDynamic(this, &UManipulationComponent_ScaleAxis::OnManipulatorEndCursorOver);
	
	MoveDirectionMesh->SetMaterial(0, ManipulatorMaterial);
	MoveDirectionMesh->SetRenderCustomDepth(true);
	ManipulatorDynamicMaterialInstance = MoveDirectionMesh->CreateDynamicMaterialInstance(0, ManipulatorMaterial);

	ManipulatorDynamicMaterialInstance->SetVectorParameterValue(FName("Color"), FLinearColor(MoveDirectionArrow->ArrowColor));

	MoveDirectionArrow->SetVisibility(false);
	MoveDirectionArrow->SetHiddenInGame(true);
}

void UManipulationComponent_ScaleAxis::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

#pragma endregion

#pragma region Mouse Events

void UManipulationComponent_ScaleAxis::OnManipulatorClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	Super::OnManipulatorClicked(TouchedComponent, ButtonPressed);
}

void UManipulationComponent_ScaleAxis::OnManipulatorReleased(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	Super::OnManipulatorReleased(TouchedComponent, ButtonPressed);
}

void UManipulationComponent_ScaleAxis::OnManipulatorBeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	CachedOriginalColor = MoveDirectionArrow->ArrowColor;
	MoveDirectionArrow->SetArrowColor(HighlightColor);

	Super::OnManipulatorBeginCursorOver(TouchedComponent);
}

void UManipulationComponent_ScaleAxis::OnManipulatorEndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	MoveDirectionArrow->SetArrowColor(CachedOriginalColor);

	Super::OnManipulatorEndCursorOver(TouchedComponent);
}

#pragma endregion

#pragma region Manipulation

void UManipulationComponent_ScaleAxis::SetManipulationEnabled(bool bInEnabled)
{
	Super::SetManipulationEnabled(bInEnabled);
	
	MoveDirectionCapsule->SetVisibility(bInEnabled);
	MoveDirectionMesh->SetVisibility(bInEnabled);
	MoveDirectionArrow->SetVisibility(bInEnabled);

	MoveDirectionCapsule->SetCollisionEnabled(bInEnabled ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	
}

void UManipulationComponent_ScaleAxis::TickManipulation(float DeltaTime)
{
	Super::TickManipulation(DeltaTime);
	
}

void UManipulationComponent_ScaleAxis::UpdateManipulationActorLocation(bool& bOutLocationWasChanged, FVector& OutNewLocation)
{
	APawn* ManipulatorPawn = GetManipulatorPawn();

	if (!ManipulatorPawn || !ManipulatorPawn->IsLocallyControlled())
	{
		return;
	}

	APlayerController* PlayerController = GetManipulatorController();

	if (!PlayerController)
	{
		return;
	}
	
	if (!MoveDirectionArrow)
	{
		return;
	}

	const FVector& ArrowOrigin = MoveDirectionArrow->GetComponentLocation();
	FVector ArrowDirection = MoveDirectionArrow->GetForwardVector();

	FVector CurrentMouseWorldLocation;
	FVector CurrentMouseWorldDirection;
	
	const bool bGotMouseLocations = GetManipulatorMouseScreenWorldLocation(CurrentMouseWorldLocation, CurrentMouseWorldDirection);
	FVector ProjectedStartLoc;
	float LineParameterOut;
	FVector NearestRayPointOut;
	float RayParameterOut;

	GizmoMath::NearestPointOnLineToRay(ArrowOrigin, ArrowDirection, CachedMouseWorldLocation_ManipulationStart, CachedMouseWorldDirection_ManipulationStart, ProjectedStartLoc, LineParameterOut, NearestRayPointOut, RayParameterOut);

	FVector ProjectedCurrentLoc;
	GizmoMath::NearestPointOnLineToRay(ArrowOrigin, ArrowDirection, CurrentMouseWorldLocation, CurrentMouseWorldDirection, ProjectedCurrentLoc, LineParameterOut, NearestRayPointOut, RayParameterOut);
	
	float CurrentMouseX;
	float CurrentMouseY;
	PlayerController->GetMousePosition(CurrentMouseX, CurrentMouseY);
	
	const FVector2D& CurrentMousePosition = FVector2D(CurrentMouseX, CurrentMouseY);
	
	const FVector& MoveDist = ProjectedCurrentLoc - ProjectedStartLoc;

	float MoveDistSize = MoveDist.Size() * ScaleSpeed;
	
	const bool bCoincident = FVector::Coincident(MoveDist, ArrowDirection, FMath::Cos(FMath::DegreesToRadians(180.0f)));
	
	MoveDistSize = bCoincident ? MoveDistSize : MoveDistSize*-1.0f;
	
	if (bScaleAllAxes)
	{
		const FVector2D& MouseDist = CurrentMousePosition - CachedMousePosition_ManipulationStart;

		MoveDistSize = MouseDist.Size() * ScaleSpeed;

		MoveDistSize = MouseDist.Y > 0 ? MoveDistSize * -1.0f : MoveDistSize;
		ArrowDirection = FVector(1.0f,1.0f,1.0f);
	}

	if (bSnapEnabled)
	{
		MoveDistSize = FMath::RoundToInt(MoveDistSize / SnapScaleIncrement) * SnapScaleIncrement;
	}

	const FVector& NewScale = CachedManipulatorWorldScale_ManipulationStart + (ArrowDirection * MoveDistSize);
	
	//const FVector& NewLocation = CachedManipulatorWorldLocation_ManipulationStart + (MoveDirection * MoveDistSize);

	bOutLocationWasChanged = NewScale != GetOwner()->GetActorScale3D();
	
	if (bOutLocationWasChanged)
	{
		OutNewLocation = NewScale;
		//move the manipulator
		GetOwner()->SetActorScale3D(NewScale);

		//TODO: Make this into a gameplay ability with a target actor
		if (!GetOwner()->HasAuthority())
		{
			Server_UpdateManipulationActorScale(NewScale);
		}
	}
}

void UManipulationComponent_ScaleAxis::OnIsManipulatingChanged()
{
	Super::OnIsManipulatingChanged();

	APlayerController* PlayerController = GetManipulatorController();

	if (!PlayerController)
	{
		return;
	}

	if (bIsManipulating)
	{
		float CurrentMouseX;
		float CurrentMouseY;
		PlayerController->GetMousePosition(CurrentMouseX, CurrentMouseY);
	
		const FVector2D& CurrentMousePosition = FVector2D(CurrentMouseX, CurrentMouseY);

		CachedMousePosition_ManipulationStart = CurrentMousePosition;
	}
	else
	{
		CachedMousePosition_ManipulationStart = FVector2D::ZeroVector;
	}
	
}

void UManipulationComponent_ScaleAxis::TriggerStartManipulatingAbility()
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

void UManipulationComponent_ScaleAxis::TriggerStopManipulatingAbility()
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

void UManipulationComponent_ScaleAxis::TriggerTickManipulatingAbility()
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