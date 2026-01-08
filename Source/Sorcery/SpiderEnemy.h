// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "SpiderEnemy.generated.h"

/**
 * 
 */
UCLASS()
class SORCERY_API ASpiderEnemy : public AEnemy
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ElementalWeaknessMesh;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float Damage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackCooldownTime;

	FTimerHandle AttackCooldownTimer;

	bool AttackCooldownActive;

protected:
	virtual void BeginPlay() override;

public:
	ASpiderEnemy();

	virtual void RandomizeElementalWeakness() override;

	virtual void AttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void AttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void StartAttackCooldown();

	void ClearAttackCooldown();
};
