// ALL FREE


#include "GameModes/WarriorSurvialGamemode.h"

void AWarriorSurvialGamemode::BeginPlay()
{
	Super::BeginPlay();
}

void AWarriorSurvialGamemode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWarriorSurvialGamemode::SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState InState)
{
	CurrentSurvialGamemodeState = InState;

	OnSurvialGameModeStateChanged.Broadcast(CurrentSurvialGamemodeState);
}
