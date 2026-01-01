// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradePickup.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AUpgradePickup::AUpgradePickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UpgradeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UpgradeMesh"));
	SetRootComponent(UpgradeMesh);

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetupAttachment(RootComponent);

	SpawnEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpawnEffect"));
	SpawnEffect->SetupAttachment(RootComponent);

	InfoSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InfoSphere"));
	InfoSphere->SetupAttachment(RootComponent);

	InfoMenu = CreateDefaultSubobject<UWidgetComponent>(TEXT("InfoMenu"));
	InfoMenu->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AUpgradePickup::BeginPlay()
{
	Super::BeginPlay();

	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &AUpgradePickup::PickupSphereBeginOverlap);

	InfoSphere->OnComponentBeginOverlap.AddDynamic(this, &AUpgradePickup::InfoSphereBeginOverlap);
	InfoSphere->OnComponentEndOverlap.AddDynamic(this, &AUpgradePickup::InfoSphereEndOverlap);
	
	SpawnEffect->Activate();
}

// Called every frame
void AUpgradePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUpgradePickup::PickupSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(OtherActor);
	if (Sorcerer)
	{
		ApplyUpgrade(Sorcerer);
		Sorcerer->UpgradePickedUp();
		Destroy();
	}
}

void AUpgradePickup::InfoSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASorceryCharacter* Sorcerer = Cast<ASorceryCharacter>(OtherActor);
	if (Sorcerer)
	{
		InfoMenu->SetVisibility(true);
		UpgradeMeshMotion(true);
	}
}

void AUpgradePickup::InfoSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	InfoMenu->SetVisibility(false);
	UpgradeMeshMotion(false);
}