// Hakan Games. All Rights Reserved.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MultiplayerShooter/Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "WeaponTypes.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");

	if (MuzzleFlashSocket) {

		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		FVector End = Start + (HitTarget - Start) * 1.25f;

		FHitResult FireHit;
		UWorld* World = GetWorld();
		if (World) {

			World->LineTraceSingleByChannel(
				FireHit,
				Start,
				End,
				ECollisionChannel::ECC_Visibility
			);

			FVector BeamEnd = End;
			if (FireHit.bBlockingHit) {

				BeamEnd = FireHit.ImpactPoint;
				APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(FireHit.GetActor());

				if (PlayerCharacter && HasAuthority() && InstigatorController) {

					UGameplayStatics::ApplyDamage(
						PlayerCharacter,
						Damage,
						InstigatorController,
						this,
						UDamageType::StaticClass()
					);
				}

				if (ImpactParticles) {

					UGameplayStatics::SpawnEmitterAtLocation(
						World,
						ImpactParticles,
						FireHit.ImpactPoint,
						FireHit.ImpactNormal.Rotation()
					);
				}

				if (HitSound) {
					UGameplayStatics::PlaySoundAtLocation(
						this,
						HitSound,
						FireHit.ImpactPoint
					);
				}

				if (BeamParticles) {
					UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
						World,
						BeamParticles,
						SocketTransform
					);
					if (Beam) {
						Beam->SetVectorParameter(FName("Target"), BeamEnd);
					}
				}

			}

			if (MuzzleFlash) {
				UGameplayStatics::SpawnEmitterAtLocation(
					World,
					MuzzleFlash,
					SocketTransform
				);
			}

			if (FireSound) {
				UGameplayStatics::PlaySoundAtLocation(
					this,
					FireSound,
					GetActorLocation()
				);
			}
		}

	}
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::RandRange(0.f, SphereRadius);
	FVector EndLoc = SphereCenter + RandVec;
	FVector ToEndLoc = EndLoc - TraceStart;

	DrawDebugSphere(
		GetWorld(),
		SphereCenter,
		SphereRadius,
		12,
		FColor::Red,
		true
	);

	DrawDebugSphere(
		GetWorld(),
		EndLoc,
		4.f,
		12,
		FColor::Orange,
		true
	);

	DrawDebugLine(
		GetWorld(),
		TraceStart,
		FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()),
		FColor::Green,
		true
	);

	return FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
}
