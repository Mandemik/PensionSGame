// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

class UStaticMesh;
class UTexture2D;
class UInventoryComponent;

UCLASS(BlueprintType, Blueprintable)
class INVENTORY_SYSTEM_API UItem : public UObject
{
	GENERATED_BODY()
	
public:

	UItem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	UStaticMesh* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	UInventoryComponent* OwningInventory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FText UseActionText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FText ItemDisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (MultiLine = true))
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	UTexture2D* Thumbnail;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	float Weight;

	virtual void Use(APawn* User) {};
	
};
