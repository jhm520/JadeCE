// Fill out your copyright notice in the Description page of Project Settings.


#include "NetVRkMathLibrary.h"

#include "Kismet/KismetMathLibrary.h"


bool UNetVRkMathLibrary::IsBoxWithinBoxWithTransform(const FTransform& BoxTransform, const FVector& BoxExtent, const FTransform& OuterBoxTransform, const FVector& OuterBoxExtent)
{
	TArray<FVector> BoxCorners;
	BoxCorners.Add(FVector(BoxExtent.X, BoxExtent.Y, BoxExtent.Z));
	BoxCorners.Add(FVector(BoxExtent.X, -BoxExtent.Y, BoxExtent.Z));
	BoxCorners.Add(FVector(-BoxExtent.X, BoxExtent.Y, BoxExtent.Z));
	BoxCorners.Add(FVector(-BoxExtent.X, -BoxExtent.Y, BoxExtent.Z));
	BoxCorners.Add(FVector(BoxExtent.X, BoxExtent.Y, -BoxExtent.Z));
	BoxCorners.Add(FVector(BoxExtent.X, -BoxExtent.Y, -BoxExtent.Z));
	BoxCorners.Add(FVector(-BoxExtent.X, BoxExtent.Y, -BoxExtent.Z));
	BoxCorners.Add(FVector(-BoxExtent.X, -BoxExtent.Y, -BoxExtent.Z));
	
	for (const FVector& BoxCorner : BoxCorners)
	{
		const FVector& BoxCornerLocation = UKismetMathLibrary::TransformLocation(BoxTransform, BoxCorner);
		
		const bool bPointIsInBox = UKismetMathLibrary::IsPointInBoxWithTransform(BoxCornerLocation, OuterBoxTransform, OuterBoxExtent);
		
		if (!bPointIsInBox)
		{
			return false;
		}
	}
	
	return true;
}

bool UNetVRkMathLibrary::IsBoxWithinBox(const FVector& BoxOrigin, const FVector& BoxExtent, const FVector& OuterBoxOrigin, const FVector& OuterBoxExtent)
{
	TArray<FVector> BoxCorners;
	BoxCorners.Add(BoxOrigin + FVector(BoxExtent.X, BoxExtent.Y, BoxExtent.Z));
	BoxCorners.Add(BoxOrigin + FVector(BoxExtent.X, -BoxExtent.Y, BoxExtent.Z));
	BoxCorners.Add(BoxOrigin + FVector(-BoxExtent.X, BoxExtent.Y, BoxExtent.Z));
	BoxCorners.Add(BoxOrigin + FVector(-BoxExtent.X, -BoxExtent.Y, BoxExtent.Z));
	BoxCorners.Add(BoxOrigin + FVector(BoxExtent.X, BoxExtent.Y, -BoxExtent.Z));
	BoxCorners.Add(BoxOrigin + FVector(BoxExtent.X, -BoxExtent.Y, -BoxExtent.Z));
	BoxCorners.Add(BoxOrigin + FVector(-BoxExtent.X, BoxExtent.Y, -BoxExtent.Z));
	BoxCorners.Add(BoxOrigin + FVector(-BoxExtent.X, -BoxExtent.Y, -BoxExtent.Z));

	//UKismetMathLibrary::IsPointInBoxWithTransform()
	
	for (const FVector& BoxCorner : BoxCorners)
	{
		if (!UKismetMathLibrary::IsPointInBox(BoxCorner, OuterBoxOrigin, OuterBoxExtent))
		{
			return false;
		}
	}
	
	return true;
}

bool pnpoly(int nvert, float *vertx, float *verty, float testx, float testy)
{
	int i = 0;
	int j = 0;
	
	bool bInside = false;
	
	for (i = 0, j = nvert-1; i < nvert; j = i++)
	{
		const bool bBelowP1y = testy < verty[i];
		const bool bBelowP2y = testy < verty[j];

		const bool bBetweenSegmentY = (bBelowP1y != bBelowP2y);

		const float SegmentXDistance = vertx[j] - vertx[i];
		const float SegmentYDistance = verty[j] - verty[i];
		const float P1XDistance = testx - vertx[i];
		const float P1YDistance = testy - verty[i];
		
		if (bBetweenSegmentY && (P1XDistance < ((P1YDistance * SegmentXDistance) / SegmentYDistance)))
		{
			bInside = !bInside;
		}
	}
	
	return bInside;
}

bool UNetVRkMathLibrary::IsPointInPolygon(const FVector2D& InPoint, const TArray<FVector2D>& PolygonPoints)
{
	const int32 NumVertices = PolygonPoints.Num();
	
	if (NumVertices < 3)
	{
		return false;
	}

	float* VertX = new float[NumVertices];
	float* VertY = new float[NumVertices];

	for (int32 i = 0; i < NumVertices; i++)
	{
		VertX[i] = PolygonPoints[i].X;
		VertY[i] = PolygonPoints[i].Y;
	}

	bool bResult = pnpoly(NumVertices, VertX, VertY, InPoint.X, InPoint.Y) != 0;

	delete [] VertX;
	delete [] VertY;
	
	return bResult;
}

FVector2D ClosestPointOnSegment(const FVector2D &Point, const FVector2D &StartPoint, const FVector2D &EndPoint)
{
	const FVector2D Segment = EndPoint - StartPoint;
	const FVector2D VectToPoint = Point - StartPoint;

	// See if closest point is before StartPoint
	const FVector2D::FReal Dot1 = VectToPoint | Segment;
	if( Dot1 <= 0 )
	{
		return StartPoint;
	}

	// See if closest point is beyond EndPoint
	const FVector2D::FReal Dot2 = Segment | Segment;
	if( Dot2 <= Dot1 )
	{
		return EndPoint;
	}

	// Closest Point is within segment
	return StartPoint + Segment * (Dot1 / Dot2);
}

float GetVector2DSize(const FVector2D& InVector)
{
	return FMath::Sqrt(InVector.X*InVector.X + InVector.Y*InVector.Y);
}

float UNetVRkMathLibrary::GetDistanceFromPointToLineSegment(const FVector2D& InPoint, const FVector2D& LineSegmentStart, const FVector2D& LineSegmentEnd)
{
	const FVector2D ClosestPoint = ClosestPointOnSegment(InPoint, LineSegmentStart, LineSegmentEnd);

	const FVector2D DistVect = InPoint - ClosestPoint;

	const float Size = GetVector2DSize(DistVect);
	
	return Size;
}

float UNetVRkMathLibrary::GetClosestDistanceFromPointToPolygonEdge(const FVector2D& InPoint, const TArray<FVector2D>& PolygonPoints)
{
	int32 i = 0;
	int32 j = 0;
	int32 n = PolygonPoints.Num();

	float MinDistance = MAX_FLT;
	
	for (i = 0, j = n-1; i < n; j = i++)
	{
		const FVector2D& P1 = PolygonPoints[i];
		const FVector2D& P2 = PolygonPoints[j];

		const float Distance = GetDistanceFromPointToLineSegment(InPoint, P1, P2);

		if (Distance < MinDistance)
		{
			MinDistance = Distance;
		}
	}
	
	return MinDistance;
}
	

