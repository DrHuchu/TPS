// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8 //0~255������ ���ڴ�
{
	IDLE,
	MOVE,
	ATTACK,
	DAMAGE,
	DIE,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYTPSGAME_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void TickIdle();
	void TickMove();
	void TickAttack();
	void TickDamage();
	void TickDie();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	EEnemyState state;

	class ATPSPlayer* target;

	class AEnemy* me;

	//���ݰ��ɰŸ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float attackRange = 200;

	float currentTime = 0;
	bool bAttackPlay;

	int hp;
	int maxHP = 2;

	//��� ���Ͽ����� ���� �̸� �����ص� ��
	void OnDamageProcess(int);
};
