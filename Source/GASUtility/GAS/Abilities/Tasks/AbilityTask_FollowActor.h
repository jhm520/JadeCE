// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_FollowActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFollowActorDelegate);

class UCurveFloat;
class UCurveVector;
class UGameplayTasksComponent;

class AActor;
class UPrimitiveComponent;
class UGameplayTasksComponent;
/**
 * 
 */
UCLASS()
class GASUTILITY_API UAbilityTask_FollowActor : public UAbilityTask
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FFollowActorDelegate OnStoppedFollowingActor;

	virtual void InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent) override;

	/** Move to the specified location, using the vector curve (range 0 - 1) if specified, otherwise the float curve (range 0 - 1) or fallback to linear interpolation */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_FollowActor* FollowActor(UGameplayAbility* OwningAbility, FName TaskInstanceName, AActor* FollowActor, UCurveFloat* OptionalInterpolationCurve, UCurveVector* OptionalVectorInterpolationCurve, float OptionalDuration = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	void StopFollowingActor();
	
	virtual void Activate() override;

	/** Tick function for this task, if bTickingTask == true */
	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool AbilityIsEnding) override;

protected:

	bool bIsFinished;
	
	UPROPERTY(Replicated)
	AActor* TargetActor;

	UPROPERTY(Replicated)
	float DurationOfMovement;


	float TimeMoveStarted;

	float TimeMoveWillEnd;

	UPROPERTY(Replicated)
	TObjectPtr<UCurveFloat> LerpCurve;

	
};
