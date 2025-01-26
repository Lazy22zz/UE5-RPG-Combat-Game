// ALL FREE


#include "DataAssets/StartupData/DataAsset_StartupDataBase.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"


void UDataAsset_StartupDataBase::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent *InASCToGive, int32 ApplyLevel)
{
    check(InASCToGive);

    GrantAbilities(ActivateOnGivenAbilities,InASCToGive,ApplyLevel);
	GrantAbilities(ReactiveAbilities,InASCToGive,ApplyLevel); 
}

void UDataAsset_StartupDataBase::GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>> &InAbilitiesToGive, UWarriorAbilitySystemComponent *InASCToGive, int32 ApplyLevel)
{

}