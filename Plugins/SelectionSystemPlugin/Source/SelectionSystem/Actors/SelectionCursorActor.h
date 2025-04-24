// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SelectionSystem/Interfaces/SelectableInterface.h"
#include "SelectionCursorActor.generated.h"

class USelectableComponent;

UCLASS()
class SELECTIONSYSTEM_API ASelectionCursorActor : public AActor, public ISelectableInterface
{
	GENERATED_BODY()
#pragma region Framework
public:	
	// Sets default values for this actor's properties
	ASelectionCursorActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
#pragma endregion

#pragma region Selectable

public:
	virtual USelectableComponent* GetSelectableComponent_Implementation() const override {return SelectableComponent;}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection")
	USelectableComponent* SelectableComponent;

	UFUNCTION()
	void OnDeselected(AActor* Selector);
#pragma endregion

#pragma region Cursor
	
	virtual void OnCursorBeginPlay();

#pragma endregion

};
