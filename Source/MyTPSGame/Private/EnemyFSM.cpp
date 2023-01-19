// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"

#include "TPSPlayer.h"
#include "Enemy.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

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

	state = EEnemyState::IDLE;

	//me를 찾고 싶다.
	me = Cast<AEnemy>(GetOwner());

	//현재 체력을 최대 체력으로 하고싶다.
	hp = maxHP;

	// ...
	
}

//대기, 플레이어를 찾으면 이동으로 전이
void UEnemyFSM::TickIdle()
{
	//1. 플레이어를 찾고 싶다.
	target = Cast<ATPSPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	//2. 만약 플레이어를 찾았으면	

	if(target != nullptr)

	//3. 이동으로 전이하고 싶다.
	{
		state = EEnemyState::MOVE;
	}
}

void UEnemyFSM::TickMove()
{
	//1. 목적지를 향하는 방향을 만들고
	FVector dir = target->GetActorLocation() - me->GetActorLocation();

	//2. 그 방향으로 이동하고
	me->AddMovementInput(dir.GetSafeNormal());

	//3. 목적지와의 거리가 공격가능 거리라면

	//둘 사이의 거리를 구하는 방법 3가지
	//float dist = target->GetDistanceTo(me);
	float dist = dir.Size();
	//float dist = FVector::Dist(target->GetActorLocation(), me->GetActorLocation());

	if(dist <= attackRange)

	//4. 공격상태로 전이하고 싶다.
	{
		state = EEnemyState::ATTACK;
	}
}

void UEnemyFSM::TickAttack()
{
	//1. 시간이 흐르다가
	currentTime += GetWorld()->GetDeltaSeconds();

	//2. 현재 시간이 공격시간을 초과하면
	if(false == bAttackPlay && currentTime > 0.1f)
	{
		bAttackPlay = true;
		//3. 공격을 하고 (조건은 공격거리 안에 있는가?)
		float dist = target->GetDistanceTo(me);
		if(dist <= attackRange)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy is Attack"));
		}
		

	}
	//4. 공격동작이 끝나다면
	if(currentTime > 2)
	//5. 계속 공격을 할 것인지 판단하고 싶다.
	{
		float dist = target->GetDistanceTo(me);
		//공격거리보다 멀어졌다면(도망갔다면)
		if(dist > attackRange)
		{
			//이동상태로 전이하고 싶다.
			state = EEnemyState::MOVE;

		}
		//공격거리 안에 있다면 다시 공격하고 싶다.
		else
		{
			currentTime = 0;
			bAttackPlay = false;
			
		}
		
	}
}

//player -> enemy 공격
void UEnemyFSM::TickDamage()
{
	currentTime += GetWorld()->GetDeltaSeconds();
	if(currentTime >1)
	{
		state = EEnemyState::MOVE;
		currentTime = 0;
	}
}

void UEnemyFSM::TickDie()
{
	currentTime += GetWorld()->GetDeltaSeconds();
	//P = P0 + vt

	me->SetActorLocation(me->GetActorLocation() + 200 * FVector(0, 0, -1) * GetWorld()->GetDeltaSeconds());
	if (currentTime > 1)
	{
		

		me->Destroy();
	}
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	switch (state)
	{
	case EEnemyState::IDLE:
		TickIdle();
		break;
	case EEnemyState::MOVE:
		TickMove();
		break;
	case EEnemyState::ATTACK:
		TickAttack();
		break;
	case EEnemyState::DAMAGE:
		TickDamage();
		break;
	case EEnemyState::DIE:
		TickDie();
		break;
	}
}

void UEnemyFSM::OnDamageProcess(int damageValue)
{
	//체력을 소모하고
	hp -= damageValue;
	//체력이 0이되면
	if(hp <= 0)

	//죽는다.
	{
		state = EEnemyState::DIE;

		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		me->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	//그렇지 않다면
	else

	//damage하고 싶다.
	{
		state = EEnemyState::DAMAGE;
	}

}

