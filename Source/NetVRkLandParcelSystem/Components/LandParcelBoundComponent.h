// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LandParcelBoundComponent.generated.h"

//this component is meant to be added to actors that are meant to be included in the land parcel system
//it contains logic to determine if the actor is allowed within a land parcel, and also how to handle conflicts with land parcels
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETVRKLANDPARCELSYSTEM_API ULandParcelBoundComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region Framework
public:	
	// Sets default values for this component's properties
	ULandParcelBoundComponent();

	virtual void PostReinitProperties() override;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion

#pragma region Bounds

	UFUNCTION(BlueprintCallable, Category = "Land Parcel")
	bool IsActorWithinLandParcelBounds(AActor* Actor) const;
#pragma endregion
		
};
