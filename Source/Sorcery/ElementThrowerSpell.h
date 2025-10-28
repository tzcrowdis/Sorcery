// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spell.h"
#include "NiagaraComponent.h"
#include "ElementThrowerSpell.generated.h"

/**
 * 
 */
UCLASS()
class SORCERY_API AElementThrowerSpell : public ASpell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Spells")
	UNiagaraComponent* ElementThrowerComp;

	UPROPERTY(EditAnywhere, Category = "Spell")
	class USphereComponent* DamageSphere;

	float MaxAimDistance;

protected:
	bool bShootCooldownActive;
	FTimerHandle ShootCooldownTimer;
	void ClearShootCooldown();
	
public:
	AElementThrowerSpell();

	FRotator GetSpellOrientation(UCameraComponent* Camera);

	virtual void ChangeElementalType(EElementalType NewType) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spell")
	bool bThrowing;

	UFUNCTION(BlueprintCallable, Category = "Spell")
	void ShootThrowerSpell(ASorceryCharacter* Sorcerer);

	UFUNCTION(BlueprintCallable, Category = "Spell")
	void ApplyThrowerDamage();

	UFUNCTION(BlueprintCallable, Category = "Spell")
	void DeactivateThrowerSpell();
};
