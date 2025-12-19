// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS_PlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "EnhancedActionKeyMapping.h"
#include "InputActionValue.h"
#include "RTS_HUD.h"
#include "IWorkerInterface.h"
#include "ResourceCollectable.h"
#include "IStorageInterface.h"
#include "CommonENUMs.h"
#include "Base_Pawn.h"
#include "Kismet/GameplayStatics.h"

ARTS_PlayerController::ARTS_PlayerController()
{
    bShowMouseCursor = true;
}

void ARTS_PlayerController::BeginPlay()
{
    Super::BeginPlay();
    rtsHUD = Cast<ARTS_HUD>(GetHUD());
    
    // Find default storage at start (optional)
    FindDefaultStorage();
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

    // Deselect previous actor
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
    if (bIsIssuingCommand) return;

    FHitResult hitResult;
    
    GetHitResultUnderCursor(ECollisionChannel::ECC_Camera, false, hitResult);
    
    if (!hitResult.bBlockingHit)
    {
        UE_LOG(LogTemp, Warning, TEXT("No blocking hit - ignoring command"));
        return;
    }
    currentlySelectedActors.Empty();    
    AActor* targetActor = hitResult.GetActor();
    

    UE_LOG(LogTemp, Warning, TEXT("Command: TargetActor = %s"), 
           targetActor ? *targetActor->GetName() : TEXT("NULL"));

    // Handle multiple selected actors
    if (currentlySelectedActors.Num() > 0)
    {
        for (AActor* selectedActor : currentlySelectedActors)
        {
            // VERIFICACIÓN CRÍTICA
            if (IsValid(selectedActor) && selectedActor->Implements<UIWorkerInterface>())
            {
                UE_LOG(LogTemp, Warning, TEXT("Enviando orden a Worker válido: %s"), *selectedActor->GetName());
        
                // Usar la forma estática de la interfaz es más seguro
                IIWorkerInterface::Execute_GatherResource(selectedActor, targetActor);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Un actor seleccionado ya no es válido o no es Worker!"));
            }
        }
    }
    else if (hitActor)
    {
        if (GetHitResultUnderCursor(ECC_Visibility, false, hitResult))
        {
            // LOG DE PRUEBA
            UE_LOG(LogTemp, Warning, TEXT("MOUSE CLICK: Actor=%s, Location=%s"), 
                   targetActor ? *targetActor->GetName() : TEXT("Suelo"), 
                   *hitResult.ImpactPoint.ToString());

            ProcessCommand(hitActor, targetActor, hitResult.ImpactPoint);
        }
        ProcessCommand(hitActor, targetActor, hitResult.ImpactPoint);
    }

    // Cooldown
    bIsIssuingCommand = true;
    GetWorld()->GetTimerManager().SetTimer(CommandCooldownTimer, [this]()
    {
        bIsIssuingCommand = false;
    }, 0.3f, false);
}

void ARTS_PlayerController::ProcessCommand(AActor* CommandedActor, AActor* TargetActor, FVector TargetLocation)
{
    UE_LOG(LogTemp, Warning, TEXT("=== ProcessCommand ENTRY ==="));
    UE_LOG(LogTemp, Warning, TEXT("CommandedActor: %s"), 
           CommandedActor ? *CommandedActor->GetName() : TEXT("NULL"));
    UE_LOG(LogTemp, Warning, TEXT("TargetActor: %s"), 
           TargetActor ? *TargetActor->GetName() : TEXT("NULL"));
    
    if (!CommandedActor)
    {
        UE_LOG(LogTemp, Error, TEXT("ProcessCommand: CommandedActor is NULL - ABORTING"));
        return;
    }

    bool bIsWorker = CommandedActor->Implements<UIWorkerInterface>();
    bool bCanMove = CommandedActor->Implements<UNavegable_Interface>();

    // PRIORITY 1: Command on Resource (SOLO SI HAY TARGET)
    if (bIsWorker && TargetActor && IsValid(TargetActor))
    {
        UE_LOG(LogTemp, Log, TEXT("Checking if target is resource..."));
        
        if (TargetActor->Implements<UResourceCollectable>())
        {
            UE_LOG(LogTemp, Log, TEXT("Target IS a resource!"));
            
            ABase_Pawn* WorkerPawn = Cast<ABase_Pawn>(CommandedActor);
            if (!WorkerPawn)
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to cast to ABase_Pawn!"));
                return;
            }

            // Ensure storage
            if (!WorkerPawn->TargetStorageActor)
            {
                if (DefaultStorageActor && IsValid(DefaultStorageActor))
                {
                    WorkerPawn->SetTargetStorage(DefaultStorageActor);
                }
                else
                {
                    AActor* NearestStorage = FindNearestStorage(WorkerPawn->GetActorLocation());
                    if (NearestStorage)
                    {
                        WorkerPawn->SetTargetStorage(NearestStorage);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("No storage available!"));
                        return;
                    }
                }
            }

            // LLAMAR GatherResource - con log
            UE_LOG(LogTemp, Warning, TEXT("CALLING Execute_GatherResource: Worker=%s, Resource=%s"), 
                   *CommandedActor->GetName(), *TargetActor->GetName());
            
            IIWorkerInterface::Execute_GatherResource(CommandedActor, TargetActor);
            
            UE_LOG(LogTemp, Log, TEXT("Execute_GatherResource returned successfully"));
            return;
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("Target is NOT a resource"));
        }
    }
    else
    {
        if (!bIsWorker)
            UE_LOG(LogTemp, Log, TEXT("Actor is not a worker"));
        if (!TargetActor)
            UE_LOG(LogTemp, Log, TEXT("No target actor - will move to location"));
    }

    // PRIORITY 2: Storage
    if (bIsWorker && TargetActor && IsValid(TargetActor))
    {
        if (TargetActor->Implements<UIStorageInterface>())
        {
            ABase_Pawn* WorkerPawn = Cast<ABase_Pawn>(CommandedActor);
            if (WorkerPawn)
            {
                WorkerPawn->SetTargetStorage(TargetActor);
                
                if (IIWorkerInterface::Execute_IsCarryingResources(CommandedActor))
                {
                    IIWorkerInterface::Execute_DeliverResource(CommandedActor, TargetActor);
                }
            }
            return;
        }
    }

    if (bCanMove)
    {
        FVector moveLocation;

        if (TargetActor && (TargetActor->Implements<UResourceCollectable>() || TargetActor->Implements<UIStorageInterface>()))
        {
            moveLocation = TargetActor->GetActorLocation();
        }
        else
        {
            moveLocation = TargetLocation;
        }
        
        UE_LOG(LogTemp, Warning, TEXT("Moving %s to destination: %s"), *CommandedActor->GetName(), *moveLocation.ToString());
        INavegable_Interface::Execute_MoveToLocation(CommandedActor, moveLocation);
    }
}


AActor* ARTS_PlayerController::FindNearestStorage(FVector FromLocation)
{
    TArray<AActor*> FoundStorages;
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UIStorageInterface::StaticClass(), FoundStorages);

    if (FoundStorages.Num() == 0)
    {
        return nullptr;
    }

    AActor* NearestStorage = nullptr;
    float NearestDistance = MAX_FLT;

    for (AActor* Storage : FoundStorages)
    {
        float Distance = FVector::Dist(FromLocation, Storage->GetActorLocation());
        if (Distance < NearestDistance)
        {
            NearestDistance = Distance;
            NearestStorage = Storage;
        }
    }

    return NearestStorage;
}

void ARTS_PlayerController::FindDefaultStorage()
{
    TArray<AActor*> FoundStorages;
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UIStorageInterface::StaticClass(), FoundStorages);

    if (FoundStorages.Num() > 0)
    {
        // Use the first one found as default, or find one tagged as "MainStorage"
        for (AActor* Storage : FoundStorages)
        {
            if (Storage->ActorHasTag(FName("MainStorage")))
            {
                DefaultStorageActor = Storage;
                UE_LOG(LogTemp, Log, TEXT("Found main storage: %s"), *Storage->GetName());
                return;
            }
        }

        // If no main storage tagged, use first one
        DefaultStorageActor = FoundStorages[0];
        UE_LOG(LogTemp, Log, TEXT("Using default storage: %s"), *DefaultStorageActor->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No storage buildings found in level!"));
    }
}

void ARTS_PlayerController::SetDefaultStorage(AActor* StorageActor)
{
    if (StorageActor && StorageActor->Implements<UIStorageInterface>())
    {
        DefaultStorageActor = StorageActor;
        UE_LOG(LogTemp, Log, TEXT("Default storage set to: %s"), *StorageActor->GetName());
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
        // Deselect all previously selected actors
        for (AActor* anActor : currentlySelectedActors)
        {
            if (anActor && anActor->GetClass()->ImplementsInterface(UISelectable::StaticClass()))
            {
                IISelectable::Execute_SelectActor(anActor, false);
            }
        }

        // Get and select new actors
        TArray<AActor*> allSelectedActors = rtsHUD->GetSelectedActors();

        for (AActor* anActor : allSelectedActors)
        {
            if (anActor && anActor->GetClass()->ImplementsInterface(UISelectable::StaticClass()))
            {
                IISelectable::Execute_SelectActor(anActor, true);
                currentlySelectedActors.AddUnique(anActor);
            }
        }

        UE_LOG(LogTemp, Log, TEXT("Selected %d actors"), currentlySelectedActors.Num());
    }
}
