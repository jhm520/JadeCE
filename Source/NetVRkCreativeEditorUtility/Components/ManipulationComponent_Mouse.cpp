// Fill out your copyright notice in the Description page of Project Settings.


#include "ManipulationComponent_Mouse.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NetVRkCreativeEditorUtility/Libraries/NetVRkCreativeEditorBlueprintLibrary.h"

#pragma region Framework

UManipulationComponent_Mouse::UManipulationComponent_Mouse()
{
	bWantsOnUpdateTransform = true;
}

void UManipulationComponent_Mouse::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);
	
	if (bAbsoluteManipulation && bWantsOnUpdateTransform)
	{
		bWantsOnUpdateTransform = false;
		
		const FRotator& AbsoluteDirectionRotation = AbsoluteDirectionAxis.Rotation();
		SetWorldRotation(AbsoluteDirectionRotation);
		
		bWantsOnUpdateTransform = true;
	}
	else
	{
		SetRelativeRotation(FRotator(0,0,0));
	}
}

#pragma endregion

#pragma region Mouse Events

bool UManipulationComponent_Mouse::GetManipulatorMouseScreenWorldLocation(FVector& OutMouseWorldLocation, FVector& OutMouseWorldDirection) const
{
	APlayerController* PlayerController = GetManipulatorController();

	if (!PlayerController)
	{
		return false;
	}

	float MouseX = 0.0f;
	float MouseY = 0.0f;
	
	const bool bGotMousePosition = PlayerController->GetMousePosition(MouseX, MouseY);

	if (!bGotMousePosition)
	{
		return false;
	}
	
	const bool bGotMouseWorldLocation = PlayerController->DeprojectScreenPositionToWorld(MouseX, MouseY, OutMouseWorldLocation, OutMouseWorldDirection);

	if (!bGotMouseWorldLocation)
	{
		return false;
	}

	return true;
	
}

void UManipulationComponent_Mouse::SetManipulationAbsolute(bool bInAbsolute)
{
	Super::SetManipulationAbsolute(bInAbsolute);

	bAbsoluteManipulation = bInAbsolute;
	OnUpdateTransform(EUpdateTransformFlags::None, ETeleportType::None);
}

void UManipulationComponent_Mouse::OnManipulatorClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!LocalPlayerController)
	{
		return;
	}

	if (LocalPlayerController != GetManipulatorController())
	{
		return;
	}
	
	APawn* PlayerPawn = LocalPlayerController->GetPawn();
	
	UNetVRkCreativeEditorBlueprintLibrary::StartManipulatingActor(PlayerPawn, GetOwner(), this);
	
	OnManipulatorClickedDelegate.Broadcast(TouchedComponent, ButtonPressed);
}

void UManipulationComponent_Mouse::OnManipulatorReleased(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	OnManipulatorReleasedDelegate.Broadcast(TouchedComponent, ButtonPressed);
}

void UManipulationComponent_Mouse::OnManipulatorBeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	CachedOriginalColor = ManipulatorDynamicMaterialInstance->K2_GetVectorParameterValue(FName("Color"));

	if (ManipulatorDynamicMaterialInstance)
	{
		ManipulatorDynamicMaterialInstance->SetVectorParameterValue(FName("Color"), FLinearColor(HighlightColor));
	}
	OnManipulatorBeginCursorOverDelegate.Broadcast(TouchedComponent);
}

void UManipulationComponent_Mouse::OnManipulatorEndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	if (ManipulatorDynamicMaterialInstance)
	{
		ManipulatorDynamicMaterialInstance->SetVectorParameterValue(FName("Color"), FLinearColor(CachedOriginalColor));
	}
	
	OnMoveDirectionEndCursorOverDelegate.Broadcast(TouchedComponent);
}

#pragma endregion

#pragma region Manipulation
	
void UManipulationComponent_Mouse::TickManipulation(float DeltaTime)
{
	bool bLocationWasChanged = false;
	FVector NewLocation = FVector::ZeroVector;
	
	UpdateManipulationActorLocation(bLocationWasChanged, NewLocation);

	const bool bShouldTriggerTickManipulatingAbility = bLocationWasChanged;

	if (bShouldTriggerTickManipulatingAbility)
	{
		TriggerTickManipulatingAbility();
	}
}

void UManipulationComponent_Mouse::UpdateManipulationActorLocation(bool& bOutLocationWasChanged, FVector& OutNewLocation)
{
	
}
	
void UManipulationComponent_Mouse::OnIsManipulatingChanged()
{
	if (bIsManipulating)
	{
		APlayerController* ManipulatorController = GetManipulatorController();

		if (ManipulatorController)
		{
			ManipulatorController->SetShowMouseCursor(true);
			
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(ManipulatorController, nullptr, EMouseLockMode::DoNotLock, false);
		
			const bool bGotMouseLocations = GetManipulatorMouseScreenWorldLocation(CachedMouseWorldLocation_ManipulationStart, CachedMouseWorldDirection_ManipulationStart);

			CachedManipulatorWorldLocation_ManipulationStart = GetOwner()->GetActorLocation();
			CachedManipulatorWorldScale_ManipulationStart = GetOwner()->GetActorScale3D();
		}
	}
	else
	{
		APlayerController* ManipulatorController = GetManipulatorController();

		if (ManipulatorController)
		{
			//set the hide cursor back to true
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(ManipulatorController, nullptr, EMouseLockMode::DoNotLock, true);
		}

		CachedManipulatorWorldScale_ManipulationStart = FVector::ZeroVector;
		CachedMouseWorldLocation_ManipulationStart = FVector::ZeroVector;
		CachedMouseWorldDirection_ManipulationStart = FVector::ZeroVector;
	}
	
	Super::OnIsManipulatingChanged();
}

void UManipulationComponent_Mouse::TriggerTickManipulatingAbility()
{
	
}

#pragma endregion