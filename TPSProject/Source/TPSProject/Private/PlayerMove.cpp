// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMove.h"

UPlayerMove::UPlayerMove()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerMove::BeginPlay()
{
	Super::BeginPlay();

	// 초기 속도를 걷기로 설정
	moveComp->MaxWalkSpeed = walkSpeed;
}

void UPlayerMove::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Move();
}

void UPlayerMove::Turn(float value)
{
	me->AddControllerYawInput(value);
}

void UPlayerMove::LookUp(float value)
{
	me->AddControllerPitchInput(value);
}

void UPlayerMove::SetupInputBinding(UInputComponent* PlayerInputComponent)
{
	// Turn(), LookUp() 바인딩
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &UPlayerMove::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &UPlayerMove::LookUp);

	// InputHorizontal(), InputVertical() 바인딩
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &UPlayerMove::InputHorizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &UPlayerMove::InputVertical);

	// 점프 입력 이벤트 바인딩
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &UPlayerMove::InputJump);

	// 달리기 이벤트 바인딩
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &UPlayerMove::InputRun);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &UPlayerMove::InputRun);
}

// 좌우 이동 입력 이벤트 처리 함수
void UPlayerMove::InputHorizontal(float value)
{
	direction.Y = value;
}

// 상하 이동 입력 이벤트 처리 함수
void UPlayerMove::InputVertical(float value)
{
	direction.X = value;
}

// 점프 입력 이벤트 처리 함수
void UPlayerMove::InputJump()
{
	me->Jump();
}

void UPlayerMove::Move()
{
	// 플레이어 이동 처리
	// 등속 운동
	// P(결과 위치) = P0(현재 위치) + v(속도) * t(시간)
	direction = FTransform(me->GetControlRotation()).TransformVector(direction);
	/*
	FVector P0 = GetActorLocation();
	FVector vt = direction * walkSpeed * DeltaTime;
	FVector P = P0 + vt;
	SetActorLocation(P);
	*/
	me->AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

void UPlayerMove::InputRun()
{
	auto movement = moveComp;
	// 현재 달리기 모드라면
	if (movement->MaxWalkSpeed > walkSpeed)
	{
		// 걷기 속도로 전환
		movement->MaxWalkSpeed = walkSpeed;
	}
	else
	{
		movement->MaxWalkSpeed = runSpeed;
	}
}

