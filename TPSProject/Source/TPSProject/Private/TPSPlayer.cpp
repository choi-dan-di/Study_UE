// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "Bullet.h"
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>

// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. ���̷�Ż�޽� �����͸� �ҷ����� �ʹ�.
	// �ּ��� ��θ� ���ڷ� �Ѱ��ش�.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		// 2. Mesh ������Ʈ�� ��ġ�� ȸ�� ���� �����ϰ� �ʹ�.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, 0, -90));
	}

	// 3. TPS ī�޶� ���̰� �ʹ�.
	// 3-1. SpringArm ������Ʈ ���̱�
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));
	springArmComp->TargetArmLength = 400;
	springArmComp->bUsePawnControlRotation = true;

	// 3-2. Camera ������Ʈ ���̱�
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);
	tpsCamComp->bUsePawnControlRotation = false;

	// Ŭ���� ����Ʈ ������
	bUseControllerRotationYaw = true;

	// 2�� ����
	JumpMaxCount = 2;

	// 4. �� ���̷�Ż �޽� ������Ʈ ���
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	// 4-1. �θ� ������Ʈ�� Mesh ������Ʈ�� ����
	gunMeshComp->SetupAttachment(GetMesh());
	// 4-2. ���̷�Ż �޽� ������ �ε�
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	// 4-3. ������ �ε尡 �����ߴٸ�
	if (TempGunMesh.Succeeded())
	{
		// 4-4. ���̷�Ż �޽� ������ �Ҵ�
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		// 4-5. ��ġ �����ϱ�
		gunMeshComp->SetRelativeLocation(FVector(-14, 52, 120));
	}

	// 5. �������۰� ������Ʈ ���
	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunComp"));
	// 5-1. �θ� ������Ʈ�� Mesh ������Ʈ�� ����
	sniperGunComp->SetupAttachment(GetMesh());
	// 5-2. �������۰� ����ƽ �޽� ������ �ε�
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("/Script/Engine.StaticMesh'/Game/SniperGun/sniper1.sniper1'"));
	// 5-3. ������ �ε尡 �����ߴٸ�
	if (TempSniperMesh.Succeeded())
	{
		// 5-4. ����ƽ �޽� ������ �Ҵ�
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);
		// 5-5. ��ġ �����ϱ�
		sniperGunComp->SetRelativeLocation(FVector(-22, 55, 120));
		// 5-6. ũ�� �����ϱ�
		sniperGunComp->SetRelativeScale3D(FVector(0.15f));
	}
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 1. �������� UI ���� �ν��Ͻ� ����
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	// 2. �Ϲ� ���� ��� UI ���� �ν��Ͻ� ����
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);

	// 3. �Ϲ� ���� UI ���
	_crosshairUI->AddToViewport();
	
	// �⺻���� �������۰��� ����ϵ��� ����
	ChangeToSniperGun();
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move();
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Turn(), LookUp() ���ε�
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATPSPlayer::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATPSPlayer::LookUp);

	// InputHorizontal(), InputVertical() ���ε�
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATPSPlayer::InputHorizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATPSPlayer::InputVertical);

	// ���� �Է� �̺�Ʈ ���ε�
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATPSPlayer::InputJump);

	// �Ѿ� �߻� �Է� �̺�Ʈ ���ε�
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATPSPlayer::InputFire);

	// �� ��ü �Է� �̺�Ʈ ���ε�
	PlayerInputComponent->BindAction(TEXT("GrenadeGun"), IE_Pressed, this, &ATPSPlayer::ChangeToGrenadeGun);
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &ATPSPlayer::ChangeToSniperGun);

	// �������� ���� �Է� �̺�Ʈ ���ε�
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Pressed, this, &ATPSPlayer::SniperAim);
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Released, this, &ATPSPlayer::SniperAim);
}

void ATPSPlayer::Turn(float value)
{
	AddControllerYawInput(value);
}

void ATPSPlayer::LookUp(float value)
{
	AddControllerPitchInput(value);
}

// �¿� �̵� �Է� �̺�Ʈ ó�� �Լ�
void ATPSPlayer::InputHorizontal(float value)
{
	direction.Y = value;
}

// ���� �̵� �Է� �̺�Ʈ ó�� �Լ�
void ATPSPlayer::InputVertical(float value)
{
	direction.X = value;
}

// ���� �Է� �̺�Ʈ ó�� �Լ�
void ATPSPlayer::InputJump()
{
	Jump();
}

void ATPSPlayer::Move()
{
	// �÷��̾� �̵� ó��
	// ��� �
	// P(��� ��ġ) = P0(���� ��ġ) + v(�ӵ�) * t(�ð�)
	direction = FTransform(GetControlRotation()).TransformVector(direction);
	/*
	FVector P0 = GetActorLocation();
	FVector vt = direction * walkSpeed * DeltaTime;
	FVector P = P0 + vt;
	SetActorLocation(P);
	*/
	AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

void ATPSPlayer::InputFire()
{
	if (bUsingGrenadeGun)
	{
		// �Ѿ� �߻� ó�� (��ź��)
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
	else
	{
		// �������۰� �߻� ó�� (���� Ʈ���̽� ����)
		// ���� Ʈ���̽��� ���� ��ġ
		FVector startPos = tpsCamComp->GetComponentLocation();
		// ���� Ʈ���̽��� ���� ��ġ
		FVector endPos = startPos + (tpsCamComp->GetForwardVector() * 5000);
		// ���� Ʈ���̽��� �浹 ���� ����
		FHitResult hitInfo;
		// �浹 �ɼ� ���� ����
		FCollisionQueryParams params;
		// �ڱ� �ڽ�(�÷��̾�)�� �浹���� ����
		params.AddIgnoredActor(this);

		// Channel ���͸� �̿��� ���� Ʈ���̽� �浹 ����
		// -> (�浹 ����, ���� ��ġ, ���� ��ġ, ���� ä��, �浹 �ɼ�)
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);

		// ���� Ʈ���̽� �浹 ��
		if (bHit)
		{
			// �浹 ó�� -> �Ѿ� ���� ȿ�� ���
			// �Ѿ� ���� ȿ���� ���� ��ġ, ȸ��, ũ�� ������ ������ Ʈ������
			FTransform bulletTrans;
			// Ʈ�������� ��ġ���� ����Ʈ���̽��� �浹�� �������� �Ҵ�
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			// �Ѿ� ���� ȿ�� �ν��Ͻ� ����
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

			// �浹�� ��ü�� ������
			// �ε��� ��ü�� ������Ʈ ������ hitComp�� ����
			auto hitComp = hitInfo.GetComponent();
			// ���� �浹 ������Ʈ�� ������ ����Ǿ� �ִٸ�
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				// ��ü�� ���ư��� �� ������ ���� ����
				FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 500000;
				// �浹 ������Ʈ�� AddForce()�� �� ���ϱ�
				hitComp->AddForce(force);
			}
		}
	}
}

void ATPSPlayer::ChangeToGrenadeGun()
{
	bUsingGrenadeGun = true;
	gunMeshComp->SetVisibility(true);
	sniperGunComp->SetVisibility(false);
}

void ATPSPlayer::ChangeToSniperGun()
{
	bUsingGrenadeGun = false;
	gunMeshComp->SetVisibility(false);
	sniperGunComp->SetVisibility(true);
}

void ATPSPlayer::SniperAim()
{
	// �������۰� ��尡 �ƴ϶�� ó������ �ʴ´�.
	if (bUsingGrenadeGun)
		return;

	// Pressed �Է� ó��
	if (bSniperAim == false)
	{
		// 1. �������� ���� ��� Ȱ��ȭ
		bSniperAim = true;
		// 2. �������� ���� UI ���
		_sniperUI->AddToViewport();
		// 3. ī�޶��� �þ߰� Field Of View ����
		tpsCamComp->SetFieldOfView(45.0f);
		// 4. �Ϲ� ���� ���� UI ����
		_crosshairUI->RemoveFromParent();
	}
	else
	{
		// Released �Է� ó��
		// 1. �������� ���� ��� ��Ȱ��ȭ
		bSniperAim = false;
		// 2. �������� ���� UI ȭ�鿡�� ����
		_sniperUI->RemoveFromParent();
		// 3. ī�޶��� �þ߰� ������� ����
		tpsCamComp->SetFieldOfView(90.0f);
		// 4. �Ϲ� ���� ���� UI ����
		_crosshairUI->AddToViewport();
	}
}

