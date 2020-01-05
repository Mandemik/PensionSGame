// Made by Andrzej Serazetdinow

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Interaction_Interface.h"
#include "PlayerInteractionComponent.h"
#include "InteractableComponent.generated.h"

class USphereComponent;
class UWidgetComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteract);

USTRUCT(BlueprintType)
struct FInteractable
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Option")
	bool bAlwaysDrawDebugStrings = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bDoesDistanceToPlayerMatter"), Category = "Interactable Option")
	float MaximumDistanceToPlayer = 125.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bDoesAngleMatter"), Category = "Interactable Option")
	float PlayersAngleMarginOfErrorToInteractable = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bRandomizePriority"), Category = "Interactable Option")
	uint8 Priority = 0;

	// Disable interaction option with this object after usage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Option")
	bool bDisableAfterUsage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Option")
	bool bRandomizePriority = false;

	/*If true the interactable will always check if player can reach to interactable (interactable is not behind wall etc), if false
	the interactable will ignore the reach to interactable.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Option")
	bool bHasToBeReacheable = true;

	/*If true the interactable will always check if distance to player is right, if false
	the interactable will ignore the distance to player.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Option")
	bool bDoesDistanceToPlayerMatter = true;

	/*If true the interactable will always check if player's angle to interactable is right, if false
	the interactable will ignore the player's angle to interactable.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Option")
	bool bDoesAngleMatter = true;

	/*If true the interactable won't work unless you change this value to false inside your
	C++ class or blueprint.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Option")
	bool bDisabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Debug Options")
	bool bOverrideDebugStringLocation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Debug Options")
	bool bDrawDebugLineForReachability = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideDebugStringLocation"), Category = "Interactable Debug Options")
	FVector NewDebugStringLocation;

};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class INTERACTION_SYSTEM_API UInteractableComponent : public USceneComponent, public IInteraction_Interface
{
	GENERATED_BODY()

private:

	UPlayerInteractionComponent* Component;

	FRotator WidgetRotation;

	uint8 AmountOfDebugStrings = 1;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableMarker")
	UWidgetComponent* InteractionMarker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractionWidgetOnInteractable")
	UWidgetComponent* InteractionWidgetOnInteractable;

	TSubclassOf<UUserWidget> InteractableMarkerClass;

	TSubclassOf<UUserWidget> InteractionWidgetOnInteractableClass;

	bool IsInteractionWidgetOnInteractableHidden = true;

	bool IsInteractionMarkerHidden = true;

	bool CanShowInteractionMarker = true;

	UPROPERTY(BlueprintAssignable)
	FOnInteract InteractDelegate;

	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	AActor* SubscribedPlayer;

	UPROPERTY(BlueprintReadWrite, Category = "Interactable Debug Options")
	FDebugStringProperties InstancedDSP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FInteractable InteractableStructure;

public:

	UFUNCTION(BlueprintCallable, Category = "InteractionWidgetOnInteractable")
	void ShowInteractionWidgetOnInteractable(UUserWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "InteractionWidgetOnInteractable")
	void HideInteractionWidgetOnInteractable();

	UFUNCTION(BlueprintCallable, Category = "InteractionMarker")
	void ShowInteractionMarker(UUserWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "InteractionMarker")
	void HideInteractionMarker();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SubscribeToPlayer(AActor* Player) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void UnsubscribeFromPlayer() override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	const uint8 GetPriority() override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	const bool CanInteract() override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact() override;

	// Enable a disabled interactable for interaction
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Enable();

private:

	UInteractableComponent();

	void DrawDebugStrings();

	bool CheckReachability() const;

	const float CheckDistanceToPlayer() const;

	const float CheckAngleToPlayer() const;

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
