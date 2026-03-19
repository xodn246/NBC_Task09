#include "Game/MyGameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/MyPlayerController.h"

void AMyGameStateBase::MulticastRPCBroadcastLoginMessaage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PlayerController) == true)
		{
			AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController);
			if (IsValid(MyPlayerController) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				MyPlayerController->PrintChatMessageString(NotificationString);
			}
		}
	}
}
