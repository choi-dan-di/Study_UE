// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Bullet.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// �ڽ� �ݶ��̴� ������Ʈ ����
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("My Box Component"));

	// ������ �ڽ� �ݶ��̴� ������Ʈ�� �ֻ�� ������Ʈ�� ����
	SetRootComponent(boxComp);

	// ����ƽ �޽� ������Ʈ ����
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("My Static Mesh"));

	// �ڽ� �ݶ��̴� ������Ʈ�� �ڽ� ������Ʈ�� ����
	meshComp->SetupAttachment(boxComp);

	// �ڽ� �ݶ��̴��� ũ�⸦ 50 x 50 x 50���� ����
	FVector boxSize = FVector(50.0f, 50.0f, 50.0f);
	boxComp->SetBoxExtent(boxSize);

	// �ѱ� ǥ�� ������Ʈ�� �����ϰ� �ڽ� ������Ʈ�� �ڽ� ������Ʈ�� ����
	firePosition = CreateDefaultSubobject<UArrowComponent>(TEXT("Fire Position"));
	firePosition->SetupAttachment(boxComp);
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ������� �Է� Ű�� �̿��ؼ�
	// 1. ���� �Է� ���� �¿� �Է� ���� �̿��ؼ� ���� ���͸� ����
	FVector dir = FVector(0, h, v);

	// 2. ���� ������ ���̰� 1�� �ǵ��� ���͸� ����ȭ�Ѵ�.
	dir.Normalize();

	// 3. �̵��� ��ġ ��ǥ�� ���Ѵ�(p = p0 + vt)
	FVector newLocation = GetActorLocation() + dir * moveSpeed * DeltaTime;

	// 4. ���� ������ ��ġ ��ǥ�� �տ��� ���� �� ��ǥ�� ����
	SetActorLocation(newLocation);
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Axis ���ε� �� ���� �Լ��� �����Ѵ�.
	PlayerInputComponent->BindAxis("Horizontal", this, &APlayerPawn::MoveHorizontal);
	PlayerInputComponent->BindAxis("Vertical", this, &APlayerPawn::MoveVertical);

	// Action ���ε� �� ���� �Լ��� �����Ѵ�.
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerPawn::Fire);
}

// �¿��� �Է� ó�� �Լ�
void APlayerPawn::MoveHorizontal(float value)
{
	h = value;
}

// ������ �Է� ó�� �Լ�
void APlayerPawn::MoveVertical(float value)
{
	v = value;
}

// ���콺 ���� ��ư �Է� ó�� �Լ�
void APlayerPawn::Fire()
{
	// �Ѿ� �������Ʈ ������ firePosition ��ġ�� ����
	ABullet* bullet = GetWorld()->SpawnActor<ABullet>(
		bulletFactory, 
		firePosition->GetComponentLocation(), 
		firePosition->GetComponentRotation()
	);

	// fireSound ������ �Ҵ�� ���� ������ ����
	UGameplayStatics::PlaySound2D(GetWorld(), fireSound);
}

