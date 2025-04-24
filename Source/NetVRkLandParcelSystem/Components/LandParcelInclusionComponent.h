// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LandParcelInclusionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInclusionPlacementStatusChanged, bool, bIsValidPlacement);

//this component is meant to be added to actors that are meant to be included in the land parcel system
//it contains logic to determine if the actor is allowed within a land parcel, and also how to handle conflicts with land parcels
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETVRKLANDPARCELSYSTEM_API ULandParcelInclusionComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region Framework
public:
	/**
 * Called after properties are overwritten, including after subobjects initialization from a CDO.
 * This could be called multiple times during an object lifetime, which is not the case for PostInitProperties which is expected to be called only once.
 */
	virtual void PostReinitProperties() override;
	
	// Sets default values for this component's properties
	ULandParcelInclusionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion

#pragma region Inclusion

	UFUNCTION(BlueprintPure, Category = "Land Parcel")
	bool IsValidPlacement() const;

	UFUNCTION(BlueprintCallable, Category = "Land Parcel")
	void CheckValidPlacement();
	
	UPROPERTY(BlueprintAssignable, Category = "Land Parcel")
	FOnInclusionPlacementStatusChanged OnInclusionPlacementStatusChanged;
	
protected:
	UPROPERTY(Transient)
	bool bIsValidPlacement = false;
	
#pragma endregion

#pragma region Land Parcel

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Parcel")
	TArray<AActor*> DefaultLandParcels;
	
	bool FindNearbyLandParcels(TArray<AActor*>& OutLandParcels) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Land Parcel")
	TEnumAsByte<ETraceTypeQuery> LandParcelTraceType;
	
#pragma endregion

};
