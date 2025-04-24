
#pragma once

#include "NetVRkLandParcelLibrary.generated.h"


UCLASS()
class NETVRKLANDPARCELSYSTEM_API UNetVRkLandParcelLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "NetVRk Land Parcel Library")
	static bool IsInclusionActorWithinLandParcelBounds(AActor* IncludedActor, AActor* LandParcelBoundsActor);

	UFUNCTION(BlueprintCallable, Category = "NetVRk Land Parcel Library")
	static bool DoesInclusionBelongToLandParcel(AActor* IncludedActor, AActor* LandParcelBoundsActor);
	
};