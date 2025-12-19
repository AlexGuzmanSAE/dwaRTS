#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ISelectable.h"
#include "Navegable_Interface.h"
#include "IWorkerInterface.h"
#include "CommonEnum.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "Base_Pawn.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UFloatingPawnMovement;
class AAIController;

UENUM(BlueprintType)
enum class EWorkerState : uint8
{
    Idle UMETA(DisplayName = "Idle"),
    MovingToResource UMETA(DisplayName = "Moving To Resource"),
    Gathering UMETA(DisplayName = "Gathering"),
    MovingToStorage UMETA(DisplayName = "Moving To Storage"),
    Delivering UMETA(DisplayName = "Delivering")
};

UCLASS()
class STARTS_API ABase_Pawn : public APawn, public IISelectable, public INavegable_Interface, public IIWorkerInterface
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCapsuleComponent> capsule;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> skeletalMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UFloatingPawnMovement> floatPawnMovement;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> selectedIndicator;

public:
    ABase_Pawn();

protected:
    virtual void BeginPlay() override;

    void Move();
    
    FVector moveTargetLocation = FVector::ZeroVector;
    bool bMoving = false;
    float minDistanceToTarget = 150.f;
    float interpSpeed = 5.f;

    // Worker State Management
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RTS | Worker")
    EWorkerState CurrentWorkerState = EWorkerState::Idle;

    // Inventory
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RTS | Inventory")
    EResourceType CurrentResourceType = EResourceType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTS | Inventory")
    float CurrentAmount = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTS | Inventory")
    float MaxCapacity = 500.0f;

    // Resource Gathering Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTS | Gathering")
    float GatheringRate = 50.0f; // Amount per second

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTS | Gathering")
    float GatheringRange = 250.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTS | Gathering")
    float DeliveryRange = 200.0f;
public:
    UFUNCTION()
    AActor* GetBestResource();
    UFUNCTION()
    void CacheAllResources();
    void CacheAllStorages();
    UPROPERTY()
    TArray<AActor*> AvailableStorages;
    UPROPERTY()
    TArray<AActor*> AvailableResources;
    // Target References
    UPROPERTY()
    AActor* TargetResourceActor;

    UPROPERTY()
    AActor* TargetStorageActor;

    AActor* FindNearestStorage(FVector FromLocation);
    // Timers
    FTimerHandle GatheringTimerHandle;
    
    // Helper Functions
    void StartGathering();
    void StopGathering();
    void ProcessGathering();
    void CompleteDelivery();
    bool IsInRangeOf(const AActor* TargetActor, float Range) const;
    void UpdateWorkerState();

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // IISelectable Interface
    virtual void SelectActor_Implementation(const bool isSelected) override;
    
    // INavegable_Interface
    virtual void MoveToLocation_Implementation(const FVector targetLocation) override;
    
    // IIWorkerInterface Interface
    virtual void GatherResource_Implementation(AActor* ResourceActor) override;
    virtual void DeliverResource_Implementation(AActor* StorageActor) override;
    virtual bool IsCarryingResources_Implementation() const override;

    // Public helper functions
    UFUNCTION(BlueprintCallable, Category = "RTS | Worker")
    void SetTargetStorage(AActor* StorageActor);

    UFUNCTION(BlueprintCallable, Category = "RTS | Worker")
    void CancelCurrentTask();

    UFUNCTION(BlueprintCallable, Category = "RTS | Worker")
    EWorkerState GetCurrentWorkerState() const { return CurrentWorkerState; }

    UFUNCTION(BlueprintCallable, Category = "RTS | Worker")
    float GetCurrentResourceAmount() const { return CurrentAmount; }

    // AI Move callback (not a UFUNCTION, bound via AddUObject)
    void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
};