// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
#include "Item.h"

UInventoryComponent::UInventoryComponent()
{
	Capacity = 20;

}


bool UInventoryComponent::AddItem(UItem* Item)
{
	if (Items.Num() >= Capacity || !Item)
	{
		return false;
	}

	Items.Add(Item);
	Item->OwningInventory = this;

	if (OnInventoryUpdated.IsBound())
	{
		OnInventoryUpdated.Broadcast();
	}

	return true;
}

bool UInventoryComponent::RemoveItem(UItem* Item)
{
	if (!Items.Contains(Item) || !Item)
	{
		return false;
	}

	Items.Pop(Item);

	if (OnInventoryUpdated.IsBound())
	{
		OnInventoryUpdated.Broadcast();
	}

	return true;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for (const auto& Item : DefaultItems)
	{
		AddItem(Item);
	}
	
}

