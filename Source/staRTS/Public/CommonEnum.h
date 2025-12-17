
// CommonEnums.h
#pragma once
#include "CoreMinimal.h"
#include "CommonEnum.generated.h" // Línea crítica

UENUM(BlueprintType)
enum class EResourceType : uint8
{
    Wood  UMETA(DisplayName = "Wood"),
    Stone UMETA(DisplayName = "Stone"),
    Gold  UMETA(DisplayName = "Gold"),
    Food  UMETA(DisplayName = "Food")
};