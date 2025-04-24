#include "AbilityTask_Rotate.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveVector.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilityTask_Rotate)

UAbilityTask_Rotate::UAbilityTask_Rotate(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bTickingTask = true;
	bSimulatedTask = true;
	bIsFinished = false;
}

UAbilityTask_Rotate* UAbilityTask_Rotate::Rotate(UGameplayAbility* OwningAbility, FName TaskInstanceName, FRotator Rotation, float Duration, UCurveFloat* OptionalInterpolationCurve, UCurveVector* OptionalVectorInterpolationCurve)
{
	UAbilityTask_Rotate* MyObj = NewAbilityTask<UAbilityTask_Rotate>(OwningAbility, TaskInstanceName);

	if (MyObj->GetAvatarActor() != nullptr)
	{
		MyObj->StartRotation = MyObj->GetAvatarActor()->GetActorRotation();
	}

	MyObj->TargetRotation = UKismetMathLibrary::ComposeRotators(MyObj->StartRotation, Rotation);
	MyObj->DurationOfMovement = FMath::Max(Duration, 0.001f);		// Avoid negative or divide-by-zero cases
	MyObj->TimeMoveStarted = MyObj->GetWorld()->GetTimeSeconds();
	MyObj->TimeMoveWillEnd = MyObj->TimeMoveStarted + MyObj->DurationOfMovement;
	MyObj->LerpCurve = OptionalInterpolationCurve;

	return MyObj;
}

UAbilityTask_Rotate* UAbilityTask_Rotate::SetRotation(UGameplayAbility* OwningAbility, FName TaskInstanceName, FRotator Rotation, float Duration, UCurveFloat* OptionalInterpolationCurve, UCurveVector* OptionalVectorInterpolationCurve)
{
	UAbilityTask_Rotate* MyObj = NewAbilityTask<UAbilityTask_Rotate>(OwningAbility, TaskInstanceName);

	if (MyObj->GetAvatarActor() != nullptr)
	{
		MyObj->StartRotation = MyObj->GetAvatarActor()->GetActorRotation();
	}

	MyObj->TargetRotation = Rotation;
	MyObj->DurationOfMovement = FMath::Max(Duration, 0.001f);		// Avoid negative or divide-by-zero cases
	MyObj->TimeMoveStarted = MyObj->GetWorld()->GetTimeSeconds();
	MyObj->TimeMoveWillEnd = MyObj->TimeMoveStarted + MyObj->DurationOfMovement;
	MyObj->LerpCurve = OptionalInterpolationCurve;

	return MyObj;
}

void UAbilityTask_Rotate::Activate()
{

}

void UAbilityTask_Rotate::InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent)
{
	Super::InitSimulatedTask(InGameplayTasksComponent);

	TimeMoveStarted = GetWorld()->GetTimeSeconds();
	TimeMoveWillEnd = TimeMoveStarted + DurationOfMovement;
}

//TODO: This is still an awful way to do this and we should scrap this task or do it right.
void UAbilityTask_Rotate::TickTask(float DeltaTime)
{
	if (bIsFinished)
	{
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

		if (CurrentTime >= TimeMoveWillEnd)
		{
			bIsFinished = true;

			// Teleport in attempt to find a valid collision spot
			MyActor->TeleportTo(MyActor->GetActorLocation(), TargetRotation);
			
			if (!bIsSimulating)
			{
				MyActor->ForceNetUpdate();
				if (ShouldBroadcastAbilityTaskDelegates())
				{
					OnTargetRotationReached.Broadcast();
				}
				EndTask();
			}
		}
		else
		{
			FRotator NewRotation;

			float MoveFraction = (CurrentTime - TimeMoveStarted) / DurationOfMovement;
			
			if (LerpCurve)
			{
				MoveFraction = LerpCurve->GetFloatValue(MoveFraction);
			}

			NewRotation = FMath::Lerp<FRotator, float>(StartRotation, TargetRotation, MoveFraction);

			MyActor->SetActorRotation(NewRotation);
		}
	}
	else
	{
		bIsFinished = true;
		EndTask();
	}
}

void UAbilityTask_Rotate::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	DOREPLIFETIME(UAbilityTask_Rotate, StartRotation);
	DOREPLIFETIME(UAbilityTask_Rotate, TargetRotation);
	DOREPLIFETIME(UAbilityTask_Rotate, DurationOfMovement);
	DOREPLIFETIME(UAbilityTask_Rotate, LerpCurve);
}

void UAbilityTask_Rotate::OnDestroy(bool AbilityIsEnding)
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