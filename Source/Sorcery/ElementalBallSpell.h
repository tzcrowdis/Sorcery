// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spell.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "SorceryCharacter.h"
#include "ElementalBallSpell.generated.h"

/**
 * 
 */
UCLASS()
class SORCERY_API AElementalBallSpell : public ASpell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Aim")
	float MaxAimDistance;

	/* Muzzle flash */
	UPROPERTY(EditAnywhere, Category = "Muzzle Flash")
	UNiagaraSystem* SpellMuzzleFlash;

	UPROPERTY(EditAnywhere, Category = "Muzzle Flash")
	UNiagaraComponent* MuzzleFlashComp;

protected:
	virtual void BeginPlay() override;

	// player camera for trace
	class UCameraComponent* Camera;

public:
	AElementalBallSpell();

	/** Default Projectile classes to spawn for each element */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ASorceryProjectile> ProjectileClass;

	UFUNCTION(BlueprintCallable, Category = "Spell")
	void ShootElementalBall(ASorceryCharacter* Sorcerer);

	virtual void ChangeElementalType(EElementalType NewType) override;
};
