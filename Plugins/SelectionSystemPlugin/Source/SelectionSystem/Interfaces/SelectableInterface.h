// © 2024 Jade Miller. All rights reserved
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SelectableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USelectableInterface : public UInterface
{
	GENERATED_BODY()
};

class USelectableComponent;
/** This is an interface for actors that are selectable
 * 
 */
class SELECTIONSYSTEM_API ISelectableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Selection")
	USelectableComponent* GetSelectableComponent() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Selection")
	bool CanSelect(const USelectorComponent* SelectorComponent) const;

	virtual bool CanSelect_Implementation(const USelectorComponent* SelectorComponent) const { return true; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Selection")
	bool CanDeselect(const USelectorComponent* SelectorComponent) const;

	virtual bool CanDeselect_Implementation(const USelectorComponent* SelectorComponent) const { return true; }

};
