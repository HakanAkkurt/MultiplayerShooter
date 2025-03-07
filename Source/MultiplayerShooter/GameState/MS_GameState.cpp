// Hakan Games. All Rights Reserved.


#include "MS_GameState.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerShooter/PlayerState/MS_PlayerState.h"

void AMS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMS_GameState, TopScoringPlayers);
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
