// Hakan Games. All Rights Reserved.


#include "MS_PlayerController.h"
#include "MultiplayerShooter/HUD/MS_HUD.h"
#include "MultiplayerShooter/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MultiplayerShooter/Character/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerShooter/GameMode/MS_GameMode.h"
#include "MultiplayerShooter/HUD/Announcement.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/Components/CombatComponent.h"
#include "MultiplayerShooter/GameState/MS_GameState.h"
#include "MultiplayerShooter/PlayerState/MS_PlayerState.h"
#include "Components/Image.h"

void AMS_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	MS_HUD = Cast<AMS_HUD>(GetHUD());

	ServerCheckMatchState();
}

void AMS_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();

	CheckTimeSync(DeltaTime);

	PollInit();

	CheckPing(DeltaTime);
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

void AMS_PlayerController::HighPingWarning()
{
	MS_HUD = MS_HUD == nullptr ? Cast<AMS_HUD>(GetHUD()) : MS_HUD;

	if (MS_HUD && MS_HUD->CharacterOverlay &&
		MS_HUD->CharacterOverlay->HighPingImage &&
		MS_HUD->CharacterOverlay->HighPingAnimation){

		MS_HUD->CharacterOverlay->HighPingImage->SetOpacity(1.f);
		MS_HUD->CharacterOverlay->PlayAnimation(MS_HUD->CharacterOverlay->HighPingAnimation, 0.f, 5);
	}
}

void AMS_PlayerController::StopHighPingWarning()
{
	MS_HUD = MS_HUD == nullptr ? Cast<AMS_HUD>(GetHUD()) : MS_HUD;

	if (MS_HUD && MS_HUD->CharacterOverlay &&
		MS_HUD->CharacterOverlay->HighPingImage &&
		MS_HUD->CharacterOverlay->HighPingAnimation) {

		MS_HUD->CharacterOverlay->HighPingImage->SetOpacity(0.f);

		if (MS_HUD->CharacterOverlay->IsAnimationPlaying(MS_HUD->CharacterOverlay->HighPingAnimation)) {

			MS_HUD->CharacterOverlay->StopAnimation(MS_HUD->CharacterOverlay->HighPingAnimation);
		}
	}
}

void AMS_PlayerController::CheckPing(float DeltaTime)
{
	HighPingRunningTime += DeltaTime;

	if (HighPingRunningTime > CheckPingFrequency){

		if (!PlayerState) PlayerState = GetPlayerState<APlayerState>();

		if (PlayerState) {

			PlayerState = GetPlayerState<APlayerState>();
			if (PlayerState->GetPingInMilliseconds() * 4 > HighPingThreshold) { // ping is compressed; it's actually ping / 4

				HighPingWarning();
				PingAnimationRunningTime = 0.f;
			}
		}

		HighPingRunningTime = 0.f;
	}

	if (MS_HUD && MS_HUD->CharacterOverlay &&
		MS_HUD->CharacterOverlay->HighPingAnimation &&
		MS_HUD->CharacterOverlay->IsAnimationPlaying(MS_HUD->CharacterOverlay->HighPingAnimation)) {

		PingAnimationRunningTime += DeltaTime;
		if (PingAnimationRunningTime > HighPingDuration) {

			StopHighPingWarning();
		}
	}
}

void AMS_PlayerController::ServerCheckMatchState_Implementation()
{
	AMS_GameMode* GameMode = Cast<AMS_GameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode) {

		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();

		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);

	}

}

void AMS_PlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;

	OnMatchStateSet(MatchState);

	if (MS_HUD && MatchState == MatchState::WaitingToStart) {

		MS_HUD->AddAnnouncement();
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

		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown) {
		
		HandleCooldown();
	}
}

void AMS_PlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress) {

		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown) {

		HandleCooldown();
	}
}

void AMS_PlayerController::HandleMatchHasStarted()
{
	MS_HUD = MS_HUD == nullptr ? Cast<AMS_HUD>(GetHUD()) : MS_HUD;
	if (MS_HUD) {

		if (MS_HUD->CharacterOverlay == nullptr) MS_HUD->AddCharacterOverlay();

		if (MS_HUD->Announcement) {
			MS_HUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AMS_PlayerController::HandleCooldown()
{
	MS_HUD = MS_HUD == nullptr ? Cast<AMS_HUD>(GetHUD()) : MS_HUD;
	if (MS_HUD) {

		MS_HUD->CharacterOverlay->RemoveFromParent();

		if (MS_HUD->Announcement
			&& MS_HUD->Announcement->AnnouncementText
			&& MS_HUD->Announcement->InfoText) {

			MS_HUD->Announcement->SetVisibility(ESlateVisibility::Visible);

			FString AnnouncementText("New match starts in:");
			MS_HUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			AMS_GameState* MS_GameState = Cast<AMS_GameState>(UGameplayStatics::GetGameState(this));
			AMS_PlayerState* MS_PlayerState = GetPlayerState<AMS_PlayerState>();

			if (MS_GameState && MS_PlayerState) {

				TArray<AMS_PlayerState*> TopPlayers = MS_GameState->TopScoringPlayers;
				
				FString InfoTextString;
				if (TopPlayers.Num() == 0) {

					InfoTextString = FString("There is no winner.");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == MS_PlayerState) {

					InfoTextString = FString("You are the winner!");
				}
				else if (TopPlayers.Num() == 1) {

					InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1) {

					InfoTextString = FString("Players tied for the win:\n");
					for (auto TiedPlayer : TopPlayers) {
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}

				MS_HUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}

		}
	}

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());

	if (PlayerCharacter && PlayerCharacter->GetCombat()) {

		PlayerCharacter->bDisableGameplay = true;
		PlayerCharacter->GetCombat()->FireButtonPressed(false);
	}
}

void AMS_PlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (HasAuthority()) {

		MS_GameMode = MS_GameMode == nullptr ? Cast<AMS_GameMode>(UGameplayStatics::GetGameMode(this)) : MS_GameMode;
		if (MS_GameMode) {

			SecondsLeft = FMath::CeilToInt(MS_GameMode->GetCountdownTime() + LevelStartingTime);
		}
	}

	if (CountdownInt != SecondsLeft) {

		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown) {

			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress) {

			SetHUDMatchCountdown(TimeLeft);
		}
	}
	CountdownInt = SecondsLeft;
}

void AMS_PlayerController::PollInit()
{
	if (CharacterOverlay == nullptr) {

		if (MS_HUD && MS_HUD->CharacterOverlay) {

			CharacterOverlay = MS_HUD->CharacterOverlay;
			if (CharacterOverlay) {

				if (bInitializeHealth) SetHUDHealth(HUDHealth, HUDMaxHealth);
				if (bInitializeShield) SetHUDShield(HUDShield, HUDMaxShield);
				if (bInitializeScore) SetHUDScore(HUDScore);
				if (bInitializeDefeats) SetHUDDefeats(HUDDefeats);
				if (bInitializeCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
				if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);

				APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
				if (PlayerCharacter && PlayerCharacter->GetCombat()) {

					SetHUDGrenades(PlayerCharacter->GetCombat()->GetGrenades());
				}
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

		bInitializeHealth = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void AMS_PlayerController::SetHUDShield(float Shield, float MaxShield)
{
	MS_HUD = MS_HUD == nullptr ? Cast<AMS_HUD>(GetHUD()) : MS_HUD;

	if (MS_HUD
		&& MS_HUD->CharacterOverlay
		&& MS_HUD->CharacterOverlay->ShieldBar
		&& MS_HUD->CharacterOverlay->ShieldText) {

		const float ShieldPercent = Shield / MaxShield;
		MS_HUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);

		FString ShieldText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Shield), FMath::CeilToInt(MaxShield));
		MS_HUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));
	}
	else {

		bInitializeShield = true;
		HUDShield = Shield;
		HUDMaxShield = MaxShield;
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

		bInitializeScore = true;
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

		bInitializeDefeats = true;
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
	else {
		bInitializeWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;
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
	else {
		bInitializeCarriedAmmo = true;
		HUDCarriedAmmo = Ammo;
	}
}

void AMS_PlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	MS_HUD = MS_HUD == nullptr ? Cast<AMS_HUD>(GetHUD()) : MS_HUD;
	if (MS_HUD
		&& MS_HUD->CharacterOverlay
		&& MS_HUD->CharacterOverlay->MatchCountdownText) {

		if (CountdownTime < 0.f) {
			MS_HUD->CharacterOverlay->MatchCountdownText->SetText(FText());

			return;
		}

		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		MS_HUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void AMS_PlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	MS_HUD = MS_HUD == nullptr ? Cast<AMS_HUD>(GetHUD()) : MS_HUD;
	if (MS_HUD
		&& MS_HUD->Announcement
		&& MS_HUD->Announcement->WarmupTime) {

		if (CountdownTime < 0.f) {
			MS_HUD->Announcement->WarmupTime->SetText(FText());

			return;
		}

		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		MS_HUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void AMS_PlayerController::SetHUDGrenades(int32 Grenades)
{
	MS_HUD = MS_HUD == nullptr ? Cast<AMS_HUD>(GetHUD()) : MS_HUD;
	if (MS_HUD
		&& MS_HUD->CharacterOverlay
		&& MS_HUD->CharacterOverlay->GrenadesText) {

		FString GrenadesText = FString::Printf(TEXT("%d"), Grenades);
		MS_HUD->CharacterOverlay->GrenadesText->SetText(FText::FromString(GrenadesText));
	}
	else {

		HUDGrenades = Grenades;
	}
}

void AMS_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(InPawn);
	if (PlayerCharacter) {

		SetHUDHealth(PlayerCharacter->GetHealth(), PlayerCharacter->GetMaxHealth());
		SetHUDShield(PlayerCharacter->GetShield(), PlayerCharacter->GetMaxShield());
		PlayerCharacter->UpdateHUDAmmo();
	}
}
