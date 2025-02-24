// Hakan Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MS_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AMS_PlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountdownTime);

	virtual void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;

	virtual float GetServerTime(); // Synced with server world clock

	virtual void ReceivedPlayer() override; // Synced with server clock as soon as possible

protected:
	virtual void BeginPlay() override;

	void SetHUDTime();

	// Sync time between client and server

	// Request the current server time, passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	// Reports the current server time to the client in repsonse to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f; // difference between client and server time

	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.f;

	void CheckTimeSync(float DeltaTime);

private:
	UPROPERTY()
	class AMS_HUD* MS_HUD;

	float MatchTime = 120.f;
	uint32 CountdownInt = 0;
};
