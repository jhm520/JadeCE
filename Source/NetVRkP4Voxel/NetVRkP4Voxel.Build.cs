// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NetVRkP4Voxel : ModuleRules
{
	public NetVRkP4Voxel(ReadOnlyTargetRules Target) : base(Target)
	{
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {"UnrealEd" });
		}
		else
		{
			PrivateDependencyModuleNames.AddRange(new string[] { });
		}
		
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "SelectionSystem" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HeadMountedDisplay",
			"EnhancedInput",
			"Json",
			"JsonUtilities",
			"NetVRkCoreUtility",
			"InteractionSystem",
			"NetVRkLandParcelSystem",
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags",
			"GASUtility",
			"NetVRkCreativeEditorUtility",
			"NetVRkSaveGameUtility",
			"NetVRkFileTransferSystem",
			"NetVRkSurfaceGridSystem"
		});
	}
}
