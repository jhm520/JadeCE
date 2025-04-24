// Fill out your copyright notice in the Description page of Project Settings.


#include "ManipulationComponent_ClickDrag.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BaseGizmos/GizmoMath.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NetVRkCreativeEditorUtility/NetVRkCreativeEditorGameplayTags.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorManipulableInterface.h"

#pragma region Framework

UManipulationComponent_ClickDrag::UManipulationComponent_ClickDrag()
{
	ClickDragSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ClickDragSphere"));
	ClickDragSphere->SetupAttachment(ManipulationRootComponent);
	//ClickDragSphere->SetUsingAbsoluteScale(true);


	ClickDragMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClickDragMesh"));
	ClickDragMesh->SetupAttachment(ClickDragSphere);
	ClickDragMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//ClickDragMesh->SetUsingAbsoluteScale(true);

	ManipulationTags.AddTag(NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Translation);

}


void UManipulationComponent_ClickDrag::BeginPlay()
{
	Super::BeginPlay();

	ClickDragSphere->OnClicked.AddDynamic(this, &UManipulationComponent_Mouse::OnManipulatorClicked);
	ClickDragSphere->OnReleased.AddDynamic(this, &UManipulationComponent_Mouse::OnManipulatorReleased);
	ClickDragSphere->OnBeginCursorOver.AddDynamic(this, &UManipulationComponent_Mouse::OnManipulatorBeginCursorOver);
	ClickDragSphere->OnEndCursorOver.AddDynamic(this, &UManipulationComponent_Mouse::OnManipulatorEndCursorOver);
	
	ClickDragMesh->SetMaterial(0, ManipulatorMaterial);
	ClickDragMesh->SetRenderCustomDepth(true);
	ManipulatorDynamicMaterialInstance = ClickDragMesh->CreateDynamicMaterialInstance(0, ManipulatorMaterial);

	ManipulatorDynamicMaterialInstance->SetVectorParameterValue(FName("Color"), MeshColor);
}


#pragma endregion

#pragma region Manipulation

void UManipulationComponent_ClickDrag::SetManipulationEnabled(bool bInEnabled)
{
	Super::SetManipulationEnabled(bInEnabled);
	
	ClickDragSphere->SetVisibility(bInEnabled);

	ClickDragMesh->SetVisibility(bInEnabled);

	ClickDragSphere->SetCollisionEnabled(bInEnabled ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);

}

void UManipulationComponent_ClickDrag::UpdateManipulationActorLocation(bool& bOutLocationWasChanged, FVector& OutNewLocation)
{
	APlayerController* PlayerController = GetManipulatorController();

	if (!PlayerController)
	{
		return;
	}

	AActor* ManipulatedActor = GetManipulatedActor();

	if (!ManipulatedActor)
	{
		return;
	}

	FVector NewLocation;
	bool bGotLocation = false;

	const bool bImplementsInterface = ManipulatedActor->Implements<UCreativeEditorManipulableInterface>();

	if (bImplementsInterface)
	{
		bGotLocation = ICreativeEditorManipulableInterface::Execute_GetManipulationLocation(ManipulatedActor, GetManipulatorController(), this, NewLocation);
	}

	if (!bGotLocation)
	{
		EManipulationClickDragType ClickDragType = EManipulationClickDragType::MouseCursor;
		bool bGotClickDragType = false;
		
		if (bImplementsInterface)
		{
			bGotClickDragType = ICreativeEditorManipulableInterface::Execute_GetManipulationClickDragType(ManipulatedActor, GetManipulatorController(), this, ClickDragType);
		}

		ClickDragType = bGotClickDragType ? ClickDragType : DefaultClickDragType;
		
		switch (ClickDragType)
		{
			case EManipulationClickDragType::MouseCursor:
			{
				FHitResult HitResult;
				PlayerController->GetHitResultUnderCursor(ClickDragTraceChannel, true, HitResult);

				NewLocation = HitResult.Location;
				break;
			}
			case EManipulationClickDragType::CameraPlane:
			{
				FHitResult HitResult;
				PlayerController->GetHitResultUnderCursor(ClickDragTraceChannel, true, HitResult);

				
				FVector MouseWorldLocation;
				FVector MouseWorldDirection;
				const bool bGotPos = GetManipulatorMouseScreenWorldLocation(MouseWorldLocation, MouseWorldDirection);

				const FVector& ProjectedLocation = FVector::PointPlaneProject(MouseWorldLocation, WorldLocation_ManipulationStart, MouseWorldDirection);

				NewLocation = ProjectedLocation;
				break;
			}
			case EManipulationClickDragType::GroundPlane:
			{
									
				FVector MouseWorldLocation;
				FVector MouseWorldDirection;
				const bool bGotPos = GetManipulatorMouseScreenWorldLocation(MouseWorldLocation, MouseWorldDirection);
				
				// FHitResult HitResult;
				// PlayerController->GetHitResultUnderCursor(ClickDragTraceChannel, true, HitResult);
				bool bIntersectsOut;
				FVector PlaneIntersectionPointOut;

				GizmoMath::RayPlaneIntersectionPoint(WorldLocation_ManipulationStart, GroundHitResult_ManipulationStart.ImpactNormal, MouseWorldLocation, MouseWorldDirection, bIntersectsOut, PlaneIntersectionPointOut);
			
				//const FVector& ProjectedLocation = FVector::PointPlaneProject(HitResult.Location, WorldLocation_ManipulationStart, GroundHitResult_ManipulationStart.ImpactNormal);

				NewLocation = PlaneIntersectionPointOut;
				break;
			}
			default:
			{
				FHitResult HitResult;
				PlayerController->GetHitResultUnderCursor(ClickDragTraceChannel, true, HitResult);

				NewLocation = HitResult.Location;
				break;
			}
		}
	}

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

void UManipulationComponent_ClickDrag::OnIsManipulatingChanged()
{
	if (bIsManipulating)
	{
		WorldLocation_ManipulationStart = GetOwner()->GetActorLocation();

		const bool bGotHit = GetWorld()->LineTraceSingleByChannel(GroundHitResult_ManipulationStart, WorldLocation_ManipulationStart, WorldLocation_ManipulationStart - FVector::UpVector * 1000, ClickDragTraceChannel);

		if (!bGotHit)
		{
			GroundHitResult_ManipulationStart = FHitResult();
			GroundHitResult_ManipulationStart.ImpactPoint = WorldLocation_ManipulationStart;
			GroundHitResult_ManipulationStart.ImpactNormal = FVector::UpVector;
			
		}
	}
	else
	{
		WorldLocation_ManipulationStart = FVector::ZeroVector;
	}
	
	Super::OnIsManipulatingChanged();
}

void UManipulationComponent_ClickDrag::TriggerStartManipulatingAbility()
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

void UManipulationComponent_ClickDrag::TriggerStopManipulatingAbility()
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


#pragma endregion