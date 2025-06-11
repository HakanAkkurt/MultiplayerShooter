// Hakan Games. All Rights Reserved.


#include "HealthPickup.h"
#include "MultiPlayerShooter/Character/PlayerCharacter.h"
#include "MultiPlayerShooter/Components/BuffComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

AHealthPickup::AHealthPickup()
{
	bReplicates = true;
	PickupEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PickupEffectComponent"));
	PickupEffectComponent->SetupAttachment(RootComponent);
}

void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter) {

		UBuffComponent* Buff = PlayerCharacter->GetBuff();
		if (Buff) {

			Buff->Heal(HealAmount, HealingTime);
		}

	}

	Destroy();
}

void AHealthPickup::Destroyed()
{
	if (PickupEffect) {

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			PickupEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	Super::Destroyed();
}