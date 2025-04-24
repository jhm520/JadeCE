// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetVRkLandParcelSystem/DataAssets/NetVRkLandParcelDataAsset.h"
#include "LandParcelManager.generated.h"

class ALandParcelBoundsActor;

UCLASS()
class NETVRKLANDPARCELSYSTEM_API ALandParcelManager : public AActor
{
	GENERATED_BODY()

#pragma region Framework
public:	
	// Sets default values for this actor's properties
	ALandParcelManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
#pragma endregion


#pragma region Land Parcel System

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Parcel System")
	UDataTable* LandParcelDataTable;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Land Parcel System")
	void SpawnAllLandParcels();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Parcel System")
	TArray<ALandParcelBoundsActor*> LandParcels;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Land Parcel System")
	void DeleteAllLandParcels();

	UFUNCTION(BlueprintCallable, Category = "Land Parcel System")
	ALandParcelBoundsActor* SpawnLegacyLandParcel(const FNetVRkLegacyLandParcelPlacementInfo& LandParcelPlacementInfo);

	bool FindLandParcelZPosition(const FVector& OriginalPosition, float& OutZPosition) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Parcel System")
	float FindLandParcelZPositionTestHeight = 10000000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "FellOffWorld")
	TEnumAsByte<ECollisionChannel> FindLandParcelZPositionTestChannel = ECollisionChannel::ECC_Visibility;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Parcel System")
	float LandParcelWorldPositionScale = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Parcel System")
	TSubclassOf<ALandParcelBoundsActor> LandParcelActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Parcel System")
	FVector LandParcelWorldPositionOffset = FVector::ZeroVector;

	
#pragma endregion

};
