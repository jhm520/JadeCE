// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CreativeEditorControllerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETVRKCREATIVEEDITORUTILITY_API UCreativeEditorControllerComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region Framework
public:
	// Sets default values for this component's properties
	UCreativeEditorControllerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion

#pragma region Controller

	UFUNCTION(BlueprintPure, Category = "Controller")
	AController* GetController();

	UFUNCTION(BlueprintPure, Category = "Controller")
	APlayerController* GetPlayerController();

	UFUNCTION(BlueprintPure, Category = "Controller")
	APawn* GetPawn();

	UPROPERTY()
	APawn* CachedOriginalPawn;

	UPROPERTY()
	APawn* CachedCreativeEditorPawn;
	
#pragma endregion

#pragma region Creative Editor

public:

	UFUNCTION()
	bool IsCreativeModeActive();

	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	bool ToggleCreativeMode();

	UFUNCTION(Server, Reliable)
	void Server_ToggleCreativeMode();
	
	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	bool ActivateCreativeMode(bool bActivate = true);

	UFUNCTION(Server, Reliable)
	void Server_ActivateCreativeMode(bool bActivate);

	UFUNCTION()
	void SpawnAndPosessCreativeEditorPawn();

	UFUNCTION()
	void DestroyCreativeEditorPawn();

	//pawn to spawn when activating creative mode
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Creative Editor")
	TSubclassOf<APawn> CreativeEditorPawnClass;

	UPROPERTY(ReplicatedUsing="OnRep_CreativeEditorPawn")
	APawn* CreativeEditorPawn;

	UFUNCTION()
	void OnRep_CreativeEditorPawn(APawn* OldCreativeEditorPawn);

	void OnCreativeModeActivated(APawn* OldCreativeEditorPawn);

	void OnCreativeModeDeactivated(APawn* OldCreativeEditorPawn);

	UFUNCTION()
	void RequestReposessOriginalPawn(FVector NewLocation = FVector::ZeroVector);

	UFUNCTION(Server, Reliable)
	void Server_RequestReposessOriginalPawn(FVector NewLocation = FVector::ZeroVector);

	UPROPERTY(EditDefaultsOnly, Category = "Creative Editor")
	bool bMoveOriginalPawnToCreativeEditorPawnLocation = true;
	
#pragma endregion
	
#pragma region Creative Editor | Registry
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> PlacedCreativeEditorActors;

	UFUNCTION()
	void RegisterPlacedCreativeEditorActor(AActor* PlacedActor);

	UFUNCTION()
	void UnregisterPlacedCreativeEditorActor(AActor* PlacedActor);

#pragma endregion
};
