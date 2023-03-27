// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "TPSPlayer.h"
#include "Enemy.h"
#include <Kismet/GameplayStatics.h>
#include "TPSProject.h"
#include <Components/CapsuleComponent.h>
#include "EnemyAnim.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// Ÿ��� �� ������ �Ҵ����ش�.
	// ���忡�� ATPSPlayer Ÿ�� ã�ƿ���
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
	// ATPSPlayer Ÿ������ ĳ����
	target = Cast<ATPSPlayer>(actor);
	// �� ���� ã�ƿ���
	me = Cast<AEnemy>(GetOwner());

	// UEnemyAnim* �Ҵ�
	anim = Cast<UEnemyAnim>(me->GetMesh()->GetAnimInstance());
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (mState)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	}
}

void UEnemyFSM::IdleState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	// ���� �ð� ��ٷȴٰ� �̵� ���·� ��ȯ
	if (currentTime > idleDelayTime)
	{
		mState = EEnemyState::Move;
		currentTime = 0;

		// �ִϸ��̼� ���� ����ȭ
		anim->animState = mState;
	}
}

void UEnemyFSM::MoveState()
{
	// 1. Ÿ�� �������� �ʿ��ϴ�.
	FVector destination = target->GetActorLocation();
	// 2. ������ �ʿ��ϴ�.
	// Ÿ�� - ���� ��ġ
	FVector dir = destination - me->GetActorLocation();
	// 3. �������� �̵��ϰ� �ʹ�.
	me->AddMovementInput(dir.GetSafeNormal());

	// Ÿ��� ��������� ���� ���·� ��ȯ�ϰ� �ʹ�.
	// ���� �Ÿ��� ���� ���� �ȿ� ������
	if (dir.Size() < attackRange)
	{
		// ���� ���·� ��ȯ
		mState = EEnemyState::Attack;
		// �ִϸ��̼� ���� ����ȭ
		anim->animState = mState;
		// ���� �ִϸ��̼� ��� Ȱ��ȭ
		anim->bAttackPlay = true;
		// ���� ���� ��ȯ �� ��� �ð��� �ٷ� �������� ó��
		currentTime = attackDelayTime;
	}
}

void UEnemyFSM::AttackState()
{
	// ���� �ð��� �� ���� �����ϰ� �ʹ�.
	// 1. �ð��� �귯�� �Ѵ�.
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. ���� �ð��� �����ϱ�
	if (currentTime > attackDelayTime)
	{
		// 3. �����ϰ� �ʹ�.
		PRINT_LOG(TEXT("Attack!!!"));
		// ��� �ð� �ʱ�ȭ
		currentTime = 0;
		anim->bAttackPlay = true;
	}

	// Ÿ���� ���� ������ ����� ���¸� �̵����� ��ȯ�ϰ� �ʹ�.
	// 1. Ÿ����� �Ÿ��� �ʿ��ϴ�.
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
	// 2. Ÿ����� �Ÿ��� ���� ������ �����
	if (distance > attackRange)
	{
		// �̵����� ��ȯ
		mState = EEnemyState::Move;
		// �ִϸ��̼� ���� ����ȭ
		anim->animState = mState;
	}
}

void UEnemyFSM::DamageState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > damageDelayTime)
	{
		// ��� ���·� ��ȯ
		mState = EEnemyState::Idle;
		currentTime = 0;
		// �ִϸ��̼� ���� ����ȭ
		anim->animState = mState;
	}
}

void UEnemyFSM::DieState()
{
	// ���� ���� �ִϸ��̼��� ������ �ʾҴٸ� �ٴ� �������� �ʵ��� ó��
	if (!anim->bDieDone)
		return;

	// ��� �Ʒ��� �������� �ʹ�.
	// ��ӿ����
	FVector P0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = P0 + vt;
	me->SetActorLocation(P);

	// ���� 2���� �̻� �����Դٸ�
	if (P.Z < -200.0f)
	{
		// ���Ž�Ų��.
		me->Destroy();
	}
}

// �ǰ� �˸� �̺�Ʈ �Լ�
void UEnemyFSM::OnDamageProcess()
{
	// ü�� ����
	hp--;
	// ü���� ����������
	if (hp > 0)
	{
		// ���¸� �ǰ����� ��ȯ
		mState = EEnemyState::Damage;

		currentTime = 0;

		// �ǰ� �ִϸ��̼� ���
		int32 index = FMath::RandRange(0, 1);
		FString sectionName = FString::Printf(TEXT("Damage%d"), 0);
		anim->PlayDamageAnim(FName(*sectionName));
	}
	else
	{
		// ���¸� �������� ��ȯ
		mState = EEnemyState::Die;
		// ĸ�� �浹ü ��Ȱ��ȭ
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// ���� �ִϸ��̼� ���
		anim->PlayDamageAnim(TEXT("Die"));
	}
	// �ִϸ��̼� ���� ����ȭ
	anim->animState = mState;
}

