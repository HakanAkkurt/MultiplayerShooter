// Hakan Games. All Rights Reserved.


#include "MS_HUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"
#include "Announcement.h"
#include "EliminateAnnouncement.h"
#include "Components/HorizontalBox.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

void AMS_HUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;

	if (GEngine) {

		GEngine->GameViewport->GetViewportSize(ViewportSize);

		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairsCenter) {

			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}

		if (HUDPackage.CrosshairsRight) {

			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}

		if (HUDPackage.CrosshairsLeft) {

			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}

		if (HUDPackage.CrosshairsTop) {

			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}

		if (HUDPackage.CrosshairsBottom) {

			FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
	}
}

void AMS_HUD::BeginPlay()
{
	Super::BeginPlay();
}

void AMS_HUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	
	if (PlayerController && CharacterOverlayClass) {

		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void AMS_HUD::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();

	if (PlayerController && AnnouncementClass) {

		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
}

void AMS_HUD::AddEliminateAnnouncement(FString Attacker, FString Victim)
{
	OwningPlayer = OwningPlayer == nullptr ? GetOwningPlayerController() : OwningPlayer;

	if (OwningPlayer && EliminateAnnouncementClass) {

		UEliminateAnnouncement* EliminateAnnouncementWidget = CreateWidget<UEliminateAnnouncement>(OwningPlayer, EliminateAnnouncementClass);
		
		if (EliminateAnnouncementWidget) {

			EliminateAnnouncementWidget->SetEliminateAnnouncementText(Attacker, Victim);
			EliminateAnnouncementWidget->AddToViewport();

			for (UEliminateAnnouncement* Message : EliminateMessages) {

				if (Message && Message->AnnouncementBox) {

					UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Message->AnnouncementBox);
					if (CanvasSlot) {

						FVector2D Position = CanvasSlot->GetPosition();
						FVector2D NewPosition(CanvasSlot->GetPosition().X,
							Position.Y - CanvasSlot->GetSize().Y);

						CanvasSlot->SetPosition(NewPosition);
					}
				}
			}

			EliminateMessages.Add(EliminateAnnouncementWidget);

			FTimerHandle EliminateMessageTimer;
			FTimerDelegate EliminateMessageDelegate;

			EliminateMessageDelegate.BindUFunction(this, FName("EliminateAnnouncementTimerFinished"), EliminateAnnouncementWidget);
			GetWorldTimerManager().SetTimer(
				EliminateMessageTimer,
				EliminateMessageDelegate,
				EliminateAnnouncementTime,
				false
			);
		}
	}
}

void AMS_HUD::EliminateAnnouncementTimerFinished(UEliminateAnnouncement* MessageToRemove)
{
	if (MessageToRemove) {
		MessageToRemove->RemoveFromParent();
	}
}

void AMS_HUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairColor
	);
}