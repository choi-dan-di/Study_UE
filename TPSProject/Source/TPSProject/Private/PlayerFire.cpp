// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerFire.h"
#include "EnemyFSM.h"
#include <Kismet/GameplayStatics.h>
#include "Bullet.h"
#include <Blueprint/UserWidget.h>
#include <Camera/CameraComponent.h>
#include "PlayerAnim.h"

UPlayerFire::UPlayerFire()
{
	// 총알 사운드 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("/Script/Engine.SoundWave'/Game/SniperGun/Rifle.Rifle'"));
	if (tempSound.Succeeded())
		bulletSound = tempSound.Object;
}

void UPlayerFire::BeginPlay()
{
	Super::BeginPlay();

	tpsCamComp = me->tpsCamComp;
	gunMeshComp = me->gunMeshComp;
	sniperGunComp = me->sniperGunComp;

	// 1. 스나이퍼 UI 위젯 인스턴스 생성
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	// 2. 일반 조준 모드 UI 위젯 인스턴스 생성
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);

	// 3. 일반 조준 UI 등록
	_crosshairUI->AddToViewport();

	// 기본으로 스나이퍼건을 사용하도록 설정
	ChangeToSniperGun();
}

void UPlayerFire::SetupInputBinding(class UInputComponent* PlayerInputComponent)
{
	// 총알 발사 입력 이벤트 바인딩
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &UPlayerFire::InputFire);

	// 총 교체 입력 이벤트 바인딩
	PlayerInputComponent->BindAction(TEXT("GrenadeGun"), IE_Pressed, this, &UPlayerFire::ChangeToGrenadeGun);
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &UPlayerFire::ChangeToSniperGun);

	// 스나이퍼 조준 입력 이벤트 바인딩
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Pressed, this, &UPlayerFire::SniperAim);
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Released, this, &UPlayerFire::SniperAim);
}

void UPlayerFire::InputFire()
{
	// 총알 발사 사운드 재생
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);

	// 카메라 셰이크 재생
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);

	// 공격 애니메이션 재생
	auto anim = Cast<UPlayerAnim>(me->GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();

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
		params.AddIgnoredActor(me);

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

			// 부딪힌 대상이 적인지 판단하기
			auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
			if (enemy)
			{
				auto enemyFSM = Cast<UEnemyFSM>(enemy);
				enemyFSM->OnDamageProcess();
			}
		}
	}
}

void UPlayerFire::ChangeToGrenadeGun()
{
	bUsingGrenadeGun = true;
	gunMeshComp->SetVisibility(true);
	sniperGunComp->SetVisibility(false);

	// 유탄총 사용할지 여부 전달
	me->OnUsingGrenade(bUsingGrenadeGun);
}

void UPlayerFire::ChangeToSniperGun()
{
	bUsingGrenadeGun = false;
	gunMeshComp->SetVisibility(false);
	sniperGunComp->SetVisibility(true);

	// 유탄총 사용할지 여부 전달
	me->OnUsingGrenade(bUsingGrenadeGun);
}

void UPlayerFire::SniperAim()
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