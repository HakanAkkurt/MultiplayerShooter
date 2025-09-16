// Hakan Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MS_HUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage {

	GENERATED_BODY()

public:
	class UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;
	float CrosshairSpread;
	FLinearColor CrosshairsColor;
};

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AMS_HUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	void AddCharacterOverlay();
	
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	UPROPERTY(EditAnywhere, Category = Announcements)
	TSubclassOf<class UUserWidget> AnnouncementClass;
	
	UPROPERTY()
	class UAnnouncement* Announcement;

	void AddAnnouncement();

	void AddEliminateAnnouncement(FString Attacker, FString Victim);

protected:
	virtual void BeginPlay() override;

private:
	
	UPROPERTY()
	class APlayerController* OwningPlayer;

	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UEliminateAnnouncement> EliminateAnnouncementClass;

	UPROPERTY(EditAnywhere)
	float EliminateAnnouncementTime = 3.0f;

	UFUNCTION()
	void EliminateAnnouncementTimerFinished(UEliminateAnnouncement* MessageToRemove);

	UPROPERTY()
	TArray<UEliminateAnnouncement*> EliminateMessages;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
	
};
