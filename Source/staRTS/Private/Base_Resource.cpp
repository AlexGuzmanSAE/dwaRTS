#include "Base_Resource.h"

ABase_Resource::ABase_Resource()
{
    PrimaryActorTick.bCanEverTick = false;
    
    resourceType = EResourceType::Gold;
    currentAmount = 1000.0f;
    maxAmount = 1000.0f;
}

void ABase_Resource::BeginPlay()
{
    Super::BeginPlay();
    
    currentAmount = maxAmount;
    
    structTypePair = FResourcePair(resourceType, currentAmount);
}

void ABase_Resource::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

FResourcePair ABase_Resource::ExtractResource_Implementation(float AmountToExtract)
{
    FResourcePair result;
    
    if (currentAmount <= 0.0f)
    {
        result.rType = EResourceType::None;
        result.amount = 0.0f;
        UE_LOG(LogTemp, Warning, TEXT("Resource %s is completely depleted!"), *GetName());
        return result;
    }
    
    float extractedAmount = FMath::Min(AmountToExtract, currentAmount);
    currentAmount -= extractedAmount;

    result.rType = resourceType;
    result.amount = extractedAmount;

    UE_LOG(LogTemp, Log, TEXT("Extracted %.2f %s from %s. Remaining: %.2f/%.2f"), 
           extractedAmount,
           *UEnum::GetValueAsString(resourceType),
           *GetName(),
           currentAmount,
           maxAmount);

    if (currentAmount <= 0.0f)
    {
        OnResourceDepleted();
    }

    return result;
}

void ABase_Resource::OnResourceDepleted()
{
    UE_LOG(LogTemp, Warning, TEXT("Resource %s has been fully depleted!"), *GetName());
    
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