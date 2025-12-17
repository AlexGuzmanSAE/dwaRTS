// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcesAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UResourcesAttributeSet::OnRep_Wood(const FGameplayAttributeData& OldWood)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourcesAttributeSet, Wood, OldWood);
}

void UResourcesAttributeSet::OnRep_Stone(const FGameplayAttributeData& OldStone)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourcesAttributeSet, Stone, OldStone);
}

void UResourcesAttributeSet::OnRep_Food(const FGameplayAttributeData& OldFood)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourcesAttributeSet, Food, OldFood);
}

void UResourcesAttributeSet::OnRep_Gold(const FGameplayAttributeData& OldGold)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourcesAttributeSet, Gold, OldGold);
}

void UResourcesAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourcesAttributeSet, Wood, COND_None, REPNOTIFY_Always);
}
