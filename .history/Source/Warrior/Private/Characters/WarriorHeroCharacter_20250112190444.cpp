// ALL FREE


#include "Characters/WarriorHeroCharacter.h"
#include "WarriorDebugHelper.h"
#include "WarriorHeroCharacter.h"


void AWarriorHeroCharacter::BeginPlay()
{
    Super::BeginPlay();

    Debug::Print(TEXT("DebugHelperFunction is working."));
}
AWarriorHeroCharacter::AWarriorHeroCharacter()
{
   GetCapsuleComponent() -> InitCapsuleSize(42.f, 96.f);
    
}