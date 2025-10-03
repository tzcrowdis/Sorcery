// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sorcery.h"
#include "Spell.generated.h"

class UCameraComponent;

UCLASS(config = Game)
class SORCERY_API ASpell : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Element Color")
	FLinearColor FireColor;

	UPROPERTY(EditAnywhere, Category = "Element Color")
	FLinearColor IceColor;

	UPROPERTY(EditAnywhere, Category = "Element Color")
	FLinearColor ShockColor;

	UPROPERTY(EditAnywhere, Category = "Element Color")
	FLinearColor AcidColor;


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

	FHitResult GetAimHitResult(UCameraComponent* PlayerCamera, float MaxAimDistance);

	FVector GetAimLocation(UCameraComponent* PlayerCamera, float MaxAimDistance);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ChangeElementalType(EElementalType NewType);
};
