// Fill out your copyright notice in the Description page of Project Settings.


#include "FellOffWorldComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UFellOffWorldComponent::UFellOffWorldComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFellOffWorldComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bDelayEnableGravity)
	{
		ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

		if (!OwnerCharacter)
		{
			return;
		}

		OwnerCharacter->GetCharacterMovement()->GravityScale = 0.0f;

		// ...
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_EnableGravity, this, &UFellOffWorldComponent::Timer_EnableGravity, EnableGravityDelay, false);
	}
	
}

void UFellOffWorldComponent::Timer_EnableGravity()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

	if (!OwnerCharacter)
	{
		return;
	}

	OwnerCharacter->GetCharacterMovement()->GravityScale = 1.0f;
}

// Called every frame
void UFellOffWorldComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector TerrainLocation;
	const bool bFellOffWorld = CheckFellOffWorld(DeltaTime, TerrainLocation);
	
	if (bFellOffWorld)
	{
		OnFellOffWorld(TerrainLocation);
	}

	// ...
}

bool UFellOffWorldComponent::CheckFellOffWorld(float DeltaTime, FVector& OutHitLocation)
{
	//check below the player for land
	const FVector& OwnerLocation = GetOwner()->GetActorLocation();
	
	const FVector& BelowEndHeight = FVector(OwnerLocation.X, OwnerLocation.Y, OwnerLocation.Z-CheckBelowPlayer);
	const FVector& BelowStartHeight = FVector(OwnerLocation.X, OwnerLocation.Y, OwnerLocation.Z);

	FCollisionQueryParams QueryParams = FCollisionQueryParams::DefaultQueryParam;
	QueryParams.AddIgnoredActor(GetOwner());
	FCollisionResponseParams ResponseParams = FCollisionResponseParams::DefaultResponseParam;
	
	FHitResult Hit;

	bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(Hit, BelowStartHeight, BelowEndHeight, CheckFellOffWorldChannel, QueryParams, ResponseParams);

	//we still have land below us
	if (bBlockingHit)
	{
		return false;
	}
	
	const FVector& StartHeight = FVector(OwnerLocation.X, OwnerLocation.Y, CheckFellOffWorldHeight);
	const FVector& EndHeight = FVector(OwnerLocation.X, OwnerLocation.Y, OwnerLocation.Z);

	QueryParams.AddIgnoredActor(GetOwner());
	
	bBlockingHit = GetWorld()->LineTraceSingleByChannel(Hit, StartHeight, EndHeight, CheckFellOffWorldChannel, QueryParams, ResponseParams);

	OutHitLocation = Hit.Location;
	return bBlockingHit;
}

void UFellOffWorldComponent::OnFellOffWorld(const FVector& OutHitLocation)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

	if (!OwnerCharacter)
	{
		return;
	}

	UCapsuleComponent* CapsuleComponent = OwnerCharacter->GetCapsuleComponent();

	if (!CapsuleComponent)
	{
		return;
	}

	const FVector& AdjustedLocation = OutHitLocation + FVector(0.0f, 0.0f, CapsuleComponent->GetScaledCapsuleHalfHeight()) + FVector(0,0,1000.0f);

	GetOwner()->SetActorLocation(AdjustedLocation);
	
}