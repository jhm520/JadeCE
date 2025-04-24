// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NetVRkMathLibrary.generated.h"

/**
 * 
 */
UCLASS()
class NETVRKCOREUTILITY_API UNetVRkMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	//returns true if an entire box is within another box
	UFUNCTION(BlueprintPure, Category = "Math|Box")
	static bool IsBoxWithinBoxWithTransform(const FTransform& BoxTransform, const FVector& BoxExtent, const FTransform& OuterBoxTransform, const FVector& OuterBoxExtent);
	
	//returns true if an entire box is within another box
	UFUNCTION(BlueprintPure, Category = "Math|Box")
	static bool IsBoxWithinBox(const FVector& BoxOrigin, const FVector& BoxExtent, const FVector& OuterBoxOrigin, const FVector& OuterBoxExtent);

	UFUNCTION(BlueprintPure, Category = "Math|Vector")
	static bool IsPointInPolygon(const FVector2D& InPoint, const TArray<FVector2D>& PolygonPoints);

	UFUNCTION(BlueprintPure, Category = "Math|Vector")
	static float GetDistanceFromPointToLineSegment(const FVector2D& InPoint, const FVector2D& LineSegmentStart, const FVector2D& LineSegmentEnd);

	UFUNCTION(BlueprintPure, Category = "Math|Vector")
	static float GetClosestDistanceFromPointToPolygonEdge(const FVector2D& InPoint, const TArray<FVector2D>& PolygonPoints);
	
};
