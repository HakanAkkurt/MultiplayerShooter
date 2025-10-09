// Hakan Games. All Rights Reserved.


#include "CaptureTheFlagGameMode.h"
#include "MultiplayerShooter/Weapon/Flag.h"
#include "MultiplayerShooter/CaptureTheFlag/FlagZone.h"
#include "MultiplayerShooter/GameState/MS_GameState.h"

void ACaptureTheFlagGameMode::PlayerEliminated(class APlayerCharacter* EliminatedCharacter, class AMS_PlayerController* VictimController, AMS_PlayerController* AttackerController)
{
	AMS_GameMode::PlayerEliminated(EliminatedCharacter, VictimController, AttackerController);
}

void ACaptureTheFlagGameMode::FlagCaptured(AFlag* Flag, AFlagZone* Zone)
{
	bool bValidCapture = Flag->GetTeam() != Zone->Team;
	AMS_GameState* MSGameState = Cast<AMS_GameState>(GameState);
	if (MSGameState) {

		if (Zone->Team == ETeam::ET_BlueTeam) {

			MSGameState->BlueTeamScores();
		}
		if (Zone->Team == ETeam::ET_RedTeam) {

			MSGameState->RedTeamScores();
		}
	}
}

