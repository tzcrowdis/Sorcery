// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Engine/DamageEvents.h"
#include "DT_Fire.h"
#include "DT_Ice.h"
#include "DT_Acid.h"
#include "DT_Shock.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DamageResistancePercent = 0.8f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UDamageType* DamageType = Cast<UDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	float Resistance = GetDamageResistance(DamageType);
	
	Health -= DamageAmount * Resistance; 
	if (Health <= 0.f)
	{
		Health = 0.f;
		Die(DamageCauser);
	}

	return DamageAmount * Resistance;
}

float AEnemy::GetDamageResistance(UDamageType* DamageType) // returns inverse of resistance?
{
	if (DamageType->IsA(UDT_Fire::StaticClass()) && ElementWeakness != EElementalType::Fire)
	{
		return 1.f - DamageResistancePercent;
	}
	else if (DamageType->IsA(UDT_Ice::StaticClass()) && ElementWeakness != EElementalType::Ice)
	{
		return 1.f - DamageResistancePercent;
	}
	else if (DamageType->IsA(UDT_Acid::StaticClass()) && ElementWeakness != EElementalType::Acid)
	{
		return 1.f - DamageResistancePercent;
	}
	else if (DamageType->IsA(UDT_Shock::StaticClass()) && ElementWeakness != EElementalType::Shock)
	{
		return 1.f - DamageResistancePercent;
	}
	else
	{
		return 1.f;
	}
}

void AEnemy::Die(AActor* DeathCauser)
{
	// TODO reactions..?
	Destroy();
}