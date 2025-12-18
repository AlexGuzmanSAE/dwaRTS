
// CommonEnums.h
#pragma once
#include "CoreMinimal.h"
#include "CommonEnum.generated.h" 

UENUM(BlueprintType)
enum class EResourceType : uint8
{
    None = 0,
    Wood  UMETA(DisplayName = "Wood"),
    Stone UMETA(DisplayName = "Stone"),
    Gold  UMETA(DisplayName = "Gold"),
    Food  UMETA(DisplayName = "Food")
};