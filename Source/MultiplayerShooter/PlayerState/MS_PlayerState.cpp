// Hakan Games. All Rights Reserved.


#include "MS_PlayerState.h"
#include "MultiplayerShooter/Character/PlayerCharacter.h"
#include "MultiplayerShooter/PlayerController/MS_PlayerController.h"

void AMS_PlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? Cast<APlayerCharacter>(GetPawn()) : Character;
	if (Character) {

		Controller = Controller == nullptr ? Cast<AMS_PlayerController>(Character->Controller) : Controller;
		if (Controller) {

			Controller->SetHUDScore(GetScore());
		}

	}
}

void AMS_PlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<APlayerCharacter>(GetPawn()) : Character;
	if (Character) {

		Controller = Controller == nullptr ? Cast<AMS_PlayerController>(Character->Controller) : Controller;
		if (Controller) {

			Controller->SetHUDScore(GetScore());

		}

	}

}
