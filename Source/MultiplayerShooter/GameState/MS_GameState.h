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


	// Teams 

	void RedTeamScores();
	void BlueTeamScores();

	TArray<AMS_PlayerState*> RedTeam;
	TArray<AMS_PlayerState*> BlueTeam;

	UPROPERTY(ReplicatedUsing = OnRep_RedTeamScore)
	float RedTeamScore = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_BlueTeamScore)
	float BlueTeamScore = 0.f;

	UFUNCTION()
	void OnRep_RedTeamScore();

	UFUNCTION()
	void OnRep_BlueTeamScore();

private:

	float TopScore = 0.f;
};
