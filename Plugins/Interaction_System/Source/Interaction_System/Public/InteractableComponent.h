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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCanInteract);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSubscribed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnsubscribed);

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

	bool bCanBroadcastCanInteract : 1;

	TArray<UPlayerInteractionComponent*> PlayerComponents;

	FRotator WidgetRotation;

	bool bRotateWidgetsTowardsCamera : 1;

	bool bRotateWidgetsTowardsPlayerPawnCMP : 1;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableMarker")
	UWidgetComponent* InteractionMarker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractionWidgetOnInteractable")
	UWidgetComponent* InteractionWidgetOnInteractable;

	TSubclassOf<UUserWidget> InteractableMarkerClass;

	TSubclassOf<UUserWidget> InteractionWidgetOnInteractableClass;

	bool IsInteractionWidgetOnInteractableHidden : 1;

	bool IsInteractionMarkerHidden : 1;

	bool CanShowInteractionMarker : 1;

	FDebugStringProperties InstancedDSP;

	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	TArray<AActor*> SubscribedPlayers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FInteractable InteractableStructure;

	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	int32 AmountOfSubscribedPlayers = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bUseRotationVariablesFromPlayerComponent = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bUseRotationVariablesFromPlayerComponent"), Category = "Interaction")
	bool bRotateWidgetsTowardsPlayerCamera = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bUseRotationVariablesFromPlayerComponent"), Category = "Interaction")
	bool bRotateWidgetsTowardsPlayerPawn = false;

#pragma region Delegates

public:

	UPROPERTY(BlueprintAssignable)
	FOnInteract InteractDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnCanInteract OnCanInteractDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnSubscribed OnSubscribedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnUnsubscribed OnUnsubscribedDelegate;

#pragma endregion

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
	void SubscribeToComponent(AActor* Player) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void UnsubscribeFromComponent(AActor* Player) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	const uint8 GetPriority() override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool IsAnySubscribedPlayerLocallyControlled();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool CanAnyPlayerInteract();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	APawn* GetLocallyControlledPlayer();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	const bool CanInteract(AActor* Player) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact() override;

	// Enable a disabled interactable for interaction
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Enable();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetWidgetRotationSettings(bool IsCameraRotation, bool IsPawnRotation);

private:

	UInteractableComponent();

	void BroadcastCanInteract(UPlayerInteractionComponent* PlayerComponent);

	void RotateWidgetsToPlayerCamera();

	void RotateWidgetsToPlayerPawn();

	void DrawDebugStrings(AActor* Player);

	bool CheckReachability(AActor* SubscribedPlayer) const;

	const float CheckDistanceToPlayer(AActor* SubscribedPlayer) const;

	const float CheckAngleToPlayer(AActor* SubscribedPlayer) const;

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
