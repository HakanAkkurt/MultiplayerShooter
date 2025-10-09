// Hakan Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TeamsGameMode.h"
#include "CaptureTheFlagGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API ACaptureTheFlagGameMode : public ATeamsGameMode
{
	GENERATED_BODY()
	
public:
	virtual void PlayerEliminated(class APlayerCharacter* EliminatedCharacter, class AMS_PlayerController* VictimController, AMS_PlayerController* AttackerController) override;
	
	void FlagCaptured(class AFlag* Flag, class AFlagZone* Zone);
};
