#include "Base_Resource.h"

ABase_Resource::ABase_Resource()
{
    PrimaryActorTick.bCanEverTick = false;
    
    // Initialize with default values
    resourceType = EResourceType::Gold;
    currentAmount = 1000.0f;
    maxAmount = 1000.0f;
}

void ABase_Resource::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize current amount to max
    currentAmount = maxAmount;
    
    // Create the resource pair
    structTypePair = FResourcePair(resourceType, currentAmount);
}

void ABase_Resource::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

FResourcePair ABase_Resource::ExtractResource_Implementation(float AmountToExtract)
{
    FResourcePair result;
    
    // Check if resource is depleted
    if (currentAmount <= 0.0f)
    {
        result.rType = EResourceType::None;
        result.amount = 0.0f;
        UE_LOG(LogTemp, Warning, TEXT("Resource %s is completely depleted!"), *GetName());
        return result;
    }

    // Calculate actual amount to extract (can't extract more than available)
    float extractedAmount = FMath::Min(AmountToExtract, currentAmount);
    currentAmount -= extractedAmount;

    // Update the struct
    result.rType = resourceType;
    result.amount = extractedAmount;

    UE_LOG(LogTemp, Log, TEXT("Extracted %.2f %s from %s. Remaining: %.2f/%.2f"), 
           extractedAmount,
           *UEnum::GetValueAsString(resourceType),
           *GetName(),
           currentAmount,
           maxAmount);

    // Optional: Visual feedback or destruction when depleted
    if (currentAmount <= 0.0f)
    {
        OnResourceDepleted();
    }

    return result;
}

void ABase_Resource::OnResourceDepleted()
{
    UE_LOG(LogTemp, Warning, TEXT("Resource %s has been fully depleted!"), *GetName());
    
    // Optional: Hide the resource or mark it as depleted
    // SetActorHiddenInGame(true);
    // SetActorEnableCollision(false);
    
    // Optional: Destroy after a delay
    // FTimerHandle DestroyTimer;
    // GetWorld()->GetTimerManager().SetTimer(DestroyTimer, [this]()
    // {
    //     Destroy();
    // }, 2.0f, false);
}

float ABase_Resource::GetRemainingAmount() const
{
    return currentAmount;
}

float ABase_Resource::GetMaxAmount() const
{
    return maxAmount;
}

bool ABase_Resource::IsDepleted() const
{
    return currentAmount <= 0.0f;
}

float ABase_Resource::GetResourcePercentage() const
{
    if (maxAmount <= 0.0f)
        return 0.0f;
    
    return (currentAmount / maxAmount) * 100.0f;
}