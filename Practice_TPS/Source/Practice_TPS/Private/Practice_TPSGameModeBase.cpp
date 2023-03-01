// Fill out your copyright notice in the Description page of Project Settings.


#include "Practice_TPSGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.h"
#include "Components/TextBlock.h"

void APractice_TPSGameModeBase::AddScore(int32 point)
{
	// �Ű����� point�� ���� �Ѱܹ��� ������ ���� ������ �����Ѵ�.
	currentScore += point;

	// ���� ������ ������ �ݿ��Ѵ�.
	PrintScore();
}

void APractice_TPSGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (mainWidget != nullptr)
	{
		// mainWidget �������Ʈ ������ �޸𸮿� �ε��Ѵ�.
		mainUI = CreateWidget<UMainWidget>(GetWorld(), mainWidget);

		// ������ �޸𸮿� �ε�Ǹ� �� ��Ʈ�� ����Ѵ�.
		if (mainUI != nullptr)
			mainUI->AddToViewport(); 
	}
}

void APractice_TPSGameModeBase::PrintScore()
{
	if (mainUI != nullptr)
	{
		// scoreData �ؽ�Ʈ ��Ͽ� ���� ���� ���� �Է�
		mainUI->scoreData->SetText(FText::AsNumber(currentScore));
	}
}