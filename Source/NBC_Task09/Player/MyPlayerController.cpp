#include "Player/MyPlayerController.h"

#include "MyPlayerState.h"
#include "UI/MyChatInput.h"
#include "NBC_Task09.h"
#include "Game/MyGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AMyPlayerController::AMyPlayerController()
{
	bReplicates = true;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController() == false)
	{
		return;
	}
	
	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);
	
	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UMyChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}
	
	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void AMyPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;
	
	if (IsLocalController() == true)
	{
		AMyPlayerState* MyPlayerState = GetPlayerState<AMyPlayerState>();
		if (IsValid(MyPlayerState) == true)
		{
			FString CombinedMessageString = MyPlayerState->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;
			
			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void AMyPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	MyChatFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void AMyPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, NotificationText);
}

void AMyPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void AMyPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		AMyGameModeBase* MyGameModeBase = Cast<AMyGameModeBase>(GM);
		if (IsValid(MyGameModeBase) == true)
		{
			MyGameModeBase->PrintChatMessageString(this, InChatMessageString);
		}
	}
}
