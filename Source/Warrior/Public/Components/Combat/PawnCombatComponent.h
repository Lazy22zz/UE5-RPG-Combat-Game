// ALL FREE

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtenComponentBase.h"
#include "NativeGameplayTags.h"
#include "PawnCombatComponent.generated.h"

class AWarriorWeaponBase;
/**
 * 
 */
UCLASS()
class WARRIOR_API UPawnCombatComponent : public UPawnExtenComponentBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = Category = "Warrior|Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon = false); // fasle for hero, true for enemy

	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	UPROPERTY(BlueprintReadWrite, Category = "Warrior|Combat")
	FGameplayTag CurrentEquippedWeaponTag;

	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon() const;

private:
	TMap< FGameplayTag, AWarriorWeaponBase* >CharacterCarriedWeaponMap;
	
};
