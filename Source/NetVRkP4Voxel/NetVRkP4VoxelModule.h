#pragma once

#include "Modules/ModuleManager.h"

class FNetVRkP4VoxelModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
