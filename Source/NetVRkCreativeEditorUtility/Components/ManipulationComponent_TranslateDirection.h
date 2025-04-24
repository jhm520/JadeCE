// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ManipulationComponent_Mouse.h"
#include "ManipulationComponent_TranslateDirection.generated.h"

class UCapsuleComponent;
class UArrowComponent;
class UPrimitiveComponent;

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETVRKCREATIVEEDITORUTILITY_API UManipulationComponent_TranslateDirection : public UManipulationComponent_Mouse
{
	GENERATED_BODY()
#pragma region Framework
public:
	UManipulationComponent_TranslateDirection();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport = ETeleportType::None) override;

#pragma endregion

#pragma region Move Direction Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move Direction")
	UCapsuleComponent* MoveDirectionCapsule;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move Direction")
	UStaticMeshComponent* MoveDirectionMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move Direction")
	UArrowComponent* MoveDirectionArrow;
#pragma endregion


#pragma region Mouse Events
protected:

	virtual void OnManipulatorClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed) override;

	virtual void OnManipulatorReleased(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed) override;

	virtual void OnManipulatorBeginCursorOver(UPrimitiveComponent* TouchedComponent) override;

	virtual void OnManipulatorEndCursorOver(UPrimitiveComponent* TouchedComponent) override;

public:


#pragma endregion

#pragma region Manipulation

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manipulation")
	bool bSnapEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manipulation")
	float SnapDistance = 10.0f;
	
	virtual void SetManipulationEnabled(bool bInEnabled) override;

	virtual void TickManipulation(float DeltaTime) override;

	virtual void UpdateManipulationActorLocation(bool& bOutLocationWasChanged, FVector& OutNewLocation) override;
	
	virtual void OnIsManipulatingChanged() override;

	virtual void TriggerStartManipulatingAbility() override;

	virtual void TriggerStopManipulatingAbility() override;
	
	virtual void TriggerTickManipulatingAbility() override;

	virtual void SetSnapEnabled(bool bInSnapEnabled) override {bSnapEnabled = bInSnapEnabled;}
	
	virtual void SetSnapIncrement(float InSnapIncrement) override { SnapDistance = InSnapIncrement; }


#pragma endregion

};
