// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ManipulationComponent_Mouse.h"
#include "ManipulationComponent_SmoothBrush.generated.h"

class UCapsuleComponent;
class UArrowComponent;
class UPrimitiveComponent;
class USphereComponent;
class ASurfaceGridActor;

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETVRKCREATIVEEDITORUTILITY_API UManipulationComponent_SmoothBrush : public UManipulationComponent_Mouse
{
	GENERATED_BODY()
#pragma region Framework
public:
	UManipulationComponent_SmoothBrush();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport = ETeleportType::None) override;

#pragma endregion

#pragma region Move Direction Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move Direction")
	USphereComponent* SmoothBrushSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move Direction")
	UStaticMeshComponent* SmoothBrushMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move Direction")
	FLinearColor SmoothBrushColor;
	
#pragma endregion


#pragma region Mouse Events
protected:

	virtual void OnManipulatorClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed) override;

	virtual void OnManipulatorReleased(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed) override;

public:


#pragma endregion

#pragma region Manipulation

	virtual void TriggerManipulatorEnabledAbility() override;

	virtual void TriggerManipulatorDisabledAbility() override;
	
	virtual void TickManipulatorEnabled(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Click Drag")
	TEnumAsByte<ECollisionChannel> SmoothBrushTraceChannel = ECollisionChannel::ECC_Visibility;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manipulation")
	bool bSnapEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manipulation")
	float SnapDistance = 10.0f;
	
	virtual void SetManipulationEnabled(bool bInEnabled) override;

	virtual void TickManipulation(float DeltaTime) override;

	virtual void UpdateManipulationActorLocation(bool& bOutLocationWasChanged, FVector& OutNewLocation) override;
	
	virtual void OnIsManipulatingChanged() override;

	void OnStartedManipulating();

	void OnStoppedManipulating();

	UPROPERTY()
	FHitResult MouseHitResult_ManipulationStart;

	UPROPERTY()
	FTransform WorldTransform_ManipulationEnabled;

	virtual void TriggerStartManipulatingAbility() override;

	virtual void TriggerStopManipulatingAbility() override;
	
	virtual void TriggerTickManipulatingAbility() override;

	virtual void SetSnapEnabled(bool bInSnapEnabled) override {bSnapEnabled = bInSnapEnabled;}
	
	virtual void SetSnapIncrement(float InSnapIncrement) override { SnapDistance = InSnapIncrement; }

	virtual void SetManipulationDepth(float InDepth) override { SmoothBrushDepth = InDepth; }


#pragma endregion

#pragma region Manipulation | Brush

	//how "deep" into the landscape the brush will go
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Manipulation")
	float SmoothBrushDepth = 100.0f;

	//the class of brush surface actor to spawn when the brush is used
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Manipulation")
	TSubclassOf<ASurfaceGridActor> BrushSurfaceActorClass;

	UPROPERTY()
	ASurfaceGridActor* BrushSurfaceActor;
	
#pragma endregion

#pragma region Manipulation | Gameplay Abilities

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Abilities", meta = (AllowPrivate))
	FGameplayTag GameplayAbilityTargetTag_ManipulatorDisabled;
#pragma endregion


};
