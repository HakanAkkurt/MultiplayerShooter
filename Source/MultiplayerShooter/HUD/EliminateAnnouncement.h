// Hakan Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EliminateAnnouncement.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API UEliminateAnnouncement : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetEliminateAnnouncementText(FString AttackerName, FString Victimname);

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* AnnouncementBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AnnouncementText;
	
};
