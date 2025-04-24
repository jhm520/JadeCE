// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorManipulationActorInterface.h"
#include "SelectionSystem/Interfaces/SelectableInterface.h"
#include "NetVRkCEManipulationGizmo.generated.h"

UCLASS()
class NETVRKP4VOXEL_API ANetVRkCEManipulationGizmo : public AActor, public ICreativeEditorManipulationActorInterface, public ISelectableInterface
{
	GENERATED_BODY()
	
#pragma region Framework
public:	
	// Sets default values for this actor's properties
	ANetVRkCEManipulationGizmo();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
#pragma endregion

#pragma region Scaling

	UPROPERTY(EditDefaultsOnly, Category = "NetVRk|Creative Editor")
	USceneComponent* DefaultRootComponent;

	UPROPERTY(EditDefaultsOnly, Category = "NetVRk|Creative Editor")
	USceneComponent* ScalingRootComponent;
	
	void UpdateScreenSize();

	UPROPERTY(EditDefaultsOnly, Category = "NetVRk|Creative Editor")
	float CameraDistanceScalar;

#pragma endregion

#pragma region Creative Editor
protected:
	UPROPERTY(EditDefaultsOnly, Category = "NetVRk|Creative Editor")
	UCreativeEditorManipulationActorComponent* ManipulationActorComponent;
public:
	virtual void OnManipulationModeChanged_Implementation(APlayerController* ManipulationController, FGameplayTag ManipulationModeTag) override;

	virtual UCreativeEditorManipulationActorComponent* GetManipulationActorComponent_Implementation() const override { return ManipulationActorComponent; }
#pragma endregion

#pragma region Selection
public:

	virtual USelectableComponent* GetSelectableComponent_Implementation() const override { return SelectableComponent; }
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "NetVRk|Creative Editor")
	USelectableComponent* SelectableComponent;

	UFUNCTION()
	void OnDeselected(AActor* Selector);

#pragma endregion

};
