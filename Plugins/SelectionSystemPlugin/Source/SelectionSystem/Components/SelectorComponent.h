// © 2024 Jade Miller. All rights reserved
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SelectorComponent.generated.h"

class USelectableComponent;

//a struct that contains rules regarding selecting certain classes of actors
USTRUCT(BlueprintType)
struct SELECTIONSYSTEM_API FSelectionRule
{
	GENERATED_BODY()

	//the list of selectable classes that this rule applies to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selection", meta = (MustImplement = "SelectableInterface"))
	TArray<TSubclassOf<AActor>> RuledSelectableClasses;

	//how many selectables of these classes can be selected by this selector, 0 means no limit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selection")
	int32 MaxSelectionCount = 0;
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectDelegate, USelectableComponent*, Selection);

//this is a component that is meant to be added to actors that care about selecting things

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SELECTIONSYSTEM_API USelectorComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region Framework

public:	
	// Sets default values for this component's properties
	USelectorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion

#pragma region Selection
protected:

	UPROPERTY(Transient, ReplicatedUsing="OnRep_SelectedComponents", BlueprintReadOnly, Category = "Selection")
	TArray<USelectableComponent*> SelectedComponents;

	UFUNCTION()
	void OnRep_SelectedComponents(const TArray<USelectableComponent*>& OldSelectedComponents);

	UFUNCTION(Server, Reliable)
	void ServerSelect(USelectableComponent* SelectableComponent);

	UFUNCTION(Server, Reliable)
	void ServerDeselect(USelectableComponent* SelectableComponent);

	UFUNCTION(Server, Reliable)
	void ServerClearSelection();
	
public:

	UFUNCTION(BlueprintCallable, Category = "Selection")
	void Select(USelectableComponent* SelectableComponent, bool bRepToServer = true);

	UFUNCTION(BlueprintCallable, Category = "Selection")
	void Deselect(USelectableComponent* SelectableComponent, bool bRepToServer = true);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Selection")
	void ClearSelection(bool bRepToServer = true);

	UFUNCTION(BlueprintPure, Category = "Selection")
	bool HasAnySelected(const TSubclassOf<AActor>& OptionalSpecifiedClass = nullptr) const;

	UFUNCTION(BlueprintPure, Category = "Selection")
	TArray<USelectableComponent*> GetSelectedComponents() const {return SelectedComponents;};

	UFUNCTION(BlueprintPure, Category = "Selection")
	ASelectionCursorActor* GetSelectionCursorActor() const;

	UFUNCTION(BlueprintCallable, Category = "Selection")
	bool CanSelect(USelectableComponent* SelectableComponent) const;

	UFUNCTION(BlueprintCallable, Category = "Selection")
	bool CanDeselect(USelectableComponent* SelectableComponent) const;

	UPROPERTY(BlueprintAssignable)
	FOnSelectDelegate OnSelectDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnSelectDelegate OnDeselectDelegate;
	
#pragma endregion

#pragma region Selection Rules

	void CheckSelectionRules();

	void CheckSelectionRule(const FSelectionRule& SelectionRule);

	bool DoesRuleApplyToActor(const FSelectionRule& SelectionRule, AActor* Actor) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection")
	TArray<FSelectionRule> SelectionRules;
	
#pragma endregion

};
