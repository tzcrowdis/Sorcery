// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretEnemy.h"

#include "EnemyController.h"
#include "SorceryCharacter.h"

ATurretEnemy::ATurretEnemy()
{

}

void ATurretEnemy::AttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(OtherActor);
		if (Sorcerer)
		{
			bInAttackRange = true;
			bAttacking = true;
		}
	}
}

void ATurretEnemy::AttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(OtherActor);
		if (Sorcerer)
		{
			bInAttackRange = false;
			bAttacking = false;
		}
	}
}