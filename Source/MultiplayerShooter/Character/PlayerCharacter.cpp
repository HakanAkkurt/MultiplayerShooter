// Hakan Games. All Rights Reserved.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerShooter/Weapon/Weapon.h"
#include "MultiplayerShooter/Components/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCharacterAnimInstance.h"
#include "MultiplayerShooter/MultiplayerShooter.h"
#include "MultiplayerShooter/PlayerController/MS_PlayerController.h"
#include "MultiplayerShooter/GameMode/MS_GameMode.h"
#include "TimerManager.h"
#include "MultiplayerShooter/PlayerState/MS_PlayerState.h"
#include "MultiplayerShooter/Weapon/WeaponTypes.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 850.f, 0.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}


void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APlayerCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(APlayerCharacter, Health);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateHUDHealth();

	if (HasAuthority()) {
		OnTakeAnyDamage.AddDynamic(this, &APlayerCharacter::ReceiveDamage);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);

	HideCamereIfCharacterClose();

	PollInit();
}


void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat) {

		Combat->Character = this;
	}
}

void APlayerCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && FireWeaponMontage) {

		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void APlayerCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && ReloadMontage) {

		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

		switch (Combat->EquippedWeapon->GetWeaponType()) {

		case EWeaponType::EWT_AssaultRifle:

			SectionName = FName("Rifle");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void APlayerCharacter::PlayEliminateMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && EliminateMontage) {

		AnimInstance->Montage_Play(EliminateMontage);
	}
}

void APlayerCharacter::Eliminate()
{
	if (Combat && Combat->EquippedWeapon) {
		Combat->EquippedWeapon->Dropped();
	}

	MulticastEliminate();

	GetWorldTimerManager().SetTimer(EliminateTimer, this, &APlayerCharacter::EliminateTimerFinished,
		EliminateDelay);
}

void APlayerCharacter::MulticastEliminate_Implementation()
{
	if (MS_PlayerController) {
	
		MS_PlayerController->SetHUDWeaponAmmo(0);
	}

	bEliminated = true;
	PlayEliminateMontage();

	// Disable character movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	if (MS_PlayerController) {

		DisableInput(MS_PlayerController);
	}

	// Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}


void APlayerCharacter::EliminateTimerFinished()
{
	AMS_GameMode* MS_GameMode = GetWorld()->GetAuthGameMode<AMS_GameMode>();
	if (MS_GameMode) {

		MS_GameMode->RequestRespawn(this, Controller);
	}
}

void APlayerCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HitReactMontage) {

		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void APlayerCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();
	PlayHitReactMontage();

	if (Health == 0.f) {

		AMS_GameMode* MS_GameMode = GetWorld()->GetAuthGameMode<AMS_GameMode>();
		if (MS_GameMode) {

			MS_PlayerController = MS_PlayerController == nullptr ? Cast<AMS_PlayerController>(Controller) : MS_PlayerController;
			AMS_PlayerController* AttackerController = Cast<AMS_PlayerController>(InstigatorController);
			MS_GameMode->PlayerEliminated(this, MS_PlayerController, AttackerController);
		}
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &APlayerCharacter::EquipButtonPressed);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::CrouchButtonPressed);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &APlayerCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &APlayerCharacter::AimButtonReleased);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacter::ReloadButtonPressed);
}

void APlayerCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f) {

		const FRotator YawRotaion(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotaion).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);

	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f) {

		const FRotator YawRotaion(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotaion).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);

	}
}

void APlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void APlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void APlayerCharacter::EquipButtonPressed()
{
	if (Combat) {

		if (HasAuthority()) {

			Combat->EquipWeapon(OverlappingWeapon);
		}
		else {

			ServerEquipButtonPressed();
		}

	}
}

void APlayerCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat) {

		Combat->EquipWeapon(OverlappingWeapon);

	}
}

void APlayerCharacter::CrouchButtonPressed()
{
	if (bIsCrouched) {

		UnCrouch();
	}
	else {
		Crouch();
	}

}

void APlayerCharacter::ReloadButtonPressed()
{
	if (Combat) {
		Combat->Reload();
	}
}

void APlayerCharacter::AimButtonPressed()
{
	if (Combat) {

		Combat->SetAiming(true);
	}
}

void APlayerCharacter::AimButtonReleased()
{
	if (Combat) {

		Combat->SetAiming(false);
	}
}

void APlayerCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;

	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) { // Standing still, not jumping

		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;

		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning) {

			InterpAO_Yaw = AO_Yaw;
		}

		bUseControllerRotationYaw = true;
		
		TurnInPlace(DeltaTime);
	}

	if (Speed > 0.f || bIsInAir) { // Running or jumping

		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;

	if (AO_Pitch > 90.f && !IsLocallyControlled()) {

		// Map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void APlayerCharacter::Jump()
{
	if (bIsCrouched) {

		UnCrouch();
	}
	else {

		Super::Jump();
	}
}

void APlayerCharacter::FireButtonPressed()
{
	if (Combat) {

		Combat->FireButtonPressed(true);
	}
}

void APlayerCharacter::FireButtonReleased()
{
	if (Combat) {

		Combat->FireButtonPressed(false);
	}
}


void APlayerCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f) {

		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f) {

		TurningInPlace = ETurningInPlace::ETIP_Left;
	}

	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning) {

		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;

		if (FMath::Abs(AO_Yaw) < 15.f) {

			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void APlayerCharacter::HideCamereIfCharacterClose()
{
	if (!IsLocallyControlled()) return;

	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold) {

		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh()) {

			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else {

		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh()) {

			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void APlayerCharacter::OnRep_Health()
{
	UpdateHUDHealth();
	PlayHitReactMontage();
}

void APlayerCharacter::UpdateHUDHealth()
{
	MS_PlayerController = MS_PlayerController == nullptr ? Cast<AMS_PlayerController>(Controller) : MS_PlayerController;
	if (MS_PlayerController) {

		MS_PlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void APlayerCharacter::PollInit()
{
	if (MS_PlayerState == nullptr) {

		MS_PlayerState = GetPlayerState<AMS_PlayerState>();
		if (MS_PlayerState) {

			MS_PlayerState->AddToScore(0.f);
			MS_PlayerState->AddToDefeats(0);
		}
	}
}

void APlayerCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (IsLocallyControlled()) {

		if (OverlappingWeapon) {

			OverlappingWeapon->ShowPickupWidget(false);
		}
	}

	OverlappingWeapon = Weapon;

	if (IsLocallyControlled()) {

		if (OverlappingWeapon) {

			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void APlayerCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon) {

		OverlappingWeapon->ShowPickupWidget(true);
	}

	if (LastWeapon) {

		LastWeapon->ShowPickupWidget(false);
	}
}

bool APlayerCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool APlayerCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* APlayerCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;

	return Combat->EquippedWeapon;
}

FVector APlayerCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

ECombatState APlayerCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;

	return Combat->CombatState;
}
