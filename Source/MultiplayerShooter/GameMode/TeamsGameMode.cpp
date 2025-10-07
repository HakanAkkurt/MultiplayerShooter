// Hakan Games. All Rights Reserved.


#include "TeamsGameMode.h"
#include "MultiplayerShooter/GameState/MS_GameState.h"
#include "MultiplayerShooter/PlayerState/MS_PlayerState.h"
#include "MultiplayerShooter/PlayerController/MS_PlayerController.h"
#include "Kismet/GameplayStatics.h"


ATeamsGameMode::ATeamsGameMode()
{
	bTeamsMatch = true;
}

void ATeamsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AMS_GameState* MSGameState = Cast<AMS_GameState>(UGameplayStatics::GetGameState(this));
	if (MSGameState)
	{
		AMS_PlayerState* MSPlayerState = NewPlayer->GetPlayerState<AMS_PlayerState>();
		if (MSPlayerState && MSPlayerState->GetTeam() == ETeam::ET_NoTeam)
		{
			if (MSGameState->BlueTeam.Num() >= MSGameState->RedTeam.Num())
			{
				MSGameState->RedTeam.AddUnique(MSPlayerState);
				MSPlayerState->SetTeam(ETeam::ET_RedTeam);
			}
			else
			{
				MSGameState->BlueTeam.AddUnique(MSPlayerState);
				MSPlayerState->SetTeam(ETeam::ET_BlueTeam);
			}
		}
	}
}

void ATeamsGameMode::Logout(AController* Exiting)
{
	AMS_GameState* MSGameState = Cast<AMS_GameState>(UGameplayStatics::GetGameState(this));
	AMS_PlayerState* MSPlayerState = Exiting->GetPlayerState<AMS_PlayerState>();
	if (MSGameState && MSPlayerState)
	{
		if (MSGameState->RedTeam.Contains(MSPlayerState))
		{
			MSGameState->RedTeam.Remove(MSPlayerState);
		}
		if (MSGameState->BlueTeam.Contains(MSPlayerState))
		{
			MSGameState->BlueTeam.Remove(MSPlayerState);
		}
	}

}

float ATeamsGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	AMS_PlayerState* AttackerPState = Attacker->GetPlayerState<AMS_PlayerState>();
	AMS_PlayerState* VictimPState = Victim->GetPlayerState<AMS_PlayerState>();

	if (AttackerPState == nullptr || VictimPState == nullptr) return BaseDamage;
	if (VictimPState == AttackerPState) {
		
		return BaseDamage;
	}

	if (AttackerPState->GetTeam() == VictimPState->GetTeam()) {
		
		return 0.f;
	}

	return BaseDamage;
}

void ATeamsGameMode::PlayerEliminated(APlayerCharacter* EliminatedCharacter, AMS_PlayerController* VictimController, AMS_PlayerController* AttackerController)
{
	Super::PlayerEliminated(EliminatedCharacter, VictimController, AttackerController);

	AMS_GameState* MSGameState = Cast<AMS_GameState>(UGameplayStatics::GetGameState(this));
	AMS_PlayerState* AttackerPlayerState = AttackerController ? Cast<AMS_PlayerState>(AttackerController->PlayerState) : nullptr;
	if (MSGameState && AttackerPlayerState)
	{
		if (AttackerPlayerState->GetTeam() == ETeam::ET_BlueTeam) {
			MSGameState->BlueTeamScores();
		}

		if (AttackerPlayerState->GetTeam() == ETeam::ET_RedTeam) {

			MSGameState->RedTeamScores();
		}
	}
}

void ATeamsGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	AMS_GameState* MSGameState = Cast<AMS_GameState>(UGameplayStatics::GetGameState(this));
	if (MSGameState)
	{
		for (auto PState : MSGameState->PlayerArray)
		{
			AMS_PlayerState* MSPlayerState = Cast<AMS_PlayerState>(PState.Get());
			if (MSPlayerState && MSPlayerState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (MSGameState->BlueTeam.Num() >= MSGameState->RedTeam.Num())
				{
					MSGameState->RedTeam.AddUnique(MSPlayerState);
					MSPlayerState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					MSGameState->BlueTeam.AddUnique(MSPlayerState);
					MSPlayerState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}
}