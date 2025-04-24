// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SceneComponent.h"
#include "ManipulationComponent.generated.h"

class UGameplayAbility;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETVRKCREATIVEEDITORUTILITY_API UManipulationComponent : public USceneComponent
{
	GENERATED_BODY()
	
#pragma region Framework

public:	
	// Sets default values for this component's properties
	UManipulationComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion

	
#pragma region Manipulation
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Manipulation")
	USceneComponent* ManipulationRootComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Manipulation")
	FGameplayTagContainer ManipulationTags;
public:

	UFUNCTION(BlueprintPure, Category = "Manipulation")
	FGameplayTagContainer GetManipulationTags() const {return ManipulationTags;}
	
	UFUNCTION(BlueprintCallable, Category = "Manipulation")
	virtual void SetManipulationEnabled(bool bInEnabled);

	UFUNCTION()
	virtual void SetSnapEnabled(bool bInSnapEnabled);

	UFUNCTION()
	virtual void SetSnapIncrement(float InSnapIncrement);

	UFUNCTION()
	virtual void SetManipulationAbsolute(bool bInAbsolute);

	UFUNCTION()
	virtual void SetManipulationDepth(float InDepth);

	friend class UCreativeEditorPawnComponent;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Manipulation")
	bool bIsManipulatorEnabled = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "Manipulation")
	bool bIsManipulating = false;

	UFUNCTION(BlueprintCallable, Category = "Manipulation")
	void SetIsManipulating(bool bInIsManipulating);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Abilities", meta = (AllowPrivate))
	TSubclassOf<UGameplayAbility> GameplayAbility_OnManipulatorEnabled;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Abilities", meta = (AllowPrivate))
	TSubclassOf<UGameplayAbility> GameplayAbility_OnManipulatorDisabled;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Abilities", meta = (AllowPrivate))
	TSubclassOf<UGameplayAbility> GameplayAbility_OnStartedManipulating;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Abilities", meta = (AllowPrivate))
	TSubclassOf<UGameplayAbility> GameplayAbility_OnStoppedManipulating;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Abilities", meta = (AllowPrivate))
	TSubclassOf<UGameplayAbility> GameplayAbility_TickManipulating;
	
	UFUNCTION()
	virtual void OnIsManipulatingChanged();

	UFUNCTION()
	virtual void TriggerManipulatorEnabledAbility();

	UFUNCTION()
	virtual void TriggerManipulatorDisabledAbility();

	UFUNCTION()
	virtual void TriggerTickManipulatingAbility();

	UFUNCTION()
	virtual void TriggerStartManipulatingAbility();

	UFUNCTION()
	virtual void TriggerStopManipulatingAbility();
	
	virtual void TickManipulation(float DeltaTime);

	virtual void TickManipulatorEnabled(float DeltaTime);
	
	UFUNCTION(BlueprintPure, Category = "Manipulation")
	APawn* GetManipulatorPawn() const;

	UFUNCTION(BlueprintPure, Category = "Manipulation")
	APlayerController* GetManipulatorController() const;

	UFUNCTION(BlueprintPure, Category = "Manipulation")
	AActor* GetManipulatedActor() const;

	UFUNCTION(BlueprintPure, Category = "Manipulation")
	UAbilitySystemComponent* GetManipulatorAbilitySystemComponent() const;

	UFUNCTION(Server, Unreliable)
	void Server_UpdateManipulationActorLocation(FVector NewLocation);

	UFUNCTION(Server, Unreliable)
	void Server_UpdateManipulationActorQuat(FQuat NewRotation);

	UFUNCTION(Server, Unreliable)
	void Server_UpdateManipulationActorScale(FVector NewScale);

	UFUNCTION(Server, Unreliable)
	void Server_UpdateManipulationActorLocationAndRotation(FVector NewLocation, FQuat NewRotation);
	
#pragma endregion

		
};
