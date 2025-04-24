
#include "NetVRkAssetManager.h"

#include "AbilitySystemGlobals.h"

void UNetVRkAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}
