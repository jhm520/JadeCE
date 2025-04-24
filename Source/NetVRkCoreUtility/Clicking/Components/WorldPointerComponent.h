#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WorldPointerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETVRKCOREUTILITY_API UWorldPointerComponent : public USceneComponent
{
	GENERATED_BODY()
#pragma region Framework

public:	
	// Sets default values for this component's properties
	UWorldPointerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
#pragma endregion

#pragma region Pointing

	UFUNCTION(BlueprintPure, Category = "Pointing", meta = (AllowPrivate))
	bool GetPointerHitResult(FHitResult& OutHitResult) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pointing", meta = (AllowPrivate))
	bool bIsPointing = true;

	UFUNCTION()
	void UpdatePointing();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pointing", meta = (AllowPrivateAccess = "true"))
	float MaxPointerDistance = 10000.0f;

	UPROPERTY(Transient)
	FHitResult CachedPointerHitResult;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pointing", meta = (AllowPrivate))
	TEnumAsByte<ECollisionChannel> PointingCollisionChannel;

#pragma endregion


#pragma region Debug

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pointing", meta = (AllowPrivate))
	bool bDrawDebug;
#pragma endregion

};
