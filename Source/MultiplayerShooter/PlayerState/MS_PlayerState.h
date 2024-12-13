// Hakan Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MS_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AMS_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	void AddToScore(float ScoreAmount);
	virtual void OnRep_Score() override;

private:
	class APlayerCharacter* Character;
	class AMS_PlayerController* Controller;
	
};
