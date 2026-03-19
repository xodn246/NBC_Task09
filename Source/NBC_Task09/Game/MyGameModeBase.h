#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "GameFramework/GameModeBase.h"
#include "Player/MyPlayerController.h"
#include "MyGameModeBase.generated.h"

class AMyPlayerController;

UCLASS

()
class NBC_TASK09_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void OnPostLogin(AController* NewPlayer) override;
	
	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);
	
	virtual void BeginPlay() override;
	
	void PrintChatMessageString(AMyPlayerController* InChattingPlayerController, const FString& InChatMessageString);
	
	void IncreaseGuessCount(AMyPlayerController* InChattingPlayerController);
	
	void ResetGame();
	
	void JudgeGame(AMyPlayerController* InChattingPlayerController, int InStrikeCount);
	
protected:
	FString SecretNumberString;

	TArray<TObjectPtr<AMyPlayerController>> AllPlayerControllers;
};
