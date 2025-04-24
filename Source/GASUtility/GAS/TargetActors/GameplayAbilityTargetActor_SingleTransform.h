// © 2024 John Henry Miller. All rights reserved
#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GameplayAbilityTargetActor_SingleTransform.generated.h"

/**
 * 
 */
UCLASS()
class GASUTILITY_API AGameplayAbilityTargetActor_SingleTransform : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
protected:
	
	AGameplayAbilityTargetActor_SingleTransform();
	
	FGameplayAbilityTargetDataHandle MakeTargetData(const FTransform& TargetTransform) const;
	
	virtual void ConfirmTargetingAndContinue() override;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn))
	FTransform TargetTransform;

};
