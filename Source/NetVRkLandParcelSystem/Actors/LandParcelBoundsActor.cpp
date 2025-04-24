// Fill out your copyright notice in the Description page of Project Settings.


#include "LandParcelBoundsActor.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "NetVRkLandParcelSystem/Components/LandParcelBoundComponent.h"

// Sets default values
#pragma region Framework

ALandParcelBoundsActor::ALandParcelBoundsActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ParcelBoundsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ParcelBoundsBox"));
	SetRootComponent(ParcelBoundsBox);

	LandParcelBoundComponent = CreateDefaultSubobject<ULandParcelBoundComponent>(TEXT("LandParcelBoundComponent"));

	DefaultLandParcelOwnerName = FName("DefaultPlayer");
	
// #if WITH_EDITORONLY_DATA
// 	BillboardSprite = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
// 	BillboardSprite->SetupAttachment(ParcelBoundsBox);
// #endif
}

void ALandParcelBoundsActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}


// Called when the game starts or when spawned
void ALandParcelBoundsActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALandParcelBoundsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
#pragma endregion

#pragma region Bounds

bool ALandParcelBoundsActor::IsActorWithinLandParcelBounds_Implementation(AActor* Other) const
{
	if (!LandParcelBoundComponent)
	{
		return false;
	}

	const bool bIsWithinBounds = LandParcelBoundComponent->IsActorWithinLandParcelBounds(Other);
	
	return bIsWithinBounds;
}

bool ALandParcelBoundsActor::GetLandParcelBounds_Implementation(FTransform& OutBoundsTransform, FVector& OutBoundsExtent) const
{
	if (!ParcelBoundsBox)
	{
		return false;
	}

	OutBoundsTransform = ParcelBoundsBox->GetComponentTransform();
	OutBoundsExtent = ParcelBoundsBox->GetScaledBoxExtent();

	return true;
}

FName ALandParcelBoundsActor::GetLandParcelOwnerName_Implementation() const
{
	return DefaultLandParcelOwnerName;
}


#pragma endregion
