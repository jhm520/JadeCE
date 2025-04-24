// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MousePointerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETVRKCOREUTILITY_API UMousePointerComponent : public UActorComponent
{
	GENERATED_BODY()
#pragma region Framework
public:	
	// Sets default values for this component's properties
	UMousePointerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion

#pragma region Controller
public:
	UFUNCTION(BlueprintPure, Category = "Controller")
	APlayerController* GetController();
#pragma endregion

#pragma region Mouse Pointer

	UPROPERTY(EditDefaultsOnly, Category = "Mouse Pointer")
	TEnumAsByte<ETraceTypeQuery> CursorTraceChannel = ETraceTypeQuery::TraceTypeQuery1;

	UPROPERTY(EditDefaultsOnly, Category = "Mouse Pointer")
	float MaxCursorDistance = 0.0f;
	
	UFUNCTION(BlueprintCallable, Category = "Mouse Pointer")
	bool GetHitResultUnderCursor(FHitResult& OutHitResult, TEnumAsByte<ETraceTypeQuery> OverrideTraceChannel, bool bOverrideTraceChannel = false);
	
#pragma endregion
		
};
