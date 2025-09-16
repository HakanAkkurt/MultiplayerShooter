// Hakan Games. All Rights Reserved.


#include "EliminateAnnouncement.h"
#include "Components/TextBlock.h"

void UEliminateAnnouncement::SetEliminateAnnouncementText(FString AttackerName, FString Victimname)
{
	FString EliminateAnnouncementText = FString::Printf(TEXT("%s eliminated %s!"), *AttackerName, *Victimname);
	
	if (AnnouncementText) {
		AnnouncementText->SetText(FText::FromString(EliminateAnnouncementText));
	}
}
