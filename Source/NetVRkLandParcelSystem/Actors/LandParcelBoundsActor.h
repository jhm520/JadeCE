// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetVRkLandParcelSystem/Interfaces/LandParcelBoundInterface.h"
#include "LandParcelBoundsActor.generated.h"

class UBoxComponent;
class ULandParcelBoundComponent;

UCLASS()
class NETVRKLANDPARCELSYSTEM_API ALandParcelBoundsActor : public AActor, public ILandParcelBoundInterface
{
	GENERATED_BODY()

#pragma region Framework
public:	
	// Sets default values for this actor's properties
	ALandParcelBoundsActor();

	virtual void OnConstruction(const FTransform& Transform) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
#pragma endregion

#pragma region Bounds

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Parcel")
	UBoxComponent* ParcelBoundsBox;
	
	/** Normal editor sprite. */
	UPROPERTY()
	TObjectPtr<class UBillboardComponent> BillboardSprite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Parcel")
	ULandParcelBoundComponent* LandParcelBoundComponent;

	virtual ULandParcelBoundComponent* GetLandParcelBoundComponent_Implementation() const override { return LandParcelBoundComponent; }

	virtual bool IsActorWithinLandParcelBounds_Implementation(AActor* Other) const override;

	virtual bool GetLandParcelBounds_Implementation(FTransform& OutBoundsTransform, FVector& OutBoundsExtent) const override;

	virtual FName GetLandParcelOwnerName_Implementation() const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Parcel System")
	FName DefaultLandParcelOwnerName;

#pragma endregion

};
