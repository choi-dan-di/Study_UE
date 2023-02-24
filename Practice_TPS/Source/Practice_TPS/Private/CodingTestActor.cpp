// Fill out your copyright notice in the Description page of Project Settings.


#include "CodingTestActor.h"

// Sets default values
ACodingTestActor::ACodingTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACodingTestActor::BeginPlay()
{
	Super::BeginPlay();
	
	// UE_LOG(LogTemp, Warning, TEXT("Hello World!"));
	int32 result = Add(number1, number2);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *name);
	UE_LOG(LogTemp, Warning, TEXT("%d"), result);
}

// Called every frame
void ACodingTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int32 ACodingTestActor::Add(int32 num1, int32 num2)
{
	return num1 + num2;
}

