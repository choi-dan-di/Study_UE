// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "PlayerPawn.h"

// Sets default values
AEnemyActor::AEnemyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	SetRootComponent(boxComp);
	boxComp->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	meshComp->SetupAttachment(boxComp);
}

// Called when the game starts or when spawned
void AEnemyActor::BeginPlay()
{
	Super::BeginPlay();
	
	// 1~100 사이의 임의의 정수 값을 추첨
	int32 drawResult = FMath::RandRange(1, 100);

	// 만일, 추첨된 값이 추적 확률 변수보다 작거나 같다면 플레이어 쪽 방향 벡터 생성
	if (drawResult <= traceRate)
	{
		// 월드 공간에 APlayerPawn 클래스로 된 액터를 모두 검색
		for (TActorIterator<APlayerPawn> player(GetWorld()); player; ++player)
		{
			// 만일 검색된 액터의 이름에 "BP_PlayerPawn"이란 문구가 포함되어 있다면
			if (player->GetName().Contains(TEXT("BP_PlayerPawn")))
			{
				// 플레이어 액터의 위치 - 자신의 위치
				dir = player->GetActorLocation() - GetActorLocation();
				dir.Normalize();
			}
		}
	}
	else
	{
		// 그렇지 않다면 정면 방향 벡터 생성
		dir = GetActorForwardVector();
	}
}

// Called every frame
void AEnemyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 이동
	FVector newLocation = GetActorLocation() + dir * moveSpeed * DeltaTime;
	SetActorLocation(newLocation);
}

