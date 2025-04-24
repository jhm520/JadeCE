#include "NetVRkLandParcelLibrary.h"

#include "NetVRkLandParcelSystem/Components/LandParcelBoundComponent.h"
#include "NetVRkLandParcelSystem/Components/LandParcelInclusionComponent.h"
#include "NetVRkLandParcelSystem/Interfaces/LandParcelBoundInterface.h"
#include "NetVRkLandParcelSystem/Interfaces/LandParcelInclusionInterface.h"

bool UNetVRkLandParcelLibrary::IsInclusionActorWithinLandParcelBounds(AActor* IncludedActor, AActor* LandParcelBoundsActor)
{
	const bool bIsLandParcelBound = LandParcelBoundsActor->Implements<ULandParcelBoundInterface>();
	
	if (!bIsLandParcelBound)
	{
		return false;
	}

	const bool bIsLandParcelInclusion = IncludedActor->Implements<ULandParcelInclusionInterface>();

	if (!bIsLandParcelInclusion)
	{
		return false;
	}

	ULandParcelBoundComponent* LandParcelBoundComponent = ILandParcelBoundInterface::Execute_GetLandParcelBoundComponent(LandParcelBoundsActor);

	if (!LandParcelBoundComponent)
	{
		return false;
	}

	ULandParcelInclusionComponent* LandParcelInclusionComponent = ILandParcelInclusionInterface::Execute_GetLandParcelInclusionComponent(IncludedActor);

	if (!LandParcelInclusionComponent)
	{
		return false;
	}

	return LandParcelBoundComponent->IsActorWithinLandParcelBounds(IncludedActor);
}

bool UNetVRkLandParcelLibrary::DoesInclusionBelongToLandParcel(AActor* IncludedActor, AActor* LandParcelBoundsActor)
{
	ILandParcelBoundInterface* LandParcelBoundInterface = Cast<ILandParcelBoundInterface>(LandParcelBoundsActor);

	if (!LandParcelBoundInterface)
	{
		return false;
	}

	ILandParcelInclusionInterface* LandParcelInclusionInterface = Cast<ILandParcelInclusionInterface>(IncludedActor);

	if (!LandParcelInclusionInterface)
	{
		return false;
	}

	const FName InclusionOwnerName = ILandParcelInclusionInterface::Execute_GetLandParcelInclusionOwnerName(IncludedActor);

	const FName BoundsOwnerName = ILandParcelBoundInterface::Execute_GetLandParcelOwnerName(LandParcelBoundsActor);

	return InclusionOwnerName == BoundsOwnerName;
}

