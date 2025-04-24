// Fill out your copyright notice in the Description page of Project Settings.


#include "MousePointerComponent.h"

#include "GameFramework/PlayerController.h"

#pragma region Framework

// Sets default values for this component's properties
UMousePointerComponent::UMousePointerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMousePointerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMousePointerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
#pragma endregion


#pragma region Controller
APlayerController* UMousePointerComponent::GetController()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());

	if (PlayerController)
	{
		return PlayerController;
	}

	APawn* Pawn = Cast<APawn>(GetOwner());

	if (Pawn)
	{
		PlayerController = Cast<APlayerController>(Pawn->GetController());

		return PlayerController;
	}

	return nullptr;
}
#pragma endregion

#pragma region Mouse Pointer

bool UMousePointerComponent::GetHitResultUnderCursor(FHitResult& OutHitResult, TEnumAsByte<ETraceTypeQuery> OverrideTraceChannel, bool bOverrideTraceChannel)
{
	APlayerController* PlayerController = GetController();

	if (!PlayerController)
	{
		return false;
	}

	const TEnumAsByte<ETraceTypeQuery> LocalTraceChannel = bOverrideTraceChannel ? OverrideTraceChannel : CursorTraceChannel;

	const bool bGotHit = PlayerController->GetHitResultUnderCursorByChannel(LocalTraceChannel, true, OutHitResult);
	
	if (MaxCursorDistance == 0.0f)
	{
		return bGotHit;
	}

	FVector MouseLocation;
	FVector MouseDirection;
	PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);

	const FVector& Distance = OutHitResult.ImpactPoint - MouseLocation;

	if (Distance.Size() > MaxCursorDistance)
	{
		return false;
	}

	return true;
	
}
	
#pragma endregion
