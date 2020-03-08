// Fill out your copyright notice in the Description page of Project Settings.


#include "Craftable_Item.h"


void UCraftable_Item::AddCraftingComponent(UItem* CraftingComponentToAdd, bool bAddUnique)
{
	if (CraftingComponentToAdd != nullptr)
	{
		if (bAddUnique)
		{
			if (CraftingComponents.Contains(CraftingComponentToAdd))
			{
				return;
			}
		}

		CraftingComponents.Add(CraftingComponentToAdd);
	}
}

void UCraftable_Item::RemoveCraftingComponent(UItem* CraftingComponentToRemove)
{
	if (CraftingComponentToRemove != nullptr &&
		CraftingComponents.Contains(CraftingComponentToRemove))
	{
		CraftingComponents.Remove(CraftingComponentToRemove);
	}
}
