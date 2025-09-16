// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"

#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SorceryCharacter.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/DamageEvents.h"
#include "DT_Fire.h"
#include "DT_Ice.h"
#include "DT_Acid.h"
#include "DT_Shock.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EnemyController = Cast<AEnemyController>(GetController());
	bIsChasing = true;
	bInAttackRange = false;
	bAttacking = false;

	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
	AttackSphere->SetupAttachment(GetRootComponent());
	
	WeakSpotComp = CreateDefaultSubobject<USphereComponent>(TEXT("WeakSpot"));
	WeakSpotComp->SetupAttachment(RootComponent);
	WeakSpotMultiplier = 1.5f;

	DamageResistancePercent = 0.8f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (EnemyController == nullptr)
		EnemyController = Cast<AEnemyController>(GetController());

	ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Sorcerer)
		EnemyController->GetBlackboard()->SetValueAsObject(TEXT("TargetActor"), Sorcerer);
	
	AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AttackSphereBeginOverlap);
	AttackSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AttackSphereEndOverlap);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::AttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(OtherActor);
		if (Sorcerer)
		{
			if (EnemyController == nullptr)
				EnemyController = Cast<AEnemyController>(GetController());

			EnemyController->GetBlackboard()->SetValueAsBool(TEXT("InAttackRange"), true);

			bInAttackRange = true;
			bAttacking = true;
		}
	}
}

void AEnemy::AttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(OtherActor);
		if (Sorcerer)
		{
			if (EnemyController == nullptr)
				EnemyController = Cast<AEnemyController>(GetController());

			EnemyController->GetBlackboard()->SetValueAsBool(TEXT("InAttackRange"), false);

			bInAttackRange = false;
			bAttacking = false;
		}
	}
}

void AEnemy::IsChasing(bool bChasing)
{
	bIsChasing = bChasing;
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UDamageType* DamageType = Cast<UDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	float Resistance = GetDamageResistance(DamageType);
	
	Health -= DamageAmount * Resistance; 
	if (Health <= 0.f)
	{
		Health = 0.f;
		Die(DamageCauser);
	}

	return DamageAmount * Resistance;
}

float AEnemy::GetDamageResistance(UDamageType* DamageType) // returns inverse of resistance?
{
	if (DamageType->IsA(UDT_Fire::StaticClass()) && ElementWeakness != EElementalType::Fire)
	{
		return 1.f - DamageResistancePercent;
	}
	else if (DamageType->IsA(UDT_Ice::StaticClass()) && ElementWeakness != EElementalType::Ice)
	{
		return 1.f - DamageResistancePercent;
	}
	else if (DamageType->IsA(UDT_Acid::StaticClass()) && ElementWeakness != EElementalType::Acid)
	{
		return 1.f - DamageResistancePercent;
	}
	else if (DamageType->IsA(UDT_Shock::StaticClass()) && ElementWeakness != EElementalType::Shock)
	{
		return 1.f - DamageResistancePercent;
	}
	else
	{
		return 1.f;
	}
}

void AEnemy::Die(AActor* DeathCauser)
{
	// TODO reactions..?
	Destroy();
}