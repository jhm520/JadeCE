// © 2024 Jade Miller. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SelectionSystemBlueprintLibrary.generated.h"

class USelectableComponent;
/**
 * 
 */
UCLASS()
class SELECTIONSYSTEM_API USelectionSystemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Selection")
	static USelectorComponent* GetSelectorComponent(AActor* Selector);
	
	UFUNCTION(BlueprintCallable, Category = "Selection")
	static void SelectActor(AActor* Selector, const AActor* SelectedActor, bool bRepToServer = true);

	UFUNCTION(BlueprintCallable, Category = "Selection")
	static bool CanSelectActor(AActor* Selector, const AActor* SelectedActor);

	UFUNCTION(BlueprintCallable, Category = "Selection")
	static bool CanDeselectActor(AActor* Selector, const AActor* SelectedActor);
	
	UFUNCTION(BlueprintCallable, Category = "Selection")
	static void SelectActors(AActor* Selector, const TArray<AActor*> SelectedActors, bool bRepToServer = true);

	UFUNCTION(BlueprintCallable, Category = "Selection")
	static void DeselectActor(AActor* Selector, const AActor* SelectedActor, bool bRepToServer = true);
	
	UFUNCTION(BlueprintCallable, Category = "Selection")
	static void ClearSelection(AActor* Selector, bool bRepToServer = true);

	UFUNCTION(BlueprintPure, Category = "Selection")
	static void GetSelectedComponents(const AActor* Selector, TArray<USelectableComponent*>& OutSelectedComponents);

	UFUNCTION(BlueprintPure, Category = "Selection")
	static void GetSelectedActors(AActor* Selector, TArray<AActor*>& SelectedActors);

	UFUNCTION(BlueprintPure, Category = "Selection")
	static void GetSelectedActorsByClass(AActor* Selector, TSubclassOf<AActor> ActorClass, TArray<AActor*>& SelectedActors);

	UFUNCTION(BlueprintPure, Category = "Selection")
	static bool IsActorSelected(AActor* Selector, AActor* SelectableActor);
	
};
