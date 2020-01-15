// Made by Andrzej Serazetdinow

#include "PlayerInteractionComponent.h"
#include "Interaction_Interface.h"
#include "InteractableComponent.h"

#include "Blueprint/UserWidget.h"

#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"

#include "InteractionLog.h"

UPlayerInteractionComponent::UPlayerInteractionComponent()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(FName("InteractionCollision"));
	PlayerInteractableForwardVector = CreateDefaultSubobject<UArrowComponent>(FName("InteractableForwardVector"));

	if (GetOwner())
	{
		SphereComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		PlayerInteractableForwardVector->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void UPlayerInteractionComponent::BeginPlay()
{
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &UPlayerInteractionComponent::OnOverlapBegin);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &UPlayerInteractionComponent::OnOverlapEnd);

	TArray<AActor*> OverlappingActors; 
	SphereComponent->GetOverlappingActors(OverlappingActors, TSubclassOf<AActor>());

	for (const auto& Actor : OverlappingActors)
	{
		if (Actor != GetOwner())
		{
			if (UInteractableComponent* Component = Cast<UInteractableComponent>(Actor->FindComponentByClass(UInteractableComponent::StaticClass())))
			{
				AddActorToInteract(Actor);
			}
		}
	}

	Super::BeginPlay();
}

void UPlayerInteractionComponent::HideInteractionWidget()
{
	if (InteractionWidgetBlueprint && !IsInteractionWidgetHidden)
	{
		InteractionWidgetBlueprint->SetVisibility(ESlateVisibility::Hidden);
		IsInteractionWidgetHidden = true;
	}
}

void UPlayerInteractionComponent::TryHideInteractionWidgetOnInteractable(UInteractableComponent* Component)
{
	Component->HideInteractionWidgetOnInteractable();
}

void UPlayerInteractionComponent::TryHideInteractionMarker(UInteractableComponent* Component)
{
	Component->HideInteractionMarker();
}

void UPlayerInteractionComponent::TryHideInteractionWidget(UInteractableComponent* Component)
{
	if (IsInteractionWidgetHidden)
	{
		return;
	}

	if (Component && !CanInteractWithAnyInteractable())
	{
		HideInteractionWidget();
	}
}

void UPlayerInteractionComponent::RemoveActorToInteract(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	UInteractableComponent* Component = Cast<UInteractableComponent>(Actor->FindComponentByClass(UInteractableComponent::StaticClass()));

	if (Component)
	{
		if (ActorsToInteract.Contains(Component))
		{

			if (Component->SubscribedPlayer)
			{
				Component->UnsubscribeFromPlayer();
				TryHideInteractionMarker(Component);
				TryHideInteractionWidgetOnInteractable(Component);
			}

			ActorsToInteract.Remove(Component);

			if (!ActorsToInteract.Num())
			{
				TryHideInteractionWidget(Component);
			}
			
			if (CanShowSystemLog)
			{
				UE_LOG(InteractionSystem, Log, TEXT("Removed %s from ActorsToInteract for %s player. Amount of actors to interact equals: %d"), *GetNameSafe(Actor), *GetNameSafe(GetOwner()), ActorsToInteract.Num());
			}
		}
		else
		{
			if (CanShowSystemLog)
			{
				UE_LOG(InteractionSystem, Log, TEXT("Actors to interact do not contain component this component. Therefore it won't be removed from ActorsToInteract for player %s."), *GetNameSafe(GetOwner()));
			}
		}
	}
}

void UPlayerInteractionComponent::ChangeInteractionWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if (WidgetClass)
	{
		ShowInteractionWidget(WidgetClass);
	}
}

void UPlayerInteractionComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetOwner())
	{
		AddActorToInteract(OtherActor);
	}
}

void UPlayerInteractionComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != GetOwner())
	{
		RemoveActorToInteract(OtherActor);
	}
}

void UPlayerInteractionComponent::InteractWithInteractables()
{
	if (ActorsToInteract.Num() > 0)
	{
		ActorsToInteract.Sort([](UInteractableComponent& LHS, UInteractableComponent& RHS)
		{
			return LHS.GetPriority() < RHS.GetPriority();
		});

		for (UInteractableComponent* ActorToInteract : ActorsToInteract)
		{
			if (ActorToInteract->CanInteract())
			{
				ActorToInteract->Interact();

				if (ActorToInteract->InteractableStructure.bDisableAfterUsage)
				{
					ActorToInteract->InteractableStructure.bDisabled = true;
					TryHideInteractionMarker(ActorToInteract);
				}

				return;
			}
		}
	}
}

void UPlayerInteractionComponent::ShowInteractionWidgetOnInteractable(TSubclassOf<UUserWidget> WidgetClass, UInteractableComponent* Component)
{
	if (Component->IsInteractionWidgetOnInteractableHidden && InteractionWidgetOnInteractable && Component->InteractionWidgetOnInteractableClass && Component->InteractionWidgetOnInteractableClass->GetClass() == WidgetClass)
	{
		Component->ShowInteractionWidgetOnInteractable(InteractionWidgetOnInteractable);
		Component->IsInteractionWidgetOnInteractableHidden = false;

		return;
	}

	if (bHideInteractionMarkerWhenPlayerCanInteract && Component && InteractionWidgetOnInteractable)
	{
		Component->ShowInteractionWidgetOnInteractable(InteractionWidgetOnInteractable);
		Component->IsInteractionWidgetOnInteractableHidden = false;

		Component->HideInteractionMarker();
		Component->IsInteractionMarkerHidden = true;

		return;
	}

	if (Component->InteractionWidgetOnInteractableClass && Component->InteractionWidgetOnInteractableClass->GetClass() != WidgetClass && PC && PC->IsLocalPlayerController())
	{
		InteractionWidgetOnInteractable = CreateWidget<UUserWidget>(PC, WidgetClass, TEXT("InteractableWidgetOnInteractable"));

		if (InteractionWidgetOnInteractable)
		{
			Component->ShowInteractionWidgetOnInteractable(InteractionWidgetOnInteractable);

			if (Component->InteractionWidgetOnInteractable)
			{
				Component->InteractionWidgetOnInteractableClass = WidgetClass;
				Component->IsInteractionWidgetOnInteractableHidden = false;
			}
		}
	}
	else
	{

		if (!PC)
		{
			SetPC();
		}

		if (PC && PC->IsLocalPlayerController())
		{
			InteractionWidgetOnInteractable = CreateWidget<UUserWidget>(PC, WidgetClass, TEXT("InteractableWidgetOnInteractable"));

			if (InteractionWidgetOnInteractable)
			{
				if (bHideInteractionMarkerWhenPlayerCanInteract && Component && InteractionWidgetOnInteractable)
				{
					Component->ShowInteractionMarker(InteractionWidgetOnInteractable);

					return;
				}

				Component->ShowInteractionWidgetOnInteractable(InteractionWidgetOnInteractable);

				if (Component->InteractionWidgetOnInteractable)
				{
					Component->InteractionWidgetOnInteractableClass = WidgetClass;
					Component->IsInteractionWidgetOnInteractableHidden = false;
				}
			}
		}

	}
}

void UPlayerInteractionComponent::ShowInteractionMarker(TSubclassOf<UUserWidget> WidgetClass, UInteractableComponent* Component)
{
	if (Component->IsInteractionMarkerHidden && InteractionMarker && Component->InteractableMarkerClass && Component->InteractableMarkerClass->GetClass() == WidgetClass)
	{
		Component->ShowInteractionMarker(InteractionMarker);
		Component->IsInteractionMarkerHidden = false;

		return;
	}

	if (Component->InteractableMarkerClass && Component->InteractableMarkerClass->GetClass() != WidgetClass && PC && PC->IsLocalPlayerController())
	{
		InteractionMarker = CreateWidget<UUserWidget>(PC, WidgetClass, TEXT("InteractableMarker"));

		if (InteractionMarker)
		{
			Component->ShowInteractionMarker(InteractionMarker);

			if (Component->InteractionMarker)
			{
				Component->InteractableMarkerClass = WidgetClass;
				Component->IsInteractionMarkerHidden = false;
			}
		}
	}
	else
	{

		if (!PC)
		{
			SetPC();
		}

		if (PC && PC->IsLocalPlayerController())
		{
			InteractionMarker = CreateWidget<UUserWidget>(PC, WidgetClass, TEXT("InteractableMarker"));

			if (InteractionMarker)
			{
				Component->ShowInteractionMarker(InteractionMarker);

				if (Component->InteractionMarker)
				{
					Component->InteractableMarkerClass = WidgetClass;
					Component->IsInteractionMarkerHidden = false;
				}
			}
		}
	}
}

void UPlayerInteractionComponent::ShowInteractionWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if (InteractionWidgetBlueprint && InteractionWidgetBlueprint->GetClass() != WidgetClass && PC && PC->IsLocalPlayerController())
	{
		InteractionWidgetBlueprint = CreateWidget<UUserWidget>(PC, WidgetClass, TEXT("InteractableWidget"));

		if (InteractionWidgetBlueprint)
		{
			InteractionWidgetBlueprint->AddToViewport();
			IsInteractionWidgetHidden = false;
		}
	}

	else if (InteractionWidgetBlueprint && InteractionWidgetBlueprint->GetVisibility() == ESlateVisibility::Hidden)
	{
		InteractionWidgetBlueprint->SetVisibility(ESlateVisibility::Visible);
		IsInteractionWidgetHidden = false;

		return;
	}

	else
	{
		if (!PC)
		{
			SetPC();
		}

		if (PC && PC->IsLocalPlayerController())
		{
			InteractionWidgetBlueprint = CreateWidget<UUserWidget>(PC, WidgetClass, TEXT("InteractableWidget"));

			if (InteractionWidgetBlueprint)
			{
				InteractionWidgetBlueprint->AddToViewport();
				IsInteractionWidgetHidden = false;
			}
		}
	}
}

void UPlayerInteractionComponent::SetPC()
{
	PWN = Cast<APawn>(GetOwner());

	if (PWN)
	{
		PC = Cast<APlayerController>(PWN->GetController());
	}
}

bool UPlayerInteractionComponent::CanInteractWithAnyInteractable()
{
	for (UInteractableComponent* ActorToInteract : ActorsToInteract)
	{
		if (ActorToInteract->CanInteract())
		{
			return true;
		}
	}

	return false;
}

void UPlayerInteractionComponent::TryShowInteractionWidgetOnInteractable(UInteractableComponent* Component)
{
	if (!Component || Component->InteractableStructure.bDisabled)
	{
		return;
	}

	if (InteractionWidgetOnInteractableBP)
	{
		ShowInteractionWidgetOnInteractable(InteractionWidgetOnInteractableBP, Component);
	}
}

void UPlayerInteractionComponent::TryShowInteractionMarker(UInteractableComponent* Component)
{
	if (!Component || Component->InteractableStructure.bDisabled || (bHideInteractionMarkerWhenPlayerCanInteract && !Component->IsInteractionWidgetOnInteractableHidden))
	{
		return;
	}

	if (InteractableMarkerBP)
	{
		ShowInteractionMarker(InteractableMarkerBP, Component);
	}
}

void UPlayerInteractionComponent::TryShowInteractionWidget(UInteractableComponent* Component)
{
	if (!Component || Component->InteractableStructure.bDisabled)
	{
		return;
	}

	if (InteractionWidgetBP)
	{
		ShowInteractionWidget(InteractionWidgetBP);
	}
}

void UPlayerInteractionComponent::AddActorToInteract(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	UInteractableComponent* Component = Cast<UInteractableComponent>(Actor->FindComponentByClass(UInteractableComponent::StaticClass()));

	if (Component && !ActorsToInteract.Contains(Component))
	{
		ActorsToInteract.Add(Component);
		Component->SubscribeToPlayer(GetOwner());
		Component->InstancedDSP = DSProperties;
		
		if (CanShowSystemLog)
		{
			UE_LOG(InteractionSystem, Log, TEXT("Added %s to ActorsToInteract for %s player. Amount of actors to interact equals: %d"), *GetNameSafe(Actor), *GetNameSafe(GetOwner()), ActorsToInteract.Num());
		}
	}
}