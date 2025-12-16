// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sorcery.h"
#include "Components/Image.h"
#include "Enemy.h"
#include "Spell.generated.h"

class UCameraComponent;

UCLASS(config = Game)
class SORCERY_API ASpell : public AActor
{
	GENERATED_BODY()

protected:
	// Particle Color
	UPROPERTY(EditAnywhere, Category = "Element Color")
	FLinearColor FireColor;

	UPROPERTY(EditAnywhere, Category = "Element Color")
	FLinearColor ShockColor;

	UPROPERTY(EditAnywhere, Category = "Element Color")
	FLinearColor AcidColor;

	UPROPERTY(EditAnywhere, Category = "Element Color")
	FLinearColor DarkColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	EElementalType Element;

	// modifiable stats shared between all spells
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	float BaseDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	float BaseAttackSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	float AttackSpeed;

	// spell sound
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	class UAudioComponent* SpellSound;

public:	
	// Sets default values for this actor's properties
	ASpell();

	virtual void EquipSpell(EElementalType ElementType, float DamagePercent, float AttackSpeedPercent);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	UClass* GetDamageType();

	FHitResult GetAimHitResult(UCameraComponent* PlayerCamera, float MaxAimDistance);

	FVector GetAimLocation(UCameraComponent* PlayerCamera, float MaxAimDistance);

	void ApplyModifiers(AActor* TargetActor);

	void ApplyDamageToEnemy(AEnemy* Enemy, UPrimitiveComponent* EnemyComp, float HitDamage, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayProjectileHitSound();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayProjectileDeathSound();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ChangeElementalType(EElementalType NewType);

	UFUNCTION(BlueprintImplementableEvent, Category = "Reticle")
	void UpdateReticle();

	UFUNCTION(BlueprintCallable, Category = "Modifiable")
	void UpdateDamage(float PercentOfBase);

	UFUNCTION(BlueprintCallable, Category = "Modifiable")
	void UpdateAttackSpeed(float PercentOfBase);
};
