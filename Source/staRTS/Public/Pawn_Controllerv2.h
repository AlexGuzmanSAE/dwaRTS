// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawn_Controller2.h"
#include "Pawn_Controllerv2.generated.h"

/**
 * 
 */
UCLASS()
class STARTS_API APawn_Controllerv2 : public APawn_Controller2
{
	GENERATED_BODY()

public:
	

public:
	// Sets default values for this pawn's properties
	APawn_Controllerv2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;



};
