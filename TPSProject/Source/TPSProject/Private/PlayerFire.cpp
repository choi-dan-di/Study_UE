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
	// �Ѿ� ���� ��������
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

	// 1. �������� UI ���� �ν��Ͻ� ����
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	// 2. �Ϲ� ���� ��� UI ���� �ν��Ͻ� ����
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);

	// 3. �Ϲ� ���� UI ���
	_crosshairUI->AddToViewport();

	// �⺻���� �������۰��� ����ϵ��� ����
	ChangeToSniperGun();
}

void UPlayerFire::SetupInputBinding(class UInputComponent* PlayerInputComponent)
{
	// �Ѿ� �߻� �Է� �̺�Ʈ ���ε�
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &UPlayerFire::InputFire);

	// �� ��ü �Է� �̺�Ʈ ���ε�
	PlayerInputComponent->BindAction(TEXT("GrenadeGun"), IE_Pressed, this, &UPlayerFire::ChangeToGrenadeGun);
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &UPlayerFire::ChangeToSniperGun);

	// �������� ���� �Է� �̺�Ʈ ���ε�
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Pressed, this, &UPlayerFire::SniperAim);
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Released, this, &UPlayerFire::SniperAim);
}

void UPlayerFire::InputFire()
{
	// �Ѿ� �߻� ���� ���
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);

	// ī�޶� ����ũ ���
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);

	// ���� �ִϸ��̼� ���
	auto anim = Cast<UPlayerAnim>(me->GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();

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
		params.AddIgnoredActor(me);

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

			// �ε��� ����� ������ �Ǵ��ϱ�
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

	// ��ź�� ������� ���� ����
	me->OnUsingGrenade(bUsingGrenadeGun);
}

void UPlayerFire::ChangeToSniperGun()
{
	bUsingGrenadeGun = false;
	gunMeshComp->SetVisibility(false);
	sniperGunComp->SetVisibility(true);

	// ��ź�� ������� ���� ����
	me->OnUsingGrenade(bUsingGrenadeGun);
}

void UPlayerFire::SniperAim()
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