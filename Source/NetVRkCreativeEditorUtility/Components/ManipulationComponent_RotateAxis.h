// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ManipulationComponent_Mouse.h"
#include "ManipulationComponent_RotateAxis.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETVRKCREATIVEEDITORUTILITY_API UManipulationComponent_RotateAxis : public UManipulationComponent_Mouse
{
	GENERATED_BODY()
#pragma region Framework
public:
	UManipulationComponent_RotateAxis();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion

#pragma region Rotate Axis Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move Direction")
	UBoxComponent* RotateAxisBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move Direction")
	UStaticMeshComponent* RotateAxisMesh;

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

	virtual void SetManipulationEnabled(bool bInEnabled) override;

	UPROPERTY()
	FQuat CachedManipulatorWorldQuat_ManipulationStart;

	UPROPERTY()
	FTransform CachedComponentWorldTransform_ManipulationStart;

	UPROPERTY()
	FTransform CachedManipulatorWorldTransform_ManipulationStart;
	
	UPROPERTY()
	FVector CachedAxisDirectionX_ManipulationStart;
	
	UPROPERTY()
	FVector CachedAxisDirectionY_ManipulationStart;
	
	UPROPERTY()
	FVector CachedAxisDirectionZ_ManipulationStart;

	UPROPERTY()
	FVector RotationAxisDirection;
	
	virtual void TickManipulation(float DeltaTime) override;

	virtual void UpdateManipulationActorLocation(bool& bOutLocationWasChanged, FVector& OutNewLocation) override;
	
	virtual void OnIsManipulatingChanged() override;

	void OnStartedManipulating();

	void OnStoppedManipulating();

	virtual void TriggerStartManipulatingAbility() override;

	virtual void TriggerStopManipulatingAbility() override;
	
	virtual void TriggerTickManipulatingAbility() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotate Axis")
	float RotationSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotate Axis")
	float RotationSnapAngle = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotate Axis")
	bool bSnapAngleEnabled = true;

	virtual void SetSnapEnabled(bool bInSnapEnabled) override { bSnapAngleEnabled = bInSnapEnabled; }

	virtual void SetSnapIncrement(float InSnapIncrement) override { RotationSnapAngle = InSnapIncrement; }

#pragma endregion

};
