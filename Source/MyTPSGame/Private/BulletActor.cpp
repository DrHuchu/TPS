// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletActor.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ABulletActor::ABulletActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SetRootComponent(sphereComp);
	sphereComp->SetSphereRadius(13);
	sphereComp->SetCollisionProfileName(TEXT("BlockAll"));

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetRelativeScale3D(FVector(0.25f));
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	movementComp->SetUpdatedComponent(sphereComp);
	movementComp->InitialSpeed = 50000;
	movementComp->MaxSpeed = 50000;
	movementComp->bShouldBounce = true;

}

// Called when the game starts or when spawned
void ABulletActor::BeginPlay()
{
	Super::BeginPlay();
	//SetLifeSpan(2.0f);
	FTimerHandle dieTimerHandle; 
	//GetWorld()->GetTimerManager().SetTimer(dieTimerHandle, this, &ABulletActor::OnDie, 2, false);

	//람다식을 활용하면 매번 타이머 안의 함수를 구현할 필요 없음
	GetWorld()->GetTimerManager().SetTimer(dieTimerHandle, FTimerDelegate::CreateLambda([this]()->void { this->Destroy(); }), 2, false);
}

// Called every frame
void ABulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABulletActor::OnDie()
{
	Destroy();
}
