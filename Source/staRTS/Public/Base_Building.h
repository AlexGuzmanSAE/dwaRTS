// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ISelectable.h"
#include "Base_Building.generated.h"

class UBoxComponent;

UCLASS()
class STARTS_API ABase_Building : public AActor, public IISelectable
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Building, meta = (AllowPrivateAccess = "true"));
	TObjectPtr<UStaticMeshComponent> meshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Building, meta = (AllowPrivateAccess = "true"));
	TObjectPtr<UStaticMeshComponent> selectedIndicator;

public:	
	// Sets default values for this actor's properties
	ABase_Building();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SelectActor_Implementation(const bool isSelected);
};
