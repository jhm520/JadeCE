//

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LandParcelOwnerInterface.generated.h"

UINTERFACE(BlueprintType)
class NETVRKLANDPARCELSYSTEM_API ULandParcelOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Actors & Objects that implement this interface must be an objective of some sort of game mode
 */
class NETVRKLANDPARCELSYSTEM_API ILandParcelOwnerInterface
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Land Parcel")
	FName GetLandParcelOwnerName() const;
};
