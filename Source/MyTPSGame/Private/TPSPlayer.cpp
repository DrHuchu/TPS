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

	//sudo code 의사코드 => 알고리즘
	//1. 외관에 해당하는 에셋을 읽어오고 싶다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));
	//2. 읽어올 수 있다면
	if(tempMesh.Succeeded())
	{
	//3. Mesh에 적용하고 싶다.
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	//4. Transform을 수정하고 싶다.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	//스프링암, 카메라 컴포넌트를 생성하고 싶다.
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArmComp"));
	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComp"));
	//스프링 암을 루트에 붙히고
	springArmComp->SetupAttachment(RootComponent);
	//카메라는 스프링 암에 붙히고 싶다.
	cameraComp->SetupAttachment(springArmComp);
	//스프링암의 위치 조정
	springArmComp->SetRelativeLocation(FVector(120, 50, 100));
	springArmComp->TargetArmLength = 250;

	//입력값을 회전에 반영하고 싶다.
	bUseControllerRotationYaw = true;
	springArmComp->bUsePawnControlRotation = true;
	cameraComp->bUsePawnControlRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//일반 총의 컴포넌트를 만들고 싶다.
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("gunMeshComp"));

	gunMeshComp->SetupAttachment(GetMesh());
	//일반 총의 에셋을 읽어서 컴포넌트에 넣고 싶다.
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

	//UI를 생성하고 싶다.
	crosshairUI = CreateWidget(GetWorld(), crosshairFactory);
	sniperUI = CreateWidget(GetWorld(), sniperFactory);

	//crosshairUI를 화면에 표시하고 싶다.
	crosshairUI->AddToViewport();

	ChooseGun(true);

	//1. 태어날 때 cui를 보이게하고 싶다.


	//2. 스나이퍼 건 일때 Zoom In을 하면 cui X, sui O


	//3. 스나이퍼 건 일때 ZoomOut을 하면 cui O, sui X


	//4. 기본 총을 선택하면 cui O, sui X
	
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//direction 방향으로 이동하고 싶다/

	//P = P0 + vt
	//SetActorLocation(GetActorLocation() + direction * walkSpeed * DeltaTime);

	//FTransform trans(GetControlRotation());
	//FVector resultDirection = trans.TransformVector(direction);

	//위아래로 입력값 들어가지 않게 0으로 만들어주기
	//resultDirection.Z = 0;
	//resultDirection.Normalize();

	//AddMovementInput(resultDirection);

	//입력을 받고 그 받은 값을 이동에 전달하고 다시 0으로 돌아갈 수 있게
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
	//만약 기본총이라면
	if(bChooseGrenadeGun)
	{
		GetWorld()->GetTimerManager().SetTimer(fireTimerHandle, this, &ATPSPlayer::DoFire, fireInterval, true, 0);

		//한발은 그냥 무조건 나오게
		//DoFire();
	}

	//그렇지 않다면
	else
	{
		FHitResult hitinfo;
		FVector start = cameraComp->GetComponentLocation();
		FVector end = start + cameraComp->GetForwardVector() * 100000;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		//바라보고 싶다.
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitinfo, start, end, ECollisionChannel::ECC_Visibility, params);

		//만약 부딪힌 것이 있다면
		if(bHit)
		{
			//상호작용을 하고 싶다.
			UPrimitiveComponent* hitComp = hitinfo.GetComponent();

			//부딪힌 곳에 폭발이펙트를 표시하고 싶다.
			FTransform trans(hitinfo.ImpactPoint);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, trans);

			//만약 부딪힌 액터가 enemy라면
			auto hitActor = hitinfo.GetActor();

			//isA로 부딪힌 액터 확인하기
			//if (hitActor && hitActor->IsA(AEnemy::StaticClass()))
			//{
			//	
			//}

			//Cast로 부딪힌 액터 확인하기
			auto enemy = Cast<AEnemy>(hitActor);
			if (enemy != nullptr)
			{
				//enemy에게 데미지를 주고싶다.

				//방법 1
				enemy->enemyFSM->OnDamageProcess(1);

				//방법 2
				//auto가 UObject*가 아니라 UEnemyFSM*을 가리켜야 함 -> 캐스트로 변신시키기
				//auto fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(TEXT("enemyFSM")));

				//fsm->OnDamageProcess(1);
			}

			//부딪힌 물체가 물리작용을 하고 있다면
			if (hitComp && hitComp->IsSimulatingPhysics())

				//힘을 가하고 싶다.
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
	//플레이어의 앞 방향으로 1미터 이동한 위치
	//loc = GetActorLocation() + GetActorForwardVector() * 1;

	FTransform t = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
	
	GetWorld()->SpawnActor<ABulletActor>(bulletFactory, t);
}

void ATPSPlayer::ChooseGun(bool bGrenade)
{
	//만약 바꾸기 전이 스나이퍼 건이고 바꾸려는 것이 유탄이면
	if(false == bChooseGrenadeGun && true == bGrenade)
	//FOV를 90, cui O, sui X
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
	//만약 유탄이라면 바로 종료
	if(true == bChooseGrenadeGun)
	{
		return;
	}

	//만약 스나이퍼건이라면 ++ 유탄총이 아니라면
	if(false == bChooseGrenadeGun)

	//FOV를 30으로 바꾸고 crosshair를 안보이게 하고 확대경을 보이게 하고 싶다.
	{
		//확대 FOV 30
		cameraComp->SetFieldOfView(30);
		crosshairUI->RemoveFromParent();
		sniperUI->AddToViewport();
	}
}

void ATPSPlayer::OnActionZoomOut()
{
	//만약 유탄이라면 바로 종료
	if (true == bChooseGrenadeGun)
	{
		return;
	}

	//만약 스나이퍼건이라면 ++ 유탄총이 아니라면
	if (false == bChooseGrenadeGun)

		//FOV를 90으로 바꾸고 crosshair를 안보이게 하고 확대경을 보이게 하고 싶다.
	{
		//축소 FOV 90
		cameraComp->SetFieldOfView(90);
		sniperUI->RemoveFromParent();
		crosshairUI->AddToViewport();
	}
}

