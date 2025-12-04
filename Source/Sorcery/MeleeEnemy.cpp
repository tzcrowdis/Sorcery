// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeEnemy.h"

#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SorceryCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

// set default variables
AMeleeEnemy::AMeleeEnemy()
{
	WeakSpotComp->SetupAttachment(GetMesh(), FName(TEXT("HeadSocket")));
	
	ElementalWeaknessMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElementalWeaknessMesh"));
	ElementalWeaknessMesh->SetupAttachment(WeakSpotComp);
	
	bIsChasing = true;

	Damage = 10.f;

	KnifeCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("KnifeCollider"));
	KnifeCollider->SetupAttachment(GetMesh(), FName(TEXT("KnifeSocket")));
	KnifeCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Sorcerer)
		EnemyController->GetBlackboard()->SetValueAsObject(TEXT("TargetActor"), Sorcerer);

	KnifeCollider->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::KnifeColliderBeginOverlap);
}

void AMeleeEnemy::RandomizeElementalWeakness()
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

void AMeleeEnemy::IsChasing(bool bChasing)
{
	bIsChasing = bChasing;
}

void AMeleeEnemy::AttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(OtherActor);
		if (Sorcerer)
		{
			if (EnemyController == nullptr)
				EnemyController = Cast<AEnemyController>(GetController());

			EnemyController->GetBlackboard()->SetValueAsBool(TEXT("InAttackRange"), true);
			EnemyController->GetBlackboard()->SetValueAsBool(TEXT("Attacking"), true);

			Attack();

			bInAttackRange = true;
			bAttacking = true;
		}
	}
}

void AMeleeEnemy::AttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(OtherActor);
		if (Sorcerer)
		{
			if (EnemyController == nullptr)
				EnemyController = Cast<AEnemyController>(GetController());

			EnemyController->GetBlackboard()->SetValueAsBool(TEXT("InAttackRange"), false);
			EnemyController->GetBlackboard()->SetValueAsBool(TEXT("Attacking"), false);

			bInAttackRange = false;
			bAttacking = false;
		}
	}
}

void AMeleeEnemy::KnifeColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		ASorceryCharacter* Player = Cast<ASorceryCharacter>(OtherActor);
		if (Player)
		{
			UGameplayStatics::ApplyDamage(Player, Damage, GetController(), this, UDamageType::StaticClass());

			DisableKnifeCollision();
		}
	}
}

void AMeleeEnemy::EnableKnifeCollision()
{
	KnifeCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMeleeEnemy::DisableKnifeCollision()
{
	KnifeCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

/*
void AMeleeEnemy::EndAttack()
{
	if (EnemyController == nullptr)
		EnemyController = Cast<AEnemyController>(GetController());
	
	EnemyController->GetBlackboard()->SetValueAsBool(TEXT("Attacking"), false);
	bAttacking = false;
}
*/