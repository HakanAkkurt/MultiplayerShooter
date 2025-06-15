// Hakan Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "JumpPickup.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AJumpPickup : public APickup
{
	GENERATED_BODY()
	
protected:

	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

private:

	UPROPERTY(EditAnywhere)
	float JumpZVelocityBuff = 1500.f;

	UPROPERTY(EditAnywhere)
	float JumpBuffTime = 20.f;
};
