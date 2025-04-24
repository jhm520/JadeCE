// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NetVRkCoreUtility/Characters/NetVRkCoreCharacter.h"
#include "InputActionValue.h"
#include "NetVRkCoreUtility/Clicking/Interfaces/WorldPointerInterface.h"
#include "SelectionSystem/Interfaces/SelectorInterface.h"
#include "NetVRkP4VoxelCharacter.generated.h"

class UVoxelSculptorComponent;
UCLASS(config=Game)
class ANetVRkP4VoxelCharacter : public ANetVRkCoreCharacter, public IWorldPointerInterface, public ISelectorInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

public:
	ANetVRkP4VoxelCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;

	
#pragma region WorldPointer
public:
	virtual UWorldPointerComponent* GetWorldPointerComponent_Implementation() const override;
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pointing", meta = (AllowPrivate))
	UWorldPointerComponent* WorldPointerComponent;
	
#pragma endregion

#pragma region Selector
public:
	virtual USelectorComponent* GetSelectorComponent_Implementation() const override {return SelectorComponent;};

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pointing", meta = (AllowPrivate))
	USelectorComponent* SelectorComponent;

#pragma endregion

};

