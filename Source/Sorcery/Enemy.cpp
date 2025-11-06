// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Sorcery.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SorceryCharacter.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/DamageEvents.h"
#include "DT_Fire.h"
#include "DT_Shock.h"
#include "DT_Dark.h"
#include "DT_Acid.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EnemyController = Cast<AEnemyController>(GetController());
	bInAttackRange = false;
	bAttacking = false;

	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
	AttackSphere->SetupAttachment(RootComponent);
	
	WeakSpotComp = CreateDefaultSubobject<USphereComponent>(TEXT("WeakSpot"));
	WeakSpotComp->SetupAttachment(RootComponent);
	WeakSpotMultiplier = 1.5f;

	ElementalWeaknessMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElementalWeaknessMesh"));
	ElementalWeaknessMesh->SetupAttachment(WeakSpotComp);

	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(RootComponent);

	DamageResistancePercent = 0.8f;

	SoulsValue = 1;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (EnemyController == nullptr)
		EnemyController = Cast<AEnemyController>(GetController());
	
	AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AttackSphereBeginOverlap);
	AttackSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AttackSphereEndOverlap);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called after spawning in the spawner blueprint
void AEnemy::RandomizeElementalWeakness()
{
	int32 randomElement = FMath::RandRange(0, 3);
	switch (static_cast<EElementalType>(randomElement))
	{
		case EElementalType::Fire:
			ElementWeakness = EElementalType::Fire;
			ElementalWeaknessMesh->SetMaterial(0, M_Fire);
			break;
		case EElementalType::Shock:
			ElementWeakness = EElementalType::Shock;
			ElementalWeaknessMesh->SetMaterial(0, M_Shock);
			break;
		case EElementalType::Acid:
			ElementWeakness = EElementalType::Acid;
			ElementalWeaknessMesh->SetMaterial(0, M_Acid);
			break;
		case EElementalType::Dark:
			ElementWeakness = EElementalType::Dark;
			ElementalWeaknessMesh->SetMaterial(0, M_Dark);
			break;
	}
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

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UDamageType* DamageType = Cast<UDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	float Resistance = GetDamageResistance(DamageType);
	
	DamageTaken = DamageAmount * Resistance;
	Health -= DamageTaken; 
	if (Health <= 0.f)
	{
		Health = 0.f;
		Die(DamageCauser);
	}

	DrawFloatingDamageText();
	ApplyDamageFlash(DamageType);

	return DamageTaken;
}

float AEnemy::GetDamageResistance(UDamageType* DamageType) // returns inverse of resistance?
{
	if (DamageType->IsA(UDT_Fire::StaticClass()) && ElementWeakness != EElementalType::Fire)
	{
		return 1.f - DamageResistancePercent;
	}
	else if (DamageType->IsA(UDT_Shock::StaticClass()) && ElementWeakness != EElementalType::Shock)
	{
		return 1.f - DamageResistancePercent;
	}
	else if (DamageType->IsA(UDT_Dark::StaticClass()) && ElementWeakness != EElementalType::Dark)
	{
		return 1.f - DamageResistancePercent;
	}
	else if (DamageType->IsA(UDT_Acid::StaticClass()) && ElementWeakness != EElementalType::Acid)
	{
		return 1.f - DamageResistancePercent;
	}
	else
	{
		return 1.f;
	}
}

UMaterialInterface* AEnemy::GetDamageFlashMaterial(UDamageType* ElementType, bool CriticalHit)
{
	if (CriticalHit)
		return M_Critical;
	
	if (ElementType->IsA(UDT_Fire::StaticClass()))
	{
		return M_Fire;
	}
	else if (ElementType->IsA(UDT_Shock::StaticClass()))
	{
		return M_Shock;
	}
	else if (ElementType->IsA(UDT_Dark::StaticClass()))
	{
		return M_Dark;
	}
	else if (ElementType->IsA(UDT_Acid::StaticClass()))
	{
		return M_Acid;
	}

	return nullptr;
}

void AEnemy::Die(AActor* DeathCauser)
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(Player);
	if (Sorcerer)
		Sorcerer->GatherSouls(SoulsValue);
	
	// TODO reactions..?
	Destroy();
}