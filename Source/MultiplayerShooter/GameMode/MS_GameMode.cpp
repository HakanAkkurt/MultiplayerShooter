// Hakan Games. All Rights Reserved.


#include "MS_GameMode.h"
#include "MultiplayerShooter/Character/PlayerCharacter.h"
#include "MultiplayerShooter/PlayerController/MS_PlayerController.h"

void AMS_GameMode::PlayerEliminated(APlayerCharacter* EliminatedCharacter, APlayerController* VictimController, APlayerController* AttackerController)
{
	if (EliminatedCharacter) {

		EliminatedCharacter->Eliminate();
	}
}
