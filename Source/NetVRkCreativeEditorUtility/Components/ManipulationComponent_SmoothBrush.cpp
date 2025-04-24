// Fill out your copyright notice in the Description page of Project Settings.


#include "ManipulationComponent_SmoothBrush.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CreativeEditorActorComponent.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BaseGizmos/GizmoMath.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NetVRkCreativeEditorUtility/NetVRkCreativeEditorGameplayTags.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorActorInterface.h"
#include "NetVRkSurfaceGridSystem/Actors/SurfaceGridActor.h"

#pragma region Framework

// Sets default values for this component's properties
UManipulationComponent_SmoothBrush::UManipulationComponent_SmoothBrush()
{
	SmoothBrushSphere = CreateDefaultSubobject<USphereComponent>(TEXT("MoveDirectionCapsule"));
	SmoothBrushSphere->SetupAttachment(ManipulationRootComponent);
	SmoothBrushSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SmoothBrushSphere->SetCollisionProfileName(FName("Clickable"));
	//MoveDirectionCapsule->SetUsingAbsoluteScale(true);
	
	//MoveDirectionArrow->SetUsingAbsoluteScale(true);


	SmoothBrushMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MoveDirectionMesh"));
	SmoothBrushMesh->SetupAttachment(SmoothBrushSphere);
	SmoothBrushMesh->SetHiddenInGame(false);
	SmoothBrushMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//MoveDirectionMesh->SetUsingAbsoluteScale(true);

	SetIsReplicatedByDefault(false);
	
	ManipulationTags.AddTag(NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Translation);
}

void UManipulationComponent_SmoothBrush::BeginPlay()
{
	Super::BeginPlay();
	
	SmoothBrushMesh->SetMaterial(0, ManipulatorMaterial);
	SmoothBrushMesh->SetRenderCustomDepth(true);
	ManipulatorDynamicMaterialInstance = SmoothBrushMesh->CreateDynamicMaterialInstance(0, ManipulatorMaterial);

	ManipulatorDynamicMaterialInstance->SetVectorParameterValue(FName("Color"), SmoothBrushColor);
	
}

void UManipulationComponent_SmoothBrush::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UManipulationComponent_SmoothBrush::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);
}

#pragma endregion

#pragma region Mouse Events

void UManipulationComponent_SmoothBrush::OnManipulatorClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	Super::OnManipulatorClicked(TouchedComponent, ButtonPressed);
}

void UManipulationComponent_SmoothBrush::OnManipulatorReleased(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	Super::OnManipulatorReleased(TouchedComponent, ButtonPressed);
}

void UManipulationComponent_SmoothBrush::TriggerManipulatorEnabledAbility()
{
	if (!GetManipulatorPawn() || !GetManipulatorPawn()->IsLocallyControlled())
	{
		return;
	}
	
	UAbilitySystemComponent* ManipulatorAbilitySystemComponent = GetManipulatorAbilitySystemComponent();

	if (ManipulatorAbilitySystemComponent && GameplayAbility_OnManipulatorEnabled)
	{
		const FGameplayTag& OnStartedManipulatingTag = GameplayAbility_OnManipulatorEnabled.GetDefaultObject()->AbilityTags.GetByIndex(0);
	
		FGameplayEventData EventData;
		EventData.EventTag = OnStartedManipulatingTag;
		EventData.Instigator = GetManipulatorPawn();
		EventData.EventMagnitude = 0.25;
		EventData.Target = GetManipulatedActor();
		
		const FGameplayTag& StartFollowTag = GameplayAbility_OnManipulatorEnabled.GetDefaultObject()->AbilityTags.GetByIndex(1);
		EventData.TargetTags.AddTag(StartFollowTag);
		EventData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(GetOwner());

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetManipulatorPawn(), OnStartedManipulatingTag, EventData);
	}
}

void UManipulationComponent_SmoothBrush::TriggerManipulatorDisabledAbility()
{
	if (!GetManipulatorPawn() || !GetManipulatorPawn()->IsLocallyControlled())
	{
		return;
	}
	
	UAbilitySystemComponent* ManipulatorAbilitySystemComponent = GetManipulatorAbilitySystemComponent();

	//cancel the on-enabled ability
	if (ManipulatorAbilitySystemComponent && GameplayAbility_OnManipulatorEnabled)
	{
		ManipulatorAbilitySystemComponent->CancelAbility(GameplayAbility_OnManipulatorEnabled.GetDefaultObject());
	}

	//trigger the on-disabled ability
	if (ManipulatorAbilitySystemComponent && GameplayAbility_OnManipulatorDisabled)
	{
		const FGameplayTag& OnStartedManipulatingTag = GameplayAbility_OnManipulatorDisabled.GetDefaultObject()->AbilityTags.GetByIndex(0);
	
		FGameplayEventData EventData;
		EventData.EventTag = OnStartedManipulatingTag;
		EventData.Instigator = GetManipulatorPawn();
		EventData.EventMagnitude = 0.25;
		EventData.Target = GetManipulatedActor();
		
		const FGameplayTag& RotationTag = GameplayAbilityTargetTag_ManipulatorDisabled;
		EventData.TargetTags.AddTag(RotationTag);
		EventData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(GetOwner());

		FGameplayAbilityTargetingLocationInfo SourceLocation;
		SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
		SourceLocation.SourceActor = GetManipulatedActor();
	
		FGameplayAbilityTargetingLocationInfo TranslationLocation;
		TranslationLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
		TranslationLocation.LiteralTransform = GetOwner()->GetActorTransform();

		EventData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromLocations(SourceLocation, TranslationLocation);

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetManipulatorPawn(), OnStartedManipulatingTag, EventData);
	}
}

void UManipulationComponent_SmoothBrush::TickManipulatorEnabled(float DeltaTime)
{
	Super::TickManipulatorEnabled(DeltaTime);
	
	APlayerController* PlayerController = GetManipulatorController();

	if (!PlayerController)
	{
		return;
	}

	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursor(SmoothBrushTraceChannel, true, HitResult);
	
	if (bIsManipulating)
	{
		return;
	}
	
	SmoothBrushMesh->SetWorldLocation(HitResult.ImpactPoint);
	
	FVector NewNormal = HitResult.ImpactNormal;

	const FVector& Offset = NewNormal * SmoothBrushDepth;

	const FVector& NewLocation = HitResult.ImpactPoint + Offset;

	const FVector& CurrentLocation = GetOwner()->GetActorLocation();

	if (CurrentLocation == NewLocation)
	{
		return;
	}

	FRotator NewRotation = WorldTransform_ManipulationEnabled.GetRotation().Rotator();

	const FVector& StartingVector = WorldTransform_ManipulationEnabled.GetRotation().Vector();
	
	FVector NormalOrthoganal1 = NewNormal;
	NormalOrthoganal1.Z -= 1.0f;

	FVector NormalOrthoganal2 = UKismetMathLibrary::Cross_VectorVector(NewNormal, NormalOrthoganal1);

	// DrawDebugLine(	GetWorld(), 
	// 		NewLocation, 
	// 		NewLocation + NewNormal.GetSafeNormal() * 100.0f, 
	// 		FColor::Red, 
	// 		false, 
	// 		0.1f, 
	// 		0, 
	// 		1.0f);
	//
	// DrawDebugLine(	GetWorld(), 
	// 				NewLocation, 
	// 				NewLocation + NormalOrthoganal1 * 100.0f, 
	// 				FColor::Red, 
	// 				false, 
	// 				0.1f, 
	// 				0, 
	// 				1.0f);
	//
	// DrawDebugLine(	GetWorld(), 
	// 			NewLocation, 
	// 			NewLocation + NormalOrthoganal2 * 100.0f, 
	// 			FColor::Red, 
	// 			false, 
	// 			0.1f, 
	// 			0, 
	// 			1.0f);
	
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("Angle: %f"), Angle));
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString("TickManipulatorEnabled"));

	const FRotator& ModRotation = UKismetMathLibrary::MakeRotationFromAxes(NormalOrthoganal1, NormalOrthoganal2, NewNormal);
	NewRotation = ModRotation;
	
	//move the manipulator
	GetOwner()->SetActorLocation(NewLocation);
	
	GetOwner()->SetActorRotation(NewRotation);
	
	//TODO: Make this into a gameplay ability with a target actor
	if (!GetOwner()->HasAuthority())
	{
		//Server_UpdateManipulationActorLocation(NewLocation);
		Server_UpdateManipulationActorLocationAndRotation(NewLocation, NewRotation.Quaternion());
	}
}

#pragma endregion

#pragma region Manipulation

void UManipulationComponent_SmoothBrush::SetManipulationEnabled(bool bInEnabled)
{
	const bool bWasManipulatorEnabled = bIsManipulatorEnabled;
	
	if (bInEnabled)
	{
		WorldTransform_ManipulationEnabled = GetOwner()->GetActorTransform();
	}
	else
	{
		if (bWasManipulatorEnabled)
		{
			GetOwner()->SetActorRotation(WorldTransform_ManipulationEnabled.GetRotation());
		}
		
		WorldTransform_ManipulationEnabled = FTransform::Identity;
	}
	
	Super::SetManipulationEnabled(bInEnabled);
	
	SmoothBrushSphere->SetVisibility(bInEnabled);
	SmoothBrushMesh->SetVisibility(bInEnabled);

	SmoothBrushSphere->SetCollisionEnabled(bInEnabled ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void UManipulationComponent_SmoothBrush::TickManipulation(float DeltaTime)
{
	Super::TickManipulation(DeltaTime);
	
}

void UManipulationComponent_SmoothBrush::UpdateManipulationActorLocation(bool& bOutLocationWasChanged, FVector& OutNewLocation)
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
	
	if (!SmoothBrushSphere)
	{
		return;
	}

	//TODO: Implement smooth brush logic
	
	FVector CurrentMouseWorldLocation;
	FVector CurrentMouseWorldDirection;
	
	const bool bGotMouseLocations = GetManipulatorMouseScreenWorldLocation(CurrentMouseWorldLocation, CurrentMouseWorldDirection);

	if (!bGotMouseLocations)
	{
		return;
	}
	
	if (!BrushSurfaceActor)
	{
		return;
	}

	if (!BrushSurfaceActor->bSurfaceGridGenerationFinished)
	{
		return;
	}

	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursor(SmoothBrushTraceChannel, true, HitResult);

	bool bIntersectsOut = false;
	
	BrushSurfaceActor->GetRayTraceSurfaceIntersectionPoint(CurrentMouseWorldLocation, CurrentMouseWorldDirection, bIntersectsOut, OutNewLocation, true, HitResult.ImpactPoint);
	
	//DrawDebugSphere(GetWorld(), OutNewLocation, 10.0f, 12, FColor::Green, false, 0.1f, 0, 1.0f);

	bOutLocationWasChanged = OutNewLocation != GetOwner()->GetActorLocation();
	//
	if (bOutLocationWasChanged)
	{
		SmoothBrushMesh->SetWorldLocation(OutNewLocation);

		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString("UpdateManipulationActorLocation"));
		//move the manipulator
		GetOwner()->SetActorLocation(OutNewLocation);
		
		//TODO: Make this into a gameplay ability with a target actor
		if (!GetOwner()->HasAuthority())
		{
			Server_UpdateManipulationActorLocation(OutNewLocation);
			//Server_UpdateManipulationActorLocation(HitResult.ImpactPoint);
		}
	}
}


void UManipulationComponent_SmoothBrush::OnIsManipulatingChanged()
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

void UManipulationComponent_SmoothBrush::OnStartedManipulating()
{
	APlayerController* PlayerController = GetManipulatorController();

	if (!PlayerController)
	{
		return;
	}
	
	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursor(SmoothBrushTraceChannel, true, HitResult);

	MouseHitResult_ManipulationStart = HitResult;

	if (BrushSurfaceActorClass && !BrushSurfaceActor)
	{
		if (!GetManipulatorPawn() || !GetManipulatorPawn()->IsLocallyControlled())
		{
			return;
		}
		FVector CurrentMouseWorldLocation;
		FVector CurrentMouseWorldDirection;

		const bool bGotMouseLocations = GetManipulatorMouseScreenWorldLocation(CurrentMouseWorldLocation, CurrentMouseWorldDirection);

		if (!bGotMouseLocations)
		{
			return;
		}

		FVector ViewpointLocation;
		FRotator ViewpointRotation;
		PlayerController->GetPlayerViewPoint(ViewpointLocation, ViewpointRotation);

		const FQuat& ViewpointQuat = ViewpointRotation.Quaternion();
		
		const FRotator& Rotation = UKismetMathLibrary::MakeRotationFromAxes(ViewpointQuat.GetUpVector(), ViewpointQuat.GetRightVector(), CurrentMouseWorldDirection*-1.0f);
		
		BrushSurfaceActor = GetWorld()->SpawnActor<ASurfaceGridActor>(BrushSurfaceActorClass, HitResult.ImpactPoint/* + FVector(0.0f, 0.0f, 100.0f)*/, Rotation);

		if (BrushSurfaceActor)
		{
			BrushSurfaceActor->OnSurfaceGridGeneratedAsyncDelegate.AddDynamic(this, &UManipulationComponent_SmoothBrush::TriggerStartManipulatingAbility);
			BrushSurfaceActor->RegenerateSurfaceGrid_Async();
		}
	}
}

void UManipulationComponent_SmoothBrush::OnStoppedManipulating()
{
	if (!GetManipulatorPawn() || !GetManipulatorPawn()->IsLocallyControlled())
	{
		return;
	}
	
	if (BrushSurfaceActor)
	{
		BrushSurfaceActor->Destroy();
		BrushSurfaceActor = nullptr;
	}
}

void UManipulationComponent_SmoothBrush::TriggerStartManipulatingAbility()
{
	if (!GetManipulatorPawn() || !GetManipulatorPawn()->IsLocallyControlled())
	{
		return;
	}

	if (!bIsManipulating)
	{
		return;
	}

	if (!BrushSurfaceActor)
	{
		return;
	}

	if (!BrushSurfaceActor->bSurfaceGridGenerationFinished)
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

void UManipulationComponent_SmoothBrush::TriggerStopManipulatingAbility()
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

void UManipulationComponent_SmoothBrush::TriggerTickManipulatingAbility()
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