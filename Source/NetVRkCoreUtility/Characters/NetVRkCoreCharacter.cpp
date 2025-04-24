#include "NetVRkCoreCharacter.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "NetVRkCoreUtility/FellOffWorld/Components/FellOffWorldComponent.h"
#include "AbilitySystemComponent.h"
#include "GASUtility/GameplayAbilityCollection.h"

//////////////////////////////////////////////////////////////////////////
// ANetVRkP4VoxelCharacter
#pragma region Framework

ANetVRkCoreCharacter::ANetVRkCoreCharacter()
{
	FellOffWorldComponent = CreateDefaultSubobject<UFellOffWorldComponent>(TEXT("FellOffWorldComponent"));

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

void ANetVRkCoreCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANetVRkCoreCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Set up action bindings
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (!EnhancedInputComponent)
	{
		return;
	}

	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ANetVRkCoreCharacter::Input_Interact);
	EnhancedInputComponent->BindAction(DeselectAction, ETriggerEvent::Triggered, this, &ANetVRkCoreCharacter::Input_Deselect);

}

void ANetVRkCoreCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilitySystem();
	
	
}

void ANetVRkCoreCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilitySystem();
}

#pragma endregion

#pragma region Input

/** Called for movement input */
void ANetVRkCoreCharacter::Input_Interact(const FInputActionValue& Value)
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (!InteractAction)
	{
		return;
	}
	
	const bool bIsPressed = Value.Get<bool>();
	
	if (bIsPressed)
	{
		AbilitySystemComponent->PressInputID(GetTypeHash(InteractAction));
	}
	else
	{
		AbilitySystemComponent->ReleaseInputID(GetTypeHash(InteractAction));
	}
}

void ANetVRkCoreCharacter::Input_Deselect(const FInputActionValue& Value)
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (!DeselectAction)
	{
		return;
	}
	
	const bool bIsPressed = Value.Get<bool>();
	
	if (bIsPressed)
	{
		AbilitySystemComponent->PressInputID(GetTypeHash(DeselectAction));
	}
	else
	{
		AbilitySystemComponent->ReleaseInputID(GetTypeHash(DeselectAction));
	}
}

#pragma endregion

#pragma region Gameplay Ability System

void ANetVRkCoreCharacter::InitAbilitySystem()
{
	if(!AbilitySystemComponent)
	{
		return;
	}
	
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	TArray<FGameplayAbilitySpecHandle> BoundAbilityHandles;
	TArray<FGameplayAbilitySpecHandle> DefaultBoundAbilityHandles;

	for (UGameplayAbilityCollection* AbilityCollection : AbilityCollections)
	{
		if (AbilityCollection)
		{
			AbilityCollection->GiveAbilities(AbilitySystemComponent, BoundAbilityHandles, DefaultBoundAbilityHandles);
		}
	}
}
	
#pragma endregion


