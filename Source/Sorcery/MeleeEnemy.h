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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elements", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ElementalWeaknessMesh;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UStaticMeshComponent* AttackMesh;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float Damage;

public:
	UPROPERTY(BlueprintReadOnly)
	bool bIsChasing;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UCapsuleComponent* KnifeCollider;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	AMeleeEnemy();

	virtual void RandomizeElementalWeakness() override;

	// chasing
	UFUNCTION(BlueprintCallable)
	void IsChasing(bool bChasing);

	// melee attack
	virtual void AttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void AttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	virtual void KnifeColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void EnableKnifeCollision();

	UFUNCTION(BlueprintCallable)
	void DisableKnifeCollision();

	UFUNCTION(BlueprintImplementableEvent)
	void Attack();

	//UFUNCTION(BlueprintCallable)
	//void EndAttack();
};
