// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ISelectable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UISelectable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STARTS_API IISelectable
{
	GENERATED_BODY()

	
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selectable Interface")

	void SelectActor(const bool isSelected);
};
