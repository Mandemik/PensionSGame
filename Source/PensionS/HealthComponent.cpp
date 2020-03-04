#include "HealthComponent.h"

#include <string>

#include "GameFramework/Actor.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	PlayerOwner = GetOwner();
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	//TODO: Get health from saved data
	CurrentHealth = MaxHealth;
}

void UHealthComponent::ActorHit(AActor* Victim, AActor* Assasin, FVector NormalImpulse, const FHitResult& Hit)
{
	if (CurrentHealth > 0)
	{
	/*TODO: Check if this is a Bullet
	 *Get Hit location default damage
	 *Get Hit location Blocked damage by the armour
	 *Apply damage to actor
	 *Kill if dead
	 */

		if (Victim == PlayerOwner)
		{
			float Damage = 50.f; //TODO: HardCoded value, need to be calculated
			float TempHealth = CurrentHealth - Damage;
			
			CurrentHealth = FMath::Clamp(TempHealth, 0.f, MaxHealth);
			UE_LOG(LogTemp, Warning, TEXT("Actor was hit: %s\nCurrent Health: %f"), *Hit.ToString(), CurrentHealth);

			if (CurrentHealth == 0)
			{
				OnDeath.Broadcast(Victim, Assasin, Hit);
			}
		}
	}
}
