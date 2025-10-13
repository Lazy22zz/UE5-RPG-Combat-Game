// ALL FREE

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "WarriorGamemode.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorGamemode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWarriorGamemode();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Settings")
	EWarriorGameplayDifficulty WarriorGameplaydifficulty;

public:
	FORCEINLINE EWarriorGameplayDifficulty GetCurrentGameDifficulty() const { return WarriorGameplaydifficulty; }
};
