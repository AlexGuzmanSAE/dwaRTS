
#include "Base_Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "AITypes.h"
#include "NavigationSystem.h"
#include "ResourceCollectable.h"
#include "IStorageInterface.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "CommonEnum.h"
#include "Kismet/GameplayStatics.h"

ABase_Pawn::ABase_Pawn()
{
    PrimaryActorTick.bCanEverTick = true;

    capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule_Component"));
    capsule->InitCapsuleSize(32.f, 96.0f);
    RootComponent = capsule;
    capsule->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
    capsule->SetCanEverAffectNavigation(false);

    capsule->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
    capsule->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);

    skeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal_Mesh"));
    skeletalMesh->SetupAttachment(RootComponent);

    floatPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating_Pawn_Movement"));

    selectedIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Selected_Indicator"));
    selectedIndicator->SetupAttachment(RootComponent);
    selectedIndicator->SetHiddenInGame(true);
    selectedIndicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    selectedIndicator->SetCanEverAffectNavigation(false);

    AIControllerClass = AAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ABase_Pawn::BeginPlay()
{
    Super::BeginPlay();
    
    // Inicialización de variables
    TargetResourceActor = nullptr;
    TargetStorageActor = nullptr;
    AvailableResources.Empty();
    AvailableStorages.Empty(); // Limpiar también la lista de almacenes

    // Delay de inicialización para esperar a que World Partition cargue los actores
    FTimerHandle InitDelayHandle;
    GetWorld()->GetTimerManager().SetTimer(InitDelayHandle, [this]()
    {
        if (IsValid(this) && GetWorld())
        {
            CacheAllResources();
            CacheAllStorages(); // <--- IMPORTANTE cachear ambos
            
            UE_LOG(LogTemp, Log, TEXT("Worker %s: %d Resources, %d Storages found"), 
                   *GetName(), AvailableResources.Num(), AvailableStorages.Num());
        }
    }, 0.5f, false);
}

void ABase_Pawn::Move()
{
    if (!bMoving)
    {
        return;
    }

    FVector moveDirection = moveTargetLocation - GetActorLocation();
    if (moveDirection.Length() < minDistanceToTarget)
    {
        bMoving = false;
        return;
    }

    moveDirection.Normalize(1);
    
    FRotator desiredRotation = UKismetMathLibrary::MakeRotFromX(moveDirection);
    desiredRotation.Pitch = 0.f;
    desiredRotation.Roll = 0.f;

    FRotator newRotation = FMath::RInterpTo(GetActorRotation(), desiredRotation, 
                                            GetWorld()->GetDeltaSeconds(), interpSpeed);
    SetActorRotation(newRotation);
} 

void ABase_Pawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    Move();
    UpdateWorkerState();
    if (TargetResourceActor && CurrentWorkerState == EWorkerState::MovingToResource)
    {
        float CurrentDist = FVector::Dist(GetActorLocation(), TargetResourceActor->GetActorLocation());
        GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Yellow, 
            FString::Printf(TEXT("Distancia: %f / Rango: %f | bMoving: %s"), 
            CurrentDist, GatheringRange, bMoving ? TEXT("TRUE") : TEXT("FALSE")));
    }
}

void ABase_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABase_Pawn::SelectActor_Implementation(const bool isSelected)
{
    selectedIndicator->SetHiddenInGame(!isSelected);
}

void ABase_Pawn::MoveToLocation_Implementation(const FVector targetLocation)
{
    // 1. Verificación básica
    if (!IsValid(this)) return;

    if (CurrentWorkerState != EWorkerState::Idle)
    {
        if (CurrentWorkerState == EWorkerState::Gathering) StopGathering();
        CurrentWorkerState = EWorkerState::Idle;
    }

    moveTargetLocation = targetLocation + FVector(0, 0, GetDefaultHalfHeight());

    // En lugar de castear cada vez, verifica si el Controller es válido primero
    AController* CurrentController = GetController();
    if (!CurrentController)
    {
        UE_LOG(LogTemp, Error, TEXT("%s no tiene controlador asignado!"), *GetName());
        return;
    }

    AAIController* pawnAIController = Cast<AAIController>(CurrentController);
    if (pawnAIController)
    {
        // Forzamos el stop antes de una nueva orden para limpiar el PathFollowing
        pawnAIController->StopMovement(); 
        
        bMoving = true;
        pawnAIController->MoveToLocation(targetLocation, minDistanceToTarget);
    }
}

// ===============================================
// WORKER INTERFACE IMPLEMENTATION
// ===============================================

void ABase_Pawn::GatherResource_Implementation(AActor* ResourceActor)
{
    // LOGS CRÍTICOS PRIMERO
    UE_LOG(LogTemp, Warning, TEXT("=== GatherResource ENTRY ==="));
    UE_LOG(LogTemp, Warning, TEXT("ResourceActor pointer: %p"), ResourceActor);
    
    // 1. VERIFICACIÓN DE NULIDAD (Crucial para evitar el 0x00000018)
    if (!ResourceActor || !IsValid(ResourceActor))
    {
        UE_LOG(LogTemp, Error, TEXT("GatherResource: ResourceActor es nulo o inválido!"));
        return;
    }

    // 2. VERIFICACIÓN DE INTERFAZ
    // Nota: Usamos GetClass()->ImplementsInterface para mayor seguridad en Blueprints
    if (!ResourceActor->GetClass()->ImplementsInterface(UResourceCollectable::StaticClass()))
    {
        UE_LOG(LogTemp, Error, TEXT("GatherResource: %s no implementa la interfaz de recurso"), *ResourceActor->GetName());
        return;
    }

    if (ResourceActor->GetClass()->ImplementsInterface(UResourceCollectable::StaticClass()))
    {
        TargetResourceActor = ResourceActor;
        CurrentWorkerState = EWorkerState::MovingToResource;
        
        MoveToLocation_Implementation(TargetResourceActor->GetActorLocation());
    }
    
    UE_LOG(LogTemp, Log, TEXT("SUCCESS: Worker %s assigned to gather from %s"), 
           *GetName(), *TargetResourceActor->GetName());
}

void ABase_Pawn::DeliverResource_Implementation(AActor* StorageActor)
{
    // Si nos pasan un nulo, intentamos buscar el más cercano por nuestra cuenta
    AActor* ActualStorage = StorageActor;
    if (!IsValid(ActualStorage))
    {
        ActualStorage = FindNearestStorage(GetActorLocation());
    }

    if (IsValid(ActualStorage))
    {
        TargetStorageActor = ActualStorage;
        CurrentWorkerState = EWorkerState::MovingToStorage;
        MoveToLocation_Implementation(TargetStorageActor->GetActorLocation());
    }
    if (!StorageActor || !StorageActor->Implements<UIStorageInterface>())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid storage actor or doesn't implement IStorageInterface"));
        return;
    }

    TargetStorageActor = StorageActor;
    CurrentWorkerState = EWorkerState::MovingToStorage;
    
    // Move to storage
    FVector storageLocation = TargetStorageActor->GetActorLocation();
    MoveToLocation_Implementation(storageLocation);
    
    UE_LOG(LogTemp, Log, TEXT("Worker moving to deliver resources to %s"), *TargetStorageActor->GetName());
}

bool ABase_Pawn::IsCarryingResources_Implementation() const
{
    return CurrentAmount > 0.0f;
}

// ===============================================
// HELPER FUNCTIONS
// ===============================================

void ABase_Pawn::UpdateWorkerState()
{
    switch (CurrentWorkerState)
    {
        case EWorkerState::Idle:
            // Wait for commands - don't do anything
            break;

        case EWorkerState::MovingToResource:
        if (!IsValid(TargetResourceActor)) {
            CurrentWorkerState = EWorkerState::Idle;
            break;
        }

        // USAREMOS GetDistanceTo que es más fiable que FVector::Dist
        if (GetDistanceTo(TargetResourceActor) <= GatheringRange)
        {
            // IMPORTANTE: Primero cambiamos el estado, LUEGO paramos el movimiento
            CurrentWorkerState = EWorkerState::Gathering;
        
            AAIController* AIC = Cast<AAIController>(GetController());
            if (AIC) AIC->StopMovement();
        
            bMoving = false;
        
            UE_LOG(LogTemp, Warning, TEXT("!!! DISTANCIA CORRECTA: INICIANDO StartGathering !!!"));
            StartGathering();
        }
            break;

        case EWorkerState::Gathering:
            // Gathering is handled by timer
            // Check if resource is still valid
            if (!TargetResourceActor || !IsValid(TargetResourceActor))
            {
                StopGathering();
                CurrentWorkerState = EWorkerState::Idle;
                TargetResourceActor = nullptr;
                
                // If carrying resources, try to deliver
                if (CurrentAmount > 0.0f && TargetStorageActor && IsValid(TargetStorageActor))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Worker %s: Deberia ir a storage"), *GetName());
                    CurrentWorkerState = EWorkerState::MovingToStorage;
                    DeliverResource_Implementation(TargetStorageActor);
                }
                break;
            }
            
            // Check if inventory full
            if (CurrentAmount >= MaxCapacity)
            {
                StopGathering();
                
                // If we have resources, deliver them
                if (CurrentAmount > 0.0f && TargetStorageActor && IsValid(TargetStorageActor))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Worker %s: Deberia ir a storage FULL INVENTORY"), *GetName());
                    CurrentWorkerState = EWorkerState::MovingToStorage;
                    DeliverResource_Implementation(TargetStorageActor);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Worker %s: Inventory full but no valid storage!"), *GetName());
                    CurrentWorkerState = EWorkerState::Idle;
                }
            }
            break;

    case EWorkerState::MovingToStorage:
        // 1. Si el almacén ya no es válido, buscamos el más cercano de la lista que cacheamos
        if (!IsValid(TargetStorageActor))
        {
            UE_LOG(LogTemp, Warning, TEXT("Storage perdido, buscando uno nuevo..."));
            TargetStorageActor = FindNearestStorage(GetActorLocation());
        
            if (IsValid(TargetStorageActor))
            {
                MoveToLocation_Implementation(TargetStorageActor->GetActorLocation());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("No hay más almacenes disponibles. Idle."));
                CurrentWorkerState = EWorkerState::Idle;
                bMoving = false;
            }
            break;
        }
    
        // 2. Verificación de llegada (Rango + Parada de movimiento)
        // Usamos el rango directamente para evitar que el trabajador se quede "bailando" cerca del almacén
        if (IsInRangeOf(TargetStorageActor, DeliveryRange))
        {
            // Detenemos la IA para que no intente seguir caminando dentro del edificio
            AAIController* AIC = Cast<AAIController>(GetController());
            if (AIC) AIC->StopMovement();
        
            bMoving = false;
            CurrentWorkerState = EWorkerState::Delivering;
        
            UE_LOG(LogTemp, Log, TEXT("Almacén alcanzado. Iniciando CompleteDelivery."));
            CompleteDelivery(); 
        }
        break;

        case EWorkerState::Delivering:
            // Delivery is instant, state changes after CompleteDelivery
            break;
    }
}

AActor* ABase_Pawn::GetBestResource()
{
    if (AvailableResources.Num() == 0)
    {
        return nullptr;
    }
    
    // Filtrar recursos válidos y no depletos
    TArray<AActor*> ValidResources;
    for (AActor* Resource : AvailableResources)
    {
        if (!Resource || !IsValid(Resource))
        {
            continue;
        }
        
        if (!Resource->Implements<UResourceCollectable>())
        {
            continue;
        }
        
        // Verificar si tiene recursos (extract 0 para testear)
        FResourcePair testExtract = IResourceCollectable::Execute_ExtractResource(Resource, 0.0f);
        if (testExtract.rType != EResourceType::None)
        {
            ValidResources.Add(Resource);
        }
    }
    
    if (ValidResources.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid resources with remaining materials"));
        return nullptr;
    }
    
    // Encontrar el más cercano
    AActor* BestResource = nullptr;
    float ClosestDistance = FLT_MAX;
    
    for (AActor* Resource : ValidResources)
    {
        float Distance = FVector::Dist(GetActorLocation(), Resource->GetActorLocation());
        if (Distance < ClosestDistance)
        {
            ClosestDistance = Distance;
            BestResource = Resource;
        }
    }
    
    return BestResource;
}

void ABase_Pawn::CacheAllResources()
{
    TArray<AActor*> FoundResources;
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UResourceCollectable::StaticClass(), FoundResources);
    
    AvailableResources.Empty();
    
    for (AActor* resourceActor : FoundResources)
    {
        if (IsValid(resourceActor))
        {
            AvailableResources.Add(resourceActor);
            UE_LOG(LogTemp, Log, TEXT("Cached resource: %s"), *resourceActor->GetName());
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Worker %s cached %d resources"), *GetName(), AvailableResources.Num());
}

void ABase_Pawn::CacheAllStorages()
{
    TArray<AActor*> FoundStorages;
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UIStorageInterface::StaticClass(), FoundStorages);
    
    AvailableStorages.Empty();
    for (AActor* StorageActor : FoundStorages)
    {
        if (IsValid(StorageActor))
        {
            AvailableStorages.Add(StorageActor);
        }
    }
    UE_LOG(LogTemp, Log, TEXT("Worker %s cached %d storages"), *GetName(), AvailableStorages.Num());
}

AActor* ABase_Pawn::FindNearestStorage(FVector FromLocation)
{
    // Si la lista está vacía o hay dudas, recacheamos rápidamente
    if (AvailableStorages.Num() == 0) CacheAllStorages();

    AActor* BestStorage = nullptr;
    float ClosestDistance = FLT_MAX;

    for (AActor* Storage : AvailableStorages)
    {
        if (IsValid(Storage))
        {
            float Distance = FVector::Dist(FromLocation, Storage->GetActorLocation());
            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                BestStorage = Storage;
            }
        }
    }
    return BestStorage;
}

void ABase_Pawn::StartGathering()
{
    if (!TargetResourceActor)
    {
        return;
    }
    CurrentAmount = 0.0f;
    GetWorld()->GetTimerManager().ClearTimer(GatheringTimerHandle);
    // Face the resource
    FVector directionToResource = (TargetResourceActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    FRotator lookAtRotation = directionToResource.Rotation();
    lookAtRotation.Pitch = 0.f;
    lookAtRotation.Roll = 0.f;
    SetActorRotation(lookAtRotation);

    // Start gathering timer (every second)
    GetWorld()->GetTimerManager().SetTimer(
        GatheringTimerHandle,
        this,
        &ABase_Pawn::ProcessGathering,
        1.0f,
        true
    );

    UE_LOG(LogTemp, Log, TEXT("Started gathering from %s"), *TargetResourceActor->GetName());
}

void ABase_Pawn::StopGathering()
{
    GetWorld()->GetTimerManager().ClearTimer(GatheringTimerHandle);
    UE_LOG(LogTemp, Log, TEXT("Stopped gathering"));
}

void ABase_Pawn::ProcessGathering()
{
    UE_LOG(LogTemp, Warning, TEXT("%f"), CurrentAmount);
    // Verificación doble
    if (!IsValid(TargetResourceActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("TargetResourceActor is no longer valid!"));
        StopGathering();
        CurrentWorkerState = EWorkerState::Idle;
        TargetResourceActor = nullptr;
        return;
    }

    if (!TargetResourceActor->GetClass()->ImplementsInterface(UResourceCollectable::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("TargetResourceActor no longer implements IResourceCollectable!"));
        StopGathering();
        CurrentWorkerState = EWorkerState::Idle;
        TargetResourceActor = nullptr;
        return;
    }

    // Calculate how much we can gather
    float remainingCapacity = MaxCapacity - CurrentAmount;
    float amountToGather = FMath::Min(GatheringRate, remainingCapacity);

    if (amountToGather <= 0.0f)
    {
        StopGathering();
        
        // Inventory full, move to storage
        if (TargetStorageActor && IsValid(TargetStorageActor))
        {
            CurrentWorkerState = EWorkerState::MovingToStorage;
            MoveToLocation_Implementation(TargetStorageActor->GetActorLocation());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Inventory full but no storage assigned!"));
            CurrentWorkerState = EWorkerState::Idle;
        }
        return;
    }

    // Extraer recurso solo si el actor es válido
    if (IsValid(TargetResourceActor))
    {
        FResourcePair extractedResource = IResourceCollectable::Execute_ExtractResource(
            TargetResourceActor, 
            amountToGather
        );

        // Add to inventory
        CurrentAmount += extractedResource.amount;
        CurrentResourceType = extractedResource.rType;

        UE_LOG(LogTemp, Log, TEXT("Gathered %.2f of %s. Inventory: %.2f/%.2f"), 
               extractedResource.amount,
               *UEnum::GetValueAsString(CurrentResourceType),
               CurrentAmount, 
               MaxCapacity);

        // Check if resource is depleted
        if (extractedResource.amount <= 0.0f)
        {
            UE_LOG(LogTemp, Warning, TEXT("Resource depleted"));
            StopGathering();
            TargetResourceActor = nullptr;
            
            // Move to storage with what we have
            if (CurrentAmount > 0.0f && TargetStorageActor && IsValid(TargetStorageActor))
            {
                CurrentWorkerState = EWorkerState::MovingToStorage;
                MoveToLocation_Implementation(TargetStorageActor->GetActorLocation());
            }
            else
            {
                CurrentWorkerState = EWorkerState::Idle;
            }
        }
    }
    else
    {
        StopGathering();
        CurrentWorkerState = EWorkerState::Idle;
        TargetResourceActor = nullptr;
    }
}

void ABase_Pawn::CompleteDelivery()
{
    if (IsValid(TargetStorageActor))
    {
        // Ejecutar la interfaz para depositar
        IIStorageInterface::Execute_DepositResource(TargetStorageActor, CurrentResourceType, CurrentAmount);
        
        // Resetear inventario local
        CurrentAmount = 0.0f;
        CurrentResourceType = EResourceType::None;

        // BUCLE AUTOMÁTICO: Regresar al recurso original
        if (IsValid(TargetResourceActor))
        {
            CurrentWorkerState = EWorkerState::MovingToResource;
            MoveToLocation_Implementation(TargetResourceActor->GetActorLocation());
            UE_LOG(LogTemp, Warning, TEXT("Regresando automáticamente al recurso: %s"), *TargetResourceActor->GetName());
        }
        else
        {
            // Si el recurso se agotó, buscamos el siguiente mejor (el más cercano)
            AActor* nextResource = GetBestResource();
            if (nextResource)
            {
                GatherResource_Implementation(nextResource);
            }
            else
            {
                CurrentWorkerState = EWorkerState::Idle;
            }
        }
    }
}

bool ABase_Pawn::IsInRangeOf(const AActor* TargetActor, float Range) const
{
    if (!TargetActor)
    {
        return false;
    }

    float distance = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
    return distance <= Range;
}

void ABase_Pawn::SetTargetStorage(AActor* StorageActor)
{
    if (StorageActor && StorageActor->Implements<UIStorageInterface>())
    {
        TargetStorageActor = StorageActor;
        UE_LOG(LogTemp, Log, TEXT("Target storage set to %s"), *StorageActor->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid storage actor"));
    }
}

void ABase_Pawn::CancelCurrentTask()
{
    // Stop gathering if currently gathering
    if (CurrentWorkerState == EWorkerState::Gathering)
    {
        StopGathering();
    }

    // Stop movement
    bMoving = false;
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        AIController->StopMovement();
    }

    // Clear targets and go idle
    TargetResourceActor = nullptr;
    CurrentWorkerState = EWorkerState::Idle;
    
    UE_LOG(LogTemp, Log, TEXT("Worker %s: Task cancelled, going Idle"), *GetName());
}