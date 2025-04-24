// Fill out your copyright notice in the Description page of Project Settings.


#include "LandParcelInclusionComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "NetVRkLandParcelSystem/Interfaces/LandParcelBoundInterface.h"
#include "NetVRkLandParcelSystem/Interfaces/LandParcelInclusionInterface.h"
#include "NetVRkLandParcelSystem/Libraries/NetVRkLandParcelLibrary.h"

#pragma region Framework
// Sets default values for this component's properties
ULandParcelInclusionComponent::ULandParcelInclusionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bIsValidPlacement = true;
	// ...
}


// Called when the game starts
void ULandParcelInclusionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void ULandParcelInclusionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void ULandParcelInclusionComponent::PostReinitProperties()
{
	Super::PostReinitProperties();
}

#pragma endregion

#pragma region Inclusion

void ULandParcelInclusionComponent::CheckValidPlacement()
{
	const bool bLocalIsValidPlacement = IsValidPlacement();

	if (bIsValidPlacement == bLocalIsValidPlacement)
	{
		return;
	}
	
	bIsValidPlacement = bLocalIsValidPlacement;

	OnInclusionPlacementStatusChanged.Broadcast(bLocalIsValidPlacement);
}

bool ULandParcelInclusionComponent::IsValidPlacement() const
{
	bool bHadLandParcel = false;
	
	for (AActor* LandParcel : DefaultLandParcels)
	{
		if (!LandParcel)
		{
			continue;
		}

		bHadLandParcel = true;
		
		const bool bIsWithinLandParcel = UNetVRkLandParcelLibrary::IsInclusionActorWithinLandParcelBounds(GetOwner(), LandParcel);

		if (bIsWithinLandParcel)
		{
			return true;
		}
	}

	TArray<AActor*> FoundLandParcels;

	const bool bFoundLandParcels = FindNearbyLandParcels(FoundLandParcels);

	if (!bFoundLandParcels)
	{
		const FName InclusionOwnerName = ILandParcelInclusionInterface::Execute_GetLandParcelInclusionOwnerName(GetOwner());

		const bool bIsOwnerless = InclusionOwnerName == NAME_None;
		
		return bIsOwnerless;
	}

	for (AActor* LandParcel : FoundLandParcels)
	{
		bHadLandParcel = true;
		
		const bool bBelongs = UNetVRkLandParcelLibrary::DoesInclusionBelongToLandParcel(GetOwner(), LandParcel);

		//if the inclusion does not belong to the nearby land parcel, then it is not a valid placement
		if (!bBelongs)
		{
			return false;
		}

		//if the inclusion is all the way within the land parcel that it belongs to, then it is a valid placement
		const bool bIsWithinLandParcel = UNetVRkLandParcelLibrary::IsInclusionActorWithinLandParcelBounds(GetOwner(), LandParcel);

		if (bIsWithinLandParcel)
		{
			return true;
		}
	}

	if (!bHadLandParcel)
	{
		return true;
	}
	
	return false;
}

#pragma endregion

#pragma region Land Parcel

bool ULandParcelInclusionComponent::FindNearbyLandParcels(TArray<AActor*>& OutLandParcels) const
{
	const bool bImplementsInterface = GetOwner()->Implements<ULandParcelInclusionInterface>();

	if (!bImplementsInterface)
	{
		return false;
	}

	FTransform BoundTransform;
	FVector BoundExtent;

	AActor* Owner = GetOwner();
	
	const bool bGotInclusionBounds = ILandParcelInclusionInterface::Execute_GetLandParcelInclusionBounds(Owner, BoundTransform, BoundExtent);

	if (!bGotInclusionBounds)
	{
		return false;
	}

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(GetOwner());

	TArray<FHitResult> Hits;
	
	const bool bGotBlockingHit = UKismetSystemLibrary::BoxTraceMulti(GetWorld(), BoundTransform.GetLocation(), BoundTransform.GetLocation(), BoundExtent, BoundTransform.GetRotation().Rotator(), LandParcelTraceType, false, IgnoredActors, EDrawDebugTrace::None, Hits, true);

	for (const FHitResult& Hit : Hits)
	{
		ILandParcelBoundInterface* LandParcelBoundInterface = Cast<ILandParcelBoundInterface>(Hit.GetActor());

		if (LandParcelBoundInterface)
		{
			OutLandParcels.Add(Hit.GetActor());
		}
	}
	
	return OutLandParcels.Num() > 0;
}

#pragma endregion