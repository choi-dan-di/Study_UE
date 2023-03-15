// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"
#include "TPSPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>

void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 플레이어의 이동 속도를 가져와 speed에 할당하고 싶다.
	// 소유 폰 얻어와서 플레이어로 캐스팅하기
	ATPSPlayer* player = Cast<ATPSPlayer>(TryGetPawnOwner());
	// 캐스팅 성공
	if (player != nullptr)
	{
		// 이동 속도 필요
		FVector velocity = player->GetVelocity();
		// 플레이어의 전방 벡터 필요
		FVector forwardVector = player->GetActorForwardVector();
		// speed에 값(내적) 할당하기
		speed = FVector::DotProduct(forwardVector, velocity);
		// 좌우 속도 할당하기
		FVector rightVector = player->GetActorRightVector();
		direction = FVector::DotProduct(rightVector, velocity);

		// 플레이어가 현재 공중에 있는지 여부를 기억하고 싶다.
		auto movement = player->GetCharacterMovement();
		isInAir = movement->IsFalling();
	}
}

void UPlayerAnim::PlayAttackAnim()
{
	Montage_Play(attackAnimMontage);
}
