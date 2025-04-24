// Fill out your copyright notice in the Description page of Project Settings.


#include "NetVRkUtilityLibrary.h"

#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

void UNetVRkUtilityLibrary::SerializeObjectToByteBuffer(UObject* target, TArray<uint8>& OutData)
{
	if (!target)
	{
		return;
	}
	
	FMemoryWriter memoryWriter(OutData, false);
	FObjectAndNameAsStringProxyArchive ar(memoryWriter, false);
	target->Serialize(ar);
}

void UNetVRkUtilityLibrary::DeserializeObjectFromByteBuffer(UObject* target, const TArray<uint8>& InData)
{
	if (!target)
	{
		return;
	}

	FMemoryReader memoryReader(InData, false);
	//memoryReader.SetLimitSize(InData.GetAllocatedSize() * 1.5);
	FObjectAndNameAsStringProxyArchive ar(memoryReader, false);
	target->Serialize(ar);
}
