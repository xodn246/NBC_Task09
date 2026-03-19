#include "Player/MyPlayerState.h"

#include "Net/UnrealNetwork.h"


AMyPlayerState::AMyPlayerState() 
	: PlayerNameString(TEXT("None")), 
	CurrentGuessCount(0),
	MaxGuessCount(3)
{
	bReplicates = true;
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount); // Q. MaxGuessCount가 프로퍼티 레플리케이션될 필요가 있을까?
}

FString AMyPlayerState::GetPlayerInfoString()
{
	FString PlayerInfoString = PlayerNameString + TEXT("(") + FString::FromInt(CurrentGuessCount + 1) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT(")");
	return PlayerInfoString;
}
