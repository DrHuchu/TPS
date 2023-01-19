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

	//me�� ã�� �ʹ�.
	me = Cast<AEnemy>(GetOwner());

	//���� ü���� �ִ� ü������ �ϰ�ʹ�.
	hp = maxHP;

	// ...
	
}

//���, �÷��̾ ã���� �̵����� ����
void UEnemyFSM::TickIdle()
{
	//1. �÷��̾ ã�� �ʹ�.
	target = Cast<ATPSPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	//2. ���� �÷��̾ ã������	

	if(target != nullptr)

	//3. �̵����� �����ϰ� �ʹ�.
	{
		state = EEnemyState::MOVE;
	}
}

void UEnemyFSM::TickMove()
{
	//1. �������� ���ϴ� ������ �����
	FVector dir = target->GetActorLocation() - me->GetActorLocation();

	//2. �� �������� �̵��ϰ�
	me->AddMovementInput(dir.GetSafeNormal());

	//3. ���������� �Ÿ��� ���ݰ��� �Ÿ����

	//�� ������ �Ÿ��� ���ϴ� ��� 3����
	//float dist = target->GetDistanceTo(me);
	float dist = dir.Size();
	//float dist = FVector::Dist(target->GetActorLocation(), me->GetActorLocation());

	if(dist <= attackRange)

	//4. ���ݻ��·� �����ϰ� �ʹ�.
	{
		state = EEnemyState::ATTACK;
	}
}

void UEnemyFSM::TickAttack()
{
	//1. �ð��� �帣�ٰ�
	currentTime += GetWorld()->GetDeltaSeconds();

	//2. ���� �ð��� ���ݽð��� �ʰ��ϸ�
	if(false == bAttackPlay && currentTime > 0.1f)
	{
		bAttackPlay = true;
		//3. ������ �ϰ� (������ ���ݰŸ� �ȿ� �ִ°�?)
		float dist = target->GetDistanceTo(me);
		if(dist <= attackRange)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy is Attack"));
		}
		

	}
	//4. ���ݵ����� �����ٸ�
	if(currentTime > 2)
	//5. ��� ������ �� ������ �Ǵ��ϰ� �ʹ�.
	{
		float dist = target->GetDistanceTo(me);
		//���ݰŸ����� �־����ٸ�(�������ٸ�)
		if(dist > attackRange)
		{
			//�̵����·� �����ϰ� �ʹ�.
			state = EEnemyState::MOVE;

		}
		//���ݰŸ� �ȿ� �ִٸ� �ٽ� �����ϰ� �ʹ�.
		else
		{
			currentTime = 0;
			bAttackPlay = false;
			
		}
		
	}
}

//player -> enemy ����
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
	//ü���� �Ҹ��ϰ�
	hp -= damageValue;
	//ü���� 0�̵Ǹ�
	if(hp <= 0)

	//�״´�.
	{
		state = EEnemyState::DIE;

		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		me->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	//�׷��� �ʴٸ�
	else

	//damage�ϰ� �ʹ�.
	{
		state = EEnemyState::DAMAGE;
	}

}

