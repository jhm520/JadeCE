// Fill out your copyright notice in the Description page of Project Settings.


#include "CreativeEditorActorBase.h"

#include "AbilitySystemComponent.h"
#include "GASUtility/GameplayAbilityCollection.h"
#include "NetVRkCreativeEditorUtility/Components/CreativeEditorActorComponent.h"

#pragma region Framework

// Sets default values
ACreativeEditorActorBase::ACreativeEditorActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

// Called when the game starts or when spawned
void ACreativeEditorActorBase::BeginPlay()
{
	Super::BeginPlay();

	InitAbilitySystem();
}

// Called every frame
void ACreativeEditorActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
#pragma endregion


#pragma region Gameplay Ability System

void ACreativeEditorActorBase::InitAbilitySystem()
{
	if(!AbilitySystemComponent)
	{
		return;
	}

	if (!GetOwner())
	{
		return;
	}
	
	AbilitySystemComponent->InitAbilityActorInfo(GetOwner(), this);

	TArray<FGameplayAbilitySpecHandle> BoundAbilityHandles;
	TArray<FGameplayAbilitySpecHandle> DefaultBoundAbilityHandles;

	for (UGameplayAbilityCollection* AbilityCollection : AbilityCollections)
	{
		if (AbilityCollection)
		{
			AbilityCollection->GiveAbilities(AbilitySystemComponent, BoundAbilityHandles, DefaultBoundAbilityHandles);
		}
	}
}
	
#pragma endregion

#pragma region Creative Editor Actor Interface

void ACreativeEditorActorBase::OnCreativeModeDeactivated_Implementation(APawn* OldCreativeEditorPawn)
{
	UCreativeEditorActorComponent* CreativeEditorActorComponent = ICreativeEditorActorInterface::Execute_GetCreativeEditorActorComponent(this);

	if (!CreativeEditorActorComponent)
	{
		return;
	}
	
	//if the actor is still in preview mode, and we're deactivating creative editor mode, destroy the actor
	if (CreativeEditorActorComponent->GetIsPreview())
	{
		Destroy();
		return;
	}
}

#pragma endregion