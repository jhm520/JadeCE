#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "NetVRkAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class NETVRKP4VOXEL_API UNetVRkAssetManager : public UAssetManager
{
	GENERATED_BODY()

	virtual void StartInitialLoading() override;
	
};
