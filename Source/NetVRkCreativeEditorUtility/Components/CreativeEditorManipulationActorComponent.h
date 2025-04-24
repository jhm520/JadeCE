// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CreativeEditorManipulationActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETVRKCREATIVEEDITORUTILITY_API UCreativeEditorManipulationActorComponent : public UActorComponent
{
	GENERATED_BODY()
#pragma region Framework
public:	
	// Sets default values for this component's properties
	UCreativeEditorManipulationActorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnManipulatedActorDestroyed(AActor* DestroyedActor);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion
	
#pragma region Creative Editor
	UFUNCTION(BlueprintPure, Category = "Manipulation")
	APawn* GetManipulatorPawn() const;

	UFUNCTION(BlueprintPure, Category = "Manipulation")
	APlayerController* GetManipulatorController() const;
	
	UFUNCTION(BlueprintPure, Category = "Manipulation")
	AActor* GetManipulatedActor() const;
#pragma endregion

#pragma region Manipulation

	UFUNCTION(BlueprintCallable, Category = "Manipulation")
	void SetManipulationMode(FGameplayTag InManipulationModeTag);

	UFUNCTION(BlueprintCallable, Category = "Manipulation")
	void SetManipulationSnapEnabled(bool bInSnapEnabled, FGameplayTagContainer InManipulationTags);

	UFUNCTION(BlueprintCallable, Category = "Manipulation")
	void SetManipulationSnapIncrement(float InSnapIncrement, FGameplayTagContainer InManipulationTags);

	UFUNCTION(BlueprintCallable, Category = "Manipulation")
	void SetManipulationAbsolute(bool bInAbsolute, FGameplayTagContainer InManipulationTags);

	UFUNCTION(BlueprintCallable, Category = "Manipulation")
	void SetManipulationDepth(float InDepth, FGameplayTagContainer InManipulationTags);

	UFUNCTION(BlueprintCallable, Category = "Manipulation")
	UManipulationComponent* GetManipulationComponentByTag(const FGameplayTag InManipulationTag) const;
#pragma endregion

};
