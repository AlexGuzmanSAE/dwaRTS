// Fill out your copyright notice in the Description page of Project Settings.


#include "RTS_PlayerState.h"
#include "AbilitySystemComponent.h"
#include "FResourceMessage.h"
#include "ResourcesAttributeSet.h"
//#include "GameFramework/

ARTS_PlayerState::ARTS_PlayerState()
{
	abilityComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	resourceAttributeSet = CreateDefaultSubobject<UResourcesAttributeSet>(TEXT("ResourceAttributeSet"));
}

void ARTS_PlayerState::OnResourceCountChanged()
{
	FGameplayTag messageTag = FGameplayTag::RequestGameplayTag(FName("Resources.Updated"));
	FResourceMessage resourceMessage;
	
	TArray<FGameplayAttribute>& allAttributeData = resourceMessage.resourceAttributes;
	abilityComponent->GetAllAttributes(allAttributeData); 
	

}
