// © 2024 Jade Miller. All rights reserved

#include "SelectorComponent.h"
#include "SelectableComponent.h"
#include "Net/UnrealNetwork.h"
#include "SelectionSystem/SelectionSystemBlueprintLibrary.h"
#include "SelectionSystem/Actors/SelectionCursorActor.h"
#include "SelectionSystem/Interfaces/SelectorInterface.h"

#pragma region Framework

// Sets default values for this component's properties
USelectorComponent::USelectorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	// ...
}


// Called when the game starts
void USelectorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void USelectorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearSelection(true);
	
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void USelectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USelectorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(USelectorComponent, SelectedComponents, COND_None, REPNOTIFY_Always);

}

#pragma endregion

#pragma region Selection

void USelectorComponent::Select(USelectableComponent* SelectableComponent, bool bRepToServer)
{
	if (!SelectableComponent)
	{
		return;
	}
	
	const bool bCanSelect = USelectionSystemBlueprintLibrary::CanSelectActor(GetOwner(), SelectableComponent->GetOwner());
	
	if (!bCanSelect)
	{
		return;
	}
	
	if (bRepToServer && !GetOwner()->HasAuthority())
	{
		ServerSelect(SelectableComponent);
	}

	const TArray<USelectableComponent*> OldSelectedComponents = SelectedComponents;

	if (OldSelectedComponents.Contains(SelectableComponent))
	{
		return;
	}
	
	SelectedComponents.AddUnique(SelectableComponent);
	
	OnRep_SelectedComponents(OldSelectedComponents);
}

void USelectorComponent::Deselect(USelectableComponent* SelectableComponent, bool bRepToServer)
{
	if (!SelectableComponent)
	{
		return;
	}

	const bool bCanDeselect = USelectionSystemBlueprintLibrary::CanDeselectActor(GetOwner(), SelectableComponent->GetOwner());
	
	if (!bCanDeselect)
	{
		return;
	}

	if (bRepToServer && !GetOwner()->HasAuthority())
	{
		ServerDeselect(SelectableComponent);
	}

	const TArray<USelectableComponent*> OldSelectedComponents = SelectedComponents;

	if (!OldSelectedComponents.Contains(SelectableComponent))
	{
		return;
	}

	SelectedComponents.Remove(SelectableComponent);
	
	OnRep_SelectedComponents(OldSelectedComponents);
}

void USelectorComponent::ClearSelection(bool bRepToServer)
{
	if (bRepToServer && !GetOwner()->HasAuthority())
	{
		ServerClearSelection();
	}
	
	const TArray<USelectableComponent*> OldSelectedComponents = SelectedComponents;

	SelectedComponents.Empty();
	
	OnRep_SelectedComponents(OldSelectedComponents);
}

bool USelectorComponent::HasAnySelected(const TSubclassOf<AActor>& OptionalSpecifiedClass) const
{
	if (!OptionalSpecifiedClass)
	{
		return SelectedComponents.Num() > 0;
	}
	
	for (USelectableComponent* SelectedComponent : SelectedComponents)
	{
		if (!SelectedComponent)
		{
			continue;
		}

		AActor* SelectedActor = SelectedComponent->GetOwner();

		if (!SelectedActor)
		{
			continue;
		}

		if (SelectedActor->IsA(OptionalSpecifiedClass))
		{
			return true;
		}
	}

	return false;
}

void USelectorComponent::OnRep_SelectedComponents(const TArray<USelectableComponent*>& OldSelectedComponents)
{
	CheckSelectionRules();
	
	for (USelectableComponent* OldSelectedComponent : OldSelectedComponents)
	{
		if (OldSelectedComponent && !SelectedComponents.Contains(OldSelectedComponent))
		{
			OldSelectedComponent->OnDeselected(GetOwner());
			OnDeselectDelegate.Broadcast(OldSelectedComponent);
		}
	}

	TArray<USelectableComponent*> LocalSelectedComponents = GetSelectedComponents();

	for (USelectableComponent* SelectedComponent : LocalSelectedComponents)
	{
		if (SelectedComponent && !OldSelectedComponents.Contains(SelectedComponent))
		{
			SelectedComponent->OnSelected(GetOwner());
			OnSelectDelegate.Broadcast(SelectedComponent);
		}
	}
}

void USelectorComponent::ServerSelect_Implementation(USelectableComponent* SelectableComponent)
{
	Select(SelectableComponent);
}

void USelectorComponent::ServerDeselect_Implementation(USelectableComponent* SelectableComponent)
{
	Deselect(SelectableComponent);
}

void USelectorComponent::ServerClearSelection_Implementation()
{
	ClearSelection();
}

ASelectionCursorActor* USelectorComponent::GetSelectionCursorActor() const
{
	TArray<USelectableComponent*> LocalSelectedComponents = GetSelectedComponents();

	for (USelectableComponent* SelectedComponent : LocalSelectedComponents)
	{
		AActor* SelectedOwner = SelectedComponent->GetOwner();

		if (!SelectedOwner)
		{
			continue;
		}

		ASelectionCursorActor* SelectionCursorActor = Cast<ASelectionCursorActor>(SelectedOwner);

		if (!SelectionCursorActor)
		{
			continue;
		}

		return SelectionCursorActor;
	}

	return nullptr;
}

bool USelectorComponent::CanSelect(USelectableComponent* SelectableComponent) const
{
	if (!SelectableComponent)
	{
		return false;
	}

	const bool bIsSelector = GetOwner()->Implements<USelectorInterface>();

	if (!bIsSelector)
	{
		return false;
	}

	const bool bCanSelect = ISelectorInterface::Execute_CanSelect(GetOwner(), SelectableComponent);

	const bool bIsSelectable = SelectableComponent->GetOwner()->Implements<USelectableInterface>();

	if (!bIsSelectable)
	{
		return false;
	}
	
	const bool bCanBeSelected = ISelectableInterface::Execute_CanSelect(SelectableComponent->GetOwner(), this);
	
	return bCanSelect && bCanBeSelected;
}

bool USelectorComponent::CanDeselect(USelectableComponent* SelectableComponent) const
{
	//this function always returns true by default because if anything is invalid we should be able to de-select it

	if (!SelectableComponent)
	{
		return true;
	}

	const bool bIsSelector = GetOwner()->Implements<USelectorInterface>();

	if (!bIsSelector)
	{
		return true;
	}

	const bool bCanSelect = ISelectorInterface::Execute_CanDeselect(GetOwner(), SelectableComponent);

	const bool bIsSelectable = SelectableComponent->GetOwner()->Implements<USelectableInterface>();

	if (!bIsSelectable)
	{
		return true;
	}
	
	const bool bCanBeSelected = ISelectableInterface::Execute_CanDeselect(SelectableComponent->GetOwner(), this);
	
	return bCanSelect && bCanBeSelected;
}

#pragma endregion

#pragma region Selection Rules

void USelectorComponent::CheckSelectionRules()
{
	for (const FSelectionRule& SelectionRule : SelectionRules)
	{
		CheckSelectionRule(SelectionRule);
	}
}

void USelectorComponent::CheckSelectionRule(const FSelectionRule& SelectionRule)
{
	TArray<USelectableComponent*> LocalSelectedComponents = GetSelectedComponents();

	TArray<USelectableComponent*> RuledComponents;

	//go through all the selected components and check if the rule applies to them
	for (USelectableComponent* SelectedComponent : LocalSelectedComponents)
	{
		if (!SelectedComponent)
		{
			continue;
		}

		AActor* SelectedActor = SelectedComponent->GetOwner();

		if (!SelectedActor)
		{
			continue;
		}

		const bool bRuleApplies = DoesRuleApplyToActor(SelectionRule, SelectedActor);

		if (!bRuleApplies)
		{
			continue;
		}

		RuledComponents.Add(SelectedComponent);
	}

	//if no components are selected that the rule applies to, return
	if (RuledComponents.Num() == 0)
	{
		return;
	}

	if (SelectionRule.MaxSelectionCount > 0 && RuledComponents.Num() > SelectionRule.MaxSelectionCount)
	{
		const int32 ExcessSelectionCount = RuledComponents.Num() - SelectionRule.MaxSelectionCount;

		for (int32 i = 0; i < ExcessSelectionCount; i++)
		{
			Deselect(RuledComponents[0], false);
		}
	}
}

bool USelectorComponent::DoesRuleApplyToActor(const FSelectionRule& SelectionRule, AActor* Actor) const
{
	for (TSubclassOf<AActor> RuleClass : SelectionRule.RuledSelectableClasses)
	{
		if (Actor && Actor->IsA(RuleClass))
		{
			return true;
		}
	}

	return false;
}

#pragma endregion