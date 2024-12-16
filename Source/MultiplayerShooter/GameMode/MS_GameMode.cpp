// Hakan Games. All Rights Reserved.


#include "MS_GameMode.h"
#include "MultiplayerShooter/Character/PlayerCharacter.h"
#include "MultiplayerShooter/PlayerController/MS_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "MultiplayerShooter/PlayerState/MS_PlayerState.h"

void AMS_GameMode::PlayerEliminated(APlayerCharacter* EliminatedCharacter, APlayerController* VictimController, APlayerController* AttackerController)
{
	AMS_PlayerState* AttackerPlayerState = AttackerController ? Cast<AMS_PlayerState>(AttackerController->PlayerState) : nullptr;
	AMS_PlayerState* VictimPlayerState = VictimController ? Cast<AMS_PlayerState>(VictimController->PlayerState) : nullptr;

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState) {

		AttackerPlayerState->AddToScore(1.f);
	}

	if (VictimPlayerState) {

		VictimPlayerState->AddToDefeats(1);
	}

	if (EliminatedCharacter) {

		EliminatedCharacter->Eliminate();
	}
}

void AMS_GameMode::RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController)
{
	if (EliminatedCharacter) {

		EliminatedCharacter->Reset();
		EliminatedCharacter->Destroy();
	}

	if (EliminatedController) {

		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);

		RestartPlayerAtPlayerStart(EliminatedController, PlayerStarts[Selection]);
	}
}
