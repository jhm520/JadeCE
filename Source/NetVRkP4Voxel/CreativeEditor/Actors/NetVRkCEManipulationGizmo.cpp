// Fill out your copyright notice in the Description page of Project Settings.


#include "NetVRkCEManipulationGizmo.h"

#include "Kismet/GameplayStatics.h"
#include "NetVRkCreativeEditorUtility/NetVRkCreativeEditorGameplayTags.h"
#include "NetVRkCreativeEditorUtility/Components/CreativeEditorManipulationActorComponent.h"
#include "SelectionSystem/SelectionSystemBlueprintLibrary.h"
#include "SelectionSystem/Components/SelectableComponent.h"

#pragma region Framework

// Sets default values
ANetVRkCEManipulationGizmo::ANetVRkCEManipulationGizmo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootComponent"));
	SetRootComponent(DefaultRootComponent);
	
	ScalingRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ScalingRootComponent"));
	ScalingRootComponent->SetupAttachment(GetRootComponent());
	ScalingRootComponent->SetUsingAbsoluteScale(true);

	ManipulationActorComponent = CreateDefaultSubobject<UCreativeEditorManipulationActorComponent>(TEXT("ManipulationActorComponent"));

	SelectableComponent = CreateDefaultSubobject<USelectableComponent>(TEXT("SelectableComponent"));
	
	CameraDistanceScalar = 0.001f;
}

// Called when the game starts or when spawned
void ANetVRkCEManipulationGizmo::BeginPlay()
{
	Super::BeginPlay();

	APawn* ManipulatorPawn = ManipulationActorComponent->GetManipulatorPawn();

	if (!ManipulatorPawn)
	{
		return;
	}

	USelectionSystemBlueprintLibrary::SelectActor(ManipulatorPawn, this, true);

	SelectableComponent->OnDeselectedDelegate.AddUniqueDynamic(this, &ANetVRkCEManipulationGizmo::OnDeselected);

	
}

// Called every frame
void ANetVRkCEManipulationGizmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateScreenSize();
}
#pragma endregion

#pragma region Scaling

void ANetVRkCEManipulationGizmo::UpdateScreenSize()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!PlayerController)
	{
		return;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation );

	const FVector& GizmoLocation = GetActorLocation();

	const FVector& GizmoToCamera = ViewLocation - GizmoLocation;

	const float Distance = GizmoToCamera.Size();

	const float NewScale = Distance * CameraDistanceScalar;
	
	ScalingRootComponent->SetWorldScale3D(FVector(NewScale));
}

#pragma endregion

#pragma region Manipulation

void ANetVRkCEManipulationGizmo::OnManipulationModeChanged_Implementation(APlayerController* ManipulationController, FGameplayTag ManipulationModeTag)
{
	if (ManipulationModeTag == NetVRkCreativeEditorManipulationGameplayTags::Manipulation_SmoothBrush)
	{

	}
}

void ANetVRkCEManipulationGizmo::OnDeselected(AActor* Selector)
{
	AActor* ManipulatedActor = ManipulationActorComponent->GetManipulatedActor();

	if (!ManipulatedActor)
	{
		return;
	}

	APawn* ManipulatorPawn = ManipulationActorComponent->GetManipulatorPawn();

	if (!ManipulatorPawn)
	{
		return;
	}

	USelectionSystemBlueprintLibrary::DeselectActor(ManipulatorPawn, ManipulatedActor, true);
}
#pragma endregion
