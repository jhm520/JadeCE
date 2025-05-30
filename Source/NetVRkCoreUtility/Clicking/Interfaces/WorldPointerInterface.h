#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NetVRkCoreUtility/Clicking/Components/WorldPointerComponent.h"
#include "WorldPointerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWorldPointerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NETVRKCOREUTILITY_API IWorldPointerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "WorldPointer")
	UWorldPointerComponent* GetWorldPointerComponent() const;
	
};
