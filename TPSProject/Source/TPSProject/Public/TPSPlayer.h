// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

UCLASS()
class TPSPROJECT_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* springArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* tpsCamComp;

	// 총 스켈레탈 메시
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* gunMeshComp;

	// 총알 공장
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;

	// 총알 발사 처리 함수
	void InputFire();

	// 스나이퍼건 스태틱 메시
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class UStaticMeshComponent* sniperGunComp;

	// 총 교체
	// 유탄총 사용 중인지 여부
	bool bUsingGrenadeGun = true;
	// 유탄총으로 변경
	void ChangeToGrenadeGun();
	// 스나이퍼건으로 변경
	void ChangeToSniperGun();

	// 스나이퍼 조준 모드
	void SniperAim();
	// 스나이퍼 조준 중인지 여부
	bool bSniperAim = false;

	// 위젯 UI 담을 인스턴스 추가
	// 스나이퍼 UI 위젯 공장
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> sniperUIFactory;
	// 스나이퍼 UI 위젯 인스턴스
	class UUserWidget* _sniperUI;

	// 총알 파편 이펙트 추가
	UPROPERTY(EditAnywhere, Category = BulletEffect)
	class UParticleSystem* bulletEffectFactory;

	// 일반 조준 모드 UI 추가
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> crosshairUIFactory;
	// 이로부터 만들어질 인스턴스 저장
	class UUserWidget* _crosshairUI;

	

	// 카메라 셰이크 블루프린트를 저장할 변수
	UPROPERTY(EditDefaultsOnly, Category = CameraMotion)
	TSubclassOf<class UCameraShakeBase> cameraShake;

	// 총알 발사 사운드
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* bulletSound;

public:
	UPROPERTY(VisibleAnywhere, Category = Component)
	class UPlayerBaseComponent* playerMove;
};
