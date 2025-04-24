// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectionCursorActor.h"

#include "SelectionSystem/SelectionSystemBlueprintLibrary.h"
#include "SelectionSystem/Components/SelectableComponent.h"
#include "SelectionSystem/Interfaces/SelectorInterface.h"

#pragma region Framework

// Sets default values
ASelectionCursorActor::ASelectionCursorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SelectableComponent = CreateDefaultSubobject<USelectableComponent>(TEXT("SelectableComponent"));
}

// Called when the game starts or when spawned
void ASelectionCursorActor::BeginPlay()
{
	Super::BeginPlay();

	SelectableComponent->OnDeselectedDelegate.AddDynamic(this, &ASelectionCursorActor::OnDeselected);

	OnCursorBeginPlay();
}

// Called every frame
void ASelectionCursorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
#pragma endregion

#pragma region Selectable

void ASelectionCursorActor::OnDeselected(AActor* Selector)
{
	if (!Selector)
	{
		return;
	}

	Destroy();
}

#pragma endregion

#pragma region Cursor
	
void ASelectionCursorActor::OnCursorBeginPlay()
{
	AActor* LocalOwner = GetOwner();

	TArray<USelectableComponent*> OutSelectedComponents;
	USelectionSystemBlueprintLibrary::GetSelectedComponents(LocalOwner, OutSelectedComponents);

	for (USelectableComponent* SelectedComponent : OutSelectedComponents)
	{
		if (!SelectedComponent)
		{
			continue;
		}
		
		AActor* SelectedOwner = SelectedComponent->GetOwner();

		if (!SelectedOwner)
		{
			continue;
		}

		AActor* SelectedCharacterOwner = SelectedOwner->GetOwner();
		
		if (SelectedCharacterOwner != LocalOwner)
		{
			continue;
		}
		
		if (!SelectedOwner->IsA<ASelectionCursorActor>())
		{
			continue;
		}
		
		USelectionSystemBlueprintLibrary::DeselectActor(LocalOwner, SelectedOwner, false);
	}
	
	USelectionSystemBlueprintLibrary::SelectActor(LocalOwner, this, false);
}

#pragma endregion