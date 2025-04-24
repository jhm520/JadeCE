// © 2024 John Henry Miller. All rights reserved

#include "GameplayAbilityTargetActor_SingleTransform.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilityTargetActorInterface.h"
#include "Abilities/GameplayAbility.h"

AGameplayAbilityTargetActor_SingleTransform::AGameplayAbilityTargetActor_SingleTransform()
{
	ShouldProduceTargetDataOnServer = false;
}

FGameplayAbilityTargetDataHandle AGameplayAbilityTargetActor_SingleTransform::MakeTargetData(const FTransform& InTargetTransform) const
{
	FGameplayAbilityTargetingLocationInfo SourceLocation;
	SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
	SourceLocation.SourceActor = OwningAbility->GetAvatarActorFromActorInfo();
	
	FGameplayAbilityTargetingLocationInfo TranslationLocation;
	TranslationLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	TranslationLocation.LiteralTransform = InTargetTransform;
	
	return UAbilitySystemBlueprintLibrary::AbilityTargetDataFromLocations(SourceLocation, TranslationLocation);
}

void AGameplayAbilityTargetActor_SingleTransform::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	
	bDebug = false;
	FGameplayAbilityTargetDataHandle Handle = MakeTargetData(TargetTransform);
	TargetDataReadyDelegate.Broadcast(Handle);
}