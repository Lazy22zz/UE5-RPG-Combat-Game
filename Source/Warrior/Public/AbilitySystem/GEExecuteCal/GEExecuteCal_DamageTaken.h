// ALL FREE

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEExecuteCal_DamageTaken.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UGEExecuteCal_DamageTaken : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	
	UGEExecuteCal_DamageTaken();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
