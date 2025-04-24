// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FellOffWorldComponent.generated.h"

//component to check if the actor fell off the world
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETVRKCOREUTILITY_API UFellOffWorldComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFellOffWorldComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool CheckFellOffWorld(float DeltaTime, FVector& OutHitLocation);

	void OnFellOffWorld(const FVector& OutHitLocation);

	UPROPERTY(EditDefaultsOnly, Category = "FellOffWorld")
	float CheckFellOffWorldHeight = 10000000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "FellOffWorld")
	float CheckBelowPlayer = 10000000.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "FellOffWorld")
	TEnumAsByte<ECollisionChannel> CheckFellOffWorldChannel = ECollisionChannel::ECC_Visibility;

	UPROPERTY(EditDefaultsOnly, Category = "FellOffWorld")
	bool bDelayEnableGravity = false;
	
	UFUNCTION()
	void Timer_EnableGravity();

	UPROPERTY()
	FTimerHandle TimerHandle_EnableGravity;

	UPROPERTY(EditDefaultsOnly, Category = "FellOffWorld")
	float EnableGravityDelay = 10.0f;
};
