// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMenuWidget::Restart()
{
	// 레벨을 다시 로드한다.
	UGameplayStatics::OpenLevel(GetWorld(), "ShootingMap");
}

void UMenuWidget::Quit()
{
	// 앱을 종료한다.
	UKismetSystemLibrary::QuitGame(
		GetWorld(), 
		GetWorld()->GetFirstPlayerController(), 
		EQuitPreference::Quit, 
		false
	);
}

// 델리게이트에 함수를 연결해주는 역할의 함수. BeginPlay와 비슷하다.
void UMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 각 버튼 입력 이벤트에 대응하는 함수를 연결한다.
	button_Restart->OnClicked.AddDynamic(this, &UMenuWidget::Restart);
	button_Quit->OnClicked.AddDynamic(this, &UMenuWidget::Quit);
}
