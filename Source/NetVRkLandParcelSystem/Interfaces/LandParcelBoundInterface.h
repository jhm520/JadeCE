//

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LandParcelBoundInterface.generated.h"

UINTERFACE(BlueprintType)
class NETVRKLANDPARCELSYSTEM_API ULandParcelBoundInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Actors & Objects that implement this interface must be an objective of some sort of game mode
 */
class NETVRKLANDPARCELSYSTEM_API ILandParcelBoundInterface
{
	GENERATED_BODY()
public:
	// Getter function for the InventorySystemComponent variable
	// Getter function for the ObjectiveAssigneeComponent variable
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Land Parcel")
	ULandParcelBoundComponent* GetLandParcelBoundComponent() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Land Parcel")
	bool IsActorWithinLandParcelBounds(AActor* Actor) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Land Parcel")
	bool GetLandParcelBounds(FTransform& OutBoundsTransform, FVector& OutBoundsExtent) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Land Parcel")
	FName GetLandParcelOwnerName() const;
};
