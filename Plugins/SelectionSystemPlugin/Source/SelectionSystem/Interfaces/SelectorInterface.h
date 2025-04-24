// © 2024 Jade Miller. All rights reserved
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SelectorInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USelectorInterface : public UInterface
{
	GENERATED_BODY()
};

class USelectorComponent;

/** This is an interface for actors that care about selecting
 * 
 */
class SELECTIONSYSTEM_API ISelectorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Selection")
	USelectorComponent* GetSelectorComponent() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Selection")
	bool CanSelect(const USelectableComponent* Selected) const;

	virtual bool CanSelect_Implementation(const USelectableComponent* Selected) const { return true; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Selection")
	bool CanDeselect(const USelectableComponent* Selected) const;

	virtual bool CanDeselect_Implementation(const USelectableComponent* Selected) const { return true; }

};
