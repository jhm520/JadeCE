// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorActorInterface.h"
#include "CreativeEditorActorBase.generated.h"

class UGameplayAbilityCollection;
UCLASS()
class NETVRKCREATIVEEDITORUTILITY_API ACreativeEditorActorBase : public AActor, public IAbilitySystemInterface, public ICreativeEditorActorInterface
{
	GENERATED_BODY()
#pragma region Framework
public:	
	// Sets default values for this actor's properties
	ACreativeEditorActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
#pragma endregion

#pragma region Ability System
public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override {return AbilitySystemComponent;};
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Ability System")
	class UAbilitySystemComponent* AbilitySystemComponent;

	UFUNCTION()
	void InitAbilitySystem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Ability System", meta = (AllowPrivateAccess = "true"))
	TArray<UGameplayAbilityCollection*> AbilityCollections;
	
#pragma endregion

#pragma region Creative Editor Actor Interface

	virtual void OnCreativeModeDeactivated_Implementation(APawn* OldCreativeEditorPawn) override;

#pragma endregion

};
