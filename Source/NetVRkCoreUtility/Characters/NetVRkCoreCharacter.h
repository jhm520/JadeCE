#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "NetVRkCoreCharacter.generated.h"

class UFellOffWorldComponent;
class UAbilitySystemComponent;
class UGameplayAbilityCollection;

UCLASS(config=Game)
class NETVRKCOREUTILITY_API ANetVRkCoreCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

#pragma region Framework

public:
	ANetVRkCoreCharacter();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

#pragma endregion

#pragma region Input

	/** Called for movement input */
	void Input_Interact(const FInputActionValue& Value);

	void Input_Deselect(const FInputActionValue& Value);


	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

	/** Deselect Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DeselectAction;

#pragma endregion

#pragma region Interaction

	
#pragma endregion

#pragma region Fell Off World

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	UFellOffWorldComponent* FellOffWorldComponent;

#pragma endregion

#pragma region Gameplay Ability System
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Ability System", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;

	UFUNCTION()
	void InitAbilitySystem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Ability System", meta = (AllowPrivateAccess = "true"))
	TArray<UGameplayAbilityCollection*> AbilityCollections;
	
#pragma endregion


};