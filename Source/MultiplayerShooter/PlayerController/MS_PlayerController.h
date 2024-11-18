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


private:
	class AMS_HUD* MS_HUD;

protected:
	virtual void BeginPlay() override;

public:
	void SetHUDHealth(float Health, float MaxHealth);
};
