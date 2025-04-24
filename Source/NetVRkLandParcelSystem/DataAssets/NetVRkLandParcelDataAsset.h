// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "NetVRkLandParcelDataAsset.generated.h"

USTRUCT(BlueprintType)
struct NETVRKLANDPARCELSYSTEM_API FNetVRkLandParcelPlacementInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FNetVRkLandParcelPlacementInfo(){}
protected:
	//the sculpt actor class we're using for the sculpt
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Parcel")
	FString ParcelName = "";

	UPROPERTY()
	FString name;
	
	UPROPERTY()
	float x;

	UPROPERTY()
	float y;

	UPROPERTY()
	float matrixa;

	UPROPERTY()
	float matrixb;

	UPROPERTY()
	float matrixc;

	UPROPERTY()
	float matrixd;

	UPROPERTY()
	float matrixe;

	UPROPERTY()
	float matrixf;

public:
	//the sculpt actor class we're using for the sculpt
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Parcel")
	FVector2D ParcelLocation = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Parcel")
	FTransform ParcelTransform = FTransform::Identity;

};

USTRUCT(BlueprintType)
struct NETVRKLANDPARCELSYSTEM_API FNetVRkLegacyLandParcelPlacementInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FNetVRkLegacyLandParcelPlacementInfo(){}
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Parcel")
	FString ParcelName = "";
	
	UPROPERTY()
	float x;

	UPROPERTY()
	float y;

	UPROPERTY()
	float rot;

};

/**
 * 
 */
UCLASS()
class NETVRKLANDPARCELSYSTEM_API UNetVRkLandParcelDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Parcel")
	TArray<FNetVRkLandParcelPlacementInfo> ParcelPlacementInfos;
	
};
