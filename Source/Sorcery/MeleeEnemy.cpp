// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeEnemy.h"

#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SorceryCharacter.h"
#include "Kismet/GameplayStatics.h"

// set default variables
AMeleeEnemy::AMeleeEnemy()
{
	bIsChasing = true;
}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Sorcerer)
		EnemyController->GetBlackboard()->SetValueAsObject(TEXT("TargetActor"), Sorcerer);
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

			Attack(); // TODO replace with playing attack anim

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
			//EnemyController->GetBlackboard()->SetValueAsBool(TEXT("Attacking"), false);

			bInAttackRange = false;
			//bAttacking = false;
		}
	}
}

void AMeleeEnemy::EndAttack()
{
	if (EnemyController == nullptr)
		EnemyController = Cast<AEnemyController>(GetController());
	
	EnemyController->GetBlackboard()->SetValueAsBool(TEXT("Attacking"), false);
	bAttacking = false;
}