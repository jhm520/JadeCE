#include "WorldPointerComponent.h"

#include "ClickingComponent.h"

#pragma region Framework

// Sets default values for this component's properties
UWorldPointerComponent::UWorldPointerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWorldPointerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWorldPointerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsPointing)
	{
		UpdatePointing();
	}
	else
	{
		CachedPointerHitResult = FHitResult();
	}
	// ...
}

#pragma endregion

#pragma region Pointing

bool UWorldPointerComponent::GetPointerHitResult(FHitResult& OutHitResult) const
{
	if (!bIsPointing)
	{
		return false;
	}
	
	OutHitResult = CachedPointerHitResult;
	return true;
}

void UWorldPointerComponent::UpdatePointing()
{
	FHitResult Hit;

	FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
	Params.AddIgnoredActor(GetOwner());

	FCollisionResponseParams ResponseParams = FCollisionResponseParams::DefaultResponseParam;
	
	const bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(Hit, GetComponentLocation(), GetComponentLocation() + GetForwardVector() * MaxPointerDistance, PointingCollisionChannel, Params, ResponseParams);

	if (bBlockingHit)
	{
		CachedPointerHitResult = Hit;

		if (bDrawDebug)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.0f, 12, FColor::Red, false, 0.1f);
		}
	}
}

#pragma endregion