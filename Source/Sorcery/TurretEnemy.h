// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
//#include "TurretProjectile.h"
#include "TurretEnemy.generated.h"

/**
 * 
 */
UCLASS()
class SORCERY_API ATurretEnemy : public AEnemy
{
	GENERATED_BODY()

	USceneComponent* AttackTarget;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	bool bShootCooldownActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float VolleyIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float VolleyCount;
	
	FTimerHandle ShootCooldownTimer;
	
	UPROPERTY(EditAnywhere, Category = Projectile)
	float ShootCooldownTime;
	
public:
	// additional weak spots for this enemy
	UPROPERTY(VisibleDefaultsOnly, Category = "Combat")
	class USphereComponent* WeakSpotComp1;

	UPROPERTY(VisibleDefaultsOnly, Category = "Combat")
	class USphereComponent* WeakSpotComp2;
	
	// projectiles
	UPROPERTY(EditAnywhere, Category = Projectile)
	USceneComponent* ProjectileSpawnPoint;
	
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ATurretProjectile> ProjectileClass;

public:
	ATurretEnemy();

	virtual void RandomizeElementalWeakness() override;

	UFUNCTION(BlueprintCallable)
	void SpawnHomingProjectile();

	UFUNCTION(BlueprintCallable)
	void StartShootCooldown();

	UFUNCTION(BlueprintCallable)
	void ClearShootCooldown();
	
	virtual void AttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void AttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
