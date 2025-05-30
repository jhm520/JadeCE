
#include "GASUtilityHelperLibrary.h"

#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

bool UGASUtilityHelperLibrary::GetCooldownRemainingForTag(UAbilitySystemComponent* Target, FGameplayTagContainer InCooldownTags, float& TimeRemaining, float& CooldownDuration)
{
	if (IsValid(Target) && InCooldownTags.Num() > 0)
	{
		TimeRemaining = 0.f;
		CooldownDuration = 0.f;

		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTags);
		TArray<TPair<float, float>> DurationAndTimeRemaining = Target->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}

			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;

			return true;
		}
	}

	return false;
}

FGameplayAbilitySpec* UGASUtilityHelperLibrary::FindAbilitySpecFromSuperClass(UAbilitySystemComponent* InASC, TSubclassOf<UGameplayAbility> InAbilityClass)
{
	if (!InASC)
	{
		return nullptr;
	}
	
	TArray<FGameplayAbilitySpec>& ActivatableAbilities = InASC->GetActivatableAbilities();
	
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities)
	{
		if (Spec.Ability.IsA(InAbilityClass))
		{
			return &Spec;
		}
	}

	return nullptr;
}

void UGASUtilityHelperLibrary::CancelAbilityOnActorByClass(AActor* Actor, TSubclassOf<UGameplayAbility> InAbilityClass)
{
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Actor);

	if (!AbilitySystemInterface)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();

	if (!AbilitySystemComponent)
	{
		return;
	}
	
	AbilitySystemComponent->CancelAbility(InAbilityClass.GetDefaultObject());
}

FGameplayAbilitySpec* UGASUtilityHelperLibrary::FindAbilitySpecByTag(UAbilitySystemComponent* InASC, FGameplayTag InAbilityTag)
{
	if (!InASC)
	{
		return nullptr;
	}
	
	TArray<FGameplayAbilitySpec>& ActivatableAbilities = InASC->GetActivatableAbilities();
	
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities)
	{
		UGameplayAbility* Ability = Spec.Ability;
		
		if (!Ability)
		{
			continue;
		}

		if (Ability->AbilityTags.HasTagExact(InAbilityTag))
		{
			return &Spec;
		}
	}
	
	return nullptr;
}


void UGASUtilityHelperLibrary::CancelAbilityOnActorByTag(AActor* Actor, FGameplayTag InAbilityTag)
{
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Actor);

	if (!AbilitySystemInterface)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();

	if (!AbilitySystemComponent)
	{
		return;
	}
	
	FGameplayAbilitySpec* AbilitySpec = UGASUtilityHelperLibrary::FindAbilitySpecByTag(AbilitySystemComponent, InAbilityTag);

	if (!AbilitySpec)
	{
		return;
	}
	
	AbilitySystemComponent->CancelAbility(AbilitySpec->Ability);
}

