// Hakan Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MS_GameMode.generated.h"

namespace MatchState
{
	extern MULTIPLAYERSHOOTER_API const FName Cooldown; // Match duration has been reached. Display winner and begin cooldown timer.
}
/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AMS_GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AMS_GameMode();

	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class APlayerCharacter* EliminatedCharacter, class AMS_PlayerController* VictimController, class AMS_PlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController);
	void PlayerLeftGame(class AMS_PlayerState* PlayerLeaving);

	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	float LevelStartingTime = 0.f;

	bool bTeamsMatch = false;
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountdownTime = 0.f;

public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
};
