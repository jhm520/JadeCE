// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CreativeEditorPawnComponent.generated.h"
//this is a component that is meant to be added to Voxel Actors, contains a list of it's sculpt infos

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPawnOwnerDestroyedDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreativeModeActivatedDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnManipulatedActorTransformChangedDelegate, AActor*, ManipulatedActor, const FTransform&, NewTransform);


class UCreativeEditorMenuWidget;
class UInputMappingContext;

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent) )
class NETVRKCREATIVEEDITORUTILITY_API UCreativeEditorPawnComponent : public UActorComponent
{
	GENERATED_BODY()
#pragma region Framework
public:

	UCreativeEditorPawnComponent();

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPawnOwnerDestroyed(AActor* DestroyedActor);

	UPROPERTY(BlueprintAssignable)
	FOnPawnOwnerDestroyedDelegate OnPawnOwnerDestroyedDelegate;

	UFUNCTION()
	void OnPawnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);

#pragma endregion

#pragma region Debug Commands
	
	UFUNCTION(Exec, BlueprintCallable, Category = "Creative Editor")
	void ToggleManipulationMode();

	UFUNCTION(Exec, BlueprintCallable, Category = "Creative Editor")
	void SetManipulationMode_AbsoluteTranslation(bool bInAbsolute);

	UFUNCTION(Exec, BlueprintCallable, Category = "Creative Editor")
	void SetManipulationMode_AbsoluteRotation(bool bInAbsolute);

	UFUNCTION(Exec, BlueprintCallable, Category = "Creative Editor")
	void SetManipulationMode_AbsoluteScale(bool bInAbsolute);
	
#pragma endregion

#pragma region Input
	UPROPERTY(EditDefaultsOnly, Category = "Creative Editor")
	UInputMappingContext* CreativeEditorInputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Creative Editor")
	bool bShowMouseCursor = true;
#pragma endregion

#pragma region User Interface
	
	UPROPERTY(EditDefaultsOnly, Category = "Creative Editor")
	TSubclassOf<UCreativeEditorMenuWidget> CreativeEditorMenuWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "Creative Editor")
	UCreativeEditorMenuWidget* CreativeEditorMenuWidget;
#pragma endregion

#pragma region Controller
	UFUNCTION(BlueprintPure, Category = "Creative Editor")
	APlayerController* GetPlayerController() const;
#pragma endregion

#pragma region Creative Editor
protected:

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Creative Editor")
	void OnCreativeModeActivated(APlayerController* PlayerController);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Creative Editor")
	void OnCreativeModeDeactivated(APlayerController* PlayerController);

	UPROPERTY(BlueprintAssignable)
	FOnCreativeModeActivatedDelegate OnCreativeModeActivatedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnCreativeModeActivatedDelegate OnCreativeModeDeactivatedDelegate;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Creative Editor")
	void PreviewActorPlacement(TSubclassOf<AActor> ActorClass, const FTransform& Transform);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Creative Editor")
	void ConfirmActorPlacement(TSubclassOf<AActor> ActorClass, const FTransform& Transform);

	UFUNCTION()
	void OnEndedPreviewActorPlacement(AActor* DestroyedActor);

	UFUNCTION(Server, Reliable)
	void Server_ConfirmActorPlacement(TSubclassOf<AActor> ActorClass, const FTransform& Transform);
	
	UPROPERTY(BlueprintReadOnly, Category = "Creative Editor")
	AActor* PreviewedActor;
#pragma endregion


#pragma region Creative Editor | Manipulation

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Creative Editor")
	UManipulationComponent* CurrentManipulationComponent;

	UFUNCTION()
	void SetCurrentManipulationComponent(UManipulationComponent* InCurrentManipulationComponent);

	UFUNCTION()
	virtual void OnCurrentManipulationComponentChanged(UManipulationComponent* PreviousManipulationComponent);

	void OnManipulatedActorTransformUpdated(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport) const;

	//the list of actors this pawn currently has selected for manipulations (the gizmos, not the actors themselves)
	UPROPERTY()
	TArray<TObjectPtr<AActor>> ManipulationGizmoActors;
	
public:

	UFUNCTION()
	const UManipulationComponent* GetCurrentManipulationComponent() const {return CurrentManipulationComponent;};

	UFUNCTION(BlueprintPure, Category = "Creative Editor")
	UManipulationComponent* GetManipulationComponentFromManipulationModeContext();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Creative Editor")
	void StartManipulation(UManipulationComponent* ManipulationComponent = nullptr);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Creative Editor")
	void StopManipulation();

	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	bool IsManipulating() const;

	//set the manipulation mode for the actors we are currently editing, or a specified actor we are currently manipulating
	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	void SetManipulationMode(FGameplayTag InManipulationModeTag, AActor* ManipulatedActor = nullptr);

	//enable snapping in the manipulation mode for the actors we are currently editing
	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	void SetManipulationSnapEnabled(bool bInSnappingEnabled, FGameplayTagContainer InManipulationTags);

	//change the snap increment for the manipulation mode for the actors we are currently editing
	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	void SetManipulationSnapIncrement(float InSnapIncrement, FGameplayTagContainer InManipulationTags);

	//set the manipulation mode to be in absolute world space
	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	void SetManipulationAbsolute(bool bAbsolute, FGameplayTagContainer InManipulationTags);

	//set the manipulation mode to be in absolute world space
	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	void SetManipulationDepth(float InDepth, FGameplayTagContainer InManipulationTags);
	
	UFUNCTION(BlueprintPure, Category = "Creative Editor")
	const FGameplayTag& GetManipulationModeTag() const {return CurrentManipulationModeTag;};
	
	UPROPERTY()
	FGameplayTag CurrentManipulationModeTag;
	
	UFUNCTION(BlueprintPure, Category = "Creative Editor")
	TArray<AActor*> GetManipulationGizmoActors() const {return ManipulationGizmoActors;};

	UFUNCTION(BlueprintPure, Category = "Creative Editor")
	TArray<AActor*> GetManipulationActors() const;
	
	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	void DeleteManipulationActors();

	UFUNCTION(Server, Reliable)
	void Server_DeleteManipulationActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	void AddManipulationGizmoActor(AActor* InGizmo) {ManipulationGizmoActors.Add(InGizmo);};

	UFUNCTION(BlueprintCallable, Category = "Creative Editor")
	void RemoveManipulationGizmoActor(AActor* InGizmo) {ManipulationGizmoActors.Remove(InGizmo);};

	UPROPERTY(BlueprintAssignable)
	FOnManipulatedActorTransformChangedDelegate OnManipulatedActorTransformChangedDelegate;

	FDelegateHandle OnManipulatedActorTransformChangedDelegateHandle;
#pragma endregion

};