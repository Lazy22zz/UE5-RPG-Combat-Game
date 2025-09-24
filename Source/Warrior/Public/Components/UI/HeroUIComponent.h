// ALL FREE

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/UI/PawnUIComponent.h"
#include "HeroUIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedWeaponChangedDelegate, TSoftObjectPtr<UTexture2D>, SoftWeaponIcon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityIconSlotUpdateDelegate, FGameplayTag, AbilityInputTag, TSoftObjectPtr<UMaterialInterface>, SoftAbilityIconMaterial);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAbilityCooldownBeginDelegate, FGameplayTag, AbilityInputTag, float, TotalCooldownTime, float, RemainingCooldownTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoneInteractedDelegate, bool, bShouldDisplayInputKey);
/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangedDelegate OnCurrentRageChanged;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnEquippedWeaponChangedDelegate OnEquippedWeaponChanged;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnAbilityIconSlotUpdateDelegate OnAbilityIconSlotUpdateDelegate;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnAbilityCooldownBeginDelegate OnAbilityCooldownBeginDelegate;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnStoneInteractedDelegate OnStoneInteractedDelegate;
};
