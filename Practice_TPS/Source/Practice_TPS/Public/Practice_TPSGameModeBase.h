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

	// ���� �� ��Ʈ�� �ε�� ���� ����� ����
	class UMainWidget* mainUI;

	class UMenuWidget* menuUI;

	void PrintScore();
};
