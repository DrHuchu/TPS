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

	//마우스 왼쪽 버튼을 누르면 총알공장에서 총알을 만들어서 총구 위치에 배치하고 싶다.
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

	//위젯을 생성하고 싶다. Crosshair, Sniper
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> crosshairFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> sniperFactory;

	UPROPERTY()
		class UUserWidget* crosshairUI;

	UPROPERTY()
		class UUserWidget* sniperUI;

	//1번키와 2번키에 대한 입력 처리를 하고 싶다.
	UFUNCTION()
	void OnActionGrenade();
	UFUNCTION()
	void OnActionSniper();

	//Zoom을 구현하고 싶다 In/Out
	void OnActionZoomIn(); //확대 FOV 30
 	void OnActionZoomOut(); //축소 FOV 90

	UPROPERTY(EditAnywhere)
		class UParticleSystem* bulletImpactFactory;
};


