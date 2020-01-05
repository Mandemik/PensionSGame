// Made by Andrzej Serazetdinow

#include "InteractableComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/WidgetComponent.h"

#include "InteractionLog.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	InteractionMarker = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractableMarker"));
	InteractionWidgetOnInteractable = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidgetOnInteractable"));

	if (GetOwner())
	{
		this->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

		InteractionMarker->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
		InteractionWidgetOnInteractable->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void UInteractableComponent::Interact()
{
	if (InteractDelegate.IsBound())
	{
		InteractDelegate.Broadcast();
	}
}

void UInteractableComponent::UnsubscribeFromPlayer()
{
	SetComponentTickEnabled(false);

	SubscribedPlayer = nullptr;
	Component = nullptr;
}

void UInteractableComponent::SubscribeToPlayer(AActor* Player)
{
	if (Player)
	{
		SetComponentTickEnabled(true);
		SubscribedPlayer = Player;

		if (SubscribedPlayer)
		{
			Component = Cast<UPlayerInteractionComponent>(SubscribedPlayer->FindComponentByClass(UPlayerInteractionComponent::StaticClass()));
		}
	}
}

const uint8 UInteractableComponent::GetPriority()
{
	return InteractableStructure.Priority;
}

const bool UInteractableComponent::CanInteract()
{
	if (InteractableStructure.bDisabled)
	{
		return false;
	}

	if (InteractableStructure.bHasToBeReacheable)
	{
		if (!CheckReachability())
		{ 
			return false;
		}
	}

	if (InteractableStructure.bDoesAngleMatter)
	{
		if (CheckAngleToPlayer() > InteractableStructure.PlayersAngleMarginOfErrorToInteractable)
		{
			return false;
		}
	}

	if (InteractableStructure.bDoesDistanceToPlayerMatter)
	{
		if (CheckDistanceToPlayer() > InteractableStructure.MaximumDistanceToPlayer)
		{
			return false;
		}
	}

	return true;
}

bool UInteractableComponent::CheckReachability() const
{
	if (GetOwner() && SubscribedPlayer && GetWorld())
	{
		const FVector& InteractableLocation = GetComponentLocation();
		const FVector& PlayerLocation = SubscribedPlayer->GetActorLocation();

		FCollisionQueryParams CollisionParams;
		FHitResult OutHit;

		if (InteractableStructure.bDrawDebugLineForReachability)
		{
			DrawDebugLine(GetWorld(), PlayerLocation, InteractableLocation, FColor::Green, false, 0.1f, 1, 1.f);
		}

		if (GetWorld()->LineTraceSingleByChannel(OutHit, PlayerLocation, InteractableLocation, ECC_Visibility, CollisionParams))
		{
			if (OutHit.bBlockingHit)
			{
				if(OutHit.GetActor() && OutHit.GetActor()->FindComponentByClass(this->GetClass()) == this)
				{
					return true;
				}
			}
		}
	}

	return false;
}

const float UInteractableComponent::CheckDistanceToPlayer() const
{
	if (GetOwner() && SubscribedPlayer)
	{
		const FVector& InteractableLocation = GetComponentLocation();
		const FVector& PlayerLocation = SubscribedPlayer->GetActorLocation();

		return FVector::Dist(InteractableLocation, PlayerLocation);
	}

	return -1.f;
}

const float UInteractableComponent::CheckAngleToPlayer() const
{
	FVector InteractableLocation = GetComponentLocation();

	if (SubscribedPlayer)
	{
		UArrowComponent* Arrow = Cast<UArrowComponent>(SubscribedPlayer->FindComponentByClass(UArrowComponent::StaticClass()));

		if (Arrow)
		{
			FVector PlayerForwardVector = Arrow->GetForwardVector();
			FVector const PlayerToInteractableLocation = (InteractableLocation - SubscribedPlayer->GetActorLocation()).GetSafeNormal();

			PlayerForwardVector.Normalize();

			float Dot = FVector::DotProduct(PlayerForwardVector, PlayerToInteractableLocation);
			float Angle = FMath::Acos(Dot);

			return Angle * (180.f / PI); //Rad to Degrees
		}
	}

	return 0.f;
}

void UInteractableComponent::DrawDebugStrings()
{

	if (GetOwner() && GetWorld())
	{
		AmountOfDebugStrings = 1;

		// Priority
		DrawDebugString(GetWorld(),
			InteractableStructure.bOverrideDebugStringLocation ? InteractableStructure.NewDebugStringLocation
			: GetComponentLocation(), FString("Priority: ") + FString::FromInt(InteractableStructure.Priority), NULL,
			InstancedDSP.ValidTextColor, 0.01f, InstancedDSP.bDrawShadow, InstancedDSP.FontScale);

		// Whether or not the object is disabled
		DrawDebugString(GetWorld(),
			InteractableStructure.bOverrideDebugStringLocation ? InteractableStructure.NewDebugStringLocation
			: FVector(GetComponentLocation().X, GetComponentLocation().Y, GetComponentLocation().Z - InstancedDSP.HeightDifferenceInDebugStrings),
			FString("Usability: ") + (InteractableStructure.bDisabled ? FString("Disabled") : FString("Enabled")), NULL,
			!InteractableStructure.bDisabled ? InstancedDSP.ValidTextColor : InstancedDSP.InvalidTextColor,
			0.01f, InstancedDSP.bDrawShadow, InstancedDSP.FontScale);

		AmountOfDebugStrings++;

		// Distance to player
		if (InteractableStructure.bDoesDistanceToPlayerMatter)
		{
			DrawDebugString(GetWorld(),
				InteractableStructure.bOverrideDebugStringLocation ? InteractableStructure.NewDebugStringLocation
				: FVector(GetComponentLocation().X, GetComponentLocation().Y, GetComponentLocation().Z - (AmountOfDebugStrings * InstancedDSP.HeightDifferenceInDebugStrings)),
				FString("Distance: ") + FString::SanitizeFloat(CheckDistanceToPlayer(), 2), NULL,
				CheckDistanceToPlayer() < InteractableStructure.MaximumDistanceToPlayer ? InstancedDSP.ValidTextColor : InstancedDSP.InvalidTextColor,
				0.01f, InstancedDSP.bDrawShadow, InstancedDSP.FontScale);

			AmountOfDebugStrings++;
		}

		// Reachability
		if (InteractableStructure.bHasToBeReacheable)
		{
			DrawDebugString(GetWorld(),
				InteractableStructure.bOverrideDebugStringLocation ? InteractableStructure.NewDebugStringLocation
				: FVector(GetComponentLocation().X, GetComponentLocation().Y, GetComponentLocation().Z - (AmountOfDebugStrings * InstancedDSP.HeightDifferenceInDebugStrings)),
				CheckReachability() ? FString("Reachability: Reachable") : FString("Reachability: Not Reachable"), NULL,
				CheckReachability() ? InstancedDSP.ValidTextColor : InstancedDSP.InvalidTextColor,
				0.01f, InstancedDSP.bDrawShadow, InstancedDSP.FontScale);

			AmountOfDebugStrings++;
		}
		
		// Angle
		if (InteractableStructure.bDoesAngleMatter)
		{
			DrawDebugString(GetWorld(),
				InteractableStructure.bOverrideDebugStringLocation ? InteractableStructure.NewDebugStringLocation
				: FVector(GetComponentLocation().X, GetComponentLocation().Y, GetComponentLocation().Z - (AmountOfDebugStrings * InstancedDSP.HeightDifferenceInDebugStrings)),
				FString("Angle: ") + FString::SanitizeFloat(CheckAngleToPlayer(), 2), NULL,
				CheckAngleToPlayer() <= InteractableStructure.PlayersAngleMarginOfErrorToInteractable ? InstancedDSP.ValidTextColor : InstancedDSP.InvalidTextColor,
				0.01f, InstancedDSP.bDrawShadow, InstancedDSP.FontScale);

			AmountOfDebugStrings++;
		}

	}
}

void UInteractableComponent::HideInteractionWidgetOnInteractable()
{
	if (InteractionWidgetOnInteractable)
	{
		InteractionWidgetOnInteractable->SetVisibility(false);
		IsInteractionWidgetOnInteractableHidden = true;
	}
}

void UInteractableComponent::HideInteractionMarker()
{
	if (InteractionMarker)
	{
		InteractionMarker->SetVisibility(false);
		IsInteractionMarkerHidden = true;
	}
}

void UInteractableComponent::ShowInteractionWidgetOnInteractable(UUserWidget* Widget = nullptr)
{
	if (Widget && InteractionWidgetOnInteractable->GetWidgetClass() != Widget->GetClass())
	{
		InteractionWidgetOnInteractable->SetWidget(Widget);
		InteractionWidgetOnInteractable->SetVisibility(true);
	}

	else
	{
		InteractionWidgetOnInteractable->SetVisibility(true);
		IsInteractionWidgetOnInteractableHidden = false;
	}
}

void UInteractableComponent::ShowInteractionMarker(UUserWidget* Widget = nullptr)
{
	if (Widget && InteractionMarker->GetWidgetClass() != Widget->GetClass())
	{
		InteractionMarker->SetWidget(Widget);
		InteractionMarker->SetVisibility(true);
	}

	else
	{
		InteractionMarker->SetVisibility(true);
		IsInteractionMarkerHidden = false;
	}
}

void UInteractableComponent::Enable()
{
	if (InteractableStructure.bDisabled)
	{
		InteractableStructure.bDisabled = false;
	}
}

void UInteractableComponent::BeginPlay()
{
	if (InteractableStructure.bRandomizePriority)
	{
		InteractableStructure.Priority = FMath::RandRange(0, 255);
	}

	SetComponentTickEnabled(false);

	Super::BeginPlay();
}

void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if ((InstancedDSP.bDrawDebugStringsByDefault && InteractableStructure.bAlwaysDrawDebugStrings) || InteractableStructure.bAlwaysDrawDebugStrings)
	{
		DrawDebugStrings();
	}

	if (Component)
	{
		if (!InteractableStructure.bDisabled)
		{
			if (IsInteractionMarkerHidden)
			{
				Component->TryShowInteractionMarker(this);
			}

			if (CanInteract())
			{
				Component->TryShowInteractionWidget(this);
				Component->TryShowInteractionWidgetOnInteractable(this);
			}
			else
			{
				Component->TryHideInteractionWidget(this);
				Component->TryHideInteractionWidgetOnInteractable(this);
			}
		}
		else
		{
			Component->TryHideInteractionWidget(this);
			Component->TryHideInteractionWidgetOnInteractable(this);
		}
	}

	if (InteractionMarker->IsVisible() || InteractionWidgetOnInteractable->IsVisible())
	{
		WidgetRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation());

		if (InteractionWidgetOnInteractable->IsVisible())
		{
			InteractionWidgetOnInteractable->SetWorldRotation(WidgetRotation);
		}

		if (InteractionMarker->IsVisible())
		{
			InteractionMarker->SetWorldRotation(WidgetRotation);
		}
	}

	
}

