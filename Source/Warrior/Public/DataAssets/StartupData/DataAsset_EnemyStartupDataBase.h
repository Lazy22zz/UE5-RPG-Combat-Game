// ALL FREE

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/StartupData/DataAsset_StartupDataBase.h"
#include "DataAsset_EnemyStartupDataBase.generated.h"

class UWarriorEnemyGameplayAbility;

/**
 * 
 */
UCLASS()
class WARRIOR_API UDataAsset_EnemyStartupDataBase : public UDataAsset_StartupDataBase
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray< TSubclassOf < UWarriorEnemyGameplayAbility > > EnemyCombatAbilities;
};
