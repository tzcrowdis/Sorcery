// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell.h"

#include "DT_Fire.h"
#include "DT_Ice.h"
#include "DT_Acid.h"
#include "DT_Shock.h"

// Sets default values
ASpell::ASpell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UClass* ASpell::GetDamageType()
{
	switch (Element)
	{
	case EElementalType::Fire:
		return UDT_Fire::StaticClass();
	case EElementalType::Ice:
		return UDT_Ice::StaticClass();
	case EElementalType::Shock:
		return UDT_Shock::StaticClass();
	case EElementalType::Acid:
		return UDT_Acid::StaticClass();
	}

	return nullptr;
}

void ASpell::ChangeElementalType(EElementalType NewType)
{
	Element = NewType;
}