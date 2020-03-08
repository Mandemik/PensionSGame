// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Item.h"
#include "Craftable_Item.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class INVENTORY_SYSTEM_API UCraftable_Item : public UItem
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Crafting")
	TArray<UItem*> CraftingComponents;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Crafting")
	float CraftingCost = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Crafting")
	bool CanCraft = true;

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void SetCraftingCost(float NewCraftingCost)
	{
		CraftingCost = NewCraftingCost;
	}

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void AddCraftingComponent(UItem* CraftingComponentToAdd, bool bAddUnique);

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void RemoveCraftingComponent(UItem* CraftingComponentToRemove);

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void SetCraftingComponents(TArray<UItem*>& NewCraftingComponents)
	{
		CraftingComponents = NewCraftingComponents;
	}

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void LockCrafting()
	{
		CanCraft = false;
	}

	UFUNCTION(BlueprintCallable, Category = "Crafting")
		void UnlockCrafting()
	{
		CanCraft = true;
	}

};
