// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Practice_TPSGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PRACTICE_TPS_API APractice_TPSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void AddScore(int32 point);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMainWidget> mainWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMenuWidget> menuWidget;

	void ShowMenu();

protected:
	virtual void BeginPlay() override;

private:
	int32 currentScore = 0;

	// 현재 뷰 포트에 로드된 위젯 저장용 변수
	class UMainWidget* mainUI;

	class UMenuWidget* menuUI;

	void PrintScore();
};
