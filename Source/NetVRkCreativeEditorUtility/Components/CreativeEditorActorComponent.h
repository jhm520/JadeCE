// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CreativeEditorActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETVRKCREATIVEEDITORUTILITY_API UCreativeEditorActorComponent : public UActorComponent
{
	GENERATED_BODY()
#pragma region Framework
public:	
	// Sets default values for this component's properties
	UCreativeEditorActorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion
	
#pragma region Creative Editor
public:
	
	UFUNCTION(BlueprintPure, Category = "Creative Editor")
	APawn* GetCreativeEditorPawn() const;

	UFUNCTION(BlueprintPure, Category = "Creative Editor")
	APlayerController* GetCreativeEditorController() const;
	
	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	void SetIsPreview(bool bInIsPreview);

	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	void SetIsPlaced(bool bInIsPlaced);
	
	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
    void SetIsManipulation(bool bInIsManipulation);

	bool GetIsPreview() const {return bIsPreview;};

	bool GetIsPlaced() const {return bIsPlaced;};

	bool GetIsManipulation() const {return bIsManipulation;};

protected:
	UPROPERTY(ReplicatedUsing="OnRep_bIsPreview", BlueprintReadOnly, Category = "Creative Editor")
	bool bIsPreview = false;

	UFUNCTION()
	void OnRep_bIsPreview();

	UPROPERTY(ReplicatedUsing="OnRep_bIsPlaced", BlueprintReadOnly, Category = "Creative Editor")
	bool bIsPlaced = false;

	UFUNCTION()
	void OnRep_bIsPlaced();

	UPROPERTY(ReplicatedUsing="OnRep_bIsManipulation", BlueprintReadOnly, Category = "Creative Editor")
	bool bIsManipulation = false;

	UFUNCTION()
	void OnRep_ManipulationActor();
	
	UFUNCTION()
	void OnRep_bIsManipulation();

	void OnBeginManipulation();

	void OnEndManipulation();
	
	UPROPERTY(EditDefaultsOnly, Category = "Creative Editor")
	TSubclassOf<AActor> ManuplationActorClass;

	UPROPERTY(ReplicatedUsing="OnRep_ManipulationActor", BlueprintReadOnly, Category = "Creative Editor")
	TObjectPtr<AActor> ManipulationActor;

#pragma endregion

#pragma region Creative Editor | Controller

	UFUNCTION()
	void RegisterWithController();

	UFUNCTION()
	void UnregisterWithController();
	
public:
	UFUNCTION()
	void OnCreativeModeActivated(APawn* CreativeEditorPawn);

	UFUNCTION()
	void OnCreativeModeDeactivated(APawn* OldCreativeEditorPawn);
#pragma endregion

};
