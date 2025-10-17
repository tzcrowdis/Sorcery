// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "TurretEnemy.generated.h"

/**
 * 
 */
UCLASS()
class SORCERY_API ATurretEnemy : public AEnemy
{
	GENERATED_BODY()

	// TODO sit still
	// TODO homing particle
public:
	//UPROPERTY(EditDefaultsOnly, Category = Projectile)
	//TSubclassOf<class ASorceryProjectile> ProjectileClass;

public:
	ATurretEnemy();
	
	virtual void AttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void AttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	// TODO homing particle spawning
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnHomingParticles();
};
