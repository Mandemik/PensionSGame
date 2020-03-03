// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"
#include "InventoryComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"

UItem::UItem()
{
	Weight = 1.f;

	ItemDisplayName = FText::FromString("Item");
	UseActionText = FText::FromString("Use");
}

