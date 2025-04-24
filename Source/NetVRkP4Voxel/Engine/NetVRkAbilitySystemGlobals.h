#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "NetVRkAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class NETVRKP4VOXEL_API UNetVRkAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()


	/** Should be called once as part of project setup to load global data tables and tags */
	virtual void InitGlobalData() override;
};
