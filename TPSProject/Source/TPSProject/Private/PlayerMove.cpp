// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMove.h"

UPlayerMove::UPlayerMove()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerMove::BeginPlay()
{
	Super::BeginPlay();

	// �ʱ� �ӵ��� �ȱ�� ����
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
	// Turn(), LookUp() ���ε�
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &UPlayerMove::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &UPlayerMove::LookUp);

	// InputHorizontal(), InputVertical() ���ε�
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &UPlayerMove::InputHorizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &UPlayerMove::InputVertical);

	// ���� �Է� �̺�Ʈ ���ε�
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &UPlayerMove::InputJump);

	// �޸��� �̺�Ʈ ���ε�
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &UPlayerMove::InputRun);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &UPlayerMove::InputRun);
}

// �¿� �̵� �Է� �̺�Ʈ ó�� �Լ�
void UPlayerMove::InputHorizontal(float value)
{
	direction.Y = value;
}

// ���� �̵� �Է� �̺�Ʈ ó�� �Լ�
void UPlayerMove::InputVertical(float value)
{
	direction.X = value;
}

// ���� �Է� �̺�Ʈ ó�� �Լ�
void UPlayerMove::InputJump()
{
	me->Jump();
}

void UPlayerMove::Move()
{
	// �÷��̾� �̵� ó��
	// ��� �
	// P(��� ��ġ) = P0(���� ��ġ) + v(�ӵ�) * t(�ð�)
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
	// ���� �޸��� �����
	if (movement->MaxWalkSpeed > walkSpeed)
	{
		// �ȱ� �ӵ��� ��ȯ
		movement->MaxWalkSpeed = walkSpeed;
	}
	else
	{
		movement->MaxWalkSpeed = runSpeed;
	}
}

