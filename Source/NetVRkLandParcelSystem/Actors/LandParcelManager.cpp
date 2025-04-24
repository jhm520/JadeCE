// Fill out your copyright notice in the Description page of Project Settings.


#include "LandParcelManager.h"
#include "NetVRkLandParcelSystem/Actors/LandParcelBoundsActor.h"
#include "GameFramework/GameStateBase.h"

#pragma region Framework

// Sets default values
ALandParcelManager::ALandParcelManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALandParcelManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALandParcelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#pragma endregion

#pragma region Land Parcel System

void ALandParcelManager::SpawnAllLandParcels()
{
	TArray<FNetVRkLegacyLandParcelPlacementInfo*> LandParcelPlacementInfoPtrs;;
	LandParcelDataTable->GetAllRows<FNetVRkLegacyLandParcelPlacementInfo>(FString(""), LandParcelPlacementInfoPtrs);

	for (FNetVRkLegacyLandParcelPlacementInfo* LandParcelPlacementInfoPtr : LandParcelPlacementInfoPtrs)
	{
		if (!LandParcelPlacementInfoPtr)
		{
			continue;
		}

		ALandParcelBoundsActor* NewLandParcel = SpawnLegacyLandParcel(*LandParcelPlacementInfoPtr);

		if (!NewLandParcel)
		{
			continue;
		}
		
		LandParcels.Add(NewLandParcel);
	}
}

void ALandParcelManager::DeleteAllLandParcels()
{
	for (ALandParcelBoundsActor* LandParcel : LandParcels)
	{
		if (!LandParcel)
		{
			continue;
		}

		LandParcel->Destroy();
	}

	LandParcels.Empty();
}

bool ALandParcelManager::FindLandParcelZPosition(const FVector& OriginalPosition, float& OutZPosition) const
{
	const FVector& OwnerLocation = OriginalPosition;

	const FVector& StartHeight = FVector(OwnerLocation.X, OwnerLocation.Y, FindLandParcelZPositionTestHeight);
	const FVector& EndHeight = FVector(OwnerLocation.X, OwnerLocation.Y, -FindLandParcelZPositionTestHeight);

	FCollisionQueryParams QueryParams = FCollisionQueryParams::DefaultQueryParam;
	QueryParams.AddIgnoredActor(GetOwner());
	FCollisionResponseParams ResponseParams = FCollisionResponseParams::DefaultResponseParam;
	
	FHitResult Hit;

	const bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(Hit, StartHeight, EndHeight, FindLandParcelZPositionTestChannel, QueryParams, ResponseParams);

	OutZPosition = Hit.Location.Z;
	
	return bBlockingHit;
}

ALandParcelBoundsActor* ALandParcelManager::SpawnLegacyLandParcel(const FNetVRkLegacyLandParcelPlacementInfo& LandParcelPlacementInfo)
{
	FVector SpawnLocation;
	SpawnLocation.X = (LandParcelPlacementInfo.x * LandParcelWorldPositionScale) + LandParcelWorldPositionOffset.X;
	SpawnLocation.Y = (LandParcelPlacementInfo.y * LandParcelWorldPositionScale) + LandParcelWorldPositionOffset.Y;

	float ZPosition;
	const bool bGotLand = FindLandParcelZPosition(SpawnLocation, ZPosition);

	if (bGotLand)
	{
		SpawnLocation.Z = ZPosition;
	}
	else
	{
		SpawnLocation.Z = LandParcelWorldPositionOffset.Z;
	}
	
	FRotator SpawnRotation;
	SpawnRotation.Yaw = LandParcelPlacementInfo.rot;
	
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(SpawnRotation.Quaternion());

	AGameStateBase* GameState = GetWorld()->GetGameState();

	ALandParcelBoundsActor* NewParcelActor = GetWorld()->SpawnActorDeferred<ALandParcelBoundsActor>(LandParcelActorClass, SpawnTransform, GameState, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!NewParcelActor)
	{
		return nullptr;
	}

	NewParcelActor->SetActorScale3D(FVector(LandParcelWorldPositionScale));

	NewParcelActor->FinishSpawning(SpawnTransform);

	return NewParcelActor;
}
	
#pragma endregion

