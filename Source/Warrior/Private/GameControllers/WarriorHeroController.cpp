// ALL FREE


#include "GameControllers/WarriorHeroController.h"

AWarriorHeroController::AWarriorHeroController()
{
	HeroTeamID = FGenericTeamId(0);
}

FGenericTeamId AWarriorHeroController::GetGenericTeamId() const
{
	return HeroTeamID;
}
