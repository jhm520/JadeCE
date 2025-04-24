// Fill out your copyright notice in the Description page of Project Settings.


#include "NetVRkGameplayAbility_EditMove.h"

#include "GAS/Abilities/Tasks/AbilityTask_Rotate.h"
#include "NetVRkCoreUtility/NetVRkEditMoveGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_MoveToLocation.h"
#include "GAS/Abilities/Tasks/AbilityTask_FollowActor.h"
#include "NetVRkCreativeEditorUtility/NetVRkVoxelSculptGameplayTags.h"

void UNetVRkGameplayAbility_EditMove::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!TriggerEventData)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!TriggerEventData->TargetTags.IsValidIndex(0))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const FGameplayAbilityTargetData* TargetData = TriggerEventData->TargetData.Get(0);

	if (!TargetData)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const float MovementTime = TriggerEventData->EventMagnitude;

	const FRotator& Rotation = TargetData->GetEndPointTransform().GetRotation().Rotator();
	
	const FGameplayTag& TargetTag = TriggerEventData->TargetTags.GetByIndex(0);

	if (TargetTag == NetVRkVoxelSculptEditMoveGameplayTags::Move)
	{
		const FVector& TargetLocation = GetAvatarActorFromActorInfo()->GetActorLocation() + TargetData->GetEndPointTransform().GetLocation();
		UAbilityTask_MoveToLocation* MoveTask = UAbilityTask_MoveToLocation::MoveToLocation(this, FName("Move"), TargetLocation, MovementTime, nullptr, nullptr);

		if (MoveTask)
		{
			MoveTask->OnTargetLocationReached.AddDynamic(this, &UNetVRkGameplayAbility_EditMove::OnMoveFinished);
			MoveTask->ReadyForActivation();
		}
		
	}
	if (TargetTag == NetVRkVoxelSculptEditMoveGameplayTags::MoveToLocation)
	{
		const FVector& TargetLocation = TargetData->GetEndPointTransform().GetLocation();
		UAbilityTask_MoveToLocation* MoveTask = UAbilityTask_MoveToLocation::MoveToLocation(this, FName("Move"), TargetLocation, MovementTime, nullptr, nullptr);
		
		if (MoveTask)
		{
			MoveTask->OnTargetLocationReached.AddDynamic(this, &UNetVRkGameplayAbility_EditMove::OnMoveFinished);
			MoveTask->ReadyForActivation();
		}
	}
	if (TargetTag == NetVRkVoxelSculptEditMoveGameplayTags::Rotate)
	{
		UAbilityTask_Rotate* RotateTask = UAbilityTask_Rotate::Rotate(this, FName("Rotate"), Rotation, MovementTime, nullptr, nullptr);
		if (RotateTask)
		{
			RotateTask->OnTargetRotationReached.AddDynamic(this, &UNetVRkGameplayAbility_EditMove::OnMoveFinished);
			RotateTask->ReadyForActivation();
		}
	}
	if (TargetTag == NetVRkVoxelSculptEditMoveGameplayTags::SetRotation)
	{
		UAbilityTask_Rotate* RotateTask = UAbilityTask_Rotate::SetRotation(this, FName("Rotate"), Rotation, MovementTime, nullptr, nullptr);
		RotateTask->OnTargetRotationReached.AddDynamic(this, &UNetVRkGameplayAbility_EditMove::OnMoveFinished);
		if (RotateTask)
		{
			RotateTask->ReadyForActivation();
		}
	}
	if (TargetTag == NetVRkVoxelSculptEditMoveGameplayTags::FollowActor)
	{
		if (!TargetData->GetActors().IsValidIndex(0))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
		
		AActor* FollowTargetActor = TargetData->GetActors()[0].Get();

		if (!FollowTargetActor)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
		
		FollowTask = UAbilityTask_FollowActor::FollowActor(this, FName("Follow"), FollowTargetActor, nullptr, nullptr, 0.0f);
		
		if (FollowTask)
		{
			FollowTask->OnStoppedFollowingActor.AddDynamic(this, &UNetVRkGameplayAbility_EditMove::OnMoveFinished);
			FollowTask->ReadyForActivation();
		}
	}
	
	if (TargetTag == NetVRkVoxelSculptEditMoveGameplayTags::StopFollowingActor)
	{
		if (!FollowTask)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;	
		}

		FollowTask->StopFollowingActor();
	}
}

void UNetVRkGameplayAbility_EditMove::OnMoveFinished()
{
	EndAbility( GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UNetVRkGameplayAbility_EditMove::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}