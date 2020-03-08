#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PENSIONS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDeathEvent, AActor*, Victim, AActor*, Assasin, const FHitResult&, HitResult);
	
	UPROPERTY(BlueprintAssignable)
	FDeathEvent OnDeath;
	
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin="0"))
	float MaxHealth = 200.f;
	UPROPERTY()
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly)
	/*Every Bone should have damage multiplier and Name set up here.
	 * Once Bone was hit it will calculate damage based on gun, bone and bone protection*/
	TMap<FName, float> DamageMultiplier;

	UHealthComponent();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ActorHit(AActor* Victim, AActor* Assasin, FVector NormalImpulse, const FHitResult& Hit);

private:

	AActor* PlayerOwner;
		
};
