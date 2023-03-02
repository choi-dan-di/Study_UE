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

	// 1. 스켈레탈메시 데이터를 불러오고 싶다.
	// 애셋의 경로를 인자로 넘겨준다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		// 2. Mesh 컴포넌트의 위치와 회전 값을 설정하고 싶다.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, 0, -90));
	}

	// 3. TPS 카메라를 붙이고 싶다.
	// 3-1. SpringArm 컴포넌트 붙이기
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));
	springArmComp->TargetArmLength = 400;
	springArmComp->bUsePawnControlRotation = true;

	// 3-2. Camera 컴포넌트 붙이기
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);
	tpsCamComp->bUsePawnControlRotation = false;

	// 클래스 디폴트 설정값
	bUseControllerRotationYaw = true;

	// 2단 점프
	JumpMaxCount = 2;

	// 4. 총 스켈레탈 메시 컴포넌트 등록
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	// 4-1. 부모 컴포넌트를 Mesh 컴포넌트로 설정
	gunMeshComp->SetupAttachment(GetMesh());
	// 4-2. 스켈레탈 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	// 4-3. 데이터 로드가 성공했다면
	if (TempGunMesh.Succeeded())
	{
		// 4-4. 스켈레탈 메시 데이터 할당
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		// 4-5. 위치 조정하기
		gunMeshComp->SetRelativeLocation(FVector(-14, 52, 120));
	}

	// 5. 스나이퍼건 컴포넌트 등록
	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunComp"));
	// 5-1. 부모 컴포넌트를 Mesh 컴포넌트로 설정
	sniperGunComp->SetupAttachment(GetMesh());
	// 5-2. 스나이퍼건 스태틱 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("/Script/Engine.StaticMesh'/Game/SniperGun/sniper1.sniper1'"));
	// 5-3. 데이터 로드가 성공했다면
	if (TempSniperMesh.Succeeded())
	{
		// 5-4. 스태틱 메시 데이터 할당
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);
		// 5-5. 위치 조정하기
		sniperGunComp->SetRelativeLocation(FVector(-22, 55, 120));
		// 5-6. 크기 조정하기
		sniperGunComp->SetRelativeScale3D(FVector(0.15f));
	}
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 1. 스나이퍼 UI 위젯 인스턴스 생성
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	// 2. 일반 조준 모드 UI 위젯 인스턴스 생성
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);

	// 3. 일반 조준 UI 등록
	_crosshairUI->AddToViewport();
	
	// 기본으로 스나이퍼건을 사용하도록 설정
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

	// Turn(), LookUp() 바인딩
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATPSPlayer::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATPSPlayer::LookUp);

	// InputHorizontal(), InputVertical() 바인딩
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATPSPlayer::InputHorizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATPSPlayer::InputVertical);

	// 점프 입력 이벤트 바인딩
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATPSPlayer::InputJump);

	// 총알 발사 입력 이벤트 바인딩
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATPSPlayer::InputFire);

	// 총 교체 입력 이벤트 바인딩
	PlayerInputComponent->BindAction(TEXT("GrenadeGun"), IE_Pressed, this, &ATPSPlayer::ChangeToGrenadeGun);
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &ATPSPlayer::ChangeToSniperGun);

	// 스나이퍼 조준 입력 이벤트 바인딩
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

// 좌우 이동 입력 이벤트 처리 함수
void ATPSPlayer::InputHorizontal(float value)
{
	direction.Y = value;
}

// 상하 이동 입력 이벤트 처리 함수
void ATPSPlayer::InputVertical(float value)
{
	direction.X = value;
}

// 점프 입력 이벤트 처리 함수
void ATPSPlayer::InputJump()
{
	Jump();
}

void ATPSPlayer::Move()
{
	// 플레이어 이동 처리
	// 등속 운동
	// P(결과 위치) = P0(현재 위치) + v(속도) * t(시간)
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
		// 총알 발사 처리 (유탄총)
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
	else
	{
		// 스나이퍼건 발사 처리 (라인 트레이스 적용)
		// 라인 트레이스의 시작 위치
		FVector startPos = tpsCamComp->GetComponentLocation();
		// 라인 트레이스의 종료 위치
		FVector endPos = startPos + (tpsCamComp->GetForwardVector() * 5000);
		// 라인 트레이스에 충돌 정보 저장
		FHitResult hitInfo;
		// 충돌 옵션 설정 변수
		FCollisionQueryParams params;
		// 자기 자신(플레이어)은 충돌에서 제외
		params.AddIgnoredActor(this);

		// Channel 필터를 이용한 라인 트레이스 충돌 검출
		// -> (충돌 정보, 시작 위치, 종료 위치, 검출 채널, 충돌 옵션)
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);

		// 라인 트레이스 충돌 시
		if (bHit)
		{
			// 충돌 처리 -> 총알 파편 효과 재생
			// 총알 파편 효과가 놓일 위치, 회전, 크기 정보를 저장할 트랜스폼
			FTransform bulletTrans;
			// 트랜스폼의 위치값을 라인트레이스가 충돌한 지점으로 할당
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			// 총알 파편 효과 인스턴스 생성
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

			// 충돌한 물체를 날리기
			// 부딪힌 물체의 컴포넌트 변수를 hitComp에 저장
			auto hitComp = hitInfo.GetComponent();
			// 만약 충돌 컴포넌트에 물리가 적용되어 있다면
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				// 물체가 날아가야 할 방향의 단위 벡터
				FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 500000;
				// 충돌 컴포넌트의 AddForce()로 힘 가하기
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
	// 스나이퍼건 모드가 아니라면 처리하지 않는다.
	if (bUsingGrenadeGun)
		return;

	// Pressed 입력 처리
	if (bSniperAim == false)
	{
		// 1. 스나이퍼 조준 모드 활성화
		bSniperAim = true;
		// 2. 스나이퍼 조준 UI 등록
		_sniperUI->AddToViewport();
		// 3. 카메라의 시야각 Field Of View 설정
		tpsCamComp->SetFieldOfView(45.0f);
		// 4. 일반 조준 위젯 UI 제거
		_crosshairUI->RemoveFromParent();
	}
	else
	{
		// Released 입력 처리
		// 1. 스나이퍼 조준 모드 비활성화
		bSniperAim = false;
		// 2. 스나이퍼 조준 UI 화면에서 제거
		_sniperUI->RemoveFromParent();
		// 3. 카메라의 시야각 원래대로 복원
		tpsCamComp->SetFieldOfView(90.0f);
		// 4. 일반 조준 위젯 UI 복원
		_crosshairUI->AddToViewport();
	}
}

