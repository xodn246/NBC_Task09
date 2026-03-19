#include "Game/MyGameModeBase.h"

#include "MyGameStateBase.h"
#include "Player/MyPlayerState.h"

void AMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	SecretNumberString = GenerateSecretNumber();
	
	UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumberString);
}

void AMyGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(NewPlayer);
	if (IsValid(MyPlayerController) == true){
		MyPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
		
		AllPlayerControllers.Add((MyPlayerController));
		
		AMyPlayerState* MyPlayerState = MyPlayerController->GetPlayerState<AMyPlayerState>();
		if (IsValid(MyPlayerState) == true)
		{
			MyPlayerState->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}
		
		AMyGameStateBase* MyGameStateBase = GetGameState<AMyGameStateBase>();
		if (IsValid(MyGameStateBase) == true)
		{
			MyGameStateBase->MulticastRPCBroadcastLoginMessaage(MyPlayerState->PlayerNameString);
		}
	}
}

void AMyGameModeBase::PrintChatMessageString(AMyPlayerController* InChattingPlayerController,
	const FString& InChatMessageString)
{
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);
		
		IncreaseGuessCount(InChattingPlayerController);
		
		for (TActorIterator<AMyPlayerController> It(GetWorld()); It; ++It)
		{
			AMyPlayerController* MyPlayerController = *It;
			if (IsValid(MyPlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
				MyPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
				
				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(InChattingPlayerController, StrikeCount);
			}
		}
	}
	else
	{
		for (TActorIterator<AMyPlayerController> It(GetWorld()); It; ++It)
		{
			AMyPlayerController* MyPlayerController = *It;
			if (IsValid(MyPlayerController) == true)
			{
				MyPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

FString AMyGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });
	
	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool AMyGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}
			
			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;
		
	} while (false);	

	return bCanPlay;
}

FString AMyGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else 
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;				
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void AMyGameModeBase::IncreaseGuessCount(AMyPlayerController* InChattingPlayerController)
{
	AMyPlayerState* MyPlayerState = InChattingPlayerController->GetPlayerState<AMyPlayerState>();
	if (IsValid(MyPlayerState) == true)
	{
		MyPlayerState->CurrentGuessCount++;
	}
}

void AMyGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& MyPlayerController : AllPlayerControllers)
	{
		AMyPlayerState* MyPlayerState = MyPlayerController->GetPlayerState<AMyPlayerState>();
		if (IsValid(MyPlayerState) == true)
		{
			MyPlayerState->CurrentGuessCount = 0;
		}
	}
}

void AMyGameModeBase::JudgeGame(AMyPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		AMyPlayerState* CXPS = InChattingPlayerController->GetPlayerState<AMyPlayerState>();
		for (const auto& CXPlayerController : AllPlayerControllers)
		{
			if (IsValid(CXPS) == true)
			{
				FString CombinedMessageString = CXPS->PlayerNameString + TEXT(" has win.");
				CXPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& CXPlayerController : AllPlayerControllers)
		{
			AMyPlayerState* CXPS = CXPlayerController->GetPlayerState<AMyPlayerState>();
			if (IsValid(CXPS) == true)
			{
				if (CXPS->CurrentGuessCount < CXPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& CXPlayerController : AllPlayerControllers)
			{
				CXPlayerController->NotificationText = FText::FromString(TEXT("Draw."));

				ResetGame();
			}
		}
	}
}
	
	
