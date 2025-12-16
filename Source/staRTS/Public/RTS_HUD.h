// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RTS_HUD.generated.h"

UCLASS()
class STARTS_API ARTS_HUD : public AHUD
{
	GENERATED_BODY()

private:
	bool drawSelectionBox = false;
	bool bSelectActors = false;
	FVector2D selectionBoxStart;
	FVector2D selectionBoxSize;


	TArray<AActor*> selectedActors;

	void SelectActorsInBox();

protected:
	virtual void DrawHUD() override;

public:
	void ShowSelectBox(const FVector2D SelectionBoxStart, const FVector2D SelectionBoxSize);
	void HideSelectionBox();

	TArray<AActor*> GetSelectedActors() const { return selectedActors; }
};
