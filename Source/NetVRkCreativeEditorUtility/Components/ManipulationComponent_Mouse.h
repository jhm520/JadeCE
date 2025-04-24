// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ManipulationComponent.h"
#include "ManipulationComponent_Mouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnManipulatorClickedDelegate, UPrimitiveComponent*, TouchedComponent, FKey, ButtonPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManipulatorCursorOverDelegate, UPrimitiveComponent*, TouchedComponent);

UENUM(BlueprintType)
enum class EManipulationClickDragType : uint8
{
	// Click drag will move the actor to the mouse cursor hit location
	MouseCursor,
	// click drag will move the actor to the mouse cursor hit location, but constrain the movement to the camera direction plane
	CameraPlane,
	// click drag will move the actor to the mouse cursor hit location, but constrain the movement to the ground plane
	GroundPlane
};

/**
 * 
 */
UCLASS()
class NETVRKCREATIVEEDITORUTILITY_API UManipulationComponent_Mouse : public UManipulationComponent
{
	GENERATED_BODY()
#pragma region Framework
protected:
	UManipulationComponent_Mouse();

	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport = ETeleportType::None) override;

#pragma endregion
	
#pragma region Manipulator Components
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manipulation")
	bool bAbsoluteManipulation = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manipulation")
	FVector AbsoluteDirectionAxis = FVector(1, 0, 0);

	virtual void SetManipulationAbsolute(bool bInAbsolute) override;

#pragma endregion
#pragma region Mouse Events
public:
	UFUNCTION()
	virtual void OnManipulatorClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	
	UFUNCTION()
	virtual void OnManipulatorReleased(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	UFUNCTION()
	virtual void OnManipulatorBeginCursorOver(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	virtual void OnManipulatorEndCursorOver(UPrimitiveComponent* TouchedComponent);
protected:

	UPROPERTY()
	FVector CachedMouseWorldLocation_ManipulationStart;

	UPROPERTY()
	FVector CachedMouseWorldDirection_ManipulationStart;

	UPROPERTY()
	FVector CachedManipulatorWorldLocation_ManipulationStart;

	UPROPERTY()
	FVector CachedManipulatorWorldScale_ManipulationStart;

	UFUNCTION()
	virtual bool GetManipulatorMouseScreenWorldLocation(FVector& OutMouseWorldLocation, FVector& OutMouseWorldDirection) const;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnManipulatorClickedDelegate OnManipulatorClickedDelegate;

	//DEPRECATED
	UPROPERTY(BlueprintAssignable)
	FOnManipulatorClickedDelegate OnManipulatorReleasedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnManipulatorCursorOverDelegate OnManipulatorBeginCursorOverDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnManipulatorCursorOverDelegate OnMoveDirectionEndCursorOverDelegate;
#pragma endregion

#pragma region Material
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Material")
	UMaterialInterface* ManipulatorMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* ManipulatorDynamicMaterialInstance;

#pragma endregion
	
#pragma region Highlight
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Highlight")
	FLinearColor HighlightColor = FLinearColor::Yellow;
	
	UPROPERTY()
	FLinearColor CachedOriginalColor;
	
#pragma endregion


#pragma region Manipulation
	
	virtual void TickManipulation(float DeltaTime) override;

	virtual void UpdateManipulationActorLocation(bool& bOutLocationWasChanged, FVector& OutNewLocation);
	
	virtual void OnIsManipulatingChanged() override;

	virtual void TriggerTickManipulatingAbility() override;

#pragma endregion
};
