// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathWidget.h"



void UDeathWidget::HandleDeath(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	
}

void UDeathWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FScriptDelegate Delegate;
    Delegate.BindUFunction(this, "HandleDeath");
	
	APlayerController* PlayerController = GetOwningPlayer();
	PlayerController->OnTakeAnyDamage.AddUnique(Delegate);
}
