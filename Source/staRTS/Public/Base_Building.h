// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ISelectable.h"
#include "IStorageInterface.h"
#include "Base_Building.generated.h"

class UBoxComponent;
struct FInputActionVale;
struct FInputActionValue;
class UInputAction;

UCLASS()
class STARTS_API ABase_Building : public AActor, public IISelectable, public IIStorageInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Building, meta = (AllowPrivateAccess = "true"));
	TObjectPtr<UStaticMeshComponent> meshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Building, meta = (AllowPrivateAccess = "true"));
	TObjectPtr<UStaticMeshComponent> selectedIndicator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"));
	TObjectPtr<UInputAction> placeBuildingAction;

	UPROPERTY()
	FTimerHandle validationTimerHandle;
	
	UPROPERTY()
	FName requiredTag = "CanPlaceBuilding";
	
	UPROPERTY()
	bool canPlaceBuilding = false;

	UPROPERTY()
	FVector buildAreaExtents = FVector(500.f, 500.f, 500.f);

public:	
	ABase_Building();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void SelectActor_Implementation(const bool isSelected);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void EnablePlacingMode();
	void CheckLocationValid();
	void PlaceBuilding(const FInputActionValue& value);
	void CancelBuildingPlacement(const FInputActionValue& value);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Building")
	void ToggleBuildingValidity(bool isValid);
};
