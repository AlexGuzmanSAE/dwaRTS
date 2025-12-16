// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTS_PlayerController.generated.h"


class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class ABase_Pawn;
class ARTS_HUD;

UCLASS()
class STARTS_API ARTS_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public :

	ARTS_PlayerController();

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"));
	UInputMappingContext* DefaultMappingContext;

	bool bIsIssuingCommand = false;
	FTimerHandle CommandCooldownTimer;

protected:

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;
	void Select(const FInputActionValue& value);
	void CommandSelectedActors(const FInputActionValue& value);
	void SelectStart(const FInputActionValue& value);
	void SelectOnGoing(const FInputActionValue& value);
	void SelectEnd(const FInputActionValue& value);
	void SelectMultipleActors();

	UPROPERTY();
	TObjectPtr<AActor> hitActor;


	UPROPERTY();
	TObjectPtr<ARTS_HUD> rtsHUD;
private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"));
	TObjectPtr<UInputAction> selectAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"));
	TObjectPtr<UInputAction> commandAction;

	FVector2D selectionStartPoint;
	FVector2D selectionSize;
	TArray<AActor*> currentlySelectedActors;
};
