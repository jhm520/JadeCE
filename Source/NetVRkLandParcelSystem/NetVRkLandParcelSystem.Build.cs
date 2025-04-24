using UnrealBuildTool;

public class NetVRkLandParcelSystem : ModuleRules
{
	public NetVRkLandParcelSystem(ReadOnlyTargetRules Target) : base(Target)
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
            "NetVRkCoreUtility"
        });
		
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {"EditorScriptingUtilities", "UnrealEd", "AssetTools" });
		}
		else
		{
			PrivateDependencyModuleNames.AddRange(new string[] { });
		}
		
		
		/*Voxel Plugin*/
		PublicDependencyModuleNames.AddRange(new string[]
		{
		});
		
		//PublicIncludePaths.Add("CoreUtility");
	}
}
