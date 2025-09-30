// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sorcery.h"
#include "Spell.generated.h"

UCLASS(config = Game)
class SORCERY_API ASpell : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	EElementalType Element;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	float Damage;

public:	
	// Sets default values for this actor's properties
	ASpell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UClass* GetDamageType();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ChangeElementalType(EElementalType NewType);
};
