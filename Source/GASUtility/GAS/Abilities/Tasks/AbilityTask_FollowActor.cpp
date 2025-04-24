#include "AbilityTask_FollowActor.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveVector.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilityTask_FollowActor)

UAbilityTask_FollowActor::UAbilityTask_FollowActor(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bTickingTask = true;
	bSimulatedTask = true;
	bIsFinished = false;

	DurationOfMovement = 0.0f;
	TimeMoveStarted = 0.0f;
	TimeMoveWillEnd = 0.0f;
	TargetActor = nullptr;
}

UAbilityTask_FollowActor* UAbilityTask_FollowActor::FollowActor(UGameplayAbility* OwningAbility, FName TaskInstanceName, AActor* FollowActor, UCurveFloat* OptionalInterpolationCurve, UCurveVector* OptionalVectorInterpolationCurve, float OptionalDuration)
{
	UAbilityTask_FollowActor* MyObj = NewAbilityTask<UAbilityTask_FollowActor>(OwningAbility, TaskInstanceName);
	
	MyObj->TargetActor = FollowActor;

	MyObj->TimeMoveStarted = MyObj->GetWorld()->GetTimeSeconds();

	if (OptionalDuration)
	{
		MyObj->DurationOfMovement = FMath::Max(OptionalDuration, 0.001f);		// Avoid negative or divide-by-zero cases
		MyObj->TimeMoveWillEnd = MyObj->TimeMoveStarted + MyObj->DurationOfMovement;
	}

	MyObj->LerpCurve = OptionalInterpolationCurve;

	return MyObj;
}

void UAbilityTask_FollowActor::StopFollowingActor()
{
	bIsFinished = true;
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnStoppedFollowingActor.Broadcast();
	}
	EndTask();
}


void UAbilityTask_FollowActor::Activate()
{

}

void UAbilityTask_FollowActor::InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent)
{
	Super::InitSimulatedTask(InGameplayTasksComponent);

	TimeMoveStarted = GetWorld()->GetTimeSeconds();
	TimeMoveWillEnd = TimeMoveStarted + DurationOfMovement;
}

//TODO: This is still an awful way to do this and we should scrap this task or do it right.
void UAbilityTask_FollowActor::TickTask(float DeltaTime)
{
	if (bIsFinished)
	{
		return;
	}

	if (!TargetActor)
	{
		bIsFinished = true;
		EndTask();
		return;
	}

	Super::TickTask(DeltaTime);
	AActor* MyActor = GetAvatarActor();
	if (MyActor)
	{
		ACharacter* MyCharacter = Cast<ACharacter>(MyActor);
		if (MyCharacter)
		{
			UCharacterMovementComponent* CharMoveComp = Cast<UCharacterMovementComponent>(MyCharacter->GetMovementComponent());
			if (CharMoveComp)
			{
				CharMoveComp->SetMovementMode(MOVE_Custom, 0);
			}
		}


		float CurrentTime = GetWorld()->GetTimeSeconds();

		if (TimeMoveWillEnd > 0.0f && CurrentTime >= TimeMoveWillEnd)
		{
			bIsFinished = true;

			const FVector& TargetLocation = TargetActor->GetActorLocation();
			const FRotator& TargetRotation = TargetActor->GetActorRotation();
			const FVector& TargetScale = TargetActor->GetActorScale3D();

			// Teleport in attempt to find a valid collision spot
			MyActor->TeleportTo(TargetLocation, TargetRotation);
			MyActor->SetActorScale3D(TargetScale);
			
			if (!bIsSimulating)
			{
				MyActor->ForceNetUpdate();
				if (ShouldBroadcastAbilityTaskDelegates())
				{
					OnStoppedFollowingActor.Broadcast();
				}
				EndTask();
			}
		}
		else
		{
			const FVector& TargetLocation = TargetActor->GetActorLocation();
			const FRotator& TargetRotation = TargetActor->GetActorRotation();
			const FVector& TargetScale = TargetActor->GetActorScale3D();

			float MoveFraction = (CurrentTime - TimeMoveStarted) / DurationOfMovement;
			
			if (LerpCurve)
			{
				MoveFraction = LerpCurve->GetFloatValue(MoveFraction);
			}

			// const FVector& NewLocation = FMath::Lerp<FVector, float>(MyActor->GetActorLocation(), TargetLocation, MoveFraction);
			// const FRotator& NewRotation = FMath::Lerp<FRotator, float>(MyActor->GetActorRotation(), TargetRotation, MoveFraction);

			MyActor->SetActorLocationAndRotation(TargetLocation, TargetRotation);
			MyActor->SetActorScale3D(TargetScale);

			// if (IsLocallyControlled())
			// {
			// 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("TargetLocation: %s"), *TargetLocation.ToString()));
			// }
			// else if (GetAvatarActor()->HasAuthority())
			// {
			// 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("TargetLocation: %s"), *TargetLocation.ToString()));
			// }
			// else
			// {
			// 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("TargetLocation: %s"), *TargetLocation.ToString()));
			// }
		}
	}
	else
	{
		bIsFinished = true;
		EndTask();
	}
}

void UAbilityTask_FollowActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	DOREPLIFETIME(UAbilityTask_FollowActor, TargetActor);
	DOREPLIFETIME(UAbilityTask_FollowActor, DurationOfMovement);
	DOREPLIFETIME(UAbilityTask_FollowActor, LerpCurve);
}

void UAbilityTask_FollowActor::OnDestroy(bool AbilityIsEnding)
{
	AActor* MyActor = GetAvatarActor();
	if (MyActor)
	{
		ACharacter* MyCharacter = Cast<ACharacter>(MyActor);
		if (MyCharacter)
		{
			UCharacterMovementComponent* CharMoveComp = Cast<UCharacterMovementComponent>(MyCharacter->GetMovementComponent());
			if (CharMoveComp && CharMoveComp->MovementMode == MOVE_Custom)
			{
				CharMoveComp->SetMovementMode(MOVE_Falling);
			}
		}
	}

	Super::OnDestroy(AbilityIsEnding);
}