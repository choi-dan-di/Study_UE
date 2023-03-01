// Fill out your copyright notice in the Description page of Project Settings.


#include "Practice_TPSGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.h"
#include "Components/TextBlock.h"

void APractice_TPSGameModeBase::AddScore(int32 point)
{
	// 매개변수 point를 통해 넘겨받은 점수를 현재 점수에 누적한다.
	currentScore += point;

	// 현재 점수를 위젯에 반영한다.
	PrintScore();
}

void APractice_TPSGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (mainWidget != nullptr)
	{
		// mainWidget 블루프린트 파일을 메모리에 로드한다.
		mainUI = CreateWidget<UMainWidget>(GetWorld(), mainWidget);

		// 위젯이 메모리에 로드되면 뷰 포트에 출력한다.
		if (mainUI != nullptr)
			mainUI->AddToViewport(); 
	}
}

void APractice_TPSGameModeBase::PrintScore()
{
	if (mainUI != nullptr)
	{
		// scoreData 텍스트 블록에 현재 점수 값을 입력
		mainUI->scoreData->SetText(FText::AsNumber(currentScore));
	}
}