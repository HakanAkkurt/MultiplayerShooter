// Hakan Games. All Rights Reserved.


#include "MS_GameState.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerShooter/PlayerState/MS_PlayerState.h"
#include "MultiplayerShooter/PlayerController/MS_PlayerController.h"

void AMS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMS_GameState, TopScoringPlayers);
	DOREPLIFETIME(AMS_GameState, RedTeamScore);
	DOREPLIFETIME(AMS_GameState, BlueTeamScore);
}

void AMS_GameState::UpdateTopScore(AMS_PlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.Num() == 0) {

		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if (ScoringPlayer->GetScore() == TopScore) {

		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore) {

		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

void AMS_GameState::RedTeamScores()
{
	++RedTeamScore;

	AMS_PlayerController* MS_Player = Cast<AMS_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (MS_Player) {

		MS_Player->SetHUDRedTeamScore(RedTeamScore);
	}
}

void AMS_GameState::BlueTeamScores()
{
	++BlueTeamScore;

	AMS_PlayerController* MS_Player = Cast<AMS_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (MS_Player) {

		MS_Player->SetHUDBlueTeamScore(BlueTeamScore);
	}
}

void AMS_GameState::OnRep_RedTeamScore()
{
	AMS_PlayerController* MS_Player = Cast<AMS_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (MS_Player) {

		MS_Player->SetHUDRedTeamScore(RedTeamScore);
	}
}

void AMS_GameState::OnRep_BlueTeamScore()
{
	AMS_PlayerController* MS_Player = Cast<AMS_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (MS_Player) {

		MS_Player->SetHUDBlueTeamScore(BlueTeamScore);
	}
}
