// Hakan Games. All Rights Reserved.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText) {

		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;

	switch (RemoteRole)
	{
	case ROLE_None:

		Role = FString("None");

		break;
	case ROLE_SimulatedProxy:

		Role = FString("Simulated Proxy");

		break;
	case ROLE_AutonomousProxy:

		Role = FString("Autonomous Proxy");

		break;
	case ROLE_Authority:
		
		Role = FString("Authority");

		break;
	}

	FString PlayerName = "Unknown";

	const APlayerState* PlayerState = InPawn->GetPlayerState();
	if (PlayerState)
	{
		PlayerName = PlayerState->GetPlayerName();
	}

	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s - Name: %s"), *Role, *PlayerName);

	SetDisplayText(RemoteRoleString);
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
