// Hakan Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()
	
public:

	AProjectileRocket();

protected:

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;
};
