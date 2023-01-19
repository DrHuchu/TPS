// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"

#include "BulletActor.h"
#include "Enemy.h"
#include "EnemyFSM.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//sudo code �ǻ��ڵ� => �˰���
	//1. �ܰ��� �ش��ϴ� ������ �о���� �ʹ�.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));
	//2. �о�� �� �ִٸ�
	if(tempMesh.Succeeded())
	{
	//3. Mesh�� �����ϰ� �ʹ�.
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	//4. Transform�� �����ϰ� �ʹ�.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	//��������, ī�޶� ������Ʈ�� �����ϰ� �ʹ�.
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArmComp"));
	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComp"));
	//������ ���� ��Ʈ�� ������
	springArmComp->SetupAttachment(RootComponent);
	//ī�޶�� ������ �Ͽ� ������ �ʹ�.
	cameraComp->SetupAttachment(springArmComp);
	//���������� ��ġ ����
	springArmComp->SetRelativeLocation(FVector(120, 50, 100));
	springArmComp->TargetArmLength = 250;

	//�Է°��� ȸ���� �ݿ��ϰ� �ʹ�.
	bUseControllerRotationYaw = true;
	springArmComp->bUsePawnControlRotation = true;
	cameraComp->bUsePawnControlRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//�Ϲ� ���� ������Ʈ�� ����� �ʹ�.
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("gunMeshComp"));

	gunMeshComp->SetupAttachment(GetMesh());
	//�Ϲ� ���� ������ �о ������Ʈ�� �ְ� �ʹ�.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));

	if(tempGunMesh.Succeeded()) 
	{
		gunMeshComp->SetSkeletalMesh(tempGunMesh.Object);
	}
	gunMeshComp->SetRelativeLocationAndRotation(FVector(0, 50, 130), FRotator(0,0,0));

	sniperMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperMeshComp"));
	sniperMeshComp->SetupAttachment(GetMesh());

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempSniperMesh(TEXT("/Script/Engine.StaticMesh'/Game/SniperGUn/sniper1.sniper1'"));

	if(tempSniperMesh.Succeeded())
	{
		sniperMeshComp->SetStaticMesh(tempSniperMesh.Object);
		sniperMeshComp->SetRelativeLocationAndRotation(FVector(0, 60, 140), FRotator());
		sniperMeshComp->SetRelativeScale3D(FVector(0.15f));
	}
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	//UI�� �����ϰ� �ʹ�.
	crosshairUI = CreateWidget(GetWorld(), crosshairFactory);
	sniperUI = CreateWidget(GetWorld(), sniperFactory);

	//crosshairUI�� ȭ�鿡 ǥ���ϰ� �ʹ�.
	crosshairUI->AddToViewport();

	ChooseGun(true);

	//1. �¾ �� cui�� ���̰��ϰ� �ʹ�.


	//2. �������� �� �϶� Zoom In�� �ϸ� cui X, sui O


	//3. �������� �� �϶� ZoomOut�� �ϸ� cui O, sui X


	//4. �⺻ ���� �����ϸ� cui O, sui X
	
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//direction �������� �̵��ϰ� �ʹ�/

	//P = P0 + vt
	//SetActorLocation(GetActorLocation() + direction * walkSpeed * DeltaTime);

	//FTransform trans(GetControlRotation());
	//FVector resultDirection = trans.TransformVector(direction);

	//���Ʒ��� �Է°� ���� �ʰ� 0���� ������ֱ�
	//resultDirection.Z = 0;
	//resultDirection.Normalize();

	//AddMovementInput(resultDirection);

	//�Է��� �ް� �� ���� ���� �̵��� �����ϰ� �ٽ� 0���� ���ư� �� �ְ�
	//direction = FVector::ZeroVector;


	FTransform trans(GetControlRotation());
	FVector resultDirection = trans.TransformVector(direction);

	resultDirection.Z = 0;
	resultDirection.Normalize();

	AddMovementInput(resultDirection);

	direction = FVector::ZeroVector;

}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATPSPlayer::OnAxisHorizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATPSPlayer::OnAxisVertical);
	PlayerInputComponent->BindAxis(TEXT("Look Up"), this, &ATPSPlayer::OnAxisLookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn Right"), this, &ATPSPlayer::OnAxisTurnRight);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATPSPlayer::OnActionJump);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATPSPlayer::OnActionFirePressed);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &ATPSPlayer::OnActionFireReleased);

	PlayerInputComponent->BindAction(TEXT("GrenadeGun"), IE_Pressed, this, &ATPSPlayer::OnActionGrenade);
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &ATPSPlayer::OnActionSniper);

	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Pressed, this, &ATPSPlayer::OnActionZoomIn);
	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Released, this, &ATPSPlayer::OnActionZoomOut);


}

void ATPSPlayer::OnAxisHorizontal(float value)
{
	direction.Y = value;
}

void ATPSPlayer::OnAxisVertical(float value)
{
	direction.X = value;
}


void ATPSPlayer::OnAxisLookUp(float value)
{
	//Pitch
	AddControllerPitchInput(value);
}

void ATPSPlayer::OnAxisTurnRight(float value)
{
	//Yaw
	AddControllerYawInput(value);
}

void ATPSPlayer::OnActionJump()
{
	Jump();
}

void ATPSPlayer::OnActionFirePressed()
{
	//���� �⺻���̶��
	if(bChooseGrenadeGun)
	{
		GetWorld()->GetTimerManager().SetTimer(fireTimerHandle, this, &ATPSPlayer::DoFire, fireInterval, true, 0);

		//�ѹ��� �׳� ������ ������
		//DoFire();
	}

	//�׷��� �ʴٸ�
	else
	{
		FHitResult hitinfo;
		FVector start = cameraComp->GetComponentLocation();
		FVector end = start + cameraComp->GetForwardVector() * 100000;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		//�ٶ󺸰� �ʹ�.
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitinfo, start, end, ECollisionChannel::ECC_Visibility, params);

		//���� �ε��� ���� �ִٸ�
		if(bHit)
		{
			//��ȣ�ۿ��� �ϰ� �ʹ�.
			UPrimitiveComponent* hitComp = hitinfo.GetComponent();

			//�ε��� ���� ��������Ʈ�� ǥ���ϰ� �ʹ�.
			FTransform trans(hitinfo.ImpactPoint);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, trans);

			//���� �ε��� ���Ͱ� enemy���
			auto hitActor = hitinfo.GetActor();

			//isA�� �ε��� ���� Ȯ���ϱ�
			//if (hitActor && hitActor->IsA(AEnemy::StaticClass()))
			//{
			//	
			//}

			//Cast�� �ε��� ���� Ȯ���ϱ�
			auto enemy = Cast<AEnemy>(hitActor);
			if (enemy != nullptr)
			{
				//enemy���� �������� �ְ�ʹ�.

				//��� 1
				enemy->enemyFSM->OnDamageProcess(1);

				//��� 2
				//auto�� UObject*�� �ƴ϶� UEnemyFSM*�� �����Ѿ� �� -> ĳ��Ʈ�� ���Ž�Ű��
				//auto fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(TEXT("enemyFSM")));

				//fsm->OnDamageProcess(1);
			}

			//�ε��� ��ü�� �����ۿ��� �ϰ� �ִٸ�
			if (hitComp && hitComp->IsSimulatingPhysics())

				//���� ���ϰ� �ʹ�.
			{
				FVector force = (hitinfo.TraceEnd - hitinfo.TraceStart).GetSafeNormal();
				hitComp->AddForce(force * 1000000 * hitComp->GetMass());
			}
		}
	}
	

	
}

void ATPSPlayer::OnActionFireReleased()
{
	GetWorld()->GetTimerManager().ClearTimer(fireTimerHandle);
}

void ATPSPlayer::DoFire()
{
	// SpawnActor
	//FVector loc;
	//�÷��̾��� �� �������� 1���� �̵��� ��ġ
	//loc = GetActorLocation() + GetActorForwardVector() * 1;

	FTransform t = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
	
	GetWorld()->SpawnActor<ABulletActor>(bulletFactory, t);
}

void ATPSPlayer::ChooseGun(bool bGrenade)
{
	//���� �ٲٱ� ���� �������� ���̰� �ٲٷ��� ���� ��ź�̸�
	if(false == bChooseGrenadeGun && true == bGrenade)
	//FOV�� 90, cui O, sui X
	{
		cameraComp->SetFieldOfView(90);
		crosshairUI->AddToViewport();
		sniperUI->RemoveFromParent();
	}

	bChooseGrenadeGun = bGrenade;

	gunMeshComp->SetVisibility(bGrenade);
	sniperMeshComp->SetVisibility(!bGrenade);
}

void ATPSPlayer::OnActionGrenade()
{
	ChooseGun(true);
}

void ATPSPlayer::OnActionSniper()
{
	ChooseGun(false);
}

void ATPSPlayer::OnActionZoomIn()
{
	//���� ��ź�̶�� �ٷ� ����
	if(true == bChooseGrenadeGun)
	{
		return;
	}

	//���� �������۰��̶�� ++ ��ź���� �ƴ϶��
	if(false == bChooseGrenadeGun)

	//FOV�� 30���� �ٲٰ� crosshair�� �Ⱥ��̰� �ϰ� Ȯ����� ���̰� �ϰ� �ʹ�.
	{
		//Ȯ�� FOV 30
		cameraComp->SetFieldOfView(30);
		crosshairUI->RemoveFromParent();
		sniperUI->AddToViewport();
	}
}

void ATPSPlayer::OnActionZoomOut()
{
	//���� ��ź�̶�� �ٷ� ����
	if (true == bChooseGrenadeGun)
	{
		return;
	}

	//���� �������۰��̶�� ++ ��ź���� �ƴ϶��
	if (false == bChooseGrenadeGun)

		//FOV�� 90���� �ٲٰ� crosshair�� �Ⱥ��̰� �ϰ� Ȯ����� ���̰� �ϰ� �ʹ�.
	{
		//��� FOV 90
		cameraComp->SetFieldOfView(90);
		sniperUI->RemoveFromParent();
		crosshairUI->AddToViewport();
	}
}

