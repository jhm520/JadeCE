// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ManipulationComponent_Mouse.h"
#include "ManipulationComponent_ClickDrag.generated.h"




class USphereComponent;
class UStaticMeshComponent;
/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETVRKCREATIVEEDITORUTILITY_API UManipulationComponent_ClickDrag : public UManipulationComponent_Mouse
{
	GENERATED_BODY()

#pragma region Framework
public:
	UManipulationComponent_ClickDrag();
	virtual void BeginPlay() override;
#pragma endregion


#pragma region Click Drag Components
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Click Drag")
	USphereComponent* ClickDragSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Click Drag")
	UStaticMeshComponent* ClickDragMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Click Drag")
	FLinearColor MeshColor;

#pragma endregion


#pragma region Click Drag

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Click Drag")
	EManipulationClickDragType DefaultClickDragType = EManipulationClickDragType::MouseCursor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Click Drag")
	TEnumAsByte<ECollisionChannel> ClickDragTraceChannel = ECollisionChannel::ECC_Visibility;
#pragma endregion

#pragma region Manipulation

	virtual void SetManipulationEnabled(bool bInEnabled) override;

	virtual void UpdateManipulationActorLocation(bool& bOutLocationWasChanged, FVector& OutNewLocation) override;

	virtual void OnIsManipulatingChanged() override;

	virtual void TriggerStartManipulatingAbility() override;

	virtual void TriggerStopManipulatingAbility() override;
protected:
	UPROPERTY()
	FVector WorldLocation_ManipulationStart;

	UPROPERTY()
	FHitResult GroundHitResult_ManipulationStart;

#pragma endregion


};
