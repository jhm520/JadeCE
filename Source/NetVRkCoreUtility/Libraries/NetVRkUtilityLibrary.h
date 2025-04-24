// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NetVRkUtilityLibrary.generated.h"

/**
 * 
 */
UCLASS()
class NETVRKCOREUTILITY_API UNetVRkUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION()
	static void SerializeObjectToByteBuffer(UObject* target, TArray<uint8>& OutData);
	
	UFUNCTION()
	static void DeserializeObjectFromByteBuffer(UObject* target, const TArray<uint8>& InData);
	
};
