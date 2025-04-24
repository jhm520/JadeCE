#include "NetVRkVoxelSculptGameplayTags.h"

namespace NetVRkVoxelSculptGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(SphereRadius, TEXT("NetVRk.Voxel.SculptTool.Parameter.SphereRadius"))
	UE_DEFINE_GAMEPLAY_TAG(BoxExtent, TEXT("NetVRk.Voxel.SculptTool.Parameter.BoxExtent"))
	UE_DEFINE_GAMEPLAY_TAG(Smoothness, TEXT("NetVRk.Voxel.SculptTool.Parameter.Smoothness"))
	UE_DEFINE_GAMEPLAY_TAG(ShapeType, TEXT("NetVRk.Voxel.SculptTool.Parameter.ShapeType"))
	UE_DEFINE_GAMEPLAY_TAG(StaticMesh, TEXT("NetVRk.Voxel.SculptTool.Parameter.StaticMesh"))
	UE_DEFINE_GAMEPLAY_TAG(Subtractive, TEXT("NetVRk.Voxel.SculptTool.Parameter.Subtractive"))
	UE_DEFINE_GAMEPLAY_TAG(ShowPreview, TEXT("NetVRk.Voxel.SculptTool.Parameter.ShowPreview"))
	UE_DEFINE_GAMEPLAY_TAG(EnableSculpting, TEXT("NetVRk.Voxel.SculptTool.Parameter.EnableSculpting"))
	UE_DEFINE_GAMEPLAY_TAG(Channel, TEXT("NetVRk.Voxel.SculptTool.Parameter.Channel"))
	UE_DEFINE_GAMEPLAY_TAG(RuntimeEnabled, TEXT("NetVRk.Voxel.SculptTool.Parameter.RuntimeEnabled"))
	UE_DEFINE_GAMEPLAY_TAG(ValidPlacement, TEXT("NetVRk.Voxel.SculptTool.Parameter.ValidPlacement"))

	
}

namespace NetVRkVoxelSculptEditMoveGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Move, TEXT("NetVRk.Voxel.SculptTool.EditMove.Move"))
	UE_DEFINE_GAMEPLAY_TAG(MoveToLocation, TEXT("NetVRk.Voxel.SculptTool.EditMove.MoveToLocation"))
	UE_DEFINE_GAMEPLAY_TAG(Rotate, TEXT("NetVRk.Voxel.SculptTool.EditMove.Rotate"))
	UE_DEFINE_GAMEPLAY_TAG(SetRotation, TEXT("NetVRk.Voxel.SculptTool.EditMove.SetRotation"))
	UE_DEFINE_GAMEPLAY_TAG(FollowActor, TEXT("NetVRk.Voxel.SculptTool.EditMove.FollowActor"))
	UE_DEFINE_GAMEPLAY_TAG(StopFollowingActor, TEXT("NetVRk.Voxel.SculptTool.EditMove.StopFollowingActor"))
	
}

namespace NetVRkVoxelSculptSmoothBrushGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(SmoothBrush, TEXT("NetVRk.Voxel.SculptTool.SmoothBrush"))
	UE_DEFINE_GAMEPLAY_TAG(StopSmoothBrush, TEXT("NetVRk.Voxel.SculptTool.StopSmoothBrush"))
}