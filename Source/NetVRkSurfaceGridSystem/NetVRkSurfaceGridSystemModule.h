#pragma once

#include "Modules/ModuleManager.h"

class FNetVRkSurfaceGridSystemModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
