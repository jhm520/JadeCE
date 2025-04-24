// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetVRkGameplayAbility.h"
#include "NetVRkGameplayAbility_EditMove.generated.h"

class UAbilityTask_FollowActor;
/**
 * 
 */
UCLASS()
class NETVRKP4VOXEL_API UNetVRkGameplayAbility_EditMove : public UNetVRkGameplayAbility
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnMoveFinished();

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


	UPROPERTY()
	UAbilityTask_FollowActor* FollowTask;
	
};
