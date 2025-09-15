// Hakan Games. All Rights Reserved.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerShooter/Weapon/Weapon.h"
#include "MultiplayerShooter/Components/CombatComponent.h"
#include "MultiplayerShooter/Components/BuffComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCharacterAnimInstance.h"
#include "MultiplayerShooter/MultiplayerShooter.h"
#include "MultiplayerShooter/PlayerController/MS_PlayerController.h"
#include "MultiplayerShooter/GameMode/MS_GameMode.h"
#include "TimerManager.h"
#include "MultiplayerShooter/PlayerState/MS_PlayerState.h"
#include "MultiplayerShooter/Weapon/WeaponTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "MultiplayerShooter/Components/LagCompensationComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "MultiplayerShooter/GameState/MS_GameState.h"


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

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 850.f, 0.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attached Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Hit boxes for server-side rewind

	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), FName("head"));
	head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("head"), head);

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);

	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);

	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}


void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APlayerCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(APlayerCharacter, Health);
	DOREPLIFETIME(APlayerCharacter, Shield);
	DOREPLIFETIME(APlayerCharacter, bDisableGameplay);
}

void APlayerCharacter::MulticastGainedTheLead_Implementation()
{
	if (CrownSystem == nullptr) return;
	if (CrownComponent == nullptr)
	{
		CrownComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			CrownSystem,
			GetCapsuleComponent(),
			FName(),
			GetActorLocation() + FVector(0.f, 0.f, 110.f),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
	if (CrownComponent)
	{
		CrownComponent->Activate();
	}
}

void APlayerCharacter::MulticastLostTheLead_Implementation()
{
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnDefaultWeapon();
	UpdateHUDAmmo();
	UpdateHUDHealth();
	UpdateHUDShield();
	UpdateHUDGrenade();

	if (HasAuthority()) {
		OnTakeAnyDamage.AddDynamic(this, &APlayerCharacter::ReceiveDamage);
	}

	if (AttachedGrenade)
	{
		AttachedGrenade->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("GrenadeSocket"));
		AttachedGrenade->SetVisibility(false);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateInPlace(DeltaTime);

	HideCamereIfCharacterClose();

	PollInit();
}

void APlayerCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplay) {

		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;

		return;
	}

	// .... 
	AimOffset(DeltaTime);
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat && Buff) {

		Combat->Character = this;
		Buff->Character = this;

		Buff->SetInitialSpeeds(GetCharacterMovement()->MaxWalkSpeed,
			Combat->AimWalkSpeed,
			GetCharacterMovement()->MaxWalkSpeedCrouched);

		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
	if (LagCompensation) {

		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<AMS_PlayerController>(Controller);
		}
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

		case EWeaponType::EWT_RocketLauncher:

			SectionName = FName("RocketLauncher");
			break;

		case EWeaponType::EWT_Pistol:

			SectionName = FName("Pistol");
			break;

		case EWeaponType::EWT_SubmachineGun:

			SectionName = FName("Pistol");
			break;

		case EWeaponType::EWT_Shotgun:

			SectionName = FName("Shotgun");
			break;

		case EWeaponType::EWT_SniperRifle:

			SectionName = FName("Rifle");
			break;

		case EWeaponType::EWT_GrenadeLauncher:

			SectionName = FName("GrenadeLauncher");
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

void APlayerCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && ThrowGrenadeMontage) {

		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

void APlayerCharacter::PlaySwapMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && SwapMontage) {

		AnimInstance->Montage_Play(SwapMontage);
	}
}

void APlayerCharacter::Eliminate(bool bPlayerLeftGame)
{
	if (Combat) {

		if (Combat->EquippedWeapon) {

			DropOrDestroyWeapon(Combat->EquippedWeapon);
		}
		if (Combat->SecondaryWeapon) {

			DropOrDestroyWeapon(Combat->SecondaryWeapon);
		}
	}

	MulticastEliminate(bPlayerLeftGame);
}

void APlayerCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	if (Weapon == nullptr) return;

	if (Weapon->bDestroyWeapon) {
		Weapon->Destroy();
	}
	else {
		Weapon->Dropped();
	}
}

void APlayerCharacter::Destroyed()
{
	Super::Destroyed();

	AMS_GameMode* MS_GameMode = Cast<AMS_GameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = MS_GameMode && MS_GameMode->GetMatchState() != MatchState::InProgress;

	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress) {

		Combat->EquippedWeapon->Destroy();
	}
}

void APlayerCharacter::MulticastEliminate_Implementation(bool bPlayerLeftGame)
{
	bLeftGame = bPlayerLeftGame;
	if (MS_PlayerController) {
	
		MS_PlayerController->SetHUDWeaponAmmo(0);
	}

	bEliminated = true;
	PlayEliminateMontage();

	// Disable character movement
	bDisableGameplay = true;
	GetCharacterMovement()->DisableMovement();

	if (Combat) {

		Combat->FireButtonPressed(false);
	}

	// Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (IsLocallyControlled() && Combat && Combat->bAiming && Combat->EquippedWeapon
		&& Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle) {

		ShowSniperScopeWidget(false);
	}

	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}

	GetWorldTimerManager().SetTimer(EliminateTimer, this, &APlayerCharacter::EliminateTimerFinished,
		EliminateDelay);
}


void APlayerCharacter::EliminateTimerFinished()
{
	AMS_GameMode* MS_GameMode = GetWorld()->GetAuthGameMode<AMS_GameMode>();
	if (MS_GameMode && !bLeftGame) {

		MS_GameMode->RequestRespawn(this, Controller);
	}

	if (bLeftGame && IsLocallyControlled()) {

		OnLeftGame.Broadcast();
	}
}

void APlayerCharacter::ServerLeaveGame_Implementation()
{
	AMS_GameMode* MS_GameMode = GetWorld()->GetAuthGameMode<AMS_GameMode>();
	MS_PlayerState = MS_PlayerState == nullptr ? GetPlayerState<AMS_PlayerState>() : MS_PlayerState;

	if (MS_GameMode && MS_PlayerState) {

		MS_GameMode->PlayerLeftGame(MS_PlayerState);
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

void APlayerCharacter::GrenadeButtonPressed()
{
	if (Combat) {
		Combat->ThrowGrenade();
	}
}

void APlayerCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (bEliminated) return;

	float DamageToHealth = Damage;
	if (Shield > 0.f) {

		if (Shield >= Damage) {

			Shield = FMath::Clamp(Shield - Damage, 0.f, MaxShield);
			DamageToHealth = 0.f;
		}
		else {

			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f, Damage);
			Shield = 0.f;
		}
	}

	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);
	UpdateHUDHealth();
	UpdateHUDShield();
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
	
	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &APlayerCharacter::GrenadeButtonPressed);

}

void APlayerCharacter::MoveForward(float Value)
{
	if (bDisableGameplay) return;

	if (Controller != nullptr && Value != 0.f) {

		const FRotator YawRotaion(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotaion).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);

	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (bDisableGameplay) return;

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
	if (bDisableGameplay) return;

	if (Combat) {

		if (Combat->CombatState == ECombatState::ECS_Unoccupied) ServerEquipButtonPressed();

		if (Combat->ShouldSwapWeapons() && !HasAuthority() && Combat->CombatState == ECombatState::ECS_Unoccupied
			&& OverlappingWeapon == nullptr) {

			PlaySwapMontage();
			Combat->CombatState = ECombatState::ECS_SwappingWeapons;
			bFinishedSwapping = false;
		}
	}
}

void APlayerCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat) {

		if (OverlappingWeapon) {

			Combat->EquipWeapon(OverlappingWeapon);
		}
		else if (Combat->ShouldSwapWeapons()) {
			Combat->SwapWeapons();
		}

	}
}

void APlayerCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay) return;

	if (bIsCrouched) {

		UnCrouch();
	}
	else {
		Crouch();
	}

}

void APlayerCharacter::ReloadButtonPressed()
{
	if (bDisableGameplay) return;

	if (Combat) {
		Combat->Reload();
	}
}

void APlayerCharacter::AimButtonPressed()
{
	if (bDisableGameplay) return;

	if (Combat) {

		Combat->SetAiming(true);
	}
}

void APlayerCharacter::AimButtonReleased()
{
	if (bDisableGameplay) return;

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
	if (bDisableGameplay) return;

	if (bIsCrouched) {

		UnCrouch();
	}
	else {

		Super::Jump();
	}
}

void APlayerCharacter::FireButtonPressed()
{
	if (bDisableGameplay) return;

	if (Combat) {

		Combat->FireButtonPressed(true);
	}
}

void APlayerCharacter::FireButtonReleased()
{
	if (bDisableGameplay) return;

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

void APlayerCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (Health < LastHealth) {
		PlayHitReactMontage();
	}
}

void APlayerCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
	if (Shield < LastShield) {
		PlayHitReactMontage();
	}
}

void APlayerCharacter::UpdateHUDHealth()
{
	MS_PlayerController = MS_PlayerController == nullptr ? Cast<AMS_PlayerController>(Controller) : MS_PlayerController;
	if (MS_PlayerController) {

		MS_PlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void APlayerCharacter::UpdateHUDShield()
{
	MS_PlayerController = MS_PlayerController == nullptr ? Cast<AMS_PlayerController>(Controller) : MS_PlayerController;
	if (MS_PlayerController) {

		MS_PlayerController->SetHUDShield(Shield, MaxShield);
	}
}

void APlayerCharacter::UpdateHUDAmmo()
{
	MS_PlayerController = MS_PlayerController == nullptr ? Cast<AMS_PlayerController>(Controller) : MS_PlayerController;
	if (MS_PlayerController && Combat && Combat->EquippedWeapon) {

		MS_PlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		MS_PlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
	}
}

void APlayerCharacter::SpawnDefaultWeapon()
{
	AMS_GameMode* MS_GameMode = Cast<AMS_GameMode>(UGameplayStatics::GetGameMode(this));
	UWorld* World = GetWorld();

	if (MS_GameMode && World && !bEliminated && DefaultWeaponClass) {
		
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;

		if (Combat) {

			Combat->EquipWeapon(StartingWeapon);
		}
	}
}

void APlayerCharacter::UpdateHUDGrenade()
{
	MS_PlayerController = MS_PlayerController == nullptr ? Cast<AMS_PlayerController>(Controller) : MS_PlayerController;
	if (MS_PlayerController) {

		MS_PlayerController->SetHUDGrenades(Combat->GetGrenades());
	}
}

void APlayerCharacter::PollInit()
{
	if (MS_PlayerState == nullptr) {

		MS_PlayerState = GetPlayerState<AMS_PlayerState>();
		if (MS_PlayerState) {

			MS_PlayerState->AddToScore(0.f);
			MS_PlayerState->AddToDefeats(0);

			AMS_GameState* MS_GameState = Cast<AMS_GameState>(UGameplayStatics::GetGameState(this));
		
			if (MS_GameState && MS_GameState->TopScoringPlayers.Contains(MS_PlayerState)) {

				MulticastGainedTheLead();
			}
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

bool APlayerCharacter::IsLocallyReloading()
{
	if (Combat == nullptr) return false;

	return Combat->bLocallyReloading;
}
