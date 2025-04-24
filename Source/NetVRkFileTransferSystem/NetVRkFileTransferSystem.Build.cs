using UnrealBuildTool;

public class NetVRkFileTransferSystem : ModuleRules
{
	public NetVRkFileTransferSystem(ReadOnlyTargetRules Target) : base(Target)
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
            "EnhancedInput",
            "InputCore",
            "InteractiveToolsFramework",
            "GameplayAbilities",
            "GameplayTags"
            
        });
		
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {"EditorScriptingUtilities", "UnrealEd", "AssetTools" });
		}
		else
		{
			PrivateDependencyModuleNames.AddRange(new string[] { });
		}
		
		//PublicIncludePaths.Add("CoreUtility");
	}
}
