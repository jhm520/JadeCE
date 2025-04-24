// © 2024 Jade Miller. All rights reserved

#include "SelectionSystemBlueprintLibrary.h"
#include "Interfaces/SelectableInterface.h"
#include "Interfaces/SelectorInterface.h"
#include "SelectionSystem/Components/SelectableComponent.h"
#include "SelectionSystem/Components/SelectorComponent.h"


USelectorComponent* USelectionSystemBlueprintLibrary::GetSelectorComponent(AActor* Selector)
{
	if (!Selector)
	{
		return nullptr;
	}

	const bool bIsSelector = Selector->Implements<USelectorInterface>();
	
	if (!bIsSelector)
	{
		return nullptr;
	}

	return ISelectorInterface::Execute_GetSelectorComponent(Selector);
}

void USelectionSystemBlueprintLibrary::SelectActor(AActor* Selector, const AActor* SelectedActor, bool bRepToServer)
{
	if (!Selector || !SelectedActor)
	{
		return;
	}

	const bool bIsSelector = Selector->Implements<USelectorInterface>();

	if (!bIsSelector)
	{
		return;
	}

	USelectorComponent* SelectorComponent = ISelectorInterface::Execute_GetSelectorComponent(Selector);

	if (!SelectorComponent)
	{
		return;
	}

	const bool bIsSelectable = SelectedActor->Implements<USelectableInterface>();

	if (!bIsSelectable)
	{
		return;
	}

	USelectableComponent* SelectableComponent = ISelectableInterface::Execute_GetSelectableComponent(SelectedActor);

	if (!SelectableComponent)
	{
		return;
	}
	
	//tell the selector component to select this character
	SelectorComponent->Select(SelectableComponent, bRepToServer);
}

bool USelectionSystemBlueprintLibrary::CanSelectActor(AActor* Selector, const AActor* SelectedActor)
{
	if (!Selector || !SelectedActor)
	{
		return false;
	}

	const bool bIsSelector = Selector->Implements<USelectorInterface>();

	if (!bIsSelector)
	{
		return false;
	}

	USelectorComponent* SelectorComponent = ISelectorInterface::Execute_GetSelectorComponent(Selector);

	if (!SelectorComponent)
	{
		return false;
	}

	const bool bIsSelectable = SelectedActor->Implements<USelectableInterface>();

	if (!bIsSelectable)
	{
		return false;
	}

	USelectableComponent* SelectableComponent = ISelectableInterface::Execute_GetSelectableComponent(SelectedActor);

	if (!SelectableComponent)
	{
		return false;
	}

	const bool bCanSelect = SelectorComponent->CanSelect(SelectableComponent);

	return bCanSelect;
}

bool USelectionSystemBlueprintLibrary::CanDeselectActor(AActor* Selector, const AActor* SelectedActor)
{
	//this function always returns true by default because if anything is invalid we should be able to de-select it
	if (!Selector || !SelectedActor)
	{
		return true;
	}

	const bool bIsSelector = Selector->Implements<USelectorInterface>();

	if (!bIsSelector)
	{
		return true;
	}

	USelectorComponent* SelectorComponent = ISelectorInterface::Execute_GetSelectorComponent(Selector);

	if (!SelectorComponent)
	{
		return true;
	}

	const bool bIsSelectable = SelectedActor->Implements<USelectableInterface>();

	if (!bIsSelectable)
	{
		return true;
	}

	USelectableComponent* SelectableComponent = ISelectableInterface::Execute_GetSelectableComponent(SelectedActor);

	if (!SelectableComponent)
	{
		return true;
	}

	const bool bCanSelect = SelectorComponent->CanDeselect(SelectableComponent);

	return bCanSelect;
}

void USelectionSystemBlueprintLibrary::SelectActors(AActor* Selector, const TArray<AActor*> SelectedActors, bool bRepToServer)
{
	for (AActor* SelectedActor : SelectedActors)
	{
		SelectActor(Selector, SelectedActor, bRepToServer);
	}
}

void USelectionSystemBlueprintLibrary::DeselectActor(AActor* Selector, const AActor* SelectedActor, bool bRepToServer)
{
	if (!Selector || !SelectedActor)
	{
		return;
	}

	const bool bIsSelector = Selector->Implements<USelectorInterface>();
	
	if (!bIsSelector)
	{
		return;
	}

	USelectorComponent* SelectorComponent = ISelectorInterface::Execute_GetSelectorComponent(Selector);

	if (!SelectorComponent)
	{
		return;
	}

	const bool bIsSelectable = SelectedActor->Implements<USelectableInterface>();
	
	if (!bIsSelectable)
	{
		return;
	}

	USelectableComponent* SelectableComponent = ISelectableInterface::Execute_GetSelectableComponent(SelectedActor);

	if (!SelectableComponent)
	{
		return;
	}
	
	//tell the selector component to select this character
	SelectorComponent->Deselect(SelectableComponent, bRepToServer);
}

void USelectionSystemBlueprintLibrary::ClearSelection(AActor* Selector, bool bRepToServer)
{
	if (!Selector)
	{
		return;
	}

	const bool bIsSelector = Selector->Implements<USelectorInterface>();
	
	if (!bIsSelector)
	{
		return;
	}

	USelectorComponent* SelectorComponent = ISelectorInterface::Execute_GetSelectorComponent(Selector);

	if (!SelectorComponent)
	{
		return;
	}
	
	//tell the selector component to select this character
	SelectorComponent->ClearSelection(bRepToServer);
}

void USelectionSystemBlueprintLibrary::GetSelectedComponents(const AActor* Selector, TArray<USelectableComponent*>& OutSelectedComponents)
{
	if (!Selector)
	{
		return;
	}
	
	const bool bIsSelector = Selector->Implements<USelectorInterface>();

	if (!bIsSelector)
	{
		return;
	}
	
	USelectorComponent* SelectorComponent = ISelectorInterface::Execute_GetSelectorComponent(Selector);

	if (!SelectorComponent)
	{
		return;
	}

	//Get the selected units
	OutSelectedComponents = SelectorComponent->GetSelectedComponents();
}

void USelectionSystemBlueprintLibrary::GetSelectedActors(AActor* Selector, TArray<AActor*>& SelectedActors)
{
	TArray<USelectableComponent*> OutSelectedComponents;
	GetSelectedComponents(Selector, OutSelectedComponents);

	for (USelectableComponent* SelectableComponent : OutSelectedComponents)
	{
		if (!SelectableComponent)
		{
			continue;
		}
		
		AActor* SelectedActor = SelectableComponent->GetOwner();

		if (SelectedActor)
		{
			SelectedActors.Add(SelectedActor);
		}
	}
}

void USelectionSystemBlueprintLibrary::GetSelectedActorsByClass(AActor* Selector, TSubclassOf<AActor> ActorClass, TArray<AActor*>& SelectedActors)
{
	TArray<USelectableComponent*> OutSelectedComponents;
	GetSelectedComponents(Selector, OutSelectedComponents);

	for (USelectableComponent* SelectableComponent : OutSelectedComponents)
	{
		if (!SelectableComponent)
		{
			continue;
		}
		
		AActor* SelectedActor = SelectableComponent->GetOwner();

		if (SelectedActor->IsA(ActorClass))
		{
			SelectedActors.Add(SelectedActor);
		}
	}
}

bool USelectionSystemBlueprintLibrary::IsActorSelected(AActor* Selector, AActor* SelectableActor)
{
	if (!Selector || !SelectableActor)
	{
		return false;
	}
	
	TArray<AActor*> SelectedActors;

	GetSelectedActors(Selector, SelectedActors);

	if (SelectedActors.Contains(SelectableActor))
	{
		return true;
	}

	return false;
}
