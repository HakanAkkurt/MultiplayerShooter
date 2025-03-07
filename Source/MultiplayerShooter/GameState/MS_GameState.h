// Hakan Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MS_GameState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AMS_GameState : public AGameState
{
	GENERATED_BODY()
	
public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void UpdateTopScore(class AMS_PlayerState* ScoringPlayer);

	UPROPERTY(Replicated)
	TArray<AMS_PlayerState*> TopScoringPlayers;

private:

	float TopScore = 0.f;
};
