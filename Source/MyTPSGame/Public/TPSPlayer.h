// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

UCLASS()
class MYTPSGAME_API ATPSPlayer : public ACharacter
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

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* springArmComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UCameraComponent* cameraComp;

		void OnAxisHorizontal(float value);
		void OnAxisVertical(float value);
		void OnAxisLookUp(float value);
		void OnAxisTurnRight(float value);
		void OnActionJump();
		void OnActionFirePressed();
		void OnActionFireReleased();

		void DoFire();

		//FVector direction;

		FVector direction;

	//���콺 ���� ��ư�� ������ �Ѿ˰��忡�� �Ѿ��� ���� �ѱ� ��ġ�� ��ġ�ϰ� �ʹ�.
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ABulletActor> bulletFactory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float walkSpeed = 600;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class USkeletalMeshComponent* gunMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* sniperMeshComp;

	UPROPERTY(EditAnywhere)
	float fireInterval = 0.1f;

	FTimerHandle fireTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bChooseGrenadeGun;
	void ChooseGun(bool bGrenade);

	//������ �����ϰ� �ʹ�. Crosshair, Sniper
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> crosshairFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> sniperFactory;

	UPROPERTY()
		class UUserWidget* crosshairUI;

	UPROPERTY()
		class UUserWidget* sniperUI;

	//1��Ű�� 2��Ű�� ���� �Է� ó���� �ϰ� �ʹ�.
	UFUNCTION()
	void OnActionGrenade();
	UFUNCTION()
	void OnActionSniper();

	//Zoom�� �����ϰ� �ʹ� In/Out
	void OnActionZoomIn(); //Ȯ�� FOV 30
 	void OnActionZoomOut(); //��� FOV 90

	UPROPERTY(EditAnywhere)
		class UParticleSystem* bulletImpactFactory;
};


