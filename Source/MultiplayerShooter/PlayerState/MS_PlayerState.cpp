// Hakan Games. All Rights Reserved.


#include "MS_PlayerState.h"
#include "MultiplayerShooter/Character/PlayerCharacter.h"
#include "MultiplayerShooter/PlayerController/MS_PlayerController.h"
#include "Net/UnrealNetwork.h"


void AMS_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMS_PlayerState, Defeats);
	DOREPLIFETIME(AMS_PlayerState, Team);
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

void AMS_PlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<APlayerCharacter>(GetPawn()) : Character;
	if (Character) {

		Controller = Controller == nullptr ? Cast<AMS_PlayerController>(Character->Controller) : Controller;
		if (Controller) {

			Controller->SetHUDDefeats(Defeats);
		}

	}
}


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

void AMS_PlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? Cast<APlayerCharacter>(GetPawn()) : Character;
	if (Character) {

		Controller = Controller == nullptr ? Cast<AMS_PlayerController>(Character->Controller) : Controller;
		if (Controller) {

			Controller->SetHUDDefeats(Defeats);
		}

	}
}

void AMS_PlayerState::OnRep_Team()
{
	APlayerCharacter* P_Character = Cast <APlayerCharacter>(GetPawn());
	if (P_Character)
	{
		P_Character->SetTeamColor(Team);
	}
}

void AMS_PlayerState::SetTeam(ETeam TeamToSet)
{
	Team = TeamToSet;

	APlayerCharacter* P_Character = Cast <APlayerCharacter>(GetPawn());
	if (P_Character)
	{
		P_Character->SetTeamColor(Team);
	}
}