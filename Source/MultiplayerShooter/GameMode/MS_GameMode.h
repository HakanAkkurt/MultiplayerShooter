// Hakan Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MS_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AMS_GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void PlayerEliminated(class APlayerCharacter* EliminatedCharacter, class APlayerController* VictimController,
		class APlayerController* AttackerController);

	virtual void RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController);
	
};
