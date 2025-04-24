// Fill out your copyright notice in the Description page of Project Settings.


#include "SurfaceGridActor.h"

#include "BaseGizmos/GizmoMath.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
#pragma region Framework

ASurfaceGridActor::ASurfaceGridActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false;
}

// Called when the game starts or when spawned
void ASurfaceGridActor::BeginPlay()
{
	Super::BeginPlay();

	const FVector& OriginalLocation = GetActorLocation();
	
	const FVector& NewLocation = SnapPointToGrid(OriginalLocation);
	
	SetActorLocation(NewLocation);

	//RegenerateSurfaceGrid();
	
}

// Called every frame
void ASurfaceGridActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<FVector> SurfaceGridSquareLocations;
	GridPointMap.GenerateValueArray(SurfaceGridSquareLocations);

	// for (const FVector& SurfaceGridLocation : SurfaceGridSquareLocations)
	// {
	// 	DrawDebugPoint(GetWorld(), SurfaceGridLocation, 10, FColor::Red, false, 0.1f, 0);
	// }
}
#pragma endregion

#pragma region Surface Grid

void ASurfaceGridActor::RegenerateSurfaceGrid()
{
	FScopeLock Lock(&DataGuard);
	
	float CurrentX = -GridRadius;
	float CurrentY = -GridRadius;

	const FVector& CurrentActorLocation = GetActorLocation();

	const FTransform& CurrentActorTransform = GetActorTransform();

	while(CurrentY <= GridRadius)
	{
		CurrentX = -GridRadius;
		
		while (CurrentX <= GridRadius)
		{
			const FVector& Start = UKismetMathLibrary::TransformLocation(CurrentActorTransform, FVector(CurrentX, CurrentY, GridTraceZDistance));
			const FVector& End = UKismetMathLibrary::TransformLocation(CurrentActorTransform, FVector(CurrentX, CurrentY, -GridTraceZDistance));
			
			FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
			FCollisionResponseParams CollisionResponseParams = FCollisionResponseParams::DefaultResponseParam;
			
			FHitResult HitResult;
			
			const bool bGotHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, GridTraceChannel, CollisionQueryParams, CollisionResponseParams);
			
			GridPointMap.Add(FVector2D(CurrentX, CurrentY), HitResult.ImpactPoint);
			
			CurrentX += GridSpacing;
		}
		
		CurrentY += GridSpacing;
	}
	
}

void ASurfaceGridActor::RegenerateSurfaceGrid_Async()
{
	if (bSurfaceGridGenerationInProgress)
	{
		return;
	}

	bSurfaceGridGenerationInProgress = true;
	
	OnGridPointMapGeneratedDelegate.AddUObject(this, &ASurfaceGridActor::OnGridPointMapRegenerated);
	RegenerateSurfaceGridAsyncTask = new FAsyncTask<FAsyncTask_GenerateGridPointMap>(this, GridPointMap, OnGridPointMapGeneratedDelegate);
	RegenerateSurfaceGridAsyncTask->StartBackgroundTask();
}

void ASurfaceGridActor::OnGridPointMapRegenerated()
{
	bSurfaceGridGenerationFinished = true; 
	OnSurfaceGridGeneratedAsyncDelegate.Broadcast();
}

#pragma endregion

#pragma region Surface Grid | Query

bool ASurfaceGridActor::GetPointOnSurfaceNearestToWorldLocation(const FVector& InWorldLocation, FVector& OutSurfaceLocation) const
{
	//transform the location to the actor's local space
	const FTransform& CurrentActorTransform = GetActorTransform();
	const FVector& TransformedLocation = UKismetMathLibrary::InverseTransformLocation(CurrentActorTransform, InWorldLocation);

	//get the 4 corners of the grid square that the location is in
	const FVector& SnappedCornerLocation1 = SnapPointToGrid(TransformedLocation);
	const FVector& SnappedCornerLocation2 = SnapPointToGrid(TransformedLocation + FVector(FMath::Sign(TransformedLocation.X) * GridSpacing, 0, 0));
	const FVector& SnappedCornerLocation3 = SnapPointToGrid(TransformedLocation + FVector(0, FMath::Sign(TransformedLocation.Y) * GridSpacing, 0));
	const FVector& SnappedCornerLocation4 = SnapPointToGrid(TransformedLocation + FVector(FMath::Sign(TransformedLocation.X) * GridSpacing, FMath::Sign(TransformedLocation.Y) * GridSpacing, 0));

	//convert the 4 corners to 2D
	const FVector2D& SnappedCornerLocation1_2D = FVector2D(SnappedCornerLocation1.X, SnappedCornerLocation1.Y);
	const FVector2D& SnappedCornerLocation2_2D = FVector2D(SnappedCornerLocation2.X, SnappedCornerLocation2.Y);
	const FVector2D& SnappedCornerLocation3_2D = FVector2D(SnappedCornerLocation3.X, SnappedCornerLocation3.Y);
	const FVector2D& SnappedCornerLocation4_2D = FVector2D(SnappedCornerLocation4.X, SnappedCornerLocation4.Y);

	//find the 4 corners in the grid map
	const FVector* SurfaceGridLocation1Ptr = GridPointMap.Find(SnappedCornerLocation1_2D);
	const FVector* SurfaceGridLocation2Ptr = GridPointMap.Find(SnappedCornerLocation2_2D);
	const FVector* SurfaceGridLocation3Ptr = GridPointMap.Find(SnappedCornerLocation3_2D);
	const FVector* SurfaceGridLocation4Ptr = GridPointMap.Find(SnappedCornerLocation4_2D);

	//validate the 4 corners
	if (!SurfaceGridLocation1Ptr || !SurfaceGridLocation2Ptr || !SurfaceGridLocation3Ptr || !SurfaceGridLocation4Ptr)
	{
		return false;
	}

	//get the 4 corner locations on the grid map

	TArray<FVector> SurfaceGridSquareLocations;
	SurfaceGridSquareLocations.Add(*SurfaceGridLocation1Ptr);
	SurfaceGridSquareLocations.Add(*SurfaceGridLocation2Ptr);
	SurfaceGridSquareLocations.Add(*SurfaceGridLocation3Ptr);
	SurfaceGridSquareLocations.Add(*SurfaceGridLocation4Ptr);
	

	TArray<FVector> BestTriangle;

	//get the best triangle from the square
	const bool bGotTriangle = GetBestTriangleFromSquare(TransformedLocation, SurfaceGridSquareLocations, BestTriangle);

	if (!bGotTriangle)
	{
		return false;
	}

	//get the normal of the plane
	const FVector& PlaneNormal = FVector::CrossProduct(BestTriangle[1] - BestTriangle[0], BestTriangle[2] - BestTriangle[0]).GetSafeNormal();

	//get the plane base from the average of the 3 triangle points
	const FVector& PlaneBase = (BestTriangle[0] + BestTriangle[1] + BestTriangle[2]) / 3;

	//DrawDebugSphere(GetWorld(), PlaneBase, 10, 12, FColor::Blue, false, 30, 0, 2);
	
	//project the location onto the plane, return it
	OutSurfaceLocation = UKismetMathLibrary::ProjectPointOnToPlane(InWorldLocation, PlaneBase, PlaneNormal);
	
	return true;
}

bool ASurfaceGridActor::GetRayTraceSurfaceIntersectionPoint(const FVector& InRayTraceOrigin, const FVector& InRayTraceDirection, bool& bOutIntersects, FVector& OutSurfaceLocation, const bool bUseOptionalGridOriginPoint, const FVector& OptionalGridOriginPoint) const
{
		//transform the location to the actor's local space
	const FTransform& CurrentActorTransform = GetActorTransform();
	
	const FVector& TransformedLocation = UKismetMathLibrary::InverseTransformLocation(CurrentActorTransform, bUseOptionalGridOriginPoint ? OptionalGridOriginPoint : InRayTraceOrigin);

	//get the 4 corners of the grid square that the location is in
	const FVector& SnappedCornerLocation1 = SnapPointToGrid(TransformedLocation);
	const FVector& SnappedCornerLocation2 = SnapPointToGrid(TransformedLocation + FVector(FMath::Sign(TransformedLocation.X) * GridSpacing, 0, 0));
	const FVector& SnappedCornerLocation3 = SnapPointToGrid(TransformedLocation + FVector(0, FMath::Sign(TransformedLocation.Y) * GridSpacing, 0));
	const FVector& SnappedCornerLocation4 = SnapPointToGrid(TransformedLocation + FVector(FMath::Sign(TransformedLocation.X) * GridSpacing, FMath::Sign(TransformedLocation.Y) * GridSpacing, 0));

	//convert the 4 corners to 2D
	const FVector2D& SnappedCornerLocation1_2D = FVector2D(SnappedCornerLocation1.X, SnappedCornerLocation1.Y);
	const FVector2D& SnappedCornerLocation2_2D = FVector2D(SnappedCornerLocation2.X, SnappedCornerLocation2.Y);
	const FVector2D& SnappedCornerLocation3_2D = FVector2D(SnappedCornerLocation3.X, SnappedCornerLocation3.Y);
	const FVector2D& SnappedCornerLocation4_2D = FVector2D(SnappedCornerLocation4.X, SnappedCornerLocation4.Y);

	//find the 4 corners in the grid map
	const FVector* SurfaceGridLocation1Ptr = GridPointMap.Find(SnappedCornerLocation1_2D);
	const FVector* SurfaceGridLocation2Ptr = GridPointMap.Find(SnappedCornerLocation2_2D);
	const FVector* SurfaceGridLocation3Ptr = GridPointMap.Find(SnappedCornerLocation3_2D);
	const FVector* SurfaceGridLocation4Ptr = GridPointMap.Find(SnappedCornerLocation4_2D);

	//validate the 4 corners
	if (!SurfaceGridLocation1Ptr || !SurfaceGridLocation2Ptr || !SurfaceGridLocation3Ptr || !SurfaceGridLocation4Ptr)
	{
		return false;
	}

	//get the 4 corner locations on the grid map

	TArray<FVector> SurfaceGridSquareLocations;
	SurfaceGridSquareLocations.Add(*SurfaceGridLocation1Ptr);
	SurfaceGridSquareLocations.Add(*SurfaceGridLocation2Ptr);
	SurfaceGridSquareLocations.Add(*SurfaceGridLocation3Ptr);
	SurfaceGridSquareLocations.Add(*SurfaceGridLocation4Ptr);


	// for (const FVector& SurfaceGridLocation : SurfaceGridSquareLocations)
	// {
	// 	DrawDebugSphere(GetWorld(), SurfaceGridLocation, 10, 12, FColor::Green, false, 30, 0, 2);
	// }

	TArray<FVector> BestTriangle;

	//get the best triangle from the square
	const bool bGotTriangle = GetBestTriangleFromSquare(TransformedLocation, SurfaceGridSquareLocations, BestTriangle);

	if (!bGotTriangle)
	{
		return false;
	}

	//get the normal of the plane
	const FVector& PlaneNormal = FVector::CrossProduct(BestTriangle[1] - BestTriangle[0], BestTriangle[2] - BestTriangle[0]).GetSafeNormal();

	//get the plane base from the average of the 3 triangle points
	const FVector& PlaneBase = (BestTriangle[0] + BestTriangle[1] + BestTriangle[2]) / 3;

	//DrawDebugSphere(GetWorld(), PlaneBase, 10, 12, FColor::Blue, false, 30, 0, 2);
	
	//project the location onto the plane, return it
	GizmoMath::RayPlaneIntersectionPoint(PlaneBase, PlaneNormal, InRayTraceOrigin, InRayTraceDirection, bOutIntersects, OutSurfaceLocation);
	
	if (!bOutIntersects)
	{
		return false;
	}
	
	return true;
}

FVector ASurfaceGridActor::SnapPointToGrid(const FVector& InWorldLocation) const
{
	const FVector& OriginalLocation = InWorldLocation;

	const int32 OriginalLocationXRounded = FMath::RoundToInt32(OriginalLocation.X);
	const int32 OriginalLocationYRounded = FMath::RoundToInt32(OriginalLocation.Y);
	const int32 OriginalLocationZRounded = FMath::RoundToInt32(OriginalLocation.Z);

	const FVector& OriginalLocationRounded = FVector(OriginalLocationXRounded, OriginalLocationYRounded, OriginalLocationZRounded);

	int32 ModOffsetX = OriginalLocationXRounded % GridSpacing;
	int32 ModOffsetY = OriginalLocationYRounded % GridSpacing;
	int32 ModOffsetZ = OriginalLocationZRounded % GridSpacing;

	// if (FMath::Abs(ModOffsetX) > (GridSpacing/2))
	// {
	// 	const int32 Sign = FMath::Sign(ModOffsetX) * -1;
	// 	ModOffsetX = (GridSpacing - FMath::Abs(ModOffsetX)) * Sign;
	// }
	//
	// if (FMath::Abs(ModOffsetY) > (GridSpacing/2))
	// {
	// 	const int32 Sign = FMath::Sign(ModOffsetY) * -1;
	// 	ModOffsetY = (GridSpacing - FMath::Abs(ModOffsetY)) * Sign;
	// }
	//
	// if (FMath::Abs(ModOffsetZ) > (GridSpacing/2))
	// {
	// 	const int32 Sign = FMath::Sign(ModOffsetZ) * -1;
	// 	ModOffsetZ = (GridSpacing - FMath::Abs(ModOffsetZ)) * Sign;
	// }

	const FVector& ModOffsetLocation = FVector(ModOffsetX, ModOffsetY, ModOffsetZ);

	const FVector& AdjustedLocation = OriginalLocationRounded - ModOffsetLocation;

	return AdjustedLocation;
}

bool ASurfaceGridActor::GetBestTriangleFromSquare(const FVector& InWorldLocation, const TArray<FVector>& SquareArray, TArray<FVector>& OutTriangleArray) const
{
	if (SquareArray.Num() != 4)
	{
		return false;
	}

	TArray<FVector> SquareArrayCopy = SquareArray;
	
	int32 ClosestIndex = -1;

	for (int32 j = 0; j < 3; j++)
	{
		float ClosestDistance = TNumericLimits<float>::Max();
		
		for (int32 i = 0; i < SquareArrayCopy.Num(); i++)
		{
			const float CurrentDistance = FVector::Dist(SquareArrayCopy[i], InWorldLocation);

			if (CurrentDistance < ClosestDistance)
			{
				ClosestDistance = CurrentDistance;
				ClosestIndex = i;
			}
		}

		OutTriangleArray.Add(SquareArrayCopy[ClosestIndex]);

		SquareArrayCopy.RemoveAt(ClosestIndex);
	}

	if (OutTriangleArray.Num() != 3)
	{
		return false;
	}
	
	return true;
}

#pragma endregion

#pragma region Surface Grid | Async Task

void FAsyncTask_GenerateGridPointMap::DoWork()
{
	if (!SurfaceGridActor)
	{
		return;
	}

	SurfaceGridActor->RegenerateSurfaceGrid();
	
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		OnGridPointMapGenerated.Broadcast();
	});	
}

#pragma endregion