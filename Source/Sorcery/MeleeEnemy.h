// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "MeleeEnemy.generated.h"

/**
 * 
 */
UCLASS()
class SORCERY_API AMeleeEnemy : public AEnemy
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Combat")
	UStaticMeshComponent* AttackMesh;

public:
	UPROPERTY(BlueprintReadOnly)
	bool bIsChasing;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	AMeleeEnemy();

	// chasing
	UFUNCTION(BlueprintCallable)
	void IsChasing(bool bChasing);

	// melee attack
	virtual void AttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void AttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION(BlueprintImplementableEvent)
	void Attack();

	UFUNCTION(BlueprintCallable)
	void EndAttack();
};
