// Hakan Games. All Rights Reserved.


#include "MS_PlayerController.h"
#include "MultiplayerShooter/HUD/MS_HUD.h"
#include "MultiplayerShooter/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MultiplayerShooter/Character/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerShooter/GameMode/MS_GameMode.h"

void AMS_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	MS_HUD = Cast<AMS_HUD>(GetHUD());
}

void AMS_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();

	CheckTimeSync(DeltaTime);

	PollInit();
}

void AMS_PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMS_PlayerController, MatchState);
}

void AMS_PlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;

	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency) {

		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

float AMS_PlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AMS_PlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController()) {
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void AMS_PlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress) {

		MS_HUD = MS_HUD == nullptr ? Cast<AMS_HUD>(GetHUD()) : MS_HUD;
		if (MS_HUD) {

			MS_HUD->AddCharacterOverlay();
		}
	}
}

void AMS_PlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress) {

		MS_HUD = MS_HUD == nullptr ? Cast<AMS_HUD>(GetHUD()) : MS_HUD;
		if (MS_HUD) {

			MS_HUD->AddCharacterOverlay();
		}
	}
}

void AMS_PlayerController::SetHUDTime()
{
	uint32 SecondsLeft = FMath::CeilToInt(MatchTime - GetServerTime());

	if (CountdownInt != SecondsLeft) {

		SetHUDMatchCountdown(MatchTime - GetServerTime());
	}
	CountdownInt = SecondsLeft;
}

void AMS_PlayerController::PollInit()
{
	if (CharacterOverlay == nullptr) {

		if (MS_HUD && MS_HUD->CharacterOverlay) {

			CharacterOverlay = MS_HUD->CharacterOverlay;
			if (CharacterOverlay) {

				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
			}
		}
	}
}

void AMS_PlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AMS_PlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);

	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void AMS_PlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	MS_HUD = MS_HUD == nullptr ? Cast<AMS_HUD>(GetHUD()) : MS_HUD;

	if (MS_HUD
		&& MS_HUD->CharacterOverlay
		&& MS_HUD->CharacterOverlay->HealthBar
		&& MS_HUD->CharacterOverlay->HealthText) {

		const float HealthPercent = Health / MaxHealth;
		MS_HUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);

		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		MS_HUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else {

		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void AMS_PlayerController::SetHUDScore(float Score)
{
	MS_HUD = MS_HUD == nullptr ? Cast<AMS_HUD>(GetHUD()) : MS_HUD;
	if (MS_HUD
		&& MS_HUD->CharacterOverlay
		&& MS_HUD->CharacterOverlay->ScoreAmount) {

		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		MS_HUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else {

		bInitializeCharacterOverlay = true;
		HUDScore = Score;
	}
}

void AMS_PlayerController::SetHUDDefeats(int32 Defeats)
{
	MS_HUD = MS_HUD == nullptr ? Cast<AMS_HUD>(GetHUD()) : MS_HUD;
	if (MS_HUD
		&& MS_HUD->CharacterOverlay
		&& MS_HUD->CharacterOverlay->DefeatsAmount) {

		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		MS_HUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	else {

		bInitializeCharacterOverlay = true;
		HUDDefeats = Defeats;
	}
}

void AMS_PlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	MS_HUD = MS_HUD == nullptr ? Cast<AMS_HUD>(GetHUD()) : MS_HUD;
	if (MS_HUD
		&& MS_HUD->CharacterOverlay
		&& MS_HUD->CharacterOverlay->WeaponAmmoAmount) {

		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		MS_HUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AMS_PlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	MS_HUD = MS_HUD == nullptr ? Cast<AMS_HUD>(GetHUD()) : MS_HUD;
	if (MS_HUD
		&& MS_HUD->CharacterOverlay
		&& MS_HUD->CharacterOverlay->CarriedAmmoAmount) {

		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		MS_HUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AMS_PlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	MS_HUD = MS_HUD == nullptr ? Cast<AMS_HUD>(GetHUD()) : MS_HUD;
	if (MS_HUD
		&& MS_HUD->CharacterOverlay
		&& MS_HUD->CharacterOverlay->MatchCountdownText) {

		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		MS_HUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void AMS_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(InPawn);
	if (PlayerCharacter) {

		SetHUDHealth(PlayerCharacter->GetHealth(), PlayerCharacter->GetMaxHealth());
	}
}
