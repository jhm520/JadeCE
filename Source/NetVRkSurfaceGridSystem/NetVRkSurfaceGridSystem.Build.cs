using UnrealBuildTool;

public class NetVRkSurfaceGridSystem : ModuleRules
{
	public NetVRkSurfaceGridSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] {});

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		/*
		* Engine Modules
		*/
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"NetCore",
            "CoreUObject",
            "Engine",
            "Json",
            "JsonUtilities",
            "HTTP",
            "Slate",
            "SlateCore",
            "PlatformCrypto",
            "DeveloperSettings",
            "UMG",
            "AudioExtensions",
            "InteractiveToolsFramework"
            
        });
		
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {"UnrealEd" });
		}
		else
		{
			PrivateDependencyModuleNames.AddRange(new string[] { });
		}
		
		//PublicIncludePaths.Add("CoreUtility");
	}
}
