// ALL FREE

#pragma once

#include "CoreMinimal.h"
#include "Items/Pickups/WarriorPickupBase.h"
#include "WarriorStoneBase.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorStoneBase : public AWarriorPickupBase
{
	GENERATED_BODY()
	
protected:
	virtual void OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
