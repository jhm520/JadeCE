//

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LandParcelInclusionInterface.generated.h"

UINTERFACE(BlueprintType)
class NETVRKLANDPARCELSYSTEM_API ULandParcelInclusionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Actors & Objects that implement this interface must be an objective of some sort of game mode
 */
class NETVRKLANDPARCELSYSTEM_API ILandParcelInclusionInterface
{
	GENERATED_BODY()
public:
	// Getter function for the InventorySystemComponent variable
	// Getter function for the ObjectiveAssigneeComponent variable
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Land Parcel")
	ULandParcelInclusionComponent* GetLandParcelInclusionComponent() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Land Parcel")
	bool GetLandParcelInclusionBounds(FTransform& OutBoundsTransform, FVector& OutBoundsExtent) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Land Parcel")
	FName GetLandParcelInclusionOwnerName() const;
};
