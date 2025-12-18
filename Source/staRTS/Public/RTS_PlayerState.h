// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RTS_PlayerState.generated.h"


class UResourcesAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class STARTS_API ARTS_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ARTS_PlayerState();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources")
	const class UResourcesAttributeSet* resourceAttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources")
	UAbilitySystemComponent* abilityComponent;	
	

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void OnResourceCountChanged();
};
