// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Async/AsyncWork.h"
#include "SurfaceGridActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSuraceGridGeneratedAsyncDelegate);

DECLARE_MULTICAST_DELEGATE(FOnGridPointMapGeneratedDelegate);

#pragma region Surface Grid | Async Task

class FAsyncTask_GenerateGridPointMap : public FNonAbandonableTask
{
public:
	friend class FAutoDeleteAsyncTask<FAsyncTask_GenerateGridPointMap>;

	ASurfaceGridActor* SurfaceGridActor;
	TMap<FVector2D, FVector>& ThreadedGridPointMapRef;
	FOnGridPointMapGeneratedDelegate OnGridPointMapGenerated;

	FAsyncTask_GenerateGridPointMap(ASurfaceGridActor* InSurfaceGridActor, TMap<FVector2D, FVector>& InThreadedGridPointMapRef, FOnGridPointMapGeneratedDelegate InOnGridPointMapGenerated)
		: SurfaceGridActor(InSurfaceGridActor), ThreadedGridPointMapRef(InThreadedGridPointMapRef), OnGridPointMapGenerated(InOnGridPointMapGenerated)
	{

	}

	void DoWork();

	FORCEINLINE TStatId GetStatId() const { // Probably declares the Task to the TaskGraph
		RETURN_QUICK_DECLARE_CYCLE_STAT(FMyTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};
#pragma endregion

UCLASS()
class NETVRKSURFACEGRIDSYSTEM_API ASurfaceGridActor : public AActor
{
	GENERATED_BODY()

#pragma region Framework
public:	
	// Sets default values for this actor's properties
	ASurfaceGridActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
#pragma endregion


#pragma region Surface Grid | Properties

protected:
	//how big the grid is
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NetVRk|Surface Grid")
	int32 GridRadius = 1000.0f;

	//how far apart each grid line is
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NetVRk|Surface Grid")
	int32 GridSpacing = 100.0f;
	
	//how far apart each grid line is
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NetVRk|Surface Grid")
	int32 GridTraceZDistance = 10000.0f;

	//the trace channel used to generate the grid
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NetVRk|Surface Grid")
	TEnumAsByte<ECollisionChannel> GridTraceChannel = ECollisionChannel::ECC_Visibility;

	// Marked as mutable as it's often warranted by design
	mutable FCriticalSection DataGuard;
	
	/*	The map that represents the generated grid.
	 *	The key is a FVector2D that represents the X,Y coordinate of a ray trace point relative to the actor.
	 *	The value is an FVector that represents the ray trace collision point in world space.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "NetVRk|Surface Grid")
	TMap<FVector2D, FVector> GridPointMap;

	FAsyncTask<FAsyncTask_GenerateGridPointMap>* RegenerateSurfaceGridAsyncTask = nullptr;

#pragma endregion
	
#pragma region Surface Grid | Generation
protected:
	FOnGridPointMapGeneratedDelegate OnGridPointMapGeneratedDelegate;

public:
	/*	Regenerates the surface grid, populating GridPointMap with new values.
	 *	Does a number of ray traces radiating outwards from the center of the actor,
	 *	straight down relative to the actor.
	 *	Populates the GridPointMap with the 2D relative values as keys and the trace hit locations as the values
	 */
	UFUNCTION(BlueprintCallable, Category = "NetVRk|Surface Grid")
	void RegenerateSurfaceGrid();

	/*	Does the same thing as RegenerateSurfaceGrid(),
	 *	but does so on a separate thread to avoid hitches in the game thread
	 */
	// Begins regenerating the surface grid, populating GridPointMap with new values, but does so on a separate thread
	UFUNCTION(BlueprintCallable, Category = "NetVRk|Surface Grid")
	void RegenerateSurfaceGrid_Async();

	UPROPERTY(BlueprintReadOnly, Category = "NetVRk|Surface Grid")
	bool bSurfaceGridGenerationInProgress = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "NetVRk|Surface Grid")
	bool bSurfaceGridGenerationFinished = false;

	UPROPERTY(BlueprintAssignable)
	FOnSuraceGridGeneratedAsyncDelegate OnSurfaceGridGeneratedAsyncDelegate;

	void OnGridPointMapRegenerated();
#pragma endregion


#pragma region Surface Grid | Query

	/*	Given a world location, finds the nearest point on the surface grid to that location and returns it in OutSurfaceLocation
	 *	Returns true if success, false if not
	 */
	UFUNCTION(BlueprintPure, Category = "NetVRk|Surface Grid")
	bool GetPointOnSurfaceNearestToWorldLocation(const FVector& InWorldLocation, FVector& OutSurfaceLocation) const;

	/* Given a ray trace origin location, and ray trace direction,
	 * finds the point on the surface grid that the ray trace intersects
	 * bOutIntersects is true if the ray trace intersects, false if not
	 * OutSurfaceLocation is the point on the surface grid that the ray trace intersects
	 * bUseOptionalGridOriginPoint is true if you want to use a different origin point for the grid
	 * OptionalGridOriginPoint is the origin point to use if bUseOptionalGridOriginPoint is true
	 *		- The reason an OptionalGridOriginPoint may need to be specified
	 *		is so the surface grid can determine which "grid square and triangle"
	 *		to use when calculating the plane for the intersection plane
	 * Returns true if success, false if not
	 *
	 */
	UFUNCTION(BlueprintPure, Category = "NetVRk|Surface Grid")
	bool GetRayTraceSurfaceIntersectionPoint(const FVector& InRayTraceOrigin, const FVector& InRayTraceDirection, bool& bOutIntersects, FVector& OutSurfaceLocation, const bool bUseOptionalGridOriginPoint = false, const FVector& OptionalGridOriginPoint = FVector::ZeroVector) const;

	/* Given a world location, finds the nearest grid point on the surface grid to that location.
	 * Other functions of ASurfaceGridActor use this in various ways.
	 *
	 */
	UFUNCTION(BlueprintPure, Category = "NetVRk|Surface Grid")
	FVector SnapPointToGrid(const FVector& InWorldLocation) const;

	/* Given a world location, and a set of points in world space representing a "grid square" (4 adjacent grid points),
	 * populates OutTriangleArray with 3 points that represent the "best" triangle to use for the given world location
	 * the triangle array output by this function is used to calculate a normal that represents the tangent plane of the surface grid at the given world location
	 * returns True if success, false if otherwise
	 */
	UFUNCTION(BlueprintPure, Category = "NetVRk|Surface Grid")
	bool GetBestTriangleFromSquare(const FVector& InWorldLocation, const TArray<FVector>& SquareArray, TArray<FVector>& OutTriangleArray) const;
	
#pragma endregion

};
