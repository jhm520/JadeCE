#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnMouseScreenEdgeScrollDelegate, const FVector2D&, Direction);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INTERACTIONSYSTEM_API UInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region Framework
public:	
	// Sets default values for this component's properties
	UInteractorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion

};
