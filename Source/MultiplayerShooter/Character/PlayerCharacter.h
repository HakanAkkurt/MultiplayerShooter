// Hakan Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MultiplayerShooter/Types/TurnInPlace.h"
#include "MultiplayerShooter/Interfaces/InteractWithCrosshairsInterface.h"
#include "MultiplayerShooter/Types/CombatState.h"
#include "MultiplayerShooter/Types/Team.h"
#include "PlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS()
class MULTIPLAYERSHOOTER_API APlayerCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	// Play montages

	void PlayFireMontage(bool bAiming);

	void PlayReloadMontage();

	void PlayEliminateMontage();

	void PlayThrowGrenadeMontage();

	void PlaySwapMontage();

	void Eliminate(bool bPlayerLeftGame);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEliminate(bool bPlayerLeftGame);

	virtual void Destroyed() override;

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();
	void UpdateHUDGrenade();
	void SpawnDefaultWeapon();

	UPROPERTY()
	TMap<FName, class UBoxComponent*> HitCollisionBoxes;

	bool bFinishedSwapping = false;

	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

	FOnLeftGame OnLeftGame;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastGainedTheLead();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLostTheLead();

	void SetTeamColor(ETeam Team);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	void EquipButtonPressed();

	void CrouchButtonPressed();

	void ReloadButtonPressed();

	void AimButtonPressed();

	void AimButtonReleased();

	void AimOffset(float DeltaTime);

	virtual void Jump() override;

	void FireButtonPressed();
	void FireButtonReleased();

	void PlayHitReactMontage();

	void GrenadeButtonPressed();

	void DropOrDestroyWeapon(AWeapon* Weapon);

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	// Poll for any relevant classes and initialize our HUD
	void PollInit();

	void RotateInPlace(float DeltaTime);

	// Hit boxes used for server-side rewind

	UPROPERTY(EditAnywhere)
	class UBoxComponent* head;

	UPROPERTY(EditAnywhere)
	UBoxComponent* pelvis;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_02;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_03;

	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_r;

private:

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere)
	class UBuffComponent* Buff;

	UPROPERTY(VisibleAnywhere)
	class ULagCompensationComponent* LagCompensation;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	// Animation montages

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* EliminateMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ThrowGrenadeMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* SwapMontage;

	void HideCharacterIfCameraClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	// Player health

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	// Player shield

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")
	float Shield = 0.f;

	UFUNCTION()
	void OnRep_Shield(float LastShield);

	UPROPERTY()
	class AMS_PlayerController* MS_PlayerController;

	bool bEliminated = false;

	FTimerHandle EliminateTimer;

	UPROPERTY(EditDefaultsOnly)
	float EliminateDelay = 2.f;

	void EliminateTimerFinished();

	bool bLeftGame = false;

	UPROPERTY()
	class AMS_PlayerState* MS_PlayerState;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* CrownSystem;

	UPROPERTY()
	class UNiagaraComponent* CrownComponent;

	// Grenade

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* AttachedGrenade;

	// Default weapon
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	// Team colors

	UPROPERTY(EditAnywhere, Category = Eliminate)
	UMaterial* RedMaterial;

	UPROPERTY(EditAnywhere, Category = Eliminate)
	UMaterial* BlueMaterial;

	UPROPERTY(EditAnywhere, Category = Eliminate)
	UMaterial* OriginalMaterial;

	UPROPERTY()
	class AMS_GameMode* MS_GameMode;

public:	
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

	AWeapon* GetEquippedWeapon();

	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }

	FVector GetHitTarget() const;

	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool IsEliminated() const { return bEliminated; }

	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }

	ECombatState GetCombatState() const;

	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }

	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }

	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }

	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }

	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }

	bool IsLocallyReloading();

	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
	
	FORCEINLINE bool IsHoldingTheFlag() const;
};
