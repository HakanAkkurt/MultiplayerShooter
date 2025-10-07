// Hakan Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MS_GameMode.h"
#include "TeamsGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API ATeamsGameMode : public AMS_GameMode
{
	GENERATED_BODY()

public:
	ATeamsGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage) override;
	virtual void PlayerEliminated(class APlayerCharacter* EliminatedCharacter, class AMS_PlayerController* VictimController, class AMS_PlayerController* AttackerController) override;

protected:
	virtual void HandleMatchHasStarted() override;
	
};
