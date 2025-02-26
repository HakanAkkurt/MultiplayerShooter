// Hakan Games. All Rights Reserved.


#include "Casing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACasing::ACasing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);

	ShellEjectionImpulse = 10.f;
}

// Called when the game starts or when spawned
void ACasing::BeginPlay()
{
	Super::BeginPlay();
	
	FVector RandomShell = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(GetActorForwardVector(), 20.f);

	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);

	CasingMesh->AddImpulse(RandomShell * ShellEjectionImpulse);

	SetLifeSpan(3.f);
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound) {
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}

	CasingMesh->SetNotifyRigidBodyCollision(false);
}

