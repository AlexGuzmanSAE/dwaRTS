

#include "Base_Resource.h"



ABase_Resource::ABase_Resource()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABase_Resource::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABase_Resource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ResourcePair ABase_Resource::ExtractResource_Implementation(float AmountToExtract)
{
	//REGRESA CUANTO DEL RECURSO TIENE QUE REGRESAR Y DE QUE TIPO
	switch (resourceType)
	{
	case EResourceType::Gold :
		structTypePair.rType = EResourceType::Gold;
		structTypePair.amount = 100;
		break;
	case EResourceType::Food :
		structTypePair.rType = EResourceType::Food;
		structTypePair.amount = 50;
		break;
	case EResourceType::Stone :
		structTypePair.rType = EResourceType::Stone;
		structTypePair.amount = 150;
		break;	
	case EResourceType::Wood :
		structTypePair.rType = EResourceType::Wood;
		structTypePair.amount = 200;
		break;
	case EResourceType::None :
		structTypePair.rType = EResourceType::None;
		structTypePair.amount = 0;
		break;
	}
	return structTypePair;
}

