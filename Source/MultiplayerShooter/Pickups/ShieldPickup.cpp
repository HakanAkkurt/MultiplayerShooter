// Hakan Games. All Rights Reserved.


#include "ShieldPickup.h"
#include "MultiPlayerShooter/Character/PlayerCharacter.h"
#include "MultiPlayerShooter/Components/BuffComponent.h"

void AShieldPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter) {

		UBuffComponent* Buff = PlayerCharacter->GetBuff();
		if (Buff) {

			Buff->ReplenishShield(ShieldReplenishAmount, ShieldReplenishTime);
		}

	}

	Destroy();
}