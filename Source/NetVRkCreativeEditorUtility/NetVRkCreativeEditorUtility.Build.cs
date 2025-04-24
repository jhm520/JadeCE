using UnrealBuildTool;

public class NetVRkCreativeEditorUtility : ModuleRules
{
	public NetVRkCreativeEditorUtility(ReadOnlyTargetRules Target) : base(Target)
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
            "GameplayTags",
            "GASUtility",
            "NetVRkSurfaceGridSystem"
            
        });
		
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {"EditorScriptingUtilities", "UnrealEd", "AssetTools"});
		}
		else
		{
			PrivateDependencyModuleNames.AddRange(new string[] { });
		}
		
		//PublicIncludePaths.Add("CoreUtility");
	}
}
