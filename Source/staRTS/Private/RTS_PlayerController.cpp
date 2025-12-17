// Fill out your copyright notice in the Description page of Project Settings.


#include "RTS_PlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "EnhancedActionKeyMapping.h"
#include "InputActionValue.h"
#include "RTS_HUD.h"
#include "CommonENUMs.h"
#include <Base_Pawn.h>

ARTS_PlayerController::ARTS_PlayerController()
{
	bShowMouseCursor = true;
	
}

void ARTS_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	rtsHUD = Cast<ARTS_HUD>(GetHUD());
}

void ARTS_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!DefaultMappingContext)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* inputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (inputSubsystem)
	{
		inputSubsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(selectAction, ETriggerEvent::Completed, this, &ARTS_PlayerController::Select);

		EnhancedInputComponent->BindAction(selectAction, ETriggerEvent::Started, this, &ARTS_PlayerController::SelectStart);
		EnhancedInputComponent->BindAction(selectAction, ETriggerEvent::Triggered, this, &ARTS_PlayerController::SelectOnGoing);
		EnhancedInputComponent->BindAction(selectAction, ETriggerEvent::Completed, this, &ARTS_PlayerController::SelectEnd);

		EnhancedInputComponent->BindAction(commandAction, ETriggerEvent::Completed, this, &ARTS_PlayerController::CommandSelectedActors);
	}
}

void ARTS_PlayerController::Select(const FInputActionValue& value)
{
	FHitResult hitResult;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Camera, false, hitResult);

	if (hitActor)
	{
		if (hitActor->GetClass()->ImplementsInterface(UISelectable::StaticClass()))
		{
			IISelectable::Execute_SelectActor(hitActor, false);
		}
	}


	hitActor = hitResult.GetActor();

	if (hitActor)
	{
		UE_LOG(LogTemp, Display, TEXT("SELECTED ACTOR: %s"), *hitActor->GetName());


		if (hitActor->GetClass()->ImplementsInterface(UISelectable::StaticClass()))
		{
			IISelectable::Execute_SelectActor(hitActor, true);
		}

	}
}

void ARTS_PlayerController::CommandSelectedActors(const FInputActionValue& value)
{	
	if (bIsIssuingCommand)
	{
		return;
	}

	if (!hitActor)
	{
		return;
	}

	FHitResult hitResult;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Camera, false, hitResult);

	if (!hitResult.bBlockingHit) return;

	if(currentlySelectedActors.Num() > 0)
	{
		for (AActor* anActor : currentlySelectedActors)
		{
			if (anActor && anActor->GetClass()->ImplementsInterface(UNavegable_Interface::StaticClass()))
			{
				INavegable_Interface::Execute_MoveToLocation(anActor, hitResult.Location);
			}
		}
	}
	else
	{
		if (hitActor->GetClass()->ImplementsInterface(UNavegable_Interface::StaticClass()))
		{
			bIsIssuingCommand = true;
			INavegable_Interface::Execute_MoveToLocation(hitActor, hitResult.Location);
			GetWorld()->GetTimerManager().SetTimer(CommandCooldownTimer, [this]()
				{
					bIsIssuingCommand = false;
				}, 0.3f, false);
		}
	}
}

void ARTS_PlayerController::SelectStart(const FInputActionValue& value)
{
	float mouseX, mouseY;
	GetMousePosition(mouseX, mouseY);
	selectionStartPoint = FVector2D(mouseX, mouseY);
}

void ARTS_PlayerController::SelectOnGoing(const FInputActionValue& value)
{
	float mouseX, mouseY;
	GetMousePosition(mouseX, mouseY);
	selectionSize = FVector2D(mouseX, mouseY) - selectionStartPoint;
	if (rtsHUD)
	{
		rtsHUD->ShowSelectBox(selectionStartPoint, selectionSize);
	}
}

void ARTS_PlayerController::SelectEnd(const FInputActionValue& value)
{
	if (rtsHUD)
	{
		rtsHUD->HideSelectionBox();
		FTimerHandle timerMultiSelect;
		GetWorld()->GetTimerManager().SetTimer(timerMultiSelect, this, &ARTS_PlayerController::SelectMultipleActors, 0.05f, false);
	}
}

void ARTS_PlayerController::SelectMultipleActors()
{
	if (rtsHUD)
	{
		for (AActor* anActor : currentlySelectedActors)
		{
			if (anActor && anActor->GetClass()->ImplementsInterface(UISelectable::StaticClass()))
			{
				IISelectable::Execute_SelectActor(anActor, false);
			}
		}
		currentlySelectedActors.Empty();
		TArray<AActor*> allSelectedActors = rtsHUD->GetSelectedActors();


		for (AActor* anActor : allSelectedActors)
		{
			if (anActor && anActor->GetClass()->ImplementsInterface(UISelectable::StaticClass()))
			{
				IISelectable::Execute_SelectActor(anActor, true);
				currentlySelectedActors.AddUnique(anActor);
			}
		}

	}
}
