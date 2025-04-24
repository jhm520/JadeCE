// Fill out your copyright notice in the Description page of Project Settings.


#include "NetVRkCreativeEditorBlueprintLibrary.h"

#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "NetVRkCreativeEditorUtility/NetVRkCreativeEditorGameplayTags.h"
#include "NetVRkCreativeEditorUtility/Components/CreativeEditorControllerComponent.h"
#include "NetVRkCreativeEditorUtility/Components/CreativeEditorPawnComponent.h"
#include "NetVRkCreativeEditorUtility/Components/ManipulationComponent.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorActorInterface.h"

#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorControllerInterface.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorPawnInterface.h"

bool UNetVRkCreativeEditorBlueprintLibrary::GetCreativeEditorManipulationModeFromString(const FString& InCreativeModeString, FGameplayTag& OutCreativeModeTag)
{
	if (InCreativeModeString == "Translate" || InCreativeModeString == "Move")
	{
		OutCreativeModeTag = NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Translation;
		return true;
	}

	if (InCreativeModeString == "Rotate")
	{
		OutCreativeModeTag = NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Rotation;
		return true;
	}

	if (InCreativeModeString == "Scale")
	{
		OutCreativeModeTag = NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Scale;
		return true;
	}

	if (InCreativeModeString == "Smooth Brush")
	{
		OutCreativeModeTag = NetVRkCreativeEditorManipulationGameplayTags::Manipulation_SmoothBrush;
		return true;
	}
	
	return false;
}

bool UNetVRkCreativeEditorBlueprintLibrary::ActivateCreativeMode(AActor* Creator, bool bActivate)
{
	ICreativeEditorControllerInterface* ControllerInterface = Cast<ICreativeEditorControllerInterface>(Creator);

	if (ControllerInterface)
	{
		UCreativeEditorControllerComponent* ControllerComponent = ICreativeEditorControllerInterface::Execute_GetCreativeEditorControllerComponent(Creator);

		if (ControllerComponent)
		{
			return ControllerComponent->ActivateCreativeMode(bActivate);
		}
	}

	APawn* Pawn = Cast<APawn>(Creator);

	if (Pawn)
	{
		AController* Controller = Pawn->GetController();

		if (Controller)
		{
			const bool bControllerImplementsInterface = Controller->Implements<UCreativeEditorControllerInterface>();

			if (bControllerImplementsInterface)
			{
				UCreativeEditorControllerComponent* ControllerComponent = ICreativeEditorControllerInterface::Execute_GetCreativeEditorControllerComponent(Controller);

				if (ControllerComponent)
				{
					return ControllerComponent->ActivateCreativeMode(bActivate);
				}
			}
		}
	}
	
	return false;
}

bool UNetVRkCreativeEditorBlueprintLibrary::ToggleCreativeMode(AActor* Creator)
{
	const bool bGotInterface = Creator->Implements<UCreativeEditorControllerInterface>();
	
	if (bGotInterface)
	{
		UCreativeEditorControllerComponent* ControllerComponent = ICreativeEditorControllerInterface::Execute_GetCreativeEditorControllerComponent(Creator);

		if (ControllerComponent)
		{
			return ControllerComponent->ToggleCreativeMode();
		}
	}

	APawn* Pawn = Cast<APawn>(Creator);

	if (Pawn)
	{
		AController* Controller = Pawn->GetController();

		if (Controller)
		{
			const bool bGotPawnInterface = Controller->Implements<UCreativeEditorControllerInterface>();

			if (bGotPawnInterface)
			{
				UCreativeEditorControllerComponent* ControllerComponent = ICreativeEditorControllerInterface::Execute_GetCreativeEditorControllerComponent(Controller);

				if (ControllerComponent)
				{
					return ControllerComponent->ToggleCreativeMode();
				}
			}
		}
	}
	
	return false;
}

bool UNetVRkCreativeEditorBlueprintLibrary::IsManipulating(AActor* Manipulator)
{
	if (!Manipulator)
	{
		return false;
	}
	
	const bool bGotInterface = Manipulator->Implements<UCreativeEditorPawnInterface>();
	
	if (!bGotInterface)
	{
		return false;
	}

	UCreativeEditorPawnComponent* PawnComponent = ICreativeEditorPawnInterface::Execute_GetCreativeEditorPawnComponent(Manipulator);

	if (PawnComponent)
	{
		return PawnComponent->IsManipulating();
	}
		
	return false;
}

void UNetVRkCreativeEditorBlueprintLibrary::StartManipulatingActor(AActor* Manipulator, AActor* ManipulatedActor, UManipulationComponent* ManipulationComponent)
{
	if (!Manipulator)
	{
		return;
	}
	
	const bool bGotInterface = Manipulator->Implements<UCreativeEditorPawnInterface>();
	
	if (!bGotInterface)
	{
		return;
	}

	UCreativeEditorPawnComponent* PawnComponent = ICreativeEditorPawnInterface::Execute_GetCreativeEditorPawnComponent(Manipulator);

	if (PawnComponent)
	{
		return PawnComponent->StartManipulation(ManipulationComponent);
	}
}

void UNetVRkCreativeEditorBlueprintLibrary::StartManipulating(AActor* Manipulator)
{
	if (!Manipulator)
    {
    	return;
    }
    	
    const bool bGotInterface = Manipulator->Implements<UCreativeEditorPawnInterface>();
    
    if (!bGotInterface)
    {
    	return;
    }
    
    UCreativeEditorPawnComponent* PawnComponent = ICreativeEditorPawnInterface::Execute_GetCreativeEditorPawnComponent(Manipulator);

    if (PawnComponent)
    {
    	return PawnComponent->StartManipulation();
    }
}

void UNetVRkCreativeEditorBlueprintLibrary::StopManipulating(AActor* Manipulator)
{
	const bool bGotInterface = Manipulator->Implements<UCreativeEditorPawnInterface>();
	
	if (!bGotInterface)
	{
		return;
	}
	
	UCreativeEditorPawnComponent* PawnComponent = ICreativeEditorPawnInterface::Execute_GetCreativeEditorPawnComponent(Manipulator);

	if (PawnComponent)
	{
		return PawnComponent->StopManipulation();
	}
}

void UNetVRkCreativeEditorBlueprintLibrary::SetActorManipulationMode(AActor* Creator, AActor* ManipulatedActor, FGameplayTag ManipulationModeTag)
{
	UCreativeEditorPawnComponent* PawnComponent = UNetVRkCreativeEditorBlueprintLibrary::GetCreativeEditorPawnComponent(Creator);

	if (!PawnComponent)
	{
		return;
	}

	PawnComponent->SetManipulationMode(ManipulationModeTag, ManipulatedActor);

	
}

UCreativeEditorPawnComponent* UNetVRkCreativeEditorBlueprintLibrary::GetCreativeEditorPawnComponent(AActor* Creator)
{
	APawn* PlayerPawn = Cast<APawn>(Creator);

	if (!PlayerPawn)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Creator);

		if (PlayerController)
		{
			PlayerPawn = PlayerController->GetPawn();
		}
	}
	
	if (PlayerPawn)
	{
		bool bGotPawnInterface = PlayerPawn->Implements<UCreativeEditorPawnInterface>();

		if (bGotPawnInterface)
		{
			UCreativeEditorPawnComponent* PawnComponent = ICreativeEditorPawnInterface::Execute_GetCreativeEditorPawnComponent(PlayerPawn);

			return PawnComponent;
		}
	}
	
	return nullptr;
}

UCreativeEditorActorComponent* UNetVRkCreativeEditorBlueprintLibrary::GetCreativeEditorActorComponent(AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}
	
	bool bGotPawnInterface = Actor->Implements<UCreativeEditorActorInterface>();

	if (!bGotPawnInterface)
	{
		return nullptr;
	}
	
	UCreativeEditorActorComponent* ActorComponent = ICreativeEditorActorInterface::Execute_GetCreativeEditorActorComponent(Actor);

	return ActorComponent;
}
