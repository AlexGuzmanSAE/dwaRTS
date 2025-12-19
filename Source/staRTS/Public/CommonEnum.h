#pragma once

#include "CoreMinimal.h"
#include "CommonEnum.generated.h"  // <-- CORREGIDO: "CommonEnums.generated.h"

UENUM(BlueprintType)
enum class EResourceType : uint8
{
    None UMETA(DisplayName = "None"),
    Gold UMETA(DisplayName = "Gold"),
    Wood UMETA(DisplayName = "Wood"),
    Stone UMETA(DisplayName = "Stone"),
    Food UMETA(DisplayName = "Food")
};

USTRUCT(BlueprintType)
struct FResourcePair
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
    EResourceType rType = EResourceType::Gold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
    float amount = 0.0f;

    FResourcePair() : rType(EResourceType::Gold), amount(0.0f) {}

    FResourcePair(EResourceType InType, float InAmount)
        : rType(InType), amount(InAmount) {}
};

UENUM(BlueprintType)
enum class EWorkerState : uint8
{
    Idle UMETA(DisplayName = "Idle"),
    MovingToResource UMETA(DisplayName = "Moving To Resource"),
    Gathering UMETA(DisplayName = "Gathering"),
    MovingToStorage UMETA(DisplayName = "Moving To Storage"),
    Delivering UMETA(DisplayName = "Delivering")
};