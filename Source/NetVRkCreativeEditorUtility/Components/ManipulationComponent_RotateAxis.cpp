// Fill out your copyright notice in the Description page of Project Settings.


#include "ManipulationComponent_RotateAxis.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BaseGizmos/GizmoMath.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NetVRkCreativeEditorUtility/NetVRkCreativeEditorGameplayTags.h"
#include "NetVRkCreativeEditorUtility/Libraries/NetVRkCreativeEditorBlueprintLibrary.h"

#pragma region Framework

// Sets default values for this component's properties
UManipulationComponent_RotateAxis::UManipulationComponent_RotateAxis()
{
	RotateAxisBox = CreateDefaultSubobject<UBoxComponent>(TEXT("MoveDirectionCapsule"));
	RotateAxisBox->SetupAttachment(ManipulationRootComponent);
	RotateAxisBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RotateAxisBox->SetCollisionProfileName(FName("Clickable"));
	//RotateAxisBox->SetUsingAbsoluteScale(true);


	RotateAxisMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MoveDirectionMesh"));
	RotateAxisMesh->SetupAttachment(RotateAxisBox);
	RotateAxisMesh->SetHiddenInGame(false);
	RotateAxisMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//RotateAxisMesh->SetUsingAbsoluteScale(true);
	
	ManipulationTags.AddTag(NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Rotation);

	SetIsReplicatedByDefault(false);
}

void UManipulationComponent_RotateAxis::BeginPlay()
{
	Super::BeginPlay();

	RotateAxisBox->OnClicked.AddDynamic(this, &UManipulationComponent_RotateAxis::OnManipulatorClicked);
	RotateAxisBox->OnReleased.AddDynamic(this, &UManipulationComponent_RotateAxis::OnManipulatorReleased);
	RotateAxisBox->OnBeginCursorOver.AddDynamic(this, &UManipulationComponent_RotateAxis::OnManipulatorBeginCursorOver);
	RotateAxisBox->OnEndCursorOver.AddDynamic(this, &UManipulationComponent_RotateAxis::OnManipulatorEndCursorOver);
	
	RotateAxisMesh->SetMaterial(0, ManipulatorMaterial);
	RotateAxisMesh->SetRenderCustomDepth(true);
	ManipulatorDynamicMaterialInstance = RotateAxisMesh->CreateDynamicMaterialInstance(0, ManipulatorMaterial);
	
}

void UManipulationComponent_RotateAxis::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

#pragma endregion

#pragma region Mouse Events

void UManipulationComponent_RotateAxis::OnManipulatorClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	Super::OnManipulatorClicked(TouchedComponent, ButtonPressed);
}

void UManipulationComponent_RotateAxis::OnManipulatorReleased(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	Super::OnManipulatorReleased(TouchedComponent, ButtonPressed);
}

void UManipulationComponent_RotateAxis::OnManipulatorBeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	Super::OnManipulatorBeginCursorOver(TouchedComponent);
}

void UManipulationComponent_RotateAxis::OnManipulatorEndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	Super::OnManipulatorEndCursorOver(TouchedComponent);
}

#pragma endregion

#pragma region Manipulation

void UManipulationComponent_RotateAxis::SetManipulationEnabled(bool bInEnabled)
{
	Super::SetManipulationEnabled(bInEnabled);

	RotateAxisBox->SetVisibility(bInEnabled);

	RotateAxisMesh->SetVisibility(bInEnabled);

	RotateAxisBox->SetCollisionEnabled(bInEnabled ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);

}

void UManipulationComponent_RotateAxis::TickManipulation(float DeltaTime)
{
	Super::TickManipulation(DeltaTime);
	
}

void UManipulationComponent_RotateAxis::UpdateManipulationActorLocation(bool& bOutLocationWasChanged, FVector& OutNewLocation)
{
	Super::UpdateManipulationActorLocation(bOutLocationWasChanged, OutNewLocation);

	APawn* ManipulatorPawn = GetManipulatorPawn();

	if (!ManipulatorPawn || !ManipulatorPawn->IsLocallyControlled())
	{
		return;
	}

	FVector CurrentMouseWorldLocation;
	FVector CurrentMouseWorldDirection;
	
	const bool bGotMouseLocations = GetManipulatorMouseScreenWorldLocation(CurrentMouseWorldLocation, CurrentMouseWorldDirection);

	bool bIntersectsOut;
	FVector CurrentPlaneIntersectionPointOut;

	GizmoMath::RayPlaneIntersectionPoint(CachedManipulatorWorldLocation_ManipulationStart, CachedAxisDirectionZ_ManipulationStart, CurrentMouseWorldLocation, CurrentMouseWorldDirection, bIntersectsOut, CurrentPlaneIntersectionPointOut);

	FVector StartingPlaneIntersectionPointOut;

	GizmoMath::RayPlaneIntersectionPoint(CachedManipulatorWorldLocation_ManipulationStart, CachedAxisDirectionZ_ManipulationStart, CachedMouseWorldLocation_ManipulationStart, CachedMouseWorldDirection_ManipulationStart, bIntersectsOut, StartingPlaneIntersectionPointOut);

	const FVector& StartingLever = StartingPlaneIntersectionPointOut - GetComponentLocation();
	const FVector& CurrentLever = CurrentPlaneIntersectionPointOut - GetComponentLocation();

	//const FVector& DistanceVector = CurrentLever - StartingLever;

	const FVector& TransformedStaringVector = UKismetMathLibrary::InverseTransformLocation(CachedComponentWorldTransform_ManipulationStart, StartingPlaneIntersectionPointOut);

	const FVector& TransformedCurrentVector = UKismetMathLibrary::InverseTransformLocation(CachedComponentWorldTransform_ManipulationStart, CurrentPlaneIntersectionPointOut);

	const FVector& DistanceVector = TransformedCurrentVector - TransformedStaringVector;
	
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Distance Vector: %s"), *DistanceVector.ToString()));

	const FQuat& StartingQuat = CachedManipulatorWorldTransform_ManipulationStart.GetRotation();

	float Dist = DistanceVector.X;

	//Dist = DistanceVector.X > 0 ? Dist : -Dist;

	float Angle = Dist * RotationSpeed;

	const float SnapAngle = FMath::TruncToInt(Angle / RotationSnapAngle) * RotationSnapAngle;

	if (bSnapAngleEnabled)
	{
		Angle = SnapAngle;
	}
	
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Angle: %f"), Angle));
	
	const FVector& StartingVector = StartingQuat.Vector();
	
	const FVector& RotatedVectorX = UKismetMathLibrary::RotateAngleAxis(CachedAxisDirectionX_ManipulationStart, Angle, RotationAxisDirection);
	const FVector& RotatedVectorY = UKismetMathLibrary::RotateAngleAxis(CachedAxisDirectionY_ManipulationStart, Angle, RotationAxisDirection);
	const FVector& RotatedVectorZ = UKismetMathLibrary::RotateAngleAxis(CachedAxisDirectionZ_ManipulationStart, Angle, RotationAxisDirection);

	FRotator NewRot = UKismetMathLibrary::MakeRotationFromAxes(RotatedVectorX, RotatedVectorY, RotatedVectorZ);
	//FQuat NewQuat = RotatedVector.ToOrientationQuat();
	
	GetOwner()->SetActorRotation(NewRot.Quaternion());

	// //TODO: Make this into a gameplay ability with a target actor
	if (!GetOwner()->HasAuthority())
	{
		Server_UpdateManipulationActorQuat(NewRot.Quaternion());
	}
}

void UManipulationComponent_RotateAxis::OnStartedManipulating()
{
	CachedManipulatorWorldQuat_ManipulationStart = GetComponentQuat();
	CachedAxisDirectionX_ManipulationStart = GetOwner()->GetActorForwardVector();
	CachedAxisDirectionY_ManipulationStart = GetOwner()->GetActorRightVector();
	CachedAxisDirectionZ_ManipulationStart = GetOwner()->GetActorUpVector();
	RotationAxisDirection = RotateAxisBox->GetUpVector();
	CachedComponentWorldTransform_ManipulationStart = GetComponentTransform();
	CachedManipulatorWorldTransform_ManipulationStart = GetOwner()->GetActorTransform();
}

void UManipulationComponent_RotateAxis::OnStoppedManipulating()
{
	CachedManipulatorWorldQuat_ManipulationStart = FQuat::Identity;
	CachedComponentWorldTransform_ManipulationStart = FTransform::Identity;
}

void UManipulationComponent_RotateAxis::OnIsManipulatingChanged()
{
	if (bIsManipulating)
	{
		OnStartedManipulating();
	}
	else
	{
		OnStoppedManipulating();
	}

	// TArray<UManipulationComponent_RotateAxis*> ManipulationComponents;
	// GetOwner()->GetComponents<UManipulationComponent_RotateAxis>(ManipulationComponents);
	//
	// for (UManipulationComponent_RotateAxis* ManipulationComponent : ManipulationComponents)
	// {
	// 	if (ManipulationComponent != this)
	// 	{
	// 		if (bIsManipulating)
	// 		{
	// 			ManipulationComponent->OnStartedManipulating();
	// 		}
	// 		else
	// 		{
	// 			ManipulationComponent->OnStoppedManipulating();
	// 		}
	// 	}
	// }
	
	Super::OnIsManipulatingChanged();
}

void UManipulationComponent_RotateAxis::TriggerStartManipulatingAbility()
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
		//ManipulatorAbilitySystemComponent->TryActivateAbilityByClass(GameplayAbility_OnStartedManipulating);
	}
}

void UManipulationComponent_RotateAxis::TriggerStopManipulatingAbility()
{
	if (!GetManipulatorPawn() || !GetManipulatorPawn()->IsLocallyControlled())
	{
		return;
	}
	
	UAbilitySystemComponent* ManipulatorAbilitySystemComponent = GetManipulatorAbilitySystemComponent();

	if (ManipulatorAbilitySystemComponent && GameplayAbility_OnStoppedManipulating)
	{
		const FGameplayTag& OnStoppedManipulatingAbilityTag = GameplayAbility_OnStoppedManipulating.GetDefaultObject()->AbilityTags.GetByIndex(0);
	
		FGameplayEventData EventData;
		EventData.EventTag = OnStoppedManipulatingAbilityTag;
		EventData.Instigator = GetManipulatorPawn();
		EventData.EventMagnitude = 0.25;
		EventData.Target = GetManipulatedActor();
		
		const FGameplayTag& StopFollowTag = GameplayAbility_OnStartedManipulating.GetDefaultObject()->AbilityTags.GetByIndex(2);
		EventData.TargetTags.AddTag(StopFollowTag);
	
		EventData.TargetData = EventData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(GetOwner());

		//EventData.TargetTags.AddTag();

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetManipulatorPawn(), OnStoppedManipulatingAbilityTag, EventData);
		//ManipulatorAbilitySystemComponent->TryActivateAbilityByClass(GameplayAbility_OnStartedManipulating);
	}
}

void UManipulationComponent_RotateAxis::TriggerTickManipulatingAbility()
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