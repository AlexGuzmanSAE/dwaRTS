// Fill out your copyright notice in the Description page of Project Settings.


#include "RTS_HUD.h"

void ARTS_HUD::DrawHUD()
{
	Super::DrawHUD();

	if (drawSelectionBox)
	{
		DrawRect(FLinearColor(0.f, 1.0f, 0.25f, 0.25f), selectionBoxStart.X, selectionBoxStart.Y, selectionBoxSize.X, selectionBoxSize.Y); 
	}

	if (bSelectActors)
	{
		SelectActorsInBox();
	}
}

void ARTS_HUD::ShowSelectBox(const FVector2D SelectionBoxStart, const FVector2D SelectionBoxSize)
{
	selectionBoxStart = SelectionBoxStart;
	selectionBoxSize = SelectionBoxSize;
	drawSelectionBox = true;
}

void ARTS_HUD::HideSelectionBox()
{
	drawSelectionBox = false;
	bSelectActors = true;
}

void ARTS_HUD::SelectActorsInBox()
{
	selectedActors.Empty();
	FVector2D firstPoint = selectionBoxStart;
	FVector2D secondPoint = selectionBoxStart + selectionBoxSize;
	GetActorsInSelectionRectangle<AActor>(firstPoint, secondPoint, selectedActors, false, true);
	bSelectActors = false;

	

}