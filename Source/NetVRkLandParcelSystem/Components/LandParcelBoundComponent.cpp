// Fill out your copyright notice in the Description page of Project Settings.


#include "LandParcelBoundComponent.h"

#include "NetVRkCoreUtility/Libraries/NetVRkMathLibrary.h"
#include "NetVRkLandParcelSystem/Interfaces/LandParcelBoundInterface.h"
#include "NetVRkLandParcelSystem/Interfaces/LandParcelInclusionInterface.h"

#pragma region Framework
// Sets default values for this component's properties
ULandParcelBoundComponent::ULandParcelBoundComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULandParcelBoundComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void ULandParcelBoundComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void ULandParcelBoundComponent::PostReinitProperties()
{
	Super::PostReinitProperties();
}


#pragma endregion

#pragma region Bounds

bool ULandParcelBoundComponent::IsActorWithinLandParcelBounds(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}

	ILandParcelBoundInterface* LandParcelBoundInterface = Cast<ILandParcelBoundInterface>(GetOwner());

	if (!LandParcelBoundInterface)
	{
		return false;
	}
	
	ILandParcelInclusionInterface* LandParcelInclusionInterface = Cast<ILandParcelInclusionInterface>(Actor);

	if (!LandParcelInclusionInterface)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor %s does not implement ILandParcelInclusionInterface"), *Actor->GetName());
		return false;
	}

	FTransform InclusionBoundsTransform;
	FVector InclusionBoundsExtent;

	const bool bGotInclusionBounds = ILandParcelInclusionInterface::Execute_GetLandParcelInclusionBounds(Actor, InclusionBoundsTransform, InclusionBoundsExtent);

	FTransform LandParcelBoundsTransform;
	FVector LandParceBoundsExtent;

	const bool bGotLandParcelBounds = ILandParcelBoundInterface::Execute_GetLandParcelBounds(GetOwner(), LandParcelBoundsTransform, LandParceBoundsExtent);

	if (!bGotInclusionBounds || !bGotLandParcelBounds)
	{
		return false;
	}

	const bool bWithinBounds = UNetVRkMathLibrary::IsBoxWithinBoxWithTransform(InclusionBoundsTransform, InclusionBoundsExtent, LandParcelBoundsTransform, LandParceBoundsExtent);
	
	return bWithinBounds;
}
#pragma endregion