// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneSaveGameActorComponent.h"

#include "SceneSaveGameOwnerComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NetVRkFileTransferSystem/Components/TransferComponent.h"
#include "NetVRkSaveGameUtility/Interfaces/SceneSaveGameActorInterface.h"
#include "NetVRkSaveGameUtility/Interfaces/SceneSaveGameOwnerInterface.h"

#pragma region Framework

// Sets default values for this component's properties
USceneSaveGameActorComponent::USceneSaveGameActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TransferComponent = CreateDefaultSubobject<UTransferComponent>(TEXT("TransferComponent"));

	SetIsReplicatedByDefault(true);

	// ...
}


// Called when the game starts
void USceneSaveGameActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	RegisterWithSaveGameOwner();
}

void USceneSaveGameActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterWithSaveGameOwner();
	
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void USceneSaveGameActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USceneSaveGameActorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(USceneSaveGameActorComponent, CurrentSceneSaveInfo);
}

#pragma endregion

#pragma region Save Game | Controller

bool USceneSaveGameActorComponent::PopulateSceneActorSaveInfo(FSceneActorSaveInfo& OutSceneSaveInfo)
{
	if (!SceneActorSaveGameSlotName.IsNone())
	{
		OutSceneSaveInfo.SceneActorSaveGameSlotName = SceneActorSaveGameSlotName;
	}

	if (IsValid(SceneActorSaveGameClass))
	{
		USaveGame* NewSaveGame = UGameplayStatics::CreateSaveGameObject(SceneActorSaveGameClass);
		
		OutSceneSaveInfo.SceneActorSaveGame = NewSaveGame;
	}
	
	const bool bImplements = GetOwner()->Implements<USceneSaveGameActorInterface>();

	if (bImplements)
	{
		ISceneSaveGameActorInterface::Execute_PopulateSceneActorSaveInfo(GetOwner(), OutSceneSaveInfo);
	}
	
	return true;
}

void USceneSaveGameActorComponent::SetupSceneActorFromSaveInfo(const FSceneActorSaveInfo& ActorSaveInfo, AActor* SaveOwner)
{
	SetCurrentSceneSaveInfo(ActorSaveInfo, SaveOwner);
}

void USceneSaveGameActorComponent::SetCurrentSceneSaveInfo(const FSceneActorSaveInfo& NewSceneSaveInfo, AActor* SaveOwner)
{
	CurrentSceneSaveInfo = NewSceneSaveInfo;
	CurrentSceneSaveInfo.SaveOwnerActor = SaveOwner;
	
	OnRep_CurrentSceneSaveInfo();
}

void USceneSaveGameActorComponent::OnCurrentSceneSaveInfoChanged()
{
	const bool bImplements = GetOwner()->Implements<USceneSaveGameActorInterface>();

	if (bImplements)
	{
		ISceneSaveGameActorInterface::Execute_SetupActorFromSaveInfo(GetOwner(), CurrentSceneSaveInfo);
	}
}

void USceneSaveGameActorComponent::OnRep_CurrentSceneSaveInfo()
{
	OnCurrentSceneSaveInfoChanged();
}

void USceneSaveGameActorComponent::RegisterWithSaveGameOwner()
{
	AActor* ControllerOwner = GetOwner()->GetOwner();
	
	if (!ControllerOwner)
	{
		return;
	}
	
	const bool bControllerOwnerImplements = ControllerOwner->Implements<USceneSaveGameOwnerInterface>();
	
	if (!bControllerOwnerImplements)
	{
		return;
	}
	
	USceneSaveGameOwnerComponent* SaveGameOwnerComponent = ISceneSaveGameOwnerInterface::Execute_GetSceneSaveGameOwnerComponent(GetOwner()->GetOwner());
	
	if (!SaveGameOwnerComponent)
	{
		return;
	}
	
	SaveGameOwnerComponent->RegisterPlacedSaveGameSceneActor(GetOwner());
}

void USceneSaveGameActorComponent::UnregisterWithSaveGameOwner()
{
	AActor* ControllerOwner = GetOwner()->GetOwner();
	
	if (!ControllerOwner)
	{
		return;
	}
	
	const bool bControllerOwnerImplements = ControllerOwner->Implements<USceneSaveGameOwnerInterface>();
	
	if (!bControllerOwnerImplements)
	{
		return;
	}
	
	USceneSaveGameOwnerComponent* SaveGameOwnerComponent = ISceneSaveGameOwnerInterface::Execute_GetSceneSaveGameOwnerComponent(GetOwner()->GetOwner());
	
	if (!SaveGameOwnerComponent)
	{
		return;
	}
	
	SaveGameOwnerComponent->UnregisterPlacedSaveGameSceneActor(GetOwner());
}

#pragma endregion