// Fill out your copyright notice in the Description page of Project Settings.
#include "CreativeEditorPawnComponent.h"

#include "CreativeEditorActorComponent.h"
#include "CreativeEditorControllerComponent.h"
#include "CreativeEditorManipulationActorComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "ManipulationComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NetVRkCreativeEditorUtility/NetVRkCreativeEditorGameplayTags.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorActorInterface.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorControllerInterface.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorManipulationActorInterface.h"
#include "NetVRkCreativeEditorUtility/Interfaces/CreativeEditorPawnInterface.h"
#include "NetVRkCreativeEditorUtility/UI/CreativeEditorMenuWidget.h"

#pragma region Framework

UCreativeEditorPawnComponent::UCreativeEditorPawnComponent()
{
	CurrentManipulationModeTag = NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Translation;
}


void UCreativeEditorPawnComponent::BeginPlay()
{
	APawn* OwningPawn = Cast<APawn>(GetOwner());

	if (OwningPawn)
	{
		OwningPawn->ReceiveControllerChangedDelegate.AddDynamic(this, &UCreativeEditorPawnComponent::OnPawnControllerChanged);

		APlayerController* PlayerController = Cast<APlayerController>(OwningPawn->GetController());

		if (PlayerController)
		{
			OnPawnControllerChanged(OwningPawn, nullptr, PlayerController);
		}
	}
	

	GetOwner()->OnDestroyed.AddDynamic(this, &UCreativeEditorPawnComponent::OnPawnOwnerDestroyed);

	
	Super::BeginPlay();
}

void UCreativeEditorPawnComponent::OnPawnOwnerDestroyed(AActor* DestroyedActor)
{
	// APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	//
	// if (!PlayerController || !PlayerController->IsLocalPlayerController())
	// {
	// 	return;
	// }

	// APawn* OwningPawn = Cast<APawn>(GetOwner());
	//
	// if (!OwningPawn)
	// {
	// 	return;
	// }
	//
	// APlayerController* PlayerController = Cast<APlayerController>(OwningPawn->GetController());
	//
	// if (!PlayerController)
	// {
	// 	return;
	// }
	//
	// OnCreativeModeDeactivated(PlayerController);
}

void UCreativeEditorPawnComponent::OnPawnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	// if (!OldController && NewController)
	// {
	// 	OnCreativeModeActivated(Cast<APlayerController>(NewController));
	// }
	// // else if (OldController && !NewController)
	// // {
	// // 	OnCreativeModeDeactivated(Cast<APlayerController>(OldController));
	// // }
}

#pragma endregion

#pragma region Debug Commands

void UCreativeEditorPawnComponent::ToggleManipulationMode()
{
	if (CurrentManipulationModeTag == NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Translation)
	{
		SetManipulationMode(NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Rotation, nullptr);
	}
	else if (CurrentManipulationModeTag == NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Rotation)
	{
		SetManipulationMode(NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Scale, nullptr);
	}
	else if (CurrentManipulationModeTag == NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Scale)
	{
		SetManipulationMode(NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Translation, nullptr);
	}
	
}

void UCreativeEditorPawnComponent::SetManipulationMode_AbsoluteTranslation(bool bInAbsolute)
{
	FGameplayTagContainer ManipulationTags = FGameplayTagContainer();
	ManipulationTags.AddTag(NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Translation);
	SetManipulationAbsolute(bInAbsolute, ManipulationTags);
}

void UCreativeEditorPawnComponent::SetManipulationMode_AbsoluteRotation(bool bInAbsolute)
{
	FGameplayTagContainer ManipulationTags = FGameplayTagContainer();
	ManipulationTags.AddTag(NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Rotation);
	SetManipulationAbsolute(bInAbsolute, ManipulationTags);
}

void UCreativeEditorPawnComponent::SetManipulationMode_AbsoluteScale(bool bInAbsolute)
{
	FGameplayTagContainer ManipulationTags = FGameplayTagContainer();
	ManipulationTags.AddTag(NetVRkCreativeEditorManipulationGameplayTags::Manipulation_Scale);
	SetManipulationAbsolute(bInAbsolute, ManipulationTags);
}

#pragma endregion

#pragma region Controller
APlayerController* UCreativeEditorPawnComponent::GetPlayerController() const
{
	return Cast<APlayerController>(GetOwner()->GetOwner());
}
#pragma endregion

#pragma region Creative Editor

void UCreativeEditorPawnComponent::OnCreativeModeActivated_Implementation(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}

	if (!PlayerController->IsLocalPlayerController())
	{
		return;
	}

	PlayerController->bShowMouseCursor = bShowMouseCursor;

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, CreativeEditorMenuWidget, EMouseLockMode::DoNotLock);

	CreativeEditorMenuWidget = CreateWidget<UCreativeEditorMenuWidget>(PlayerController, CreativeEditorMenuWidgetClass);

	if (CreativeEditorMenuWidget)
	{
		CreativeEditorMenuWidget->AddToViewport();
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	if (!Subsystem)
	{
		return;
	}

	FModifyContextOptions IMCOptions = FModifyContextOptions();

	IMCOptions.bForceImmediately = false;
	IMCOptions.bIgnoreAllPressedKeysUntilRelease = true;
	Subsystem->AddMappingContext(CreativeEditorInputMappingContext, 500, IMCOptions);
	
	OnCreativeModeActivatedDelegate.Broadcast();
}

void UCreativeEditorPawnComponent::OnCreativeModeDeactivated_Implementation(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}
	
	//OnPawnOwnerDestroyedDelegate.Broadcast();

	// APawn* OwningPawn = Cast<APawn>(GetOwner());
	//
	// if (!OwningPawn)
	// {
	// 	return;
	// }
	
	if (!PlayerController->IsLocalPlayerController())
	{
		return;
	}

	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);

	PlayerController->bShowMouseCursor = false;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	if (!Subsystem)
	{
		return;
	}
	
	FModifyContextOptions IMCOptions = FModifyContextOptions();

	IMCOptions.bForceImmediately = false;
	IMCOptions.bIgnoreAllPressedKeysUntilRelease = true;
	Subsystem->RemoveMappingContext(CreativeEditorInputMappingContext, IMCOptions);

	if (CreativeEditorMenuWidget)
	{
		CreativeEditorMenuWidget->RemoveFromParent();
	}

	OnCreativeModeDeactivatedDelegate.Broadcast();

	UCreativeEditorControllerComponent* ControllerComponent = ICreativeEditorControllerInterface::Execute_GetCreativeEditorControllerComponent(PlayerController);

	if (ControllerComponent)
	{
		ControllerComponent->RequestReposessOriginalPawn(GetOwner()->GetActorLocation());
	}
}

void UCreativeEditorPawnComponent::PreviewActorPlacement_Implementation(TSubclassOf<AActor> ActorClass, const FTransform& Transform)
{
	if (PreviewedActor)
	{
		PreviewedActor->Destroy();
	}
	
	AActor* PreviewActor = GetWorld()->SpawnActorDeferred<AActor>(ActorClass, Transform, GetPlayerController(), Cast<APawn>(GetOwner()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!PreviewActor)
	{
		return;
	}

	PreviewActor->SetReplicates(false);

	PreviewedActor = PreviewActor;

	PreviewedActor->OnDestroyed.AddDynamic(this, &UCreativeEditorPawnComponent::OnEndedPreviewActorPlacement);

	PreviewActor->FinishSpawning(Transform);
	
	const bool bImplements = GetOwner()->Implements<UCreativeEditorPawnInterface>();

	if (bImplements)
	{
		ICreativeEditorPawnInterface::Execute_OnPreviewActorPlacement(GetOwner(), PreviewedActor);
	}

	const bool bActorImplements = PreviewedActor->Implements<UCreativeEditorActorInterface>();

	if (bActorImplements)
	{
		UCreativeEditorActorComponent* ActorComponent = ICreativeEditorActorInterface::Execute_GetCreativeEditorActorComponent(PreviewedActor);

		if (ActorComponent)
		{
			ActorComponent->SetIsPreview(true);
		}
	}
}

void UCreativeEditorPawnComponent::OnEndedPreviewActorPlacement(AActor* DestroyedActor)
{
	const bool bImplements = GetOwner()->Implements<UCreativeEditorPawnInterface>();

	if (bImplements)
	{
		ICreativeEditorPawnInterface::Execute_OnEndedPreviewActorPlacement(GetOwner(), DestroyedActor);
	}

	const bool bActorImplements = DestroyedActor->Implements<UCreativeEditorActorInterface>();

	if (bActorImplements)
	{
		ICreativeEditorActorInterface::Execute_OnConfirmedActorPlacement(DestroyedActor);
	}
}


void UCreativeEditorPawnComponent::ConfirmActorPlacement_Implementation(TSubclassOf<AActor> ActorClass, const FTransform& Transform)
{
	if (PreviewedActor)
	{
		PreviewedActor->Destroy();
	}
	
	if (!GetOwner()->HasAuthority())
	{
		Server_ConfirmActorPlacement(ActorClass, Transform);
		return;
	}
	
	AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(ActorClass, Transform, GetPlayerController(), Cast<APawn>(GetOwner()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!NewActor)
	{
		return;
	}

	NewActor->SetReplicates(true);
	NewActor->SetAutonomousProxy(true);

	NewActor->FinishSpawning(Transform);

	ENetRole LocalRole = NewActor->GetLocalRole();
	ENetRole RemoteRole = NewActor->GetRemoteRole();


	const bool bImplements = GetOwner()->Implements<UCreativeEditorPawnInterface>();

	if (bImplements)
	{
		ICreativeEditorPawnInterface::Execute_OnConfirmedActorPlacement(GetOwner(), NewActor);
	}

	const bool bActorImplements = NewActor->Implements<UCreativeEditorActorInterface>();

	if (bActorImplements)
	{
		UCreativeEditorActorComponent* ActorComponent = ICreativeEditorActorInterface::Execute_GetCreativeEditorActorComponent(NewActor);

		if (ActorComponent)
		{
			ActorComponent->SetIsPreview(false);
			ActorComponent->SetIsPlaced(true);
		}
	}

	if (bActorImplements)
	{
		ICreativeEditorActorInterface::Execute_OnConfirmedActorPlacement(NewActor);
	}
}

void UCreativeEditorPawnComponent::Server_ConfirmActorPlacement_Implementation(TSubclassOf<AActor> ActorClass, const FTransform& Transform)
{
	ConfirmActorPlacement(ActorClass, Transform);
}

void UCreativeEditorPawnComponent::StartManipulation_Implementation(UManipulationComponent* ManipulationComponent)
{
	if (!ManipulationComponent)
	{
		ManipulationComponent = GetManipulationComponentFromManipulationModeContext();
		
		if (!ManipulationComponent)
		{
			return;
		}
	}

	SetCurrentManipulationComponent(ManipulationComponent);

	ManipulationComponent->SetIsManipulating(true);
}
	

void UCreativeEditorPawnComponent::StopManipulation_Implementation()
{
	if (!CurrentManipulationComponent)
	{
		return;
	}

	CurrentManipulationComponent->SetIsManipulating(false);

	CurrentManipulationComponent = nullptr;
}

bool UCreativeEditorPawnComponent::IsManipulating() const
{
	return IsValid(CurrentManipulationComponent);
}

void UCreativeEditorPawnComponent::SetCurrentManipulationComponent(UManipulationComponent* InCurrentManipulationComponent)
{
	UManipulationComponent* PreviousManipulationComponent = CurrentManipulationComponent;
	
	if (CurrentManipulationComponent)
	{
		CurrentManipulationComponent->SetIsManipulating(false);
	}
	
	CurrentManipulationComponent = InCurrentManipulationComponent;

	OnCurrentManipulationComponentChanged(PreviousManipulationComponent);
}

TArray<AActor*> UCreativeEditorPawnComponent::GetManipulationActors() const
{
	TArray<AActor*> OutManipulationActors;

	TArray<AActor*> LocalManipulationGizmoActors = GetManipulationGizmoActors();

	for (AActor* ManipulationGizmoActor : LocalManipulationGizmoActors)
	{
		if (!ManipulationGizmoActor)
		{
			continue;
		}

		const bool bImplementsInterface = ManipulationGizmoActor->Implements<UCreativeEditorManipulationActorInterface>();

		if (!bImplementsInterface)
		{
			continue;
		}

		UCreativeEditorManipulationActorComponent* ManipulationActorComponent = ICreativeEditorManipulationActorInterface::Execute_GetManipulationActorComponent(ManipulationGizmoActor);

		if (!ManipulationActorComponent)
		{
			continue;
		}

		AActor* ManipulatedActor = ManipulationActorComponent->GetManipulatedActor();

		if (!ManipulatedActor)
		{
			continue;
		}

		OutManipulationActors.Add(ManipulatedActor);
	}
	
	return OutManipulationActors;
}

void UCreativeEditorPawnComponent::DeleteManipulationActors()
{
	TArray<AActor*> ManipulationActors = GetManipulationActors();

	for (AActor* ManipulationActor : ManipulationActors)
	{
		if (!ManipulationActor)
		{
			continue;
		}

		if (GetOwner()->HasAuthority())
		{
			Server_DeleteManipulationActor_Implementation(ManipulationActor);
		}
		else
		{
			Server_DeleteManipulationActor(ManipulationActor);
		}
	}
}

void UCreativeEditorPawnComponent::Server_DeleteManipulationActor_Implementation(AActor* InActor)
{
	if (!InActor)
	{
		return;
	}

	InActor->Destroy();
}

void UCreativeEditorPawnComponent::OnCurrentManipulationComponentChanged(UManipulationComponent* PreviousManipulationComponent)
{
	if (PreviousManipulationComponent)
	{
		AActor* ManipulatedActor = PreviousManipulationComponent->GetManipulatedActor();

		if (!ManipulatedActor)
		{
			return;
		}

		USceneComponent* ManipulatedRootComponent = ManipulatedActor->GetRootComponent();

		if (!ManipulatedRootComponent)
		{
			return;
		}

		ManipulatedRootComponent->TransformUpdated.Remove(OnManipulatedActorTransformChangedDelegateHandle);
	}
	
	if (!CurrentManipulationComponent)
	{
		return;
	}

	AActor* ManipulatedActor = CurrentManipulationComponent->GetManipulatedActor();

	if (!ManipulatedActor)
	{
		return;
	}

	USceneComponent* ManipulatedRootComponent = ManipulatedActor->GetRootComponent();

	if (!ManipulatedRootComponent)
	{
		return;
	}

	OnManipulatedActorTransformChangedDelegateHandle = ManipulatedRootComponent->TransformUpdated.AddUObject(this, &UCreativeEditorPawnComponent::OnManipulatedActorTransformUpdated);
}

void UCreativeEditorPawnComponent::OnManipulatedActorTransformUpdated(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport) const
{
	if (!UpdatedComponent)
	{
		return;
	}
	AActor* UpdatedActor = UpdatedComponent->GetOwner();

	if (!UpdatedActor)
	{
		return;
	}

	OnManipulatedActorTransformChangedDelegate.Broadcast(UpdatedActor, UpdatedActor->GetActorTransform());
}

UManipulationComponent* UCreativeEditorPawnComponent::GetManipulationComponentFromManipulationModeContext()
{
	const FGameplayTag LocalCurrentManipulationModeTag = GetManipulationModeTag();

	if (LocalCurrentManipulationModeTag == NetVRkCreativeEditorManipulationGameplayTags::Manipulation_SmoothBrush)
	{
		TArray<AActor*> GizmoActors = GetManipulationGizmoActors();

		for (AActor* GizmoActor : GizmoActors)
		{
			if (!GizmoActor)
			{
				continue;
			}

			const bool bImplementsInterface = GizmoActor->Implements<UCreativeEditorManipulationActorInterface>();

			if (!bImplementsInterface)
			{
				continue;
			}

			UCreativeEditorManipulationActorComponent* ManipulationActorComponent = ICreativeEditorManipulationActorInterface::Execute_GetManipulationActorComponent(GizmoActor);

			if (!ManipulationActorComponent)
			{
				continue;
			}

			return ManipulationActorComponent->GetManipulationComponentByTag(NetVRkCreativeEditorManipulationGameplayTags::Manipulation_SmoothBrush);
		}
	}
	
	return nullptr;
}

void UCreativeEditorPawnComponent::SetManipulationMode(FGameplayTag InManipulationModeTag, AActor* ManipulatedActor)
{
	CurrentManipulationModeTag = InManipulationModeTag;
	
	TArray<AActor*> LocalManipulationGizmoActors = GetManipulationGizmoActors();

	for (AActor* LocalManipulationGizmoActor : LocalManipulationGizmoActors)
	{
		if (!LocalManipulationGizmoActor)
		{
			continue;
		}

		const bool bImplementsInterface = LocalManipulationGizmoActor->Implements<UCreativeEditorManipulationActorInterface>();

		if (!bImplementsInterface)
		{
			continue;
		}

		UCreativeEditorManipulationActorComponent* ManipulationActorComponent = ICreativeEditorManipulationActorInterface::Execute_GetManipulationActorComponent(LocalManipulationGizmoActor);

		if (!ManipulationActorComponent)
		{
			continue;
		}

		ManipulationActorComponent->SetManipulationMode(InManipulationModeTag);
	}
}

void UCreativeEditorPawnComponent::SetManipulationSnapEnabled(bool bInSnapEnabled, FGameplayTagContainer InManipulationTags)
{
	TArray<AActor*> LocalManipulationGizmoActors = GetManipulationGizmoActors();

	for (AActor* LocalManipulationGizmoActor : LocalManipulationGizmoActors)
	{
		if (!LocalManipulationGizmoActor)
		{
			continue;
		}

		const bool bImplementsInterface = LocalManipulationGizmoActor->Implements<UCreativeEditorManipulationActorInterface>();

		if (!bImplementsInterface)
		{
			continue;
		}

		UCreativeEditorManipulationActorComponent* ManipulationActorComponent = ICreativeEditorManipulationActorInterface::Execute_GetManipulationActorComponent(LocalManipulationGizmoActor);

		if (!ManipulationActorComponent)
		{
			continue;
		}

		ManipulationActorComponent->SetManipulationSnapEnabled(bInSnapEnabled, InManipulationTags);
	}
}

void UCreativeEditorPawnComponent::SetManipulationSnapIncrement(float InSnapIncrement, FGameplayTagContainer InManipulationTags)
{
	TArray<AActor*> LocalManipulationGizmoActors = GetManipulationGizmoActors();

	for (AActor* LocalManipulationGizmoActor : LocalManipulationGizmoActors)
	{
		if (!LocalManipulationGizmoActor)
		{
			continue;
		}

		const bool bImplementsInterface = LocalManipulationGizmoActor->Implements<UCreativeEditorManipulationActorInterface>();

		if (!bImplementsInterface)
		{
			continue;
		}

		UCreativeEditorManipulationActorComponent* ManipulationActorComponent = ICreativeEditorManipulationActorInterface::Execute_GetManipulationActorComponent(LocalManipulationGizmoActor);

		if (!ManipulationActorComponent)
		{
			continue;
		}

		ManipulationActorComponent->SetManipulationSnapIncrement(InSnapIncrement, InManipulationTags);
	}
}

void UCreativeEditorPawnComponent::SetManipulationAbsolute(bool bAbsolute, FGameplayTagContainer InManipulationTags)
{
	TArray<AActor*> LocalManipulationGizmoActors = GetManipulationGizmoActors();

	for (AActor* LocalManipulationGizmoActor : LocalManipulationGizmoActors)
	{
		if (!LocalManipulationGizmoActor)
		{
			continue;
		}

		const bool bImplementsInterface = LocalManipulationGizmoActor->Implements<UCreativeEditorManipulationActorInterface>();

		if (!bImplementsInterface)
		{
			continue;
		}

		UCreativeEditorManipulationActorComponent* ManipulationActorComponent = ICreativeEditorManipulationActorInterface::Execute_GetManipulationActorComponent(LocalManipulationGizmoActor);

		if (!ManipulationActorComponent)
		{
			continue;
		}

		ManipulationActorComponent->SetManipulationAbsolute(bAbsolute, InManipulationTags);
	}
}

void UCreativeEditorPawnComponent::SetManipulationDepth(float InDepth, FGameplayTagContainer InManipulationTags)
{
	TArray<AActor*> LocalManipulationGizmoActors = GetManipulationGizmoActors();

	for (AActor* LocalManipulationGizmoActor : LocalManipulationGizmoActors)
	{
		if (!LocalManipulationGizmoActor)
		{
			continue;
		}

		const bool bImplementsInterface = LocalManipulationGizmoActor->Implements<UCreativeEditorManipulationActorInterface>();

		if (!bImplementsInterface)
		{
			continue;
		}

		UCreativeEditorManipulationActorComponent* ManipulationActorComponent = ICreativeEditorManipulationActorInterface::Execute_GetManipulationActorComponent(LocalManipulationGizmoActor);

		if (!ManipulationActorComponent)
		{
			continue;
		}

		ManipulationActorComponent->SetManipulationDepth(InDepth, InManipulationTags);
	}
}


#pragma endregion