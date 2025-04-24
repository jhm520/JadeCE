using UnrealBuildTool;

public class NetVRkCoreUtility : ModuleRules
{
	public NetVRkCoreUtility(ReadOnlyTargetRules Target) : base(Target)
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
            "InputCore",
            "GameplayTags",
            "GameplayAbilities",
            "EnhancedInput",
            "GASUtility"
            
        });
		
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {"UnrealEd"});
		}
		else
		{
			PrivateDependencyModuleNames.AddRange(new string[] { });
		}
		
		/*NetVRk*/
		PublicDependencyModuleNames.AddRange(new string[]
		{
			
		});
		
		//PublicIncludePaths.Add("CoreUtility");
	}
}
